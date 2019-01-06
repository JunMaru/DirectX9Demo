//=============================================================================
//
// 入力処理 [input.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _INPUT_H_
#define _INPUT_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include <windows.h>
#include <d3d9.h>
#include "config.h"

#define DIRECTINPUT_VERSION (0x0800)    // 警告対策用
#include "dinput.h"
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CInput
{
public:
	CInput();
	~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

protected:
	static LPDIRECTINPUT8 m_pDInput;
	LPDIRECTINPUTDEVICE8 m_pDIDevice;
};

#endif