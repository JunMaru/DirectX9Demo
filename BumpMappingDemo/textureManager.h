//=============================================================================
//
// �e�N�X�`���Ǘ����� [textureManager.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h�K�[�h
//*****************************************************************************
#ifndef _TEXTUREMANAGER_H_
#define _TEXTUREMANAGER_H_

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "d3dx9.h"
#include <map>
#include <string>

//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	HRESULT Init(void);
	void Uninit(void);

	LPDIRECT3DTEXTURE9 GetTexture(const char* pFileName);

private:
	void CreateAllTexture(void);
	void CreateTexture(const char* pFilePath);
	
	std::map<std::string, LPDIRECT3DTEXTURE9> m_mapTexture;
};

#endif