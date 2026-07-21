#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gDepthTexture : register(t1);
sampler gSampler : register(s0);

ConstantBuffer<PostEffectData> gPostEffect : register(b0);

// --- 簡易的な2Dノイズ関数 ---
float hash(float2 p) {
    p = frac(p * 0.3183099 + 0.1);
    p *= 17.0;
    return frac(p.x * p.y * (p.x + p.y));
}

float noise(float2 x) {
    float2 i = floor(x);
    float2 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(hash(i + float2(0.0, 0.0)), hash(i + float2(1.0, 0.0)), f.x),
                lerp(hash(i + float2(0.0, 1.0)), hash(i + float2(1.0, 1.0)), f.x), f.y);
}

// フラクタルノイズ（FBM）で自然なムラを作る
float fbm(float2 p) {
    float f = 0.0;
    f += 0.5000 * noise(p); p = p * 2.02;
    f += 0.2500 * noise(p); p = p * 2.03;
    f += 0.1250 * noise(p); p = p * 2.01;
    f += 0.0625 * noise(p);
    return f;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
  // 1. 元画像のカラーと深度値（Depth）を取得
    float32_t4 sceneColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t depth = gDepthTexture.Sample(gSampler, input.texcoord);

    // 2. デプス値をカメラからの実際の距離 (Linear Distance) に変換
    // UV座標からNDC(正規化デバイス座標)を求める [-1, 1] の範囲
    float32_t ndcX = input.texcoord.x * 2.0f - 1.0f;
    float32_t ndcY = 1.0f - input.texcoord.y * 2.0f;
    
    // ProjectionInverseを使ってView座標系に逆変換し、View空間での座標を求める
    float32_t4 viewSpacePos = mul(float32_t4(ndcX, ndcY, depth, 1.0f), gPostEffect.ProjectionInverse);
    viewSpacePos /= viewSpacePos.w; // 座標を正規化（wで除算）
    float32_t linearDistance = viewSpacePos.z;

    // --- 【距離によるフォグの開始位置調整 (Distance Offset)】 ---
    // gPostEffect.value2 を「フォグが発生し始める距離」として使用
    float32_t fogDistance = max(0.0f, linearDistance - gPostEffect.value2);

    // --- 【揺らぎの計算 (Swaying Fog)】 ---
    // 空間座標と時間を組み合わせてサイン波による揺らぎを作る
    float32_t sway = sin(viewSpacePos.x * 0.5f + gPostEffect.time * 2.0f) * 0.5f 
                   + cos(viewSpacePos.z * 0.5f + gPostEffect.time * 1.5f) * 0.5f;

    // --- 【高さによるフォグの調整 (Height Fog)】 ---
    // viewSpacePos.y はカメラより下がマイナス、上がプラスになります。
    // sway を加算することで、フォグの高さがゆらゆらと動くようになります。
    // ratio を使って「高さによるフォグの消え具合」をコントロールします。
    // ratioが0.0なら高さに関係なく均一にフォグがかかり、1.0に近づくほど上空のフォグが薄くなります。
    float32_t heightFog = saturate((viewSpacePos.y + sway) * -0.5f + 1.0f);
    heightFog = lerp(1.0f, heightFog, gPostEffect.ratio); // ratioスライダーで高さ影響をON/OFF
    
    // --- 【まばらなフォグのムラ (Patchiness)】 ---
    // 奥に向かって流れるようにZ座標に時間を足す（引く）
    float2 noisePos = float2(viewSpacePos.x, viewSpacePos.z);
    noisePos.y -= gPostEffect.time * 10.0f; // Z方向（奥）へ流れるスピード
    noisePos.x += gPostEffect.time * 2.0f;  // X方向（横）へ流れるスピード

    // FBMノイズを使って、周期的ではない自然でランダムなムラを作る
    float32_t patchiness = fbm(noisePos * 0.1f); 
    
    // patchiness (0.0~1.0) を使って、フォグの「濃度(density)」を局所的に変化させる
    // ノイズの値が大きいところは濃く、小さいところは薄くなるように調整
    // 完全にゼロにならないように最低値を足す
    float32_t density = gPostEffect.value1 * (patchiness * 3.0f + 0.1f);

    // 3. 指数関数モデルでフォグ係数を計算 (0.0 ～ 1.0)
    float32_t fogFactor = exp(-fogDistance * density * heightFog);
    fogFactor = saturate(fogFactor);

    // 4. 元の色とフォグカラーを線形補間
    // gPostEffect.color に設定された色を使用します
    float32_t3 finalColor = lerp(gPostEffect.color, sceneColor.rgb, fogFactor);

    output.color = float32_t4(finalColor, sceneColor.a);
    
    return output;
}
