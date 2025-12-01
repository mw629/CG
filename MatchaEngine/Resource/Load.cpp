#include "Load.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"
#include "Core/LogHandler.h"
#include <fstream>
#include <sstream>
#include <d3dx12.h>
#include "Texture.h"
#include "ModelManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


/// オブジェクトの読み込み///

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materiaData;//構築するMaterialData
	std::string line;//ファイルから読んだ一行を格納する
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパス
			materiaData.textureDilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materiaData;
}


ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	std::unique_ptr<ModelManager> objManager = std::make_unique<ModelManager>();

	if (objManager.get()->DuplicateConfirmation(directoryPath, filename)) {
		return objManager.get()->DuplicateReturn(directoryPath, filename);
	}

	ModelData modelData;
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//1行分の文字列を入れる変数
	VertexData Triangle[3]{};

	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position{};
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord{};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal{};
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			//面は三角形限定。そのほか未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されてるので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3]{};
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				Triangle[faceVertex] = { position ,texcoord ,normal };

			}
			modelData.vertices.push_back(Triangle[2]);
			modelData.vertices.push_back(Triangle[1]);
			modelData.vertices.push_back(Triangle[0]);
		}
		else if (identifier == "mtllib") {
			//makterialTemplateLibraryファイルの名前を取得する
			std::string materiaFilename;
			s >> materiaFilename;
			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す 
			modelData.material = LoadMaterialTemplateFile(directoryPath, materiaFilename);
		}
	}

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	modelData.textureIndex = texture->CreateTexture(modelData.material.textureDilePath);

	objManager.get()->SetModelList(modelData, directoryPath, filename);

	return modelData;

}

ModelData AssimpLoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	std::unique_ptr<ModelManager> objManager = std::make_unique<ModelManager>();

	if (objManager.get()->DuplicateConfirmation(directoryPath, filename)) {
		return objManager.get()->DuplicateReturn(directoryPath, filename);
	}

	Assimp::Importer impoter;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = impoter.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回非対称
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回非対応
		//ここからMeshの中身(Face)の解析を行っていく
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& postion =mesh->mVertices[element];
				aiVector3D& postion = mesh->mVertices[element];
				VertexData vertex;
				vertex.position = { postion.x,postion.y, postion.z,1.0f };
			}
		}
	}
	

}

///テクスチャの読み込み///


DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));//188

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミニマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{

	//metadataを基にResorceの設定

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);//Textureの幅
	resourceDesc.Height = UINT(metadata.height);//Textureの縦
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);;//奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;//TextureのFormat
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。１固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは二次元

	//利用するHeapの設定

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//細かい設定を行う


	//Resourceを作成

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc,//Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,//データ転送される設定
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = GraphicsDevice::CreateBufferResource(device, intermediateSize);

	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());


	//Tetureへの転送後は利用できるよう。D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;

}


namespace {
	int CPUNum = 1;
	int GPUNum = 1;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize)
{

	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriprtorSize * CPUNum);
	CPUNum++;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriprtorSize * GPUNum);
	GPUNum++;
	return handleGPU;
}
