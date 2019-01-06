//=============================================================================
//
// シーン処理 [scene.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _SCENE_H_
#define _SCENE_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"
#include "Direct3DDevice.h"
#include "Framework.h"

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_PRIORITY (9)

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CScene
{
public:
	typedef enum
	{
		OBJTYPE_NONE = 0,
		OBJTYPE_2D,
		OBJTYPE_3D,
		OBJTYPE_X,
		OBJTYPE_BILLBOARD,
		OBJTYPE_PLAYER,
		OBJTYPE_GIMMICK,
		OBJTYPE_BULLET,
		OBJTYPE_EXPLOSION,
		OBJTYPE_TIMER,
		OBJTYPE_SCORE,
		OBJTYPE_FADE,
		OBJTYPE_MAX,
	}OBJTYPE;

	CScene(int nPriority = 3,OBJTYPE objType = OBJTYPE_NONE);
	virtual ~CScene();

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	virtual void SetPos(float x,float y,float z) = 0;
	virtual void SetPos(D3DXVECTOR3 pos) = 0;
	virtual D3DXVECTOR3 GetPos(void) = 0;

	virtual void SetRot(float x,float y,float z) = 0;
	virtual void SetRot(D3DXVECTOR3 pos) = 0;
	virtual D3DXVECTOR3 GetRot(void) = 0;

	virtual void SetPosOld(D3DXVECTOR3 posOld){ UNREFERENCED_PARAMETER(posOld); }
	virtual D3DXVECTOR3 GetPosOld(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	virtual void SetRotOld(D3DXVECTOR3 rotOld){ UNREFERENCED_PARAMETER(rotOld); }
	virtual D3DXVECTOR3 GetRotOld(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	virtual D3DXVECTOR3 GetVertexMin(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetVertexMax(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	OBJTYPE GetObjtype(void){ return m_objType; }

	void Release(void);

	static void UpdateAll(void);
	static void DrawAll(void);
	static void DrawIndex(int nIdx);
	static void ReleaseAll(void);

	static void SetPause(bool bPause){ m_bPause = bPause; }

	CScene* GetNext(void){ return m_pNext; }
private:
	void UnLinkList(void);
	void HitChkGimmik(CScene* pScene);

	static CScene* m_pTop[NUM_PRIORITY];
	static CScene* m_pCur[NUM_PRIORITY];

	static bool m_bPause;

	CScene* m_pPrev;
	CScene* m_pNext;

	int m_nPriority;
	bool m_bDelete;

	OBJTYPE m_objType;

protected:
	CScene** GetList(void){ return m_pTop; }
};

#endif