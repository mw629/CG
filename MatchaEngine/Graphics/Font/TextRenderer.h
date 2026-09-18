#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>
#include <memory>
#include "DynamicFontAtlas.h"
#include "../../Math/Calculation.h"
#include "../PSO/GraphicsPipelineState.h"

namespace MatchaEngine {

struct TextVertex {
    Vector4 position; // x, y, z, w (スクリーン座標)
    Vector2 texcoord; // u, v (アトラスUV)
    Vector4 color;    // r, g, b, a
};

struct TextParamsConstantBuffer {
    Matrix4x4 wvp;          // 画面サイズ用正射影行列
    Vector4 outlineColor;   // アウトライン色
    float outlineWidth = 0.0f; // 0.0fでアウトラインなし
    float pxRange = 4.0f;
    Vector2 texSize = { 2048.0f, 2048.0f };
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    // 初期化
    // device: D3D12デバイス
    // descriptorHeap: SRV用デスクリプタヒープ
    // pipelineState: GraphicsPipelineState
    // fontPath: フォントファイルへのパス (NotoSansJP等)
    bool Initialize(ID3D12Device* device,
                    DescriptorHeap* descriptorHeap,
                    GraphicsPipelineState* pipelineState,
                    const std::string& fontPath,
                    int atlasWidth = 2048,
                    int atlasHeight = 2048);

    // 画面サイズの設定 (ウィンドウリサイズ時に呼び出し)
    void SetScreenSize(float screenWidth, float screenHeight);

    // 文字列の描画 (UTF-8)
    // text: 表示する文字列
    // pos: 描画開始座標 (左上ベース)
    // fontSize: 表示フォントサイズ (ピクセル単位、例: 32.0f)
    // color: 文字色 (RGBA)
    // enableOutline: 縁取りを有効にするか
    // outlineColor: 縁取り色
    // outlineWidth: 縁取り幅 (0.0f〜0.5f、通常 0.1f〜0.2f)
    void DrawString(ID3D12GraphicsCommandList* commandList,
                    const std::string& text,
                    const Vector2& pos,
                    float fontSize = 32.0f,
                    const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
                    bool enableOutline = false,
                    const Vector4& outlineColor = { 0.0f, 0.0f, 0.0f, 1.0f },
                    float outlineWidth = 0.15f);

    // 文字列の描画 (wide string)
    void DrawString(ID3D12GraphicsCommandList* commandList,
                    const std::wstring& text,
                    const Vector2& pos,
                    float fontSize = 32.0f,
                    const Vector4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
                    bool enableOutline = false,
                    const Vector4& outlineColor = { 0.0f, 0.0f, 0.0f, 1.0f },
                    float outlineWidth = 0.15f);

    // 文字列の描画幅と高さを計測
    Vector2 MeasureString(const std::string& text, float fontSize = 32.0f);
    Vector2 MeasureString(const std::wstring& text, float fontSize = 32.0f);

    // フレーム開始時のバッファリセット (毎フレーム呼び出し)
    void BeginFrame();

    DynamicFontAtlas* GetAtlas() { return atlas_.get(); }

private:
    static constexpr size_t kFrameCount = 2;
    static constexpr size_t kMaxVerticesPerFrame = 65536;
    static constexpr size_t kMaxIndicesPerFrame = 98304;
    static constexpr size_t kMaxDrawCallsPerFrame = 256;
    static constexpr size_t kConstantBufferAlignment = 256;

    ID3D12Device* device_ = nullptr;
    GraphicsPipelineState* pipelineState_ = nullptr;
    std::unique_ptr<DynamicFontAtlas> atlas_;

    float screenWidth_ = 1280.0f;
    float screenHeight_ = 720.0f;

    size_t currentFrameIndex_ = 0;

    // ダブルバッファリング対応GPUリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffers_[kFrameCount];
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViews_[kFrameCount]{};
    TextVertex* mappedVertices_[kFrameCount]{};
    size_t currentVertexOffset_[kFrameCount]{};

    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffers_[kFrameCount];
    D3D12_INDEX_BUFFER_VIEW indexBufferViews_[kFrameCount]{};
    uint32_t* mappedIndices_[kFrameCount]{};
    size_t currentIndexOffset_[kFrameCount]{};

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffers_[kFrameCount];
    uint8_t* mappedConstantBuffers_[kFrameCount]{};
    size_t currentDrawCall_[kFrameCount]{};
};

} // namespace MatchaEngine
