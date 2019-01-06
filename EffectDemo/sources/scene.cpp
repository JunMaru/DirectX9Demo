//=============================================================================
//
// �V�[������ [scene.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "scene.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CScene* CScene::m_pTop[NUM_PRIORITY] = {NULL};
CScene* CScene::m_pCur[NUM_PRIORITY] = {NULL};
bool CScene::m_bPause = false;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CScene::CScene(int nPriority,OBJTYPE objType)
{
	// �v���C�I���e�B�̊m�F
	if(nPriority < 0)
	{
		nPriority = 0;
	}

	if(nPriority > NUM_PRIORITY - 1)
	{
		nPriority = NUM_PRIORITY - 1;
	}

	// �擪�I�u�W�F�N�g�����݂��Ȃ��ꍇ
	if(m_pTop[nPriority] == NULL)
	{
		// ������擪�Ƃ��ēo�^
		m_pTop[nPriority] = this;
	}

	// �Ō���̃I�u�W�F�N�g�����݂ɂ��Ȃ��ꍇ
	if(m_pCur[nPriority] == NULL)
	{
		// �������Ō���Ƃ��ēo�^
		m_pCur[nPriority] = this;
	}

	// �Ō���̃I�u�W�F�N�g�̎��̃I�u�W�F�N�g�����̃I�u�W�F�N�g�ɂ���
	m_pCur[nPriority]->m_pNext = this;

	// ���݂̃I�u�W�F�N�g���擪�̏ꍇ
	if(m_pTop[nPriority] == this)
	{
		// ���̑O�̃I�u�W�F�N�g��NULL
		m_pPrev = NULL;
	}

	// ���݂̃I�u�W�F�N�g���擪�łȂ����
	else
	{
		// ���̃I�u�W�F�N�g�̑O�̃I�u�W�F�N�g���Ō����
		m_pPrev = m_pCur[nPriority];
	}

	// �Ō���̃I�u�W�F�N�g�����̃I�u�W�F�N�g�ɂ���
	m_pCur[nPriority] = this;

	// ���̃I�u�W�F�N�g�̎��̃I�u�W�F�N�g��NULL
	m_pNext = NULL;

	// �v���C�I���e�B�̋L��
	m_nPriority = nPriority;

	// �^�C�v�̕ۑ�
	m_objType = objType;

	// �����t���O�̏�����
	m_bDelete = false;
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CScene::~CScene()
{

}

//=============================================================================
// ���X�g���S�X�V�֐�
//=============================================================================
void CScene::UpdateAll(void)
{
	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		// �|�[�Y�����t�F�[�h�ƃ|�[�Y�̃v���C�I���e�B�ȊO�̏ꍇ
		if (m_bPause == true && nIdx < NUM_PRIORITY - 1)
		{
			// �X�V���X�L�b�v������
			continue;
		}

		CScene* pScene = m_pTop[nIdx];

		while(pScene)
		{
			if (pScene->m_bDelete == false)
			{
				pScene->Update();
			}
				pScene = pScene->m_pNext;
		}
	}

	CScene* pSceneNext;

	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		CScene* pScene = m_pTop[nIdx];

		while(pScene)
		{
			pSceneNext = pScene->m_pNext;
			
			if(pScene->m_bDelete)
			{
				pScene->UnLinkList();
				delete pScene;
			}

			pScene = pSceneNext;
		}
	}
}

//=============================================================================
// ���X�g���S�`��֐�
//=============================================================================
void CScene::DrawAll(void)
{
	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		CScene* pScene = m_pTop[nIdx];

		while(pScene)
		{
			pScene->Draw();

			pScene = pScene->m_pNext;
		}
	}
}

void CScene::DrawIndex(int nIdx)
{
	CScene* pScene = m_pTop[nIdx];

	while (pScene)
	{
		pScene->Draw();

		pScene = pScene->m_pNext;
	}

}

//=============================================================================
// ���X�g���S�J���֐�
//=============================================================================
void CScene::ReleaseAll(void)
{
	CScene* pSceneNext;

	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		CScene* pScene = m_pTop[nIdx];

		while(pScene)
		{
			pScene->Uninit();

			pScene = pScene->m_pNext;
		}
	}

	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		CScene* pScene = m_pTop[nIdx];

		while(pScene)
		{
			pSceneNext = pScene->m_pNext;
			
			if(pScene->m_bDelete)
			{
				pScene->UnLinkList();
				delete pScene;
			}

			pScene = pSceneNext;
		}
	}
}

//=============================================================================
// �J���֐�
//=============================================================================
void CScene::Release(void)
{
	// �����t���O�����Ă�
	m_bDelete = true;
}

//=============================================================================
// ���X�g�����֐�
//=============================================================================
void CScene::UnLinkList(void)
{
	// �폜�Ώۂ������A�ă����N
	// �폜�Ώۂ����X�g�̐擪�̏ꍇ
	if(this == m_pTop[m_nPriority])
	{
		// ���X�g�̗v�f��1�̏ꍇ
		if(this->m_pNext == NULL)
		{
			// ���X�g�����S��NULL������
			m_pTop[m_nPriority] = NULL;
			m_pCur[m_nPriority] = NULL;
		}

		// ���X�g�̗v�f��2�ȏ�̏ꍇ
		else
		{
			// 2�Ԗڂɂ����G��擪�ɐݒ�
			m_pTop[m_nPriority] = this->m_pNext;
		}
	}

	// �폜�Ώۂ����X�g�̍Ō���������ꍇ
	else if(this == m_pCur[m_nPriority])
	{
		this->m_pPrev->m_pNext = NULL;
		m_pCur[m_nPriority] = this->m_pPrev;
	}

	// �폜�Ώۂ����X�g�̐擪�܂��͍Ō���Ŗ����ꍇ
	else
	{
		// �u�폜�Ώ�1�O�̎��̃I�u�W�F�N�g�v���u�폜�Ώۂ̎��̃I�u�W�F�N�g�v�ɐݒ�
		this->m_pPrev->m_pNext = this->m_pNext;

		// �u�폜�Ώ�1���̑O�̃I�u�W�F�N�g�v���u�폜�Ώۂ̑O�̃I�u�W�F�N�g�v�ɐݒ�
		this->m_pNext->m_pPrev = this->m_pPrev;
	}
}