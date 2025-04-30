#include "SystemFontLoaderSubsystem.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Containers/Set.h"
#include "Logging/LogMacros.h"

// Windows API
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>
#include <shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shell32.lib")
#endif

// サブシステムの初期化
void USystemFontLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if PLATFORM_WINDOWS
	BuildFontCache(); // サブシステム初期化時にキャッシュを構築
#else
	UE_LOG(LogTemp, Log, TEXT("SystemFontLoaderSubsystem: Not on Windows platform, skipping font cache build."));
#endif
}

// サブシステムの破棄
void USystemFontLoaderSubsystem::Deinitialize()
{
#if PLATFORM_WINDOWS
	SystemFontCache.Empty(); // キャッシュをクリア
#endif
	Super::Deinitialize();
}

// キャッシュへのアクセス
const TMap<FString, FString>& USystemFontLoaderSubsystem::GetFontCache() const
{
#if PLATFORM_WINDOWS
	return SystemFontCache;
#else
	static const TMap<FString, FString> EmptyCache; // 空を返す
	return EmptyCache;
#endif
}

// フォントファイルパスの取得
bool USystemFontLoaderSubsystem::GetFontFilePath(const FString& FontName, FString& OutFilePath) const
{
#if PLATFORM_WINDOWS
	const FString* FoundPath = SystemFontCache.Find(FontName);
	if (FoundPath)
	{
		OutFilePath = *FoundPath;
		return true;
	}
#endif
	return false;
}

// キャッシュされたフォント名リストの取得
bool USystemFontLoaderSubsystem::GetCachedFontNames(TArray<FString>& OutFontNames) const
{
#if PLATFORM_WINDOWS
	SystemFontCache.GetKeys(OutFontNames);
	OutFontNames.Sort(); // 名前順にソート
	return OutFontNames.Num() > 0;
#else
	OutFontNames.Empty();
	return false;
#endif
}


#if PLATFORM_WINDOWS
FString USystemFontLoaderSubsystem::ExtractFontNameFromRegistryValue(const FString& RegistryValueName)
{
	FString BaseName = RegistryValueName;

	const TArray<FString> SuffixesToRemove = {
		TEXT(" (TrueType)"), TEXT(" (OpenType)"), TEXT(" (TrueType Collection)"), TEXT(" (TTC)"),
		TEXT(" & Italic"), TEXT(" & Bold"), TEXT(" & Bold Italic"),
		TEXT(" Regular"), TEXT(" Normal"), TEXT(" Medium")
	};

	for (const FString& Suffix : SuffixesToRemove)
	{
		int32 AmpPos = -1;
		if (BaseName.FindChar(TEXT('&'), AmpPos))
		{
			BaseName = BaseName.Left(AmpPos).TrimEnd();
		}
		if (BaseName.EndsWith(Suffix, ESearchCase::IgnoreCase))
		{
			BaseName = BaseName.LeftChop(Suffix.Len());
		}
	}

	BaseName.TrimStartAndEndInline();
	return BaseName;
}

bool USystemFontLoaderSubsystem::ResolveFontFilePath(const FString& RegistryData, FString& OutFilePath)
{
	FString FontFileNamePart = RegistryData;
	int32 CommaPos = -1;
	if (RegistryData.FindChar(TEXT(','), CommaPos))
	{
		FontFileNamePart = RegistryData.Left(CommaPos);
	}
	FontFileNamePart.TrimStartAndEndInline();

	if (FontFileNamePart.IsEmpty())	return false;

	WCHAR szFontsPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_FONTS, NULL, 0, szFontsPath)))
	{
		FString FontsDir(szFontsPath);
		FString FullPath = FontsDir + TEXT("\\") + FontFileNamePart;
		FPaths::NormalizeFilename(FullPath);

		FString DirectPath = FontFileNamePart;
		FPaths::NormalizeFilename(DirectPath);

		if (IFileManager::Get().FileExists(*FullPath)) {
			OutFilePath = FullPath;
			return true;
		}
		else if (IFileManager::Get().FileExists(*DirectPath)) {
			OutFilePath = DirectPath;
			return true;
		}
		else {
			UE_LOG(LogTemp, VeryVerbose, TEXT("ResolveFontFilePath: File not found at '%s' or '%s' (RegistryData: '%s')"), *FullPath, *DirectPath, *RegistryData);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ResolveFontFilePath: Failed to get Windows Fonts folder path. Checking direct path only."));
		FString DirectPathCheck = FontFileNamePart;
		FPaths::NormalizeFilename(DirectPathCheck);
		if (IFileManager::Get().FileExists(*DirectPathCheck)) {
			OutFilePath = DirectPathCheck;
			return true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ResolveFontFilePath: Fonts folder unavailable and direct path '%s' not found."), *DirectPathCheck);
			return false;
		}
	}
}

void USystemFontLoaderSubsystem::BuildFontCache()
{
	UE_LOG(LogTemp, Log, TEXT("SystemFontLoaderSubsystem: Building system font cache..."));
	SystemFontCache.Empty();
	TSet<FString> AddedFontFiles;

	const FString FontRegistryPath = TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");
	HKEY hKey;

	LONG lOpenResult = RegOpenKeyExW(HKEY_LOCAL_MACHINE, *FontRegistryPath, 0, KEY_READ, &hKey);
	if (lOpenResult != ERROR_SUCCESS)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildFontCache: Failed to open registry key: HKLM\\%s. Error code: %d"), *FontRegistryPath, lOpenResult);
		return;
	}

	DWORD dwIndex = 0;
	WCHAR szValueName[512];
	WCHAR szData[MAX_PATH * 2];
	DWORD dwValueNameSize = _countof(szValueName);
	DWORD dwDataSize = sizeof(szData);
	DWORD dwType;
	LONG lEnumResult = ERROR_SUCCESS;

	while ((lEnumResult = RegEnumValueW(hKey, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, (LPBYTE)szData, &dwDataSize)) == ERROR_SUCCESS)
	{
		// ループの先頭でサイズをリセット
		dwValueNameSize = _countof(szValueName);
		dwDataSize = sizeof(szData);

		if (dwType == REG_SZ)
		{
			FString CurrentValueName(szValueName);
			FString CurrentRegistryData(szData);
			FString FilePath;

			if (ResolveFontFilePath(CurrentRegistryData, FilePath))
			{
				bool bAlreadyAdded = false;
				AddedFontFiles.Add(FilePath, &bAlreadyAdded);

				if (!bAlreadyAdded)
				{
					FString DisplayName = ExtractFontNameFromRegistryValue(CurrentValueName);
					if (!DisplayName.IsEmpty() && !SystemFontCache.Contains(DisplayName))
					{
						SystemFontCache.Add(DisplayName, FilePath);
						UE_LOG(LogTemp, VeryVerbose, TEXT("Added to cache: '%s' -> '%s' (From Registry: '%s')"), *DisplayName, *FilePath, *CurrentValueName);
					}
				}
			}
		}
		dwIndex++;
	}

	if (lEnumResult != ERROR_NO_MORE_ITEMS) {
		UE_LOG(LogTemp, Error, TEXT("BuildFontCache: Registry enumeration stopped unexpectedly. Error code: %d"), lEnumResult);
	}

	RegCloseKey(hKey);
	UE_LOG(LogTemp, Log, TEXT("SystemFontLoaderSubsystem: System font cache built. Found %d unique font entries."), SystemFontCache.Num());
}
#endif // PLATFORM_WINDOWS

#if PLATFORM_WINDOWS
bool USystemFontLoaderSubsystem::GetGdiFriendlyFaceName(const FString& FontCacheKey, FString& OutFriendlyName)
{
	OutFriendlyName.Empty();
	bool bSuccess = false;

	HDC hdc = GetDC(NULL);
	if (!hdc)
	{
		UE_LOG(LogTemp, Error, TEXT("GetGdiFriendlyFaceName: Failed to get DC."));
		return false;
	}

	LOGFONTW lf;
	ZeroMemory(&lf, sizeof(lf));

	const int32 PointSize = 12;
	lf.lfHeight = -MulDiv(PointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lf.lfWeight = FW_NORMAL; // 標準の太さ
	lf.lfCharSet = DEFAULT_CHARSET; // または SHIFTJIS_CHARSET など、状況に応じて
	lf.lfOutPrecision = OUT_TT_PRECIS; // TrueType を優先
	lf.lfQuality = CLEARTYPE_QUALITY; // または DEFAULT_QUALITY

	// lfFaceName にキャッシュキー (代表名) を設定
	// LF_FACESIZE は通常 32 なので、長すぎる名前は切り捨てられる可能性がある
	FCString::Strncpy(lf.lfFaceName, *FontCacheKey, LF_FACESIZE);
	lf.lfFaceName[LF_FACESIZE - 1] = L'\0';

	HFONT hFont = CreateFontIndirectW(&lf);

	if (hFont)
	{
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
		if (hOldFont)
		{
			WCHAR wszFaceName[LF_FACESIZE];
			if (GetTextFaceW(hdc, LF_FACESIZE, wszFaceName) > 0)
			{
				OutFriendlyName = FString(wszFaceName);
				if (!OutFriendlyName.IsEmpty())
				{
					UE_LOG(LogTemp, VeryVerbose, TEXT("GetGdiFriendlyFaceName: Input='%s', Output='%s'"), *FontCacheKey, *OutFriendlyName);
					bSuccess = true;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("GetGdiFriendlyFaceName: GetTextFaceW returned an empty string for key '%s'."), *FontCacheKey);
				}

			}
			else {
				DWORD dwError = GetLastError();
				UE_LOG(LogTemp, Error, TEXT("GetGdiFriendlyFaceName: GetTextFaceW failed for key '%s'. Error code: %d"), *FontCacheKey, dwError);
			}
			SelectObject(hdc, hOldFont);
		}
		else
		{
			DWORD dwError = GetLastError();
			UE_LOG(LogTemp, Error, TEXT("GetGdiFriendlyFaceName: SelectObject failed for key '%s'. Error code: %d"), *FontCacheKey, dwError);
		}
		DeleteObject(hFont); // 作成したフォントを削除
	}
	else
	{
		DWORD dwError = GetLastError();
		UE_LOG(LogTemp, Error, TEXT("GetGdiFriendlyFaceName: CreateFontIndirectW failed for key '%s'. Error code: %d"), *FontCacheKey, dwError);
	}

	ReleaseDC(NULL, hdc);
	return bSuccess;
}
#endif // PLATFORM_WINDOWS


bool USystemFontLoaderSubsystem::GetUserFriendlyName(const FString& FontCacheKey, FString& OutFriendlyName) const
{
#if PLATFORM_WINDOWS
	if (SystemFontCache.Contains(FontCacheKey))
	{
		if (GetGdiFriendlyFaceName(FontCacheKey, OutFriendlyName))
		{
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GetUserFriendlyName: Failed to get GDI friendly name for '%s'. Returning false."), *FontCacheKey);
			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetUserFriendlyName: Font key '%s' not found in cache."), *FontCacheKey);
		OutFriendlyName.Empty();
		return false;
	}
#else
	OutFriendlyName.Empty();
	return false;
#endif
}