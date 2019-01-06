//=============================================================================
//
// シーン処理 [scene.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "scene.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CScene* CScene::m_pTop[NUM_PRIORITY] = {NULL};
CScene* CScene::m_pCur[NUM_PRIORITY] = {NULL};
bool CScene::m_bPause = false;

//=============================================================================
// コンストラクタ
//=============================================================================
CScene::CScene(int nPriority,OBJTYPE objType)
{
	// プライオリティの確認
	if(nPriority < 0)
	{
		nPriority = 0;
	}

	if(nPriority > NUM_PRIORITY - 1)
	{
		nPriority = NUM_PRIORITY - 1;
	}

	// 先頭オブジェクトが存在しない場合
	if(m_pTop[nPriority] == NULL)
	{
		// 自分を先頭として登録
		m_pTop[nPriority] = this;
	}

	// 最後尾のオブジェクトが存在にしない場合
	if(m_pCur[nPriority] == NULL)
	{
		// 自分を最後尾として登録
		m_pCur[nPriority] = this;
	}

	// 最後尾のオブジェクトの次のオブジェクトをこのオブジェクトにする
	m_pCur[nPriority]->m_pNext = this;

	// 現在のオブジェクトが先頭の場合
	if(m_pTop[nPriority] == this)
	{
		// この前のオブジェクトをNULL
		m_pPrev = NULL;
	}

	// 現在のオブジェクトが先頭でなければ
	else
	{
		// このオブジェクトの前のオブジェクトを最後尾に
		m_pPrev = m_pCur[nPriority];
	}

	// 最後尾のオブジェクトをこのオブジェクトにする
	m_pCur[nPriority] = this;

	// このオブジェクトの次のオブジェクトをNULL
	m_pNext = NULL;

	// プライオリティの記憶
	m_nPriority = nPriority;

	// タイプの保存
	m_objType = objType;

	// 消去フラグの初期化
	m_bDelete = false;
}

//=============================================================================
// デストラクタ
//=============================================================================
CScene::~CScene()
{

}

//=============================================================================
// リスト内全更新関数
//=============================================================================
void CScene::UpdateAll(void)
{
	for(int nIdx = 0;nIdx < NUM_PRIORITY;nIdx++)
	{
		// ポーズ中かつフェードとポーズのプライオリティ以外の場合
		if (m_bPause == true && nIdx < NUM_PRIORITY - 1)
		{
			// 更新をスキップをする
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
// リスト内全描画関数
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
// リスト内全開放関数
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
// 開放関数
//=============================================================================
void CScene::Release(void)
{
	// 消去フラグをたてる
	m_bDelete = true;
}

//=============================================================================
// リスト解除関数
//=============================================================================
void CScene::UnLinkList(void)
{
	// 削除対象を除き、再リンク
	// 削除対象がリストの先頭の場合
	if(this == m_pTop[m_nPriority])
	{
		// リストの要素が1つの場合
		if(this->m_pNext == NULL)
		{
			// リストを完全にNULL初期化
			m_pTop[m_nPriority] = NULL;
			m_pCur[m_nPriority] = NULL;
		}

		// リストの要素が2つ以上の場合
		else
		{
			// 2番目にいた敵を先頭に設定
			m_pTop[m_nPriority] = this->m_pNext;
		}
	}

	// 削除対象がリストの最後尾だった場合
	else if(this == m_pCur[m_nPriority])
	{
		this->m_pPrev->m_pNext = NULL;
		m_pCur[m_nPriority] = this->m_pPrev;
	}

	// 削除対象がリストの先頭または最後尾で無い場合
	else
	{
		// 「削除対象1つ前の次のオブジェクト」を「削除対象の次のオブジェクト」に設定
		this->m_pPrev->m_pNext = this->m_pNext;

		// 「削除対象1つ次の前のオブジェクト」を「削除対象の前のオブジェクト」に設定
		this->m_pNext->m_pPrev = this->m_pPrev;
	}
}