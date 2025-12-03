#include "Object3d.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
   
     //Phong
    //float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    //float RdotE = dot(reflectLight, toEye);
    //float specularPow = pow(saturate(RdotE), gMaterial.shininess);
    
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    //ディレクションライト
    
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    
    float32_t NDotH = dot(normalize(input.normal), halfVector);
    float32_t specularPow = pow(saturate(NDotH), gMaterial.shininess);
    float32_t NdirL = dot(normalize(input.normal), -gDirectionalLight.direction);
    float32_t cos = pow(NdirL * 0.5f + 0.5f, 2.0f);
    
   //ポイントライト
    
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    float32_t3 pointHalfVector = normalize(-pointLightDirection + toEye);
    
    float32_t pointDotH = dot(normalize(input.normal), pointHalfVector);
    float32_t pointSpecularPow = pow(saturate(pointDotH), gMaterial.shininess);
    float32_t pointNdirL = dot(normalize(input.normal), -pointLightDirection);
    float32_t pointCos = pow(pointNdirL * 0.5f + 0.5f, 2.0f);
    
    float32_t distance = length(gPointLight.position - input.worldPosition);//ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
    
    //スポットライト
    
    float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gSpotLight.position);
    float32_t3 spotHalfVector = normalize(-spotLightDirectionOnSurface + toEye);
    
    float32_t spotDotH = dot(normalize(input.normal), spotHalfVector);
    float32_t spotSpecularPow = pow(saturate(spotDotH), gMaterial.shininess);
    float32_t spotNdirL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
    float32_t spotCos = pow(spotNdirL * 0.5f + 0.5f, 2.0f);
    
    float32_t conAngle = dot(spotLightDirectionOnSurface, gSpotLight.direction);
    float32_t fallffFactor = saturate((conAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart));
   
      // スポットライトの距離減衰
    float32_t spotDistance = length(gSpotLight.position - input.worldPosition);
    float32_t attenuationFactor = pow(saturate(-spotDistance / gSpotLight.distance + 1.0f), gSpotLight.decay);
    
    
    
    if (gMaterial.enableLighting != 0)
    {
       
       //float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color[3] = gMaterial.color[3] * textureColor[3];

        
          //拡散反射
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        //鏡面反射
        float32_t3 specularDirectionalLight =
        gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
      
        //拡散反射
        float32_t3 diffusePointLight =
        gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * pointCos * gPointLight.intensity * factor;
        //鏡面反射
        float32_t3 specularPointLight =
        gPointLight.color.rgb * gPointLight.intensity * pointSpecularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
      
        //拡散反射
        float32_t3 diffuseSpotLight =
        gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb *
        conAngle * gSpotLight.intensity * fallffFactor * attenuationFactor;
        //鏡面反射
        float32_t3 specularSpotLight =
        gSpotLight.color.rgb * gSpotLight.intensity * spotSpecularPow *
        float32_t3(1.0f, 1.0f, 1.0f) * fallffFactor * attenuationFactor;
       
         //拡散反射＋鏡面反射
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight +
        diffusePointLight + specularPointLight +
        diffuseSpotLight + specularSpotLight;
       
       
        //αは今まで通り
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