//=============================================================================
//
// デバッグ処理 [debugProc.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _DEBUGPROC_H_
#define _DEBUGPROC_H_

//******************************************************************************
// インクルード
//******************************************************************************
#include <windows.h>
#include "d3dx9.h"
#include "config.h"

//******************************************************************************
// ライブラリのリンク
//******************************************************************************
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")

//******************************************************************************
// マクロ定義
//******************************************************************************
#define LENGTH_STRING_BUFF (4096)

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CDebugProc
{
public:
	CDebugProc();
	~CDebugProc();

	HRESULT Init(LPDIRECT3DDEVICE9 pDevice);
	void Uninit(void);

	static void Print(char* fmt, ...);
	static void Draw(void);
	static bool IsEnebleDisp(void){ return m_bDisp; }
	static void EnebleDisp(bool bDisp){ m_bDisp = bDisp; }
	static void ClearBuffer(void);

private:
	static LPD3DXFONT m_pD3DXFont;
	static char m_aStr[LENGTH_STRING_BUFF];
	static bool m_bDisp;
};

#endif