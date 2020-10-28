//-----------------------------------------------------------------------------
//
// PictBear Second Edition <Filter/Import/Export> Plugin Header File
//
// $Version 1.6 (last modified: 2004/05/08)
//
//                                Copyright(C) 1999-2004 by Yasuyuki Kashiwagi.
//                                                         All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef __PBPLUGIN_H
#define __PBPLUGIN_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
// PictBear で使用される画素データ
//-----------------------------------------------------------------------------
// すべての値は 0 ～ 255 で表現され、0 が最低値、255 が最大値を表わす。
// 255 に近づくほど、その影響が強くなり、RGBA(255, 255, 255, 255) で白となる。
//-----------------------------------------------------------------------------

#pragma pack(1)

typedef union tagPixel32 
{
	DWORD color; // カラー値

	struct 
	{
		BYTE b;  // 青(Blue)
		BYTE g;  // 緑(Green)
		BYTE r;  // 赤(Red)
		BYTE a;  // 不透明度(Opacity)
	} item;
} Pixel32;

#pragma pack()


///////////////////////////////////////////////////////////////////////////////
// プラグイン用定数
//-----------------------------------------------------------------------------
// 各プラグインのバージョン・種類を定義する。プラグインは PB_GetPluginInfo で、
// ここで定義されているバージョン・種類定数を返す必要がある。
//-----------------------------------------------------------------------------

//#define PBPLUGIN_VERSION		0		// ※旧バージョン互換
										// 　新バージョンでは下記定数を使用

#define PBPLUGIN_FILTER_VERSION	0x01	// フィルタプラグインのバージョン
#define PBPLUGIN_IMPORT_VERSION	0x00	// インポートプラグインのバージョン
#define PBPLUGIN_EXPORT_VERSION	0x00	// エクスポートプラグインのバージョン

#define PBPLUGIN_UNKNOWN		0x00	// 不明なプラグイン
#define PBPLUGIN_FILTER			0x01	// フィルタプラグイン
#define PBPLUGIN_IMPORT			0x02	// インポートプラグイン
#define PBPLUGIN_EXPORT			0x03	// エクスポートプラグイン


///////////////////////////////////////////////////////////////////////////////
// プラグイン用エラーコード
//-----------------------------------------------------------------------------
// プラグインは実行結果を表わすエラーコードを返す必要がある。実行関数を抜ける時
// は、ここに定義された定数を返すこと。
// 
//  F: フィルタプラグインで使用可能
//  I: インポートプラグインで使用可能
//  E: エクスポートプラグインで使用可能
//-----------------------------------------------------------------------------

#define PB_ERR_SUCCESS				0	// エラー無し(F/I/E)
#define PB_ERR_UNKNOWN				1	// 不明なエラー(F/I/E)
#define PB_ERR_FILE_NOT_FOUND		2	// ファイルが存在しない(I)
#define PB_ERR_FILE_CANNOT_CREATE	3	// ファイルが作成できない(E)
#define PB_ERR_BAD_FORMAT			4	// ファイルのフォーマットが異常(I)
#define PB_ERR_WRITE_ERROR			5	// 書き込みエラー(E)
#define PB_ERR_UNKNOWN_EXTENSION	6	// 不明な拡張子(I/E)
#define PB_ERR_CANCELED				7	// 実行取り消し(F/I/E)


///////////////////////////////////////////////////////////////////////////////
// フィルタプラグインの対応フォーマット定数
//-----------------------------------------------------------------------------
// フィルタプラグインは対応しているイメージタイプ定数を PBF_GetSupportedFormat 
// で返す必要がある。PBF_REFERENCE_IMAGE をセットすると、イメージ全体のデータを
// フィルタに渡す。
//-----------------------------------------------------------------------------

#define PBF_TYPE_MONO			0x01	// モノクロイメージ
#define PBF_TYPE_GRAYSCALE		0x02	// グレースケール
#define PBF_TYPE_INDEXCOLOR		0x04	// インデックスカラー
#define PBF_TYPE_FULLCOLOR		0x08	// フルカラー

#define PBF_REFERENCE_IMAGE		0x10	// イメージデータを参照


///////////////////////////////////////////////////////////////////////////////
// フィルタプラグインのマスク処理定数
//-----------------------------------------------------------------------------
// フィルタは処理の結果をマスク領域に応じてマスクするかどうかを選択することがで
// きる。PFilterImage::dwMaskFlag で指定すること。デフォルトは PBF_MASK_CLIENT
// である。
//-----------------------------------------------------------------------------

#define PBF_MASK_CLIENT			0x00	// マスクは PictBear が処理
#define PBF_MASK_PLUGIN			0x01	// マスクはプラグインが処理


///////////////////////////////////////////////////////////////////////////////
// フィルタプラグイン Version 0 用の進行状況を通知する関数
//-----------------------------------------------------------------------------
// PictBear Version 1.xx 系との互換用です。
//-----------------------------------------------------------------------------

typedef int (WINAPI *PBF_ProgressCallback)(int, int, long);


///////////////////////////////////////////////////////////////////////////////
// PBF_QueryClientInfo (PictBear から各種情報を得る)
//-----------------------------------------------------------------------------
// PictBear Version 1.xx 系との互換用です。
// PictBear Second Edition では PFilterImage/PProvidedFunc から取得すること。
//-----------------------------------------------------------------------------

typedef BOOL (WINAPI* PBF_QueryClientInfoProc)(DWORD, void*);

#define PBF_QUERY_VERSION		0x00	// int    : PictBear のバージョン
#define PBF_QUERY_DRAWCOLOR		0x01	// Pixel32: 描画色
#define PBF_QUERY_BKGNDCOLOR	0x03	// Pixel32: 背景色（a の値は不定）
#define PBF_QUERY_FGCOLOR		0x01	// PBF_QUERY_DRAWCOLOR と同じ
#define PBF_QUERY_BGCOLOR		0x03	// PBF_QUERY_BKGNDCOLOR と同じ

// 以下は PictBear Second Edition 以降ではサポートされていません。

// PictBear Version 1.40 以降でサポート(QUERY_VERSION では判断不可)
#define PBF_QUERY_MASK			0x04	// BYTE*  : マスク領域へのポインタ
#define PBF_QUERY_ABORT			0x05	// パラメータ無し

// PictBear Version 1.60 以降でサポート
#define PBF_QUERY_EDIT_MODE		0x06	// BYTE   : MASK==0, IMAGE==1


///////////////////////////////////////////////////////////////////////////////
// エクスポートプラグインの対応フォーマット定数
//-----------------------------------------------------------------------------
// エクスポートプラグインは対応しているイメージタイプ定数を 
// PBE_GetSupportedType で返す必要がある。レイヤーをサポートするには
// PBF_SUPPORT_LAYER を、透過色をサポートするには PBE_SUPPORT_TRANSPARENT を
// 設定してください。
//-----------------------------------------------------------------------------

#define PBE_TYPE_MONO			0x01	// モノクロイメージ
#define PBE_TYPE_GRAYSCALE		0x02	// グレースケール
#define PBE_TYPE_INDEXCOLOR		0x04	// インデックスカラー
#define PBE_TYPE_FULLCOLOR		0x08	// フルカラー

#define PBE_SUPPORT_LAYER		0x10	// レイヤーをサポート
#define PBE_SUPPORT_TRANSPARENT	0x20	// 透過色をサポート


//////////////////////////////////////////////////////////////////////////////
// レイヤーモード
//-----------------------------------------------------------------------------
// PBF_REFERENCE_IMAGE を指定して取得したイメージが保持するレイヤーの合成方法を
// 参照するには以下の定数を使用してください。
//-----------------------------------------------------------------------------

#define LM_NORMAL		0x00	// 通常
#define LM_PLUS			0x01	// 加算
#define LM_MINUS		0x02	// 減算
#define LM_TIMES		0x03	// 乗算
#define LM_SCREEN		0x04	// スクリーン
#define LM_OVERLAY		0x05	// オーバーレイ
#define LM_HARDLIGHT	0x06	// ハードライト
#define LM_SOFTLIGHT	0x07	// ソフトライト
#define LM_OVER			0x08	// 覆い焼き
#define LM_FIRE			0x09	// 焼き込み
#define LM_CMPLIGHT		0x0A	// 比較(明)
#define LM_CMPDARK		0x0B	// 比較(暗)
#define LM_ABS			0x0C	// 差の絶対値
#define LM_EXCEPT		0x0D	// 除外
#define LM_VIVIDLIGHT	0x0E	// ビビットライト
#define LM_LINEARLIGHT	0x0F	// リニアライト
#define LM_PINLIGHT		0x10	// ピンライト


//////////////////////////////////////////////////////////////////////////////
// イメージタイプ
//-----------------------------------------------------------------------------
// PFilterImage::nImageType に格納されているイメージタイプ用定数です。
//-----------------------------------------------------------------------------

#define IT_MONO			0x01	// モノクロ
#define IT_GRAYSCALE	0x02	// グレースケール
#define IT_INDEXCOLOR	0x03	// インデックスモード	
#define IT_FULLCOLOR	0x04	// フルカラー


//////////////////////////////////////////////////////////////////////////////
// レイヤー構造体
//-----------------------------------------------------------------------------
// インポートプラグインは直接アクセスせずに必ず PProvidedFunc 経由でアクセスし
// てください。
//-----------------------------------------------------------------------------

#pragma pack(1)

typedef struct tagPLayer
{
	int    cbSize;			// 構造体サイズ

	char   szName[1024];	// レイヤー名
	int    nLayerMode;		// モード(LM_???)
	int    bVisible;		// 可視状態
	int    nOpacity;		// 不透明度
	int    nLeft;			// 左オフセット
	int    nTop;			// 上オフセット
	int    nWidth;			// 幅
	int    nHeight;			// 高さ
	DWORD* pData;			// レイヤーデータ

	BYTE   Reserved[32];	// 予約済みデータ
} PLayer;

#pragma pack()


//////////////////////////////////////////////////////////////////////////////
// イメージ構造体
//-----------------------------------------------------------------------------
// インポートプラグインは直接アクセスせずに必ず PProvidedFunc 経由でアクセスし
// てください。
//-----------------------------------------------------------------------------

#pragma pack(1)

typedef struct tagPImage
{
	int     cbSize;				// 構造体サイズ

	int     nImageType;			// イメージタイプ(IT_???)
	PLayer* pLayer;				// レイヤーや画素データ
	int     nLayerCount;		// レイヤーの数 (1 ～ )
	int     nWidth;				// イメージの幅
	int     nHeight;			// イメージの高さ
	int     bEnableBg;			// 背景レイヤーを使用するか(IT_FULLCOLOR)
	int     nDPI;				// DPI
	Pixel32 rgbPalettes[256];	// パレット
	int     nPaletteSize;		// パレットサイズ(1 ～ 256)
	int     nTransparent;		// 透過色番号(0 ～ nPaletteSize-1)
	int     bEnableTrans;		// 透過色が有効か
	int     nCurrentLayer;		// カレントレイヤー(0 ～ nLayerCount-1)

	BYTE    Reserved[32];		// 予約済みデータ
} PImage;


///////////////////////////////////////////////////////////////////////////////
// フィルタ用イメージデータ構造体

typedef struct tagPFilterImage
{
	int     cbSize;			// 構造体サイズ

	int     nWidth;			// ターゲットレイヤーの幅
	int     nHeight;		// ターゲットレイヤーの高さ
	int     nTargetLayer;	// フィルタを適用するレイヤー
	DWORD*  pSrc;			// ターゲットレイヤーデータ
	DWORD*  pDst;			// 結果書き込み用データ
	BOOL    bMaskMode;		// マスクモード(false=通常モード, true=マスクモード)
	BYTE*   pMask;			// マスクデータ(マスクモード時はすべてゼロ)
	DWORD   dwMaskFlag;		// マスクの処理方法(PBF_MASK_CLIENT/PBF_MASK_PLUGIN)
	RECT    rcSelection;	// 選択範囲の領域(MEMO:オフセットを考慮する***************)
	PImage* pSrcImage;		// 参照用イメージ (PBF_REFERENCE_IMAGE が指定されていないときは NULL)
	LPSTR*  szArgs;			// スクリプトから呼ばれたときの引数
	int     nArgCount;		// 引数の数(通常呼び出し時は 0)
	DWORD	dwImageType;	// イメージモード(IT_???)

	BYTE    Reserved[32];	// 予約領域
} PFilterImage;


//////////////////////////////////////////////////////////////////////////////
// クライアントが提供する関数

typedef void  (WINAPI *PB_CreateMono)(PImage* pImage, int nWidth, int Height);
typedef void  (WINAPI *PB_CreateGrayScale)(PImage* pImage, int nWidth, int nHeight);
typedef void  (WINAPI *PB_CreateIndexColor)(PImage* pImage, int nWidth, int nHeight);
typedef void  (WINAPI *PB_CreateFullColor)(PImage* pImage, int nWidth, int nHeight, int nLayer, int bEnableBgLayer);

typedef void  (WINAPI *PB_ResizeLayer)(PImage* pImage, int nLayer, int nWidth, int nHeight, int nLeftOffset, int nTopOffset);

typedef void  (WINAPI *PB_SetData)(PImage* pImage, int nLayer, int x, int y, DWORD dwData);
typedef DWORD (WINAPI *PB_GetData)(PImage* pImage, int nLayer, int x, int y);

typedef void  (WINAPI *PB_ProgressStart)(int nMin, int nMax);
typedef int   (WINAPI *PB_ProgressSetPos)(int nPos);
typedef void  (WINAPI *PB_ProgressEnd)();

typedef void* (WINAPI *PB_Alloc)(int nSize);
typedef void  (WINAPI *PB_Free)(void* pData);

typedef BOOL  (WINAPI *PB_ChooseColor)(HWND hWnd, Pixel32* pColor);

typedef void  (WINAPI *PB_WriteInteger)(LPCSTR szSection, LPCSTR szKey, int nValue);
typedef void  (WINAPI *PB_WriteString) (LPCSTR szSection, LPCSTR szKey, LPCSTR szBuf);
typedef void  (WINAPI *PB_GetInteger)  (LPCSTR szSection, LPCSTR szKey, int* pResult, int nDefault);
typedef void  (WINAPI *PB_GetString)   (LPCSTR szSection, LPCSTR szKey, LPSTR szBuf, int nSize, LPCSTR szDefault);

typedef int   (WINAPI *PB_GetVersion)  ();
typedef void  (WINAPI *PB_GetFgColor)  (Pixel32* pColor);
typedef void  (WINAPI *PB_GetBgColor)  (Pixel32* pColor);
typedef void  (WINAPI *PB_GetLanguage) (LPSTR szLang, int nSize);


#pragma pack(1)

typedef struct tagPProvidedFunc
{
	int					    cbSize;				// 構造体サイズ

	PB_CreateMono			CreateMono;			// モノクロイメージの作成
	PB_CreateGrayScale		CreateGrayScale;	// グレースケールイメージの作成
	PB_CreateIndexColor		CreateIndexColor;	// インデックスカラーイメージの作成
	PB_CreateFullColor		CreateFullColor;	// フルカラーイメージの作成
	PB_ResizeLayer			ResizeLayer;		// レイヤーのサイズ変更
	PB_SetData				SetData;			// レイヤーにデータ書き込み
	PB_GetData				GetData;			// レイヤーからデータ読み込み
	PB_ChooseColor			ChooseColor;		// 色選択ダイアログを表示
	PB_ProgressStart		ProgressStart;		// 進行状況通知開始
	PB_ProgressSetPos		ProgressSetPos;		// 進行状況通知
	PB_ProgressEnd			ProgressEnd;		// 進行状況通知終了
	PB_Alloc				Alloc;				// メモリ確保
	PB_Free					Free;				// メモリ開放
	PB_WriteInteger			WriteInteger;		// Plugins.ini に整数書き出し
	PB_WriteString			WriteString;		// Plugins.ini に文字列書き出し
	PB_GetInteger			GetInteger;			// Plugins.ini から整数取得
	PB_GetString			GetString;			// Plugins.ini から文字列取得
	PB_GetVersion			GetVersion;			// クライアントのバージョンを取得
	PB_GetFgColor			GetFgColor;			// 描画色を取得
	PB_GetBgColor			GetBgColor;			// 背景色を取得
	PB_GetLanguage			GetLanguage;		// 使用言語を取得

	BYTE					Reserved[44];		// 予約済みデータ
} PProvidedFunc;

#pragma pack()


#ifdef __cplusplus
}
#endif


#endif // __PBPLUGIN_H
//-----------------------------------------------------------------------------
