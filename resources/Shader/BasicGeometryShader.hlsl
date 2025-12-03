#include "Object3d.hlsli" 

[maxvertexcount(3)]//()の中の数字が最大出力数
void main(
//プリミティブ（三角など）　入力変数型　input[頂点数]
	triangle VertexShaderOutput input[3], 
// 　出力変数型　↓Lineならワイヤーフレーム :
	inout TriangleStream< GSOutput> output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
        element.position = input[i].position;
        element.texcoord = input[i].texcoord;
		element.normal = input[i].normal;
        element.worldPosition = input[i].worldPosition;
		output.Append(element);
	}
}