// Copyright (c) 2025 yeczrtu
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Fonts/SlateFontInfo.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "SystemFontLoaderSubsystem.generated.h"

// Windows API
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windef.h> // HKEYなど基本型
#include "Windows/HideWindowsPlatformTypes.h"
#endif

UCLASS()
class SYSTEMFONTLOADER_API USystemFontLoaderSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem interface end

	/**
	 * システムフォントのキャッシュを取得します。
	 * @return フォント名(Key)とファイルパス(Value)のマップへの参照。
	 */
	const TMap<FString, FString>& GetFontCache() const;

	/**
	 * 指定されたフォント名のファイルパスをキャッシュから取得します。
	 * @param FontName 検索するフォント名。
	 * @param OutFilePath [out] 見つかった場合のファイルパス。
	 * @return パスが見つかった場合はtrue。
	 */
	bool GetFontFilePath(const FString& FontName, FString& OutFilePath) const;

	/**
	 * キャッシュされたシステムフォント名のリストを取得します。
	 * @param OutFontNames [out] 取得したフォント名の配列。
	 * @return 成功した場合はtrue、失敗した場合はfalse。
	 */
	bool GetCachedFontNames(TArray<FString>& OutFontNames) const;


	/**
	 * 指定されたキャッシュキーに対応するユーザーフレンドリーなフォント名を取得します。
	 * @param FontCacheKey フォントキャッシュのキー (代表名)。
	 * @param OutFriendlyName [out] 見つかった場合のユーザーフレンドリーな名前。
	 * @return 名前が見つかった場合はtrue。
	 */
	bool GetUserFriendlyName(const FString& FontCacheKey, FString& OutFriendlyName) const;

private:
#if PLATFORM_WINDOWS
	// フォント名とファイルパスのキャッシュ
	TMap<FString, FString> SystemFontCache;

	// レジストリをスキャンしてキャッシュを構築する内部関数
	void BuildFontCache();

	// レジストリ値の名前から代表名を抽出するヘルパー
	static FString ExtractFontNameFromRegistryValue(const FString& RegistryValueName);

	// レジストリ値のデータからファイルパスを解決するヘルパー
	static bool ResolveFontFilePath(const FString& RegistryData, FString& OutFilePath);

	// キャッシュキーからユーザーフレンドリー名を取得するGDI呼び出しヘルパー
	static bool GetGdiFriendlyFaceName(const FString& FontCacheKey, FString& OutFriendlyName);
#endif
};