#include "LoadImageFormat.h"
#include "../../Graphics/Texture.h"

std::wstring ConvertToWideString(const char* narrowString) {
    // もしnarrowStringがnullptrだった場合、空のstd::wstringを返す
    if (narrowString == nullptr) {
        return L"";
    }

    // 必要なバッファサイズを取得する
    int numWideChars = MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, nullptr, 0);

    // ワイド文字列用のバッファを確保
    std::wstring wideString(numWideChars, L'\0');

    // マルチバイト文字列をワイド文字列に変換
    MultiByteToWideChar(CP_UTF8, 0, narrowString, -1, &wideString[0], numWideChars);

    return wideString;
}

// 対応しているフォーマットの画像 のロード
std::shared_ptr<SpriteResouce> LoadImageFormat::Load(const char* imgFilename)
{
	std::shared_ptr<SpriteResouce> spriteResource = std::make_unique<SpriteResouce>();
	spriteResource->SetFilePath(imgFilename);

	LoadTextureFromFile(ConvertToWideString(imgFilename).c_str(), spriteResource->GetSrvAddres(), spriteResource->GetSize());

	return std::shared_ptr<SpriteResouce>();
}
