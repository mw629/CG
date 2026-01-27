#include "Load.h"
#include "Graphics/GraphicsDevice.h"
#include "Math/Calculation.h"
#include "Core/LogHandler.h"
#include <fstream>
#include <sstream>
#include <d3dx12.h>
#include "Texture.h"
#include "ModelManager.h"




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


//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
//{
//	std::unique_ptr<ModelManager> objManager = std::make_unique<ModelManager>();
//
//	if (objManager.get()->DuplicateConfirmation(directoryPath, filename)) {
//		return objManager.get()->DuplicateReturn(directoryPath, filename);
//	}
//
//	ModelData modelData;
//	std::vector<Vector4> positions;//位置
//	std::vector<Vector3> normals;//法線
//	std::vector<Vector2> texcoords;//テクスチャ座標
//	std::string line;//1行分の文字列を入れる変数
//	VertexData Triangle[3]{};
//
//	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
//	assert(file.is_open());
//
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		if (identifier == "v") {
//			Vector4 position{};
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			position.x *= -1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt") {
//			Vector2 texcoord{};
//			s >> texcoord.x >> texcoord.y;
//			texcoord.y = 1.0f - texcoord.y;
//			texcoords.push_back(texcoord);
//		}
//		else if (identifier == "vn") {
//			Vector3 normal{};
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1.0f;
//			normals.push_back(normal);
//		}
//		else if (identifier == "f") {
//			//面は三角形限定。そのほか未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				//頂点の要素へのIndexは「位置/UV/法線」で格納されてるので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3]{};
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/');//区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				Triangle[faceVertex] = { position ,texcoord ,normal };
//
//			}
//			modelData.vertices.push_back(Triangle[2]);
//			modelData.vertices.push_back(Triangle[1]);
//			modelData.vertices.push_back(Triangle[0]);
//		}
//		else if (identifier == "mtllib") {
//			//makterialTemplateLibraryファイルの名前を取得する
//			std::string materiaFilename;
//			s >> materiaFilename;
//			//基本的にobjファイルと同一改装にmtlは存在させるので、ディレクトリ名とファイル名を渡す 
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materiaFilename);
//		}
//	}
//
//	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
//
//	modelData.textureIndex = texture->CreateTexture(modelData.material.textureDilePath);
//
//	objManager.get()->SetModelList(modelData, directoryPath, filename);
//
//	return modelData;
//
//}

ModelData AssimpLoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	std::unique_ptr<ModelManager> objManager = std::make_unique<ModelManager>();

	if (objManager.get()->DuplicateConfirmation(directoryPath, filename)) {
		return objManager.get()->DuplicateReturn(directoryPath, filename);
	}

	ModelData modelData;

	Assimp::Importer impoter;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = impoter.ReadFile(filePath.c_str(),
		aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		//assert(mesh->HasNormals());//法線がないMeshは今回非対称
		//assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回非対応
		modelData.vertices.resize(mesh->mNumVertices);

		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcord.x,texcord.y };
		}
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D translate;
			aiVector3D scale;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(
				Vector3{ -translate.x, translate.y, translate.z }, // translate
				Vector3{ scale.x, scale.y, scale.z },              // scale
				Quaternion{ rotate.x, -rotate.y, -rotate.z, rotate.w }); // rotate
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& fence = mesh->mFaces[faceIndex];
			assert(fence.mNumIndices == 3);

			for (uint32_t element = 0; element < fence.mNumIndices; ++element) {
				uint32_t vertexIndex = fence.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}

		}
		//materialを解析する
		for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
			aiMaterial* material = scene->mMaterials[materialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
				aiString textureFilePath;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
				modelData.material.textureDilePath = directoryPath + "/" + textureFilePath.C_Str();
			}
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	modelData.textureIndex = texture->CreateTexture(modelData.material.textureDilePath);

	objManager.get()->SetModelList(modelData, directoryPath, filename);

	return modelData;

}

Node ReadNode(aiNode* node)
{
	//Node result;
	//aiMatrix4x4 aiLocalMatrix = node->mTransformation;//nodeのLocalMatrixを取得
	//aiLocalMatrix.Transpose();//列ベクトル形式を行ベクトル形式に転置
	//for (int i = 0; i < 4; i++) {
	//	for (int j = 0; j < 4; j++) {
	//		result.localMatrix.m[i][j] = aiLocalMatrix[i][j];//ほかの要素も同様に
	//	}
	//}
	Node result;

	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate); // assimpの行列からSRTを抽出する関数を利用
	result.transform.scale = { scale.x, scale.y, scale.z }; // Scaleはそのまま
	result.transform.rotate = { rotate.x, -rotate.y, -rotate.z, rotate.w }; // x軸を反転、さらに回転方向が逆なので軸を反転させる
	result.transform.translate = { -translate.x, translate.y, translate.z }; // x軸を反転
	result.localMatrix = MakeAffineMatrix(result.transform.translate, result.transform.scale, result.transform.rotate);

	result.name = node->mName.C_Str(); // Nodeの名前
	result.children.resize(node->mNumChildren); // 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

Animation LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Animation animation;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションだけ採用。複数対応させるべき
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間単位を秒に変換

	//AnimationNodeを解析
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		aiNodeAnim* AnimationNodeAssimp = animationAssimp->mChannels[channelIndex];
		AnimationNode& AnimationNode = animation.AnimationNodes[AnimationNodeAssimp->mNodeName.C_Str()];

		//Translate
		for (uint32_t keyIndex = 0; keyIndex < AnimationNodeAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = AnimationNodeAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手→左手
			AnimationNode.translate.push_back(keyframe);
		}
		//Rotate
		for (uint32_t keyIndex = 0; keyIndex < AnimationNodeAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = AnimationNodeAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w };
			AnimationNode.rotate.push_back(keyframe);
		}
		//Scale
		for (uint32_t keyIndex = 0; keyIndex < AnimationNodeAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = AnimationNodeAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			AnimationNode.scale.push_back(keyframe);
		}
	}
	return animation;
}

///テクスチャの読み込み///


DirectX::ScratchImage LoadTexture(const std::string& filePath) {

	//テクスチャファイルを読み込んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

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
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = GraphicsDevice::CreateBufferResource(intermediateSize);

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
