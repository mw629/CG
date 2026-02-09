#include "SkinningObject3d.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
StructuredBuffer<Well> gMatrixPalette : register(t0);

Skinned Skinning(VertexShaderInput input)
{
    Skinned skinned;
    // 位置の変換
    skinned.position = mul(input.position,  gMatrixPalette[input.index.x].skeletonSpaceMatrix * input.weight.x);
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix * input.weight.y);
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix * input.weight.z);
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix * input.weight.w);
    skinned.position.w = 1.0f;

    // 法線の変換
    skinned.normal = mul(input.normal,  (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix * input.weight.x);
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix * input.weight.y);
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix * input.weight.z);
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix * input.weight.w);
    skinned.normal = normalize(skinned.normal);

    return skinned;
}


VertexShaderOutput main(VertexShaderInput input)//入力頂点は当然
{
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);//まずSkinning計算を行って、SKinning後の頂点情報を手に入れる。ここでの頂点もSkeletonSpace
    //Skinning結果を使って変換
    output.position = mul(skinned.position, gTransformationMatrix.WVP);
    output.worldPosition = mul(skinned.position, gTransformationMatrix.World).xyz;
    output.normal = normalize(mul(skinned.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));
    output.texcoord = input.texcoord;
    
    return output;
}