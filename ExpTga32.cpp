// ExpTga32.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "PbPlugin.h"

#ifdef __cplusplus
#define DLLExport extern "C" __declspec (dllexport)
#else
#define DLLExport __declspec (dllexport)
#endif

#pragma comment(lib, "comdlg32.lib")

// TODO: １つのプラグインには複数の拡張子を登録できます。
//       登録する拡張子の数を定義してください。
#define SUPPORTED_TYPE_IN_PLUGIN 1

// TODO: 対応する拡張子を 拡張子|英語説明|日本語説明 で定義してください。拡張子
//       にドットはつけないで下さい。複数の拡張子を登録する場合は ';' で区切っ
//       てください。
//
//       拡張子が空の場合はエクスポートメニューに表示されます。[メールで送信]
//       のようにファイル以外に出力する場合に利用できます。
const char* g_szTypes[SUPPORTED_TYPE_IN_PLUGIN] = 
{
	"|TGA Files|TGA"
};


// TODO: サポートしているイメージタイプ、及びオプションを定義してください。 
//       PBE_Execute に渡される PImage はいずれかのデータであることがクライアン
//       トプログラムで保証されます。
//
//       レイヤーを使用しているフルカラーイメージを取り扱う場合は 
//       PBE_SUPPORT_LAYER を、透過色を利用したインデックスイメージを取り扱う場
//       合は PBE_SUPPORT_TRANSPARENT を指定してください。
DWORD g_dwFormats[SUPPORTED_TYPE_IN_PLUGIN] =
{
	PBE_TYPE_FULLCOLOR | PBE_SUPPORT_LAYER
};

// TODO: 任意のコピーライト文字列を設定してください。
const char* g_szCopyright = "Copyright (C) 2011 by tel. All Rights Reserved.";


HINSTANCE g_hInstance;


///////////////////////////////////////////////////////////////////////////////
// エントリーポイント

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) 
		g_hInstance = hinstDLL;
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// プラグイン情報取得

DLLExport WORD WINAPI PB_GetPluginInfo(LPSTR buf, int buflen)
{
	// TODO: buf にコピーライト文字列を設定し、以下の値を返してください。
	strncpy_s(buf, buflen, g_szCopyright, buflen);

	return MAKEWORD(PBPLUGIN_EXPORT, PBPLUGIN_EXPORT_VERSION);
}


///////////////////////////////////////////////////////////////////////////////
// 対応している拡張子の数を取得

DLLExport int WINAPI PBE_GetCount(void)
{
	// TODO: プラグインに対応している拡張子の数を返してください。
	return SUPPORTED_TYPE_IN_PLUGIN;
}


///////////////////////////////////////////////////////////////////////////////
// 対応しているファイルの詳細を取得

DLLExport void WINAPI PBE_GetSupportedType(int nIndex, LPSTR szType, int nSize)
{
	// TODO: 対応している拡張子を設定してください。
	strncpy_s(szType, nSize, g_szTypes[nIndex], nSize);
}


///////////////////////////////////////////////////////////////////////////////
// 対応しているイメージの種類を取得

DLLExport DWORD WINAPI PBE_GetSupportedFormat(int nIndex)
{
	// TODO: 対応しているフォーマットを設定してください。
	return g_dwFormats[nIndex];
}


#pragma pack(1)

struct TGAHeader {
	char idLength;
	char colorMapType;
	char dataTypeCode;
	short int colorMapOrigin;
	short int colorMapLength;
	char colorMapDepth;
	short int xOrigin;
	short int yOrigin;
	short width;
	short height;
	char bitsPerPixel;
	char imageDescriptor;
};

struct TGAFooter {
	int filePosition;
	int dirPosition;
	char id[17];
	char end;
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////////
// インポート実行

DLLExport int WINAPI PBE_Execute(int nIndex, HWND hWnd, LPCSTR szFileName, PImage* pImage, PProvidedFunc* pFunc)
{
	// TODO: エクスポートの内容を記述してください。
	//       PBE_Setup で設定したデータはここで読み込んでください。

	char fileName[MAX_PATH] = { '\0' };

	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrDefExt = ".tga";
	ofn.lpstrFilter = "TGA(*.tga)\0*.tga\0";
	ofn.lpstrTitle = "名前を付けて保存";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;

	if (!GetSaveFileName(&ofn)) {
		return PB_ERR_FILE_CANNOT_CREATE;
	}

	FILE* fp;
	BYTE* buf;
	int   nPaletteSize;
	int   nLineNeed;
	int   nBitCount;
	int   nImageSize;
	int   nLeft   = pImage->pLayer[0].nLeft;
	int   nTop    = pImage->pLayer[0].nTop;
	int   nWidth  = pImage->pLayer[0].nWidth  - nLeft;
	int   nHeight = pImage->pLayer[0].nHeight - nTop;
	
	if(fopen_s(&fp, fileName, "wb") != 0)
		return PB_ERR_FILE_CANNOT_CREATE;

	// 色深度を取得する。
	switch (pImage->nImageType)
	{
	case IT_MONO:
	case IT_GRAYSCALE:
	case IT_INDEXCOLOR:
		return PB_ERR_BAD_FORMAT;
	default:
		nBitCount = 32;
		nPaletteSize = 0;
	}

	// ライン分所用バイトを計算し、DWORD 整列化を行う。
	nLineNeed = nWidth * nBitCount / 8;

	// イメージサイズを計算する。
	nImageSize = nLineNeed * nHeight;

	TGAHeader header;

	header.idLength        = 0;
	header.colorMapType    = 0;
	header.dataTypeCode    = 2;
	header.colorMapOrigin  = 0;
	header.colorMapLength  = 0;
	header.colorMapDepth   = 0;
	header.xOrigin         = 0;
	header.yOrigin         = 0;
	header.width           = nWidth;
	header.height          = nHeight;
	header.bitsPerPixel    = nBitCount;
	header.imageDescriptor = 0;

	fwrite(&header, sizeof(TGAHeader), 1, fp);

	pFunc->ProgressStart(0, nHeight);

	buf = static_cast<BYTE *>(malloc(sizeof(BYTE) * nLineNeed));

	for(int i = 0; i < nHeight; i++) {
		int y = nHeight - i - 1;
		
		for(int x = 0; x < nWidth; x++) {
			Pixel32 pix;
			pix.color = pFunc->GetData(pImage, 0, nLeft + x, nTop + y);

			buf[x * 4 + 0] = pix.item.b;
			buf[x * 4 + 1] = pix.item.g;
			buf[x * 4 + 2] = pix.item.r;
			buf[x * 4 + 3] = pix.item.a;
		}

		if(fwrite(buf, sizeof(BYTE), nLineNeed, fp) != nLineNeed)
		{
			free(buf);
			fclose(fp);
			pFunc->ProgressEnd();

			return PB_ERR_WRITE_ERROR;
		}

		pFunc->ProgressSetPos(i);
	}

	free(buf);
	fclose(fp);

	pFunc->ProgressEnd();

	return PB_ERR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
// プラグインの設定

/*

DLLExport void WINAPI PBE_Setup(HWND hWnd, PProvidedFunc* pFunc)
{
	// TODO: プラグインの設定を行います。
}

*/

///////////////////////////////////////////////////////////////////////////////
// Visual C++ 用 エクスポート名の変換

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:PB_GetPluginInfo=_PB_GetPluginInfo@8")
#pragma comment(linker, "/EXPORT:PBE_GetCount=_PBE_GetCount@0")
#pragma comment(linker, "/EXPORT:PBE_GetSupportedType=_PBE_GetSupportedType@12")
#pragma comment(linker, "/EXPORT:PBE_GetSupportedFormat=_PBE_GetSupportedFormat@4")
#pragma comment(linker, "/EXPORT:PBE_Execute=_PBE_Execute@20")
//#pragma comment(linker, "/EXPORT:PBE_Setup=_PBE_Setup@8")
#endif

