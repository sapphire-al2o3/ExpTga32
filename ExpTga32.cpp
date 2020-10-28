// ExpTga32.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
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

// TODO: �P�̃v���O�C���ɂ͕����̊g���q��o�^�ł��܂��B
//       �o�^����g���q�̐����`���Ă��������B
#define SUPPORTED_TYPE_IN_PLUGIN 1

// TODO: �Ή�����g���q�� �g���q|�p�����|���{����� �Œ�`���Ă��������B�g���q
//       �Ƀh�b�g�͂��Ȃ��ŉ������B�����̊g���q��o�^����ꍇ�� ';' �ŋ�؂�
//       �Ă��������B
//
//       �g���q����̏ꍇ�̓G�N�X�|�[�g���j���[�ɕ\������܂��B[���[���ő��M]
//       �̂悤�Ƀt�@�C���ȊO�ɏo�͂���ꍇ�ɗ��p�ł��܂��B
const char* g_szTypes[SUPPORTED_TYPE_IN_PLUGIN] = 
{
	"|TGA Files|TGA"
};


// TODO: �T�|�[�g���Ă���C���[�W�^�C�v�A�y�уI�v�V�������`���Ă��������B 
//       PBE_Execute �ɓn����� PImage �͂����ꂩ�̃f�[�^�ł��邱�Ƃ��N���C�A��
//       �g�v���O�����ŕۏ؂���܂��B
//
//       ���C���[���g�p���Ă���t���J���[�C���[�W����舵���ꍇ�� 
//       PBE_SUPPORT_LAYER ���A���ߐF�𗘗p�����C���f�b�N�X�C���[�W����舵����
//       ���� PBE_SUPPORT_TRANSPARENT ���w�肵�Ă��������B
DWORD g_dwFormats[SUPPORTED_TYPE_IN_PLUGIN] =
{
	PBE_TYPE_FULLCOLOR | PBE_SUPPORT_LAYER
};

// TODO: �C�ӂ̃R�s�[���C�g�������ݒ肵�Ă��������B
const char* g_szCopyright = "Copyright (C) 2011 by tel. All Rights Reserved.";


HINSTANCE g_hInstance;


///////////////////////////////////////////////////////////////////////////////
// �G���g���[�|�C���g

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH) 
		g_hInstance = hinstDLL;
	
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// �v���O�C�����擾

DLLExport WORD WINAPI PB_GetPluginInfo(LPSTR buf, int buflen)
{
	// TODO: buf �ɃR�s�[���C�g�������ݒ肵�A�ȉ��̒l��Ԃ��Ă��������B
	strncpy_s(buf, buflen, g_szCopyright, buflen);

	return MAKEWORD(PBPLUGIN_EXPORT, PBPLUGIN_EXPORT_VERSION);
}


///////////////////////////////////////////////////////////////////////////////
// �Ή����Ă���g���q�̐����擾

DLLExport int WINAPI PBE_GetCount(void)
{
	// TODO: �v���O�C���ɑΉ����Ă���g���q�̐���Ԃ��Ă��������B
	return SUPPORTED_TYPE_IN_PLUGIN;
}


///////////////////////////////////////////////////////////////////////////////
// �Ή����Ă���t�@�C���̏ڍׂ��擾

DLLExport void WINAPI PBE_GetSupportedType(int nIndex, LPSTR szType, int nSize)
{
	// TODO: �Ή����Ă���g���q��ݒ肵�Ă��������B
	strncpy_s(szType, nSize, g_szTypes[nIndex], nSize);
}


///////////////////////////////////////////////////////////////////////////////
// �Ή����Ă���C���[�W�̎�ނ��擾

DLLExport DWORD WINAPI PBE_GetSupportedFormat(int nIndex)
{
	// TODO: �Ή����Ă���t�H�[�}�b�g��ݒ肵�Ă��������B
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
// �C���|�[�g���s

DLLExport int WINAPI PBE_Execute(int nIndex, HWND hWnd, LPCSTR szFileName, PImage* pImage, PProvidedFunc* pFunc)
{
	// TODO: �G�N�X�|�[�g�̓��e���L�q���Ă��������B
	//       PBE_Setup �Őݒ肵���f�[�^�͂����œǂݍ���ł��������B

	char fileName[MAX_PATH] = { '\0' };

	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrDefExt = ".tga";
	ofn.lpstrFilter = "TGA(*.tga)\0*.tga\0";
	ofn.lpstrTitle = "���O��t���ĕۑ�";
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

	// �F�[�x���擾����B
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

	// ���C�������p�o�C�g���v�Z���ADWORD ���񉻂��s���B
	nLineNeed = nWidth * nBitCount / 8;

	// �C���[�W�T�C�Y���v�Z����B
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
// �v���O�C���̐ݒ�

/*

DLLExport void WINAPI PBE_Setup(HWND hWnd, PProvidedFunc* pFunc)
{
	// TODO: �v���O�C���̐ݒ���s���܂��B
}

*/

///////////////////////////////////////////////////////////////////////////////
// Visual C++ �p �G�N�X�|�[�g���̕ϊ�

#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:PB_GetPluginInfo=_PB_GetPluginInfo@8")
#pragma comment(linker, "/EXPORT:PBE_GetCount=_PBE_GetCount@0")
#pragma comment(linker, "/EXPORT:PBE_GetSupportedType=_PBE_GetSupportedType@12")
#pragma comment(linker, "/EXPORT:PBE_GetSupportedFormat=_PBE_GetSupportedFormat@4")
#pragma comment(linker, "/EXPORT:PBE_Execute=_PBE_Execute@20")
//#pragma comment(linker, "/EXPORT:PBE_Setup=_PBE_Setup@8")
#endif

