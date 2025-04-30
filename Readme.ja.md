# SystemFontLoader for Unreal Engine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![UE Version](https://img.shields.io/badge/UE-5.5+-blue.svg)](https://www.unrealengine.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](#%E5%AF%BE%E5%BF%9C%E7%92%B0%E5%A2%83)

(Read this document in other languages: [English](README.md)) <!-- Optional: If you plan to provide a Japanese version -->

WindowsシステムにインストールされているフォントをUnreal Engineプロジェクト内で簡単に利用できるようにするプラグインです。

## 概要

このプラグインは、WindowsのレジストリとGDI APIを利用してシステムフォントの情報を取得し、Unreal EngineのBlueprintからアクセス可能な関数を提供します。これにより、実行時に動的にシステムフォントをUI等に適用することが可能になります。

## 機能

*   **システムフォント一覧の取得:** Windowsにインストールされているフォントの名前リストを取得します。
*   **FSlateFontInfoの生成:** 指定されたフォント名とサイズから、UMGやSlateで使用できる `FSlateFontInfo` を生成します。
*   **ユーザーフレンドリーなフォント名の取得:** フォントキャッシュの内部キーから、ユーザーが一般的に目にするフォント名を取得します。
*   **フォントキャッシュ:** エンジン起動時にシステムフォントの情報をスキャンし、キャッシュします。これにより、実行時のフォント情報取得が高速になります。

## 対応環境

*   **Unreal Engine:** 5.5 以降 (ソースコード内の `EngineVersion` に基づく)
*   **プラットフォーム:** **Windows (64bit)** のみ
    *   このプラグインはWindows API (レジストリ、GDI、Shell API) に依存しているため、他のプラットフォームでは動作しません。

## インストール

1.  **リリースページからダウンロード:**
    *   [GitHub リリースページ](https://github.com/yeczrtu/SystemFontLoader/releases) (<- URLは後で設定してください) から最新版のzipファイルをダウンロードします。
2.  **プロジェクトへの配置:**
    *   ダウンロードしたzipファイルを解凍します。
    *   解凍された `SystemFontLoader` フォルダを、あなたのUnreal Engineプロジェクトの `Plugins` フォルダにコピーします。(例: `MyProject/Plugins/SystemFontLoader`)
    *   `Plugins` フォルダが存在しない場合は作成してください。
3.  **プラグインの有効化:**
    *   Unreal Editorを開き、メニューの `編集` > `プラグイン` を選択します。
    *   `Installed` カテゴリ内、または検索バーで `SystemFontLoader` を検索し、表示されたプラグインの `有効` チェックボックスをオンにします。
    *   エディタの再起動を求められた場合は、再起動します。
4.  **(C++プロジェクトの場合) プロジェクトのビルド:**
    *   C++プロジェクトの場合は、Visual Studioでプロジェクトのソリューションファイルを再生成し、ビルドが必要になる場合があります。

## 使用方法

### Blueprint関数

プラグインを有効にすると、Blueprintエディタの関数リストで `System Font Loader` カテゴリの下に以下の関数が表示されます。

![Blueprint Functions](placeholder_blueprint_functions.png) <!-- 後で実際のスクリーンショットに差し替えてください -->

1.  **Get System Font Names:**
    *   利用可能なシステムフォント名のリスト（内部キャッシュキー）を取得します。通常、アプリケーションの起動時や設定画面の初期化時に呼び出します。
2.  **Get User Friendly Font Name:**
    *   `Get System Font Names` で取得した内部名から、コンボボックス等でユーザーに表示するための名前を取得します。
3.  **Load System Font As SlateFontInfo:**
    *   `Get System Font Names` で取得したフォント名と任意のフォントサイズを指定して、`FSlateFontInfo` を生成します。
    *   生成された `FSlateFontInfo` を `Set Font` ノードなどを使って `Text` Widget等に適用します。

**簡単な使用例 (Widget Blueprint):**

![Blueprint Example](placeholder_blueprint_example.png) <!-- 後で実際のスクリーンショットに差し替えてください -->

*   `Event Construct` で `Get System Font Names` を呼び出し、フォント名の配列を取得します。
*   取得した配列をループし、`Get User Friendly Font Name` で表示名を取得して `ComboBox String` に追加します。同時に内部名を別の配列などに保持しておくと良いでしょう。
*   ComboBoxの `On Selection Changed` イベントで、選択された表示名に対応する内部名を取得し、`Load System Font As SlateFontInfo` を呼び出して `FSlateFontInfo` を生成します。
*   生成した `FSlateFontInfo` を `TextBlock` などの `Set Font` ノードに渡します。

### デモレベル

プラグインコンテンツには、基本的な使用方法を示すデモレベルが含まれています。

*   **場所:** `SystemFontLoader Content / Demo.umap`
*   **内容:**
    *   UMGウィジェットを使用して、システムフォントの一覧をコンボボックスに表示します。
    *   コンボボックスでフォントを選択すると、下のテキストブロックのフォントが選択されたものに変わります。

![Demo Level Screenshot](placeholder_demo_level.png) <!-- 後で実際のスクリーンショットに差し替えてください -->

デモレベルを開くには、コンテンツブラウザの設定で「プラグインコンテンツを表示」を有効にする必要があります。

## Blueprint API 詳細

### Get System Font Names

*   **説明:** キャッシュされているシステムフォント名のリストを取得します。これらの名前は内部的なキャッシュキーであり、必ずしもユーザーフレンドリーな表示名とは限りません。
*   **出力:**
    *   `Out Font Names` (TArray<FString>): システムフォント名の配列。
*   **戻り値:** (Boolean): 成功した場合は `true`、失敗（Windows以外、サブシステム取得失敗など）した場合は `false`。

### Load System Font As SlateFontInfo

*   **説明:** `Get System Font Names` で取得したフォント名（キャッシュキー）とサイズを指定して `FSlateFontInfo` を生成します。
*   **入力:**
    *   `Font Name` (FString): `Get System Font Names` で取得したフォント名。
    *   `Font Size` (Integer): ポイント単位のフォントサイズ。
*   **出力:**
    *   `Out Font Info` (FSlateFontInfo): 生成された `FSlateFontInfo`。失敗した場合は空の `FSlateFontInfo` になります。
*   **戻り値:** (Boolean): `FSlateFontInfo` の生成に成功した場合は `true`、失敗（フォントが見つからない、ファイルが存在しないなど）した場合は `false`。

### Get User Friendly Font Name

*   **説明:** `Get System Font Names` で取得したフォント名（キャッシュキー）に対応する、ユーザーに表示するためのフォント名を取得します。GDI API を使用して取得を試みます。
*   **入力:**
    *   `Font Cache Key` (FString): `Get System Font Names` で取得したフォント名。
*   **出力:**
    *   `Out User Friendly Name` (FString): 取得されたユーザーフレンドリーなフォント名。取得に失敗した場合は、入力と同じか空文字列になる可能性があります。
*   **戻り値:** (Boolean): ユーザーフレンドリー名の取得に成功した場合は `true`、失敗した場合は `false`。

## 注意点・制限事項

*   **Windows専用:** このプラグインはWindows APIに強く依存しているため、macOS, Linux, モバイルプラットフォーム等では動作しません。クロスプラットフォーム対応のプロジェクトで使用する場合、プラットフォームチェックを行うか、このプラグインを使用しない代替手段を用意する必要があります。
*   **初回起動時のキャッシュ構築:** エンジン（またはエディタ）の起動時にシステムフォント情報をスキャンしてキャッシュを構築します。フォント数によっては、初回起動時にわずかな遅延が発生する可能性があります。
*   **レジストリへのアクセス:** フォント情報の取得のためにWindowsレジストリ (`HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts`) への読み取りアクセスが必要です。通常は問題ありませんが、特殊な権限設定の環境では動作しない可能性があります。
*   **フォントファイルの存在:** キャッシュはレジストリ情報に基づいて構築されますが、実際にフォントファイルが存在しない場合、`Load System Font As SlateFontInfo` は成功しても `FSlateFontInfo` が正しく機能しない（フォールバックフォントが表示される）可能性があります。
*   **フォント名の曖昧さ:** Windowsのフォント名は複雑（例: "Arial Bold", "Arial Italic" など）であり、`ExtractFontNameFromRegistryValue` 関数で基本的なスタイル情報（Bold, Italic, TrueTypeなど）を除去して代表名を生成しています。すべてのフォントで完璧な代表名が生成されるとは限りません。`Get User Friendly Font Name` はGDIを通じてより正確な表示名を取得しようとしますが、これも100%の保証はありません。

## ライセンス

このプラグインは [MITライセンス](LICENSE) の下で公開されています。

## 作者

[yeczrtu](https://github.com/yeczrtu)

---

**提案:**
*   スクリーンショットのプレースホルダー (`placeholder_*.png`) を実際の画像に差し替えてください。
*   リリースページのURLを設定してください。
*   もし可能であれば、簡単な英語の説明も追記すると、より多くの人に利用してもらいやすくなります。