#include "Object3d.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
struct DirectionalLightGroup { int32_t numLights; float32_t3 padding; DirectionalLight lights[64]; };
struct PointLightGroup { int32_t numLights; float32_t3 padding; PointLight lights[64]; };
struct SpotLightGroup { int32_t numLights; float32_t3 padding; SpotLight lights[64]; };

ConstantBuffer<DirectionalLightGroup> gDirectionalLightGroup : register(b2);
ConstantBuffer<PointLightGroup> gPointLightGroup : register(b3);
ConstantBuffer<SpotLightGroup> gSpotLightGroup : register(b4);

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
    float32_t3 normal = normalize(input.normal);
    
    // --- 氷の環境マッピングと屈折表現 ---
    float32_t3 cameraToPosition = -toEye;
    
    // フレネル反射 (Schlickの近似)
    float f0 = 0.04f; // 氷や水などの基本的な反射率
    float cosTheta = saturate(dot(toEye, normal));
    float fresnel = f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);

    // 反射
    float32_t3 reflectedVector = reflect(cameraToPosition, normal);
    float32_t4 reflectColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
    
    // 屈折 (空気 1.0 / 氷 1.31 ≈ 0.763)
    float32_t3 refractedVector = refract(cameraToPosition, normal, 0.763f);
    // 全反射して屈折ベクトルが0になる場合を考慮してブレンド
    float32_t4 refractColor = length(refractedVector) > 0.01f ? gEnvironmentTexture.Sample(gSampler, refractedVector) : reflectColor;

    // フレネル値を使って反射と屈折をブレンド
    float32_t4 environmentColor = lerp(refractColor, reflectColor, fresnel);
    
    // 氷特有のベースカラー（少し青みを持たせる）
    float32_t3 iceBaseColor = float32_t3(0.85f, 0.95f, 1.0f);
    
    if (gMaterial.enableLighting != 0)
    {
        float32_t3 diffuse = float32_t3(0.0f, 0.0f, 0.0f);
        float32_t3 specular = float32_t3(0.0f, 0.0f, 0.0f);
        
        // Directional Lights
        for (int i = 0; i < gDirectionalLightGroup.numLights; ++i)
        {
            if (gDirectionalLightGroup.lights[i].active != 0)
            {
                DirectionalLight dirLight = gDirectionalLightGroup.lights[i];
                float32_t3 halfVector = normalize(-dirLight.direction + toEye);
                float32_t NDotH = dot(normal, halfVector);
                // 氷の鋭いハイライト表現のためにshininessを少し強める
                float32_t specularPow = pow(saturate(NDotH), gMaterial.shininess * 1.5f); 
                
                // ハーフランバートによる柔らかな光の回り込み (氷の内部散乱のフェイク)
                float32_t NdirL = dot(normal, -dirLight.direction);
                float32_t cos = pow(NdirL * 0.5f + 0.5f, 2.0f);
                
                diffuse += gMaterial.color.rgb * textureColor.rgb * dirLight.color.rgb * cos * dirLight.intensity;
                specular += dirLight.color.rgb * dirLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
            }
        }
        
        // Point Lights
        for (int j = 0; j < gPointLightGroup.numLights; ++j)
        {
            if (gPointLightGroup.lights[j].active != 0)
            {
                PointLight ptLight = gPointLightGroup.lights[j];
                float32_t3 pointLightDirection = normalize(input.worldPosition - ptLight.position);
                float32_t3 pointHalfVector = normalize(-pointLightDirection + toEye);
                float32_t pointDotH = dot(normal, pointHalfVector);
                float32_t pointSpecularPow = pow(saturate(pointDotH), gMaterial.shininess * 1.5f);
                float32_t pointNdirL = dot(normal, -pointLightDirection);
                float32_t pointCos = pow(pointNdirL * 0.5f + 0.5f, 2.0f);
                float32_t distance = length(ptLight.position - input.worldPosition);
                float32_t factor = pow(saturate(-distance / ptLight.radius + 1.0f), ptLight.decay);
                
                diffuse += gMaterial.color.rgb * textureColor.rgb * ptLight.color.rgb * pointCos * ptLight.intensity * factor;
                specular += ptLight.color.rgb * ptLight.intensity * pointSpecularPow * float32_t3(1.0f, 1.0f, 1.0f) * factor;
            }
        }
        
        // Spot Lights
        for (int k = 0; k < gSpotLightGroup.numLights; ++k)
        {
            if (gSpotLightGroup.lights[k].active != 0)
            {
                SpotLight spLight = gSpotLightGroup.lights[k];
                float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - spLight.position);
                float32_t3 spotHalfVector = normalize(-spotLightDirectionOnSurface + toEye);
                float32_t spotDotH = dot(normal, spotHalfVector);
                float32_t spotSpecularPow = pow(saturate(spotDotH), gMaterial.shininess * 1.5f);
                float32_t spotNdirL = dot(normal, -spotLightDirectionOnSurface);
                float32_t spotCos = pow(spotNdirL * 0.5f + 0.5f, 2.0f);
                float32_t conAngle = dot(spotLightDirectionOnSurface, spLight.direction);
                float32_t fallffFactor = saturate((conAngle - spLight.cosAngle) / (spLight.cosFalloffStart));
                float32_t spotDistance = length(spLight.position - input.worldPosition);
                float32_t attenuationFactor = pow(saturate(-spotDistance / spLight.distance + 1.0f), spLight.decay);
                
                diffuse += gMaterial.color.rgb * textureColor.rgb * spLight.color.rgb * conAngle * spLight.intensity * fallffFactor * attenuationFactor;
                specular += spLight.color.rgb * spLight.intensity * spotSpecularPow * float32_t3(1.0f, 1.0f, 1.0f) * fallffFactor * attenuationFactor;
            }
        }
        
        // リムライト (エッジを明るくして氷の透け感・反射を強調)
        float rim = 1.0f - saturate(dot(toEye, normal));
        rim = smoothstep(0.4f, 1.0f, rim);
        float32_t3 rimColor = float32_t3(0.6f, 0.8f, 1.0f) * rim * 0.5f;

        // 最終的な色の合成 (氷の青みを全体に乗せる)
        output.color.rgb = (diffuse + specular + rimColor) * iceBaseColor;
       
        // α値
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
        output.color.rgb *= iceBaseColor;
    }
    
    //環境マッピングの色を加算 (氷らしく少し強めに影響させる)
    // 屈折成分も入っているので少し透明感が出る
    output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient * 1.2f;
    
    // 視線と法線の内積を元に、正面（中心）を透明に、外側（輪郭）を不透明・水色にする
    float viewDot = saturate(dot(toEye, normal));
    float rimFactor = 1.0f - viewDot; // 正面で0、輪郭で1
    
    float edgeAlpha = smoothstep(0.0f, 0.6f, rimFactor);
    output.color.a *= edgeAlpha;
    
    output.color.rgb = lerp(output.color.rgb, float3(0.4f, 0.8f, 1.0f), smoothstep(0.2f, 0.8f, rimFactor));
    
    if (output.color.a <= 0.0f)
    {
        discard;
    }
    
    return output;
}