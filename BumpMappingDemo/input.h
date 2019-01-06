//=============================================================================
//
// ���͏��� [input.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h�K�[�h
//*****************************************************************************
#ifndef _INPUT_H_
#define _INPUT_H_

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include <windows.h>
#include <d3d9.h>
#include "config.h"

#define DIRECTINPUT_VERSION (0x0800)    // �x���΍��p
#include "dinput.h"
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// �N���X��`
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