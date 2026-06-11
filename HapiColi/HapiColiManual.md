# HapiColi 利用マニュアル

## 初めに
HapiColiを使用していただきありがとうございます。
このドキュメントは、HapiColiの使用方法を説明するためのものです。

HapiColiは、判定の際の「ハッピー（期待する結果）」と「当たり判定（コリジョン）」を組み合わせた名前です。
本ツールはゲームエンジンに依存せず、あらゆるC++環境（UnityのNativeプラグイン、Unreal Engine、独自エンジン等）で利用可能な当たり判定デバッグ・解析ツールです。

## 概要

HapiColiは以下の機能を提供します。
1. **記録 (Recorder)**: ゲーム中のオブジェクトの座標・速度・当たり判定結果をフレームごとに記録します。
2. **解析 (Analyzer)**: 記録したデータから、期待した判定（Happy）と期待しない判定（Unhappy）の統計を算出します。
3. **提案 (Optimizer)**: 解析結果をもとに、判定抜けの改善案（コライダーの拡大、CCDの導入など）を提示します。

※本ツールを利用するには、[ImGui](https://github.com/ocornut/imgui) がプロジェクトに組み込まれている必要があります。

---

## 組み込み手順（エンジンのコードへの統合）

HapiColiはエンジン非依存のため、ユーザーのゲームループに数行のコードを追加するだけで動作します。

### 1. 初期化と更新
ゲームの初期化処理と、毎フレームの描画処理（ImGuiの描画タイミング）で以下を呼び出します。

```cpp
#include "HapiColi/HapiColi.h"

// 初期化時
HapiColi::HapiColi::GetInstance().Initialize();

// 毎フレーム（ImGui描画中）
HapiColi::HapiColi::GetInstance().Update();
```

### 2. データの記録
フレームの開始時、各オブジェクトの更新・判定後、フレームの終了時に以下のAPIを呼び出してデータを送信します。

```cpp
// フレームの最初
HapiColi::HapiColi::GetInstance().BeginFrame(deltaTime);

// 各オブジェクトの情報を送信
for (auto& obj : gameObjects) {
    HapiColi::ObjectData data;
    data.id = obj->GetId();
    data.type = obj->GetType();
    
    data.position = HapiColi::Vector3(obj->pos.x, obj->pos.y, obj->pos.z);
    data.velocity = HapiColi::Vector3(obj->vel.x, obj->vel.y, obj->vel.z);
    
    data.collision.isColliding = obj->IsColliding();
    if (obj->IsColliding()) {
        data.collision.collidedWithId = obj->GetCollidedObjectId();
    }
    
    HapiColi::HapiColi::GetInstance().RecordObject(data);
}

// フレームの最後
HapiColi::HapiColi::GetInstance().EndFrame();
```

### 3. デバッグ描画（対象オブジェクトのハイライト）
ゲームの描画ループ（ImGui描画中または前）でビュープロジェクション行列を渡すことで、現在UIで選択されているSubjectやTargetのワイヤーフレームと名前を画面上にハイライト表示できます。この機能はHapiColi内部のImGuiを用いて完結しています。

```cpp
// 行列の先頭ポインタ（float[16]）を渡す
HapiColi::HapiColi::GetInstance().DrawDebug3D(&viewProjMatrix.m[0][0]);
```

---

## ツール画面（ImGui）の使い方

`Update()` を呼び出すと、画面上に「HapiColi Debugger」ウィンドウが表示されます。

### 1. Recorder（記録機能）
- **Start Recording / Stop Recording**: データの記録を開始・停止します。
- **Clear Data**: 記録したデータをリセットします。
- **Add Target ID**: 記録対象のオブジェクトIDを指定して絞り込むことができます（未指定の場合は全オブジェクトを記録します）。

### 2. Analyzer（解析機能）
期待する動作（ルール）を定義して、記録したデータの成否を判定します。

- **Subject ID**: テスト対象のオブジェクトID（例: `Player`）
- **Expected Hit / Expected No Hit**: 期待する動作（当たるべきか、当たらないべきか）
- **Target ID**: 相手のオブジェクトID（例: `Ground` または `Wall`）
- **Add Rule**: ルールを追加します。
- **Run Analysis**: 追加したルールに基づいて、記録データを解析します。結果（Happy/Unhappyの回数と成功率）が表示されます。

### 3. Optimizer（改善提案機能）
- **Generate Suggestions**: 解析結果で「Unhappy」が発生した場合、原因を推測して最適化の提案を表示します。（例: 「コライダーを縮小してください」「Raycastを導入してください」など）

### 4. File I/O
- **Save JSON**: 現在記録されているデータを `hapicoli_log.json` としてファイルに書き出します。
