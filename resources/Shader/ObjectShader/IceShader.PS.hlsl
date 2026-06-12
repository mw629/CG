#include "Object3d.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
TextureCube<float32_t4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);

struct DirectionalLightGroup
{
    int32_t numLights;
    float32_t3 padding;
    DirectionalLight lights[64];
};
struct PointLightGroup
{
    int32_t numLights;
    float32_t3 padding;
    PointLight lights[64];
};
struct SpotLightGroup
{
    int32_t numLights;
    float32_t3 padding;
    SpotLight lights[64];
};

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
    
    // 視線ベクトルと法線の正規化
    float32_t3 normal = normalize(input.normal);
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    // ---------------------------------------------------------------
    // 1. フレネル効果（外枠を水色、中心を半透明にする）
    // ---------------------------------------------------------------
    // 視線と法線が垂直（外枠）に近づくほど 0 になり、正面（中心）ほど 1 になる
    float32_t NdotV = saturate(dot(normal, toEye));
    
    // 氷の色の定義（必要に応じてマテリアル側から渡せるようにしても良いです）
    float32_t3 iceCenterColor = float32_t3(1.0f, 1.0f, 1.0f); // 中心：白
    float32_t3 iceEdgeColor = float32_t3(0.5f, 0.8f, 1.0f); // 外枠：水色
    
    // フレネルの強さ。powの指数を大きくすると、水色がより外枠だけに集中します
    float32_t fresnel = pow(1.0f - NdotV, 3.0f);
    
    // 外枠ほど水色、中心ほど白にする
    float32_t3 baseIceColor = lerp(iceCenterColor, iceEdgeColor, fresnel);
    
    // 中心にかけて半透明（外枠は不透明に近づく）
    // マテリアルのアルファ値を基準に、フレネルで外枠の不透明度を上げます
    float32_t baseAlpha = lerp(gMaterial.color.a * 0.3f, 1.0f, fresnel);

    // ---------------------------------------------------------------
    // 2. ライティング計算（テクスチャは無視して白ベースで計算）
    // ---------------------------------------------------------------
    float32_t3 diffuse = float32_t3(0.0f, 0.0f, 0.0f);
    float32_t3 specular = float32_t3(0.0f, 0.0f, 0.0f);
    
    if (gMaterial.enableLighting != 0)
    {
        // Directional Lights
        for (int i = 0; i < gDirectionalLightGroup.numLights; ++i)
        {
            if (gDirectionalLightGroup.lights[i].active != 0)
            {
                DirectionalLight dirLight = gDirectionalLightGroup.lights[i];
                float32_t3 halfVector = normalize(-dirLight.direction + toEye);
                float32_t NDotH = dot(normal, halfVector);
                float32_t specularPow = pow(saturate(NDotH), gMaterial.shininess);
                
                float32_t NdirL = dot(normal, -dirLight.direction);
                float32_t cos = pow(NdirL * 0.5f + 0.5f, 2.0f);
                
                // テクスチャカラーを外し、baseIceColorを基準にする
                diffuse += baseIceColor * dirLight.color.rgb * cos * dirLight.intensity;
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
                float32_t pointSpecularPow = pow(saturate(pointDotH), gMaterial.shininess);
                
                float32_t pointNdirL = dot(normal, -pointLightDirection);
                float32_t pointCos = pow(pointNdirL * 0.5f + 0.5f, 2.0f);
                
                float32_t distance = length(ptLight.position - input.worldPosition);
                float32_t factor = pow(saturate(-distance / ptLight.radius + 1.0f), ptLight.decay);
                
                diffuse += baseIceColor * ptLight.color.rgb * pointCos * ptLight.intensity * factor;
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
                float32_t spotSpecularPow = pow(saturate(spotDotH), gMaterial.shininess);
                
                float32_t spotNdirL = dot(normal, -spotLightDirectionOnSurface);
                float32_t spotCos = pow(spotNdirL * 0.5f + 0.5f, 2.0f);
                
                float32_t conAngle = dot(spotLightDirectionOnSurface, spLight.direction);
                float32_t fallffFactor = saturate((conAngle - spLight.cosAngle) / (spLight.cosFalloffStart));
                
                float32_t spotDistance = length(spLight.position - input.worldPosition);
                float32_t attenuationFactor = pow(saturate(-spotDistance / spLight.distance + 1.0f), spLight.decay);
                
                diffuse += baseIceColor * spLight.color.rgb * conAngle * spLight.intensity * fallffFactor * attenuationFactor;
                specular += spLight.color.rgb * spLight.intensity * spotSpecularPow * float32_t3(1.0f, 1.0f, 1.0f) * fallffFactor * attenuationFactor;
            }
        }
        
        output.color.rgb = diffuse + specular;
        output.color.a = baseAlpha;
    }
    else
    {
        // ライトが無効な場合も氷のベース色を適用
        output.color.rgb = baseIceColor;
        output.color.a = baseAlpha;
    }
    
    // ---------------------------------------------------------------
    // 3. 環境マッピングの適用
    // ---------------------------------------------------------------
    float32_t3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
    float32_t3 reflectedVector = reflect(cameraToPosition, normal);
    float32_t4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
    
    // 環境マップの色を加算（氷の周囲の映り込みを表現）
    output.color.rgb += environmentColor.rgb * gMaterial.environmentCoefficient;
    
    // 透過処理を行うため、元のコードにあった「0.5以下でdiscard」はコメントアウト、または削除します
    // (discardしてしまうと、中心の半透明部分が消えて穴が空いてしまうため)
    /*
    if (output.color.a <= 0.5f)
    {
        discard;
    }
    */
    
    return output;
}