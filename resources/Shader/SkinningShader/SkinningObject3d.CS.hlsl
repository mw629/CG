
struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Vertex
{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};

struct VertexInfluence
{
    float32_t4 weight;
    iint32_t4 index;
};

struct SkinningInformation
{
    uint32_t numVertices;
};


StructuredBuffer<Well> gMatrixPalette : register(t0);
StructuredBuffer<Vertex> gInputVertices : register(t1);
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);



[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint32_t vertexIndex = DTid.x;
    
    if (vertexIndex < gSkinningInformation.numVertices)
    {
    //Skinningの計算
    
        Vertex input = gInputVertices[vertexIndex];
        VertexInfluence influence = gInfluences[vertexIndex];
        
        Vertex skinned;
    
        
        // 位置の変換
        skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix * input.weight.x);
        skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix * input.weight.y);
        skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix * input.weight.z);
        skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix * input.weight.w);
        skinned.position.w = 1.0f;

        // 法線の変換
        skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix * input.weight.x);
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix * input.weight.y);
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix * input.weight.z);
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix * input.weight.w);
        skinned.normal = normalize(skinned.normal);
        
        gOutputVertices[vertexIndex] = skinned;
    }
    
    
    
}