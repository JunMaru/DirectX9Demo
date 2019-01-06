//=============================================================================
//
// シーンメッシュ処理 [sceneMesh.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _SCENEMESH_H_
#define _SCENEMESH_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "scene.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 3Dポリゴン頂点フォーマット( 頂点座標[3D] / 法線 / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CSceneMesh : public CScene
{
public:
	typedef struct
	{
		D3DXVECTOR3 vtx;		// 頂点座標
		D3DXVECTOR3 nor;		// 法線ベクトル
		D3DCOLOR diffuse;		// 反射光
		D3DXVECTOR2 tex;		// テクスチャ座標
	}VERTEX_3D;

	CSceneMesh(int nPriority = 3) : CScene(nPriority,OBJTYPE_NONE){};
	virtual ~CSceneMesh(){};

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	virtual void SetPos(float x,float y,float z){m_pos.x = x;m_pos.y = y;m_pos.z = z;}
	virtual void SetPos(D3DXVECTOR3 pos){m_pos = pos;}
	virtual D3DXVECTOR3 GetPos(void){return m_pos;}

	virtual void SetRot(float x,float y,float z){m_rot.x = x;m_rot.y = y;m_rot.z = z;}
	virtual void SetRot(D3DXVECTOR3 rot){m_rot = rot;}
	virtual D3DXVECTOR3 GetRot(void){return m_rot;}

protected:
	LPDIRECT3DDEVICE9 m_pDevice;
	LPDIRECT3DTEXTURE9 m_pTexture;				// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuff;		// インデックスバッファへのポインタ
	D3DXMATRIX m_mtxWorld;

	D3DXVECTOR3 m_pos;							// 座標
	D3DXVECTOR3 m_rot;							// 回転
	D3DXVECTOR3 m_scl;							// スケール

	int m_nNumVertex;							// 総頂点数
	int m_nNumPolygon;							// 総ポリゴン数
	int m_nNumVertexIndex;						// 頂点の総インデックス数
};
#endif