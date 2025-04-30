#include "SystemFontLoaderBPLibrary.h"
#include "SystemFontLoaderSubsystem.h"
#include "Engine/Engine.h"
#include "HAL/FileManager.h"
#include "Fonts/SlateFontInfo.h"

bool USystemFontLoaderBPLibrary::GetSystemFontNames(TArray<FString>& OutFontNames)
{
	OutFontNames.Empty();
	if (GEngine)
	{
		if (USystemFontLoaderSubsystem* FontSubsystem = GEngine->GetEngineSubsystem<USystemFontLoaderSubsystem>())
		{
#if PLATFORM_WINDOWS
			return FontSubsystem->GetCachedFontNames(OutFontNames);
#else
			UE_LOG(LogTemp, Warning, TEXT("GetSystemFontNames: Not on Windows platform."));
			return false;
#endif
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetSystemFontNames: Failed to get SystemFontLoaderSubsystem."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetSystemFontNames: GEngine is null."));
	}
	return false;
}

bool USystemFontLoaderBPLibrary::LoadSystemFontAsSlateFontInfo(const FString& FontName, int32 FontSize, UPARAM(ref) FSlateFontInfo& OutFontInfo)
{
	OutFontInfo = FSlateFontInfo();

	if (FontName.IsEmpty() || FontSize <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("LoadSystemFontAsSlateFontInfo: Invalid FontName or FontSize."));
		return false;
	}

	if (GEngine)
	{
		if (USystemFontLoaderSubsystem* FontSubsystem = GEngine->GetEngineSubsystem<USystemFontLoaderSubsystem>())
		{
#if PLATFORM_WINDOWS
			FString FontFilePath;
			if (FontSubsystem->GetFontFilePath(FontName, FontFilePath))
			{
				// パスが見つかった場合、FSlateFontInfoを作成
				OutFontInfo = FSlateFontInfo(FontFilePath, FontSize);

				if (IFileManager::Get().FileExists(*FontFilePath))
				{
					UE_LOG(LogTemp, Verbose, TEXT("LoadSystemFontAsSlateFontInfo: Creating FSlateFontInfo for '%s' using path: '%s', Size: %d"), *FontName, *FontFilePath, FontSize);
					return true;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("LoadSystemFontAsSlateFontInfo: File path found in cache for '%s', but file does not exist: '%s'"), *FontName, *FontFilePath);
					OutFontInfo = FSlateFontInfo();
					return false;
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LoadSystemFontAsSlateFontInfo: Font '%s' not found in subsystem cache."), *FontName);
				return false;
			}
#else
			UE_LOG(LogTemp, Warning, TEXT("LoadSystemFontAsSlateFontInfo: Not on Windows platform."));
			return false;
#endif
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("LoadSystemFontAsSlateFontInfo: Failed to get SystemFontLoaderSubsystem."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("LoadSystemFontAsSlateFontInfo: GEngine is null."));
	}
	return false;
}

bool USystemFontLoaderBPLibrary::GetUserFriendlyFontName(const FString& FontCacheKey, FString& OutUserFriendlyName)
{
	OutUserFriendlyName = FontCacheKey;

	if (GEngine)
	{
		if (USystemFontLoaderSubsystem* FontSubsystem = GEngine->GetEngineSubsystem<USystemFontLoaderSubsystem>())
		{
#if PLATFORM_WINDOWS
			// サブシステムの関数を呼び出す
			return FontSubsystem->GetUserFriendlyName(FontCacheKey, OutUserFriendlyName);
#else
			UE_LOG(LogTemp, Warning, TEXT("GetUserFriendlyFontName: Not on Windows platform."));
			return false; // Windows以外では失敗扱い
#endif
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GetUserFriendlyFontName: Failed to get SystemFontLoaderSubsystem."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetUserFriendlyFontName: GEngine is null."));
	}
	OutUserFriendlyName = FontCacheKey;
	return false;
}