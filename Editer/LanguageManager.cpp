#include "LanguageManager.h"
#include <unordered_map>

EditorLanguage LanguageManager::currentLanguage_ = EditorLanguage::Japanese;

const char* LanguageManager::Tr(const char* englishText) {
    if (currentLanguage_ == EditorLanguage::English) {
        return englishText;
    }

    static const std::unordered_map<std::string, std::string> dictionary = {
        {"Settings", "設定"},
        {"Language", "言語"},
        
        {"Hierarchy", "ヒエラルキー"},
        {"Scene", "シーン"},
        {"Inspector", "インスペクター"},
        {"Name: %s", "名前: %s"},
        {"Active", "アクティブ"},
        {"No object selected.", "オブジェクトが選択されていません。"},
        
        {"Transform", "トランスフォーム"},
        {"Position", "位置"},
        {"Rotation", "回転"},
        {"Scale", "スケール"},
        {"Components", "コンポーネント"},
        {"Add Component", "コンポーネントを追加"},
        
        {"Material", "マテリアル"},
        {"Shader Selection", "シェーダー選択"},
        {"Blend Selection", "ブレンド選択"},
        {"Texture:", "テクスチャ:"},
        {"None", "なし"},
        
        {"Light Settings", "ライト設定"},
        {"Directional Light", "平行光源"},
        {"Point Light", "点光源"},
        {"Spot Light", "スポットライト"},
        {"Remove", "削除"},
        {"Directional Lights", "平行光源 (複数)"},
        {"Add Directional Light", "平行光源を追加"},
        {"Point Lights", "点光源 (複数)"},
        {"Add Point Light", "点光源を追加"},
        {"Spot Lights", "スポットライト (複数)"},
        {"Add Spot Light", "スポットライトを追加"},
        {"Color", "カラー"},
        {"Direction", "方向"},
        {"Intensity", "強度"},
        {"Radius", "半径"},
        {"Decay", "減衰"},
        {"Distance", "距離"},
        {"Angle", "角度"},
        {"Falloff", "フォールオフ"},
        
        {"Preview", "プレビュー"},
        {"Controls:", "コントロール:"},
        {"Gizmo Operation", "ギズモ操作"},
        {"Play / Pause", "再生 / 一時停止"},
        {"Clear Particles", "パーティクルをクリア"},
        {"Save As...", "名前を付けて保存..."},
        {"Load...", "読み込み..."},
        {"Save", "保存"},
        {"File operations", "ファイル操作"},
        {"Select Particle JSON file", "パーティクルのJSONファイルを選択"},
        {"Particle Editor", "パーティクルエディター"},
        {"Debug Info", "デバッグ情報"},
        {"Resources", "リソース"},
        {"Logs", "ログ"}
    };

    auto it = dictionary.find(englishText);
    if (it != dictionary.end()) {
        return it->second.c_str();
    }
    return englishText;
}
