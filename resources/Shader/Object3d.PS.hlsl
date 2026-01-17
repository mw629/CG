#include "Object3d.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<LightCounts> lightCounts : register(b3);


StructuredBuffer<PointLight> gPointLight : register(t1);
StructuredBuffer<SpotLight> gSpotLight : register(t2);



struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    // ディレクションライト
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float32_t NDotH = dot(normalize(input.normal), halfVector);
    float32_t specularPow = pow(saturate(NDotH), gMaterial.shininess);
    float32_t NdirL = dot(normalize(input.normal), -gDirectionalLight.direction);
    float32_t cos = pow(NdirL * 0.5f + 0.5f, 2.0f);
    
    // 拡散反射と鏡面反射の初期化
    float32_t3 diffuseLight = float32_t3(0.0f, 0.0f, 0.0f);
    float32_t3 specularLight = float32_t3(0.0f, 0.0f, 0.0f);
    
    if (gMaterial.enableLighting != 0)
    {
        // ディレクションライト
        diffuseLight += gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        specularLight += gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;
        
        // 複数のポイントライトをループ処理
        for (uint i = 0; i < lightCounts.pointLightCount; ++i)
        {
            PointLight pointLight = gPointLights[i];
            
            float32_t3 pointLightDirection = normalize(input.worldPosition - pointLight.position);
            float32_t3 pointHalfVector = normalize(-pointLightDirection + toEye);
            
            float32_t pointDotH = dot(normalize(input.normal), pointHalfVector);
            float32_t pointSpecularPow = pow(saturate(pointDotH), gMaterial.shininess);
            float32_t pointNdirL = dot(normalize(input.normal), -pointLightDirection);
            float32_t pointCos = pow(pointNdirL * 0.5f + 0.5f, 2.0f);
            
            float32_t distance = length(pointLight.position - input.worldPosition);
            float32_t factor = pow(saturate(-distance / pointLight.radius + 1.0f), pointLight.decay);
            
            diffuseLight += gMaterial.color.rgb * textureColor.rgb * pointLight.color.rgb * pointCos * pointLight.intensity * factor;
            specularLight += pointLight.color.rgb * pointLight.intensity * pointSpecularPow * factor;
        }
        
        // 複数のスポットライトをループ処理
        for (uint j = 0; j < lightCounts.spotLightCount; ++j)
        {
            SpotLight spotLight = gSpotLights[j];
            
            float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - spotLight.position);
            float32_t3 spotHalfVector = normalize(-spotLightDirectionOnSurface + toEye);
            
            float32_t spotDotH = dot(normalize(input.normal), spotHalfVector);
            float32_t spotSpecularPow = pow(saturate(spotDotH), gMaterial.shininess);
            float32_t spotNdirL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
            float32_t spotCos = pow(spotNdirL * 0.5f + 0.5f, 2.0f);
            
            float32_t conAngle = dot(spotLightDirectionOnSurface, spotLight.direction);
            float32_t fallffFactor = saturate((conAngle - spotLight.cosAngle) / (spotLight.cosFalloffStart - spotLight.cosAngle));
            
            float32_t spotDistance = length(spotLight.position - input.worldPosition);
            float32_t attenuationFactor = pow(saturate(-spotDistance / spotLight.distance + 1.0f), spotLight.decay);
            
            diffuseLight += gMaterial.color.rgb * textureColor.rgb * spotLight.color.rgb * conAngle * spotLight.intensity * fallffFactor * attenuationFactor;
            specularLight += spotLight.color.rgb * spotLight.intensity * spotSpecularPow * fallffFactor * attenuationFactor;
        }
        
        output.color.rgb = diffuseLight + specularLight;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    if (output.color.a <= 0.5f)
    {
        discard;
    }
    
    return output;
}