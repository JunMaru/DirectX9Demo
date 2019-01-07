//=============================================================================
//
// テクスチャ管理処理 [textureManager.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// ワーニング対策
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "textureManager.h"
#include <string>
#include <stdio.h>
#include "Direct3DDevice.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CTextureManager::CTextureManager()
{
}

//=============================================================================
// デストラクタ
//=============================================================================
CTextureManager::~CTextureManager()
{
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CTextureManager::Init(void)
{
	CreateAllTexture();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CTextureManager::Uninit(void)
{
	auto it = m_mapTexture.begin();

	while (it != m_mapTexture.end())
	{
		LPDIRECT3DTEXTURE9 pTex = it->second;

		pTex->Release();
		pTex = NULL;

		++it;
	}
	m_mapTexture.clear();
}

//=============================================================================
// 全テクスチャ生成処理
//=============================================================================
void CTextureManager::CreateAllTexture(void)
{
	CreateTexture("\\data\\TEXTURE\\*.png");
	CreateTexture("\\data\\TEXTURE\\*.jpg");
	CreateTexture("\\data\\TEXTURE\\*.bmp");
}

//=============================================================================
// テクスチャ生成処理
//=============================================================================
void CTextureManager::CreateTexture(const char* pFilePath)
{
	WIN32_FIND_DATA findData;
	char aCurrentDir[256];

	GetCurrentDirectory(256, aCurrentDir);
	strcat(aCurrentDir, pFilePath);
	HANDLE handle = FindFirstFile(aCurrentDir, &findData);
	BOOL bLoop = TRUE;

	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	while (bLoop)
	{
		char aFileName[256] = { "data/TEXTURE/" };
		strcat(aFileName, findData.cFileName);
		LPDIRECT3DTEXTURE9 pD3DTex;

		D3DXCreateTextureFromFile(pDevice,
			aFileName,
			&pD3DTex);

		m_mapTexture.insert(std::pair<std::string, LPDIRECT3DTEXTURE9>(aFileName, pD3DTex));
		bLoop = FindNextFile(handle, &findData);
	}
}

//=============================================================================
// テクスチャ取得処理
//=============================================================================
LPDIRECT3DTEXTURE9 CTextureManager::GetTexture(const char* pFileName)
{
	if (pFileName == NULL)
	{
		return NULL;
	}

	auto it = m_mapTexture.find(std::string(pFileName));

	if (it == m_mapTexture.end())
	{
		return NULL;
	}

	return it->second;
}