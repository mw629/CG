
#pragma once

#include <xaudio2.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <cassert>

// 必要なライブラリをリンク
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

/**
 * Audio
 * Media FoundationとXAudio2を使用したオーディオ管理クラス
 * 静的クラスとして実装されており、どこからでもアクセス可能
 */
class Audio {

private:
    // 音声データを格納する構造体
    struct SoundData {
        WAVEFORMATEX wfex{};
        BYTE* pBuffer = nullptr;
        unsigned int bufferSize = 0;
        IXAudio2SourceVoice* pSourceVoice = nullptr;
    };

    // 静的メンバ変数
    static Microsoft::WRL::ComPtr<IXAudio2> sXaudio2;
    static IXAudio2MasteringVoice* sMasterVoice;
    static std::vector<SoundData> sSoundData;

public:
    ///  オーディオエンジンを初期化します。プログラム開始時に一度だけ呼び出してください。
    static void Initialize();

    ///  オーディオエンジンを終了します。プログラム終了時に一度だけ呼び出してください。
    static void Finalize();

    /// 音声ファイルを読み込み、再生準備をします。
    /// filePath ファイルパス (例: "resource/sound.mp3")
    ///  サウンドハンドル（整数値）。再生時に使用します。
    static int Load(const std::string& filePath);

    ///  読み込んだ音声データをメモリから解放します。
    ///  soundHandle Load時に取得したハンドル
    static void Unload(int soundHandle);

    ///  音声を再生します。
    ///  soundHandle Load時に取得したハンドル
    ///  loop ループ再生するか (true: ループする, false: 一度だけ再生)
    ///  volume 音量 (0.0f - 1.0f)
    ///  再生に使用しているボイスのインスタンスID。現在未使用。
    static int Play(int soundHandle, bool loop = false, float volume = 1.0f);

    ///  再生中の音声を停止します。
    ///  soundHandle Load時に取得したハンドル
    static void Stop(int soundHandle);

};