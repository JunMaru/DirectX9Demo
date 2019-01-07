//=============================================================================
//
// �L�[�{�[�h���� [intputKeyboard.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "inputKeyboard.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define KEY_MASK (0x80)
#define KEY_REPEAT_WAIT_TIME (20)

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
BYTE CInputKeyboard::m_aKeyState[256];
BYTE CInputKeyboard::m_aKeyTrigger[256];
BYTE CInputKeyboard::m_aKeyRepeat[256];
BYTE CInputKeyboard::m_aKeyRelease[256];

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CInputKeyboard::CInputKeyboard()
{

}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{

}

//=============================================================================
// ����������
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	CInput::Init(hInstance, hWnd);

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard,
		&m_pDIDevice,
		NULL);

	if (FAILED(hr))
	{
		//TODO:���b�Z�[�W�{�b�N�X�̍쐬

		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);

	if (FAILED(hr))
	{
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));

	if (FAILED(hr))
	{
		//TODO:���b�Z�[�W�{�b�N�X�̍쐬
		return hr;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return S_OK;

}


//=============================================================================
// �I������
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// �f�o�C�X�I�u�W�F�N�g�̊J��
	if (m_pDIDevice != NULL)
	{
		m_pDIDevice->Unacquire();
		m_pDIDevice->Release();
		m_pDIDevice = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void CInputKeyboard::Update(void)
{
	BYTE aKeyState[256];

	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nCntKey = 0; nCntKey < 256; nCntKey++)
		{
			//�g���K�[���̍쐬
			m_aKeyTrigger[nCntKey] = (aKeyState[nCntKey] ^ m_aKeyState[nCntKey]) & aKeyState[nCntKey];

			//�����[�X���̍쐬
			m_aKeyRelease[nCntKey] = (aKeyState[nCntKey] ^ m_aKeyState[nCntKey]) & ~aKeyState[nCntKey];

			//���s�[�g���̍쐬
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

			//�v���X�f�[�^�i�[
			m_aKeyState[nCntKey] = aKeyState[nCntKey];

		}
	}

	else
	{
		//�A�N�Z�X���Ď擾
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �v���X���擾����
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
// �g���K�[���擾����
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
// �����[�X���擾����
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
// ���s�[�g���擾����
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
// �S�ẴL�[�̓��ǂ�ł��������牟����Ă��邩�𒲂ׂ鏈��
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