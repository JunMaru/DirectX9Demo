//=============================================================================
//
// キーボード処理 [inputKeyboard.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _INPUTKEYBOARD_H_
#define _INPUTKEYBOARD_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "input.h"

//*****************************************************************************
// 定数宣言
//*****************************************************************************
static const int KEY_MAX = 256;

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit(void);
	void Update(void);

	static bool IsPress(int nKey);
	static bool IsTrigger(int nKey);
	static bool IsRelease(int nKey);
	static bool IsRepeat(int nKey);
	static bool IsAnyPress(void);

private:
	static BYTE m_aKeyState[KEY_MAX];
	static BYTE m_aKeyTrigger[KEY_MAX];
	static BYTE m_aKeyRepeat[KEY_MAX];
	static BYTE m_aKeyRelease[KEY_MAX];

	int m_aCntRepeat[KEY_MAX];
};

#endif