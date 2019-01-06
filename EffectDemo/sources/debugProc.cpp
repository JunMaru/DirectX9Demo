//=============================================================================
//
// �f�o�b�O���� [debugProc.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "debugProc.h"
#include <stdio.h>
#include <tchar.h>

//*****************************************************************************
// �ÓI�����o�ϐ�
//*****************************************************************************
LPD3DXFONT CDebugProc::m_pD3DXFont;
char CDebugProc::m_aStr[LENGTH_STRING_BUFF];
bool CDebugProc::m_bDisp;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CDebugProc::CDebugProc()
{
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CDebugProc::~CDebugProc()
{
}

//=============================================================================
// ����������
//=============================================================================
HRESULT CDebugProc::Init(LPDIRECT3DDEVICE9 pDevice)
{
	// ���\���p�t�H���g��ݒ�
	if (FAILED(D3DXCreateFont(pDevice,
							18, 0, 0, 0, 
							FALSE, 
							SHIFTJIS_CHARSET,
							OUT_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH,
							"Terminal",
							&m_pD3DXFont)))
	{
		return E_FAIL;
	}

	// �����o�ϐ��̏�����
	ZeroMemory(&m_aStr[0], sizeof(m_aStr));
	m_bDisp = true;

	// ����I��
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void CDebugProc::Uninit(void)
{
	// �t�H���g�̊J��
	if (m_pD3DXFont != NULL)
	{
		m_pD3DXFont->Release();
		m_pD3DXFont = NULL;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void CDebugProc::Draw(void)
{
#ifdef _DEBUG

	// �\���t���O��OFF
	if (!m_bDisp)
	{
		return;
	}

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// �e�L�X�g�`��
	m_pD3DXFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

#endif
}

//=============================================================================
// �o�b�t�@�N���A����
//=============================================================================
void CDebugProc::ClearBuffer(void)
{
#ifdef _DEBUG
	// �o�b�t�@�N���A
	ZeroMemory(&m_aStr[0], sizeof(m_aStr));
#endif
}

//=============================================================================
// �\��������i�[����
//=============================================================================
void CDebugProc::Print(char* fmt, ...)
{
#ifdef _DEBUG

	char aStrBuf[LENGTH_STRING_BUFF] = "";
	va_list args;

	// �\���t���O��OFF
	if (!m_bDisp)
	{
		return;
	}

	// ������쐬
	va_start(args, fmt);
	_vsntprintf_s(&aStrBuf[0], LENGTH_STRING_BUFF, _TRUNCATE, fmt, args);

	// ������i�[
	_tcscat_s(&m_aStr[0],LENGTH_STRING_BUFF,&aStrBuf[0]);

	// �����̏I��
	va_end(args);

#endif
}