#include "Object3d.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);



struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float NdirL = dot(normalize(input.normal), -gDirectionalLight.direction);
    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
   
    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));

    float RdotE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdotE), gMaterial.shininess);
    
    //if (gMaterial.enableLighting != 0)
    //{
        float cos = pow(NdirL * 0.5f + 0.5f, 2.0f);
        output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
        output.color[3] = gMaterial.color[3] * textureColor[3];
    //}
    //else
    //{
    //    output.color = gMaterial.color * textureColor;
    //}
    
    //拡散反射
    float32_t3 diffuse =
    gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
    //鏡面反射
    float32_t3 specular =
    gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
    //拡散反射＋鏡面反射
    output.color.rgb = diffuse + specular;
    //αは今まで通り
    output.color.a = gMaterial.color.a * textureColor.a;
    
   
    
    //if (output.color.a <= 0.5f)
    //{
    //    discard;
    //}
   
    
    return output;
}