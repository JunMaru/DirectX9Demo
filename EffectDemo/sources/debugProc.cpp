//=============================================================================
//
// デバッグ処理 [debugProc.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "debugProc.h"
#include <stdio.h>
#include <tchar.h>

//*****************************************************************************
// 静的メンバ変数
//*****************************************************************************
LPD3DXFONT CDebugProc::m_pD3DXFont;
char CDebugProc::m_aStr[LENGTH_STRING_BUFF];
bool CDebugProc::m_bDisp;

//=============================================================================
// コンストラクタ
//=============================================================================
CDebugProc::CDebugProc()
{
}

//=============================================================================
// デストラクタ
//=============================================================================
CDebugProc::~CDebugProc()
{
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CDebugProc::Init(LPDIRECT3DDEVICE9 pDevice)
{
	// 情報表示用フォントを設定
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

	// メンバ変数の初期化
	ZeroMemory(&m_aStr[0], sizeof(m_aStr));
	m_bDisp = true;

	// 正常終了
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CDebugProc::Uninit(void)
{
	// フォントの開放
	if (m_pD3DXFont != NULL)
	{
		m_pD3DXFont->Release();
		m_pD3DXFont = NULL;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CDebugProc::Draw(void)
{
#ifdef _DEBUG

	// 表示フラグがOFF
	if (!m_bDisp)
	{
		return;
	}

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// テキスト描画
	m_pD3DXFont->DrawText(NULL, &m_aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0xff, 0xff, 0xff));

#endif
}

//=============================================================================
// バッファクリア処理
//=============================================================================
void CDebugProc::ClearBuffer(void)
{
#ifdef _DEBUG
	// バッファクリア
	ZeroMemory(&m_aStr[0], sizeof(m_aStr));
#endif
}

//=============================================================================
// 表示文字列格納処理
//=============================================================================
void CDebugProc::Print(char* fmt, ...)
{
#ifdef _DEBUG

	char aStrBuf[LENGTH_STRING_BUFF] = "";
	va_list args;

	// 表示フラグがOFF
	if (!m_bDisp)
	{
		return;
	}

	// 文字列作成
	va_start(args, fmt);
	_vsntprintf_s(&aStrBuf[0], LENGTH_STRING_BUFF, _TRUNCATE, fmt, args);

	// 文字列格納
	_tcscat_s(&m_aStr[0],LENGTH_STRING_BUFF,&aStrBuf[0]);

	// 処理の終了
	va_end(args);

#endif
}