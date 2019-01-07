//=============================================================================
//
// キーボード処理 [intputKeyboard.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "inputKeyboard.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define KEY_MASK (0x80)
#define KEY_REPEAT_WAIT_TIME (20)

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
BYTE CInputKeyboard::m_aKeyState[256];
BYTE CInputKeyboard::m_aKeyTrigger[256];
BYTE CInputKeyboard::m_aKeyRepeat[256];
BYTE CInputKeyboard::m_aKeyRelease[256];

//=============================================================================
// コンストラクタ
//=============================================================================
CInputKeyboard::CInputKeyboard()
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	CInput::Init(hInstance, hWnd);

	// デバイスオブジェクトを作成
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard,
		&m_pDIDevice,
		NULL);

	if (FAILED(hr))
	{
		//TODO:メッセージボックスの作成

		return hr;
	}

	// データフォーマットを設定
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);

	if (FAILED(hr))
	{
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

	if (FAILED(hr))
	{
		//TODO:メッセージボックスの作成
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	m_pDIDevice->Acquire();

	return S_OK;

}


//=============================================================================
// 終了処理
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// デバイスオブジェクトの開放
	if (m_pDIDevice != NULL)
	{
		m_pDIDevice->Unacquire();
		m_pDIDevice->Release();
		m_pDIDevice = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[256];

	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < 256; nCntKey++)
		{
			//トリガー情報の作成
			m_aKeyTrigger[nCntKey] = (aKeyState[nCntKey] ^ m_aKeyState[nCntKey]) & aKeyState[nCntKey];

			//リリース情報の作成
			m_aKeyRelease[nCntKey] = (aKeyState[nCntKey] ^ m_aKeyState[nCntKey]) & ~aKeyState[nCntKey];

			//リピート情報の作成
			if (aKeyState[nCntKey] & KEY_MASK)
			{
				m_aCntRepeat[nCntKey]++;

				if (m_aCntRepeat[nCntKey] <= KEY_REPEAT_WAIT_TIME)
				{
					m_aKeyRepeat[nCntKey] = m_aKeyTrigger[nCntKey];
				}

				if (m_aCntRepeat[nCntKey] > KEY_REPEAT_WAIT_TIME)
				{
					m_aKeyRepeat[nCntKey] = aKeyState[nCntKey];
				}
			}

			else
			{
				m_aCntRepeat[nCntKey] = 0;
				m_aKeyRepeat[nCntKey] = aKeyState[nCntKey];
			}

			//プレスデータ格納
			m_aKeyState[nCntKey] = aKeyState[nCntKey];

		}
	}

	else
	{
		//アクセス権再取得
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// プレス情報取得処理
//=============================================================================
bool CInputKeyboard::IsPress(int nKey)
{
	if (m_aKeyState[nKey] & KEY_MASK)
	{
		return true;
	}

	return false;
}

//=============================================================================
// トリガー情報取得処理
//=============================================================================
bool CInputKeyboard::IsTrigger(int nKey)
{
	if (m_aKeyTrigger[nKey] & KEY_MASK)
	{
		return true;
	}

	return false;
}

//=============================================================================
// リリース情報取得処理
//=============================================================================
bool CInputKeyboard::IsRelease(int nKey)
{
	if (m_aKeyRelease[nKey] & KEY_MASK)
	{
		return true;
	}

	return false;
}

//=============================================================================
// リピート情報取得処理
//=============================================================================
bool CInputKeyboard::IsRepeat(int nKey)
{
	if (m_aKeyRepeat[nKey] & KEY_MASK)
	{
		return true;
	}

	return false;
}

//=============================================================================
// 全てのキーの内どれでもいいから押されているかを調べる処理
//=============================================================================
bool CInputKeyboard::IsAnyPress(void)
{
	for (int nCnt = 0; nCnt < KEY_MAX; nCnt++)
	{
		if (m_aKeyState[nCnt] & KEY_MASK)
		{
			return true;
		}
	}
	
	return false;
}