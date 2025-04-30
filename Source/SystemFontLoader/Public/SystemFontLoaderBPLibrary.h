#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Fonts/SlateFontInfo.h"
#include "SystemFontLoaderBPLibrary.generated.h"

UCLASS()
class SYSTEMFONTLOADER_API USystemFontLoaderBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * システムフォントのキャッシュされた名前リストを取得します。
	 * @param OutFontNames [out] 取得したフォント名の配列。
	 * @return 成功した場合はtrue、失敗した場合はfalse。
	 */
	UFUNCTION(BlueprintCallable, Category = "System Font Loader", meta = (DisplayName = "Get System Font Names"))
	static bool GetSystemFontNames(TArray<FString>& OutFontNames);

	/**
	 * 指定されたシステムフォント名からFSlateFontInfoを生成します。
	 * キャッシュされた情報を使用します。
	 * @param FontName 使用したいシステムフォント名 (GetSystemFontNamesで取得したもの)。
	 * @param FontSize ポイント単位のフォントサイズ。
	 * @param OutFontInfo [out] 生成されたFSlateFontInfo。
	 * @return 成功した場合はtrue、フォントが見つからないかロードに失敗した場合はfalse。
	 */
	UFUNCTION(BlueprintCallable, Category = "System Font Loader", meta = (DisplayName = "Load System Font As SlateFontInfo"))
	static bool LoadSystemFontAsSlateFontInfo(const FString& FontName, int32 FontSize, UPARAM(ref) FSlateFontInfo& OutFontInfo);

	/**
	 * 指定された内部フォント名（キャッシュキー）に対応するユーザー表示用のフォント名を取得します。
	 * @param FontCacheKey GetSystemFontNamesで取得した内部名。
	 * @param OutUserFriendlyName [out] ユーザー表示用のフォント名。取得できなかった場合は入力と同じか空文字列。
	 * @return 名前の取得に成功した場合はtrue。
	 */
	UFUNCTION(BlueprintCallable, Category = "System Font Loader", meta = (DisplayName = "Get User Friendly Font Name"))
	static bool GetUserFriendlyFontName(const FString& FontCacheKey, FString& OutUserFriendlyName);
};