//=============================================================================
//
// シーンX処理 [sceneX.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルードガード
//*****************************************************************************
#ifndef _SCENEX_H_
#define _SCENEX_H_

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "scene.h"
#include "normalMapShader.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 3Dポリゴン頂点フォーマット( 頂点座標[3D] / 法線 / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// クラス定義
//*****************************************************************************
class CSceneX : public CScene
{
public:
	typedef struct
	{
		D3DXVECTOR3 vtx;		// 頂点座標
		D3DXVECTOR3 nor;		// 法線ベクトル
		D3DCOLOR diffuse;		// 反射光
		D3DXVECTOR2 tex;		// テクスチャ座標
	}VERTEX_3D;
	enum SHADERNO
	{
		AMB = 0,
		LAM,
		HALFLAM,
		RIM,
		BLEND,
		ORIGIN,
	};


	CSceneX(LPDIRECT3DDEVICE9 pDevice, int nPriority,OBJTYPE objType);
	virtual ~CSceneX();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CSceneX* Create(D3DXVECTOR3 pos, LPDIRECT3DDEVICE9 pDevice, int nPriority, const char* pFileName);

	void SetPos(float x, float y, float z){ m_pos.x = x; m_pos.y = y; m_pos.z = z; }
	void SetPos(D3DXVECTOR3 pos){ m_pos = pos; }
	void AddPos(D3DXVECTOR3 pos){ m_pos += pos; }
	void SetPosX(float fPos){ m_pos.x = fPos; }
	void SetPosY(float fPos){ m_pos.y = fPos; }
	void SetPosZ(float fPos){ m_pos.z = fPos; }
	void AddPosX(float fPos){ m_pos.x += fPos; }
	void AddPosY(float fPos){ m_pos.y += fPos; }
	void AddPosZ(float fPos){ m_pos.z += fPos; }
	D3DXVECTOR3 GetPos(void){ return m_pos; }

	void SetRot(float x, float y, float z){ m_rot.x = x; m_rot.y = y; m_rot.z = z; }
	void SetRot(D3DXVECTOR3 rot){ m_rot = rot; }
	void AddRot(D3DXVECTOR3 rot){ m_rot += rot; }
	void SetRotX(float fRot){ m_rot.x = fRot; }
	void SetRotY(float fRot){ m_rot.y = fRot; }
	void SetRotZ(float fRot){ m_rot.z = fRot; }
	void AddRotX(float fRot){ m_rot.x += fRot; }
	void AddRotY(float fRot){ m_rot.y += fRot; }
	void AddRotZ(float fRot){ m_rot.z += fRot; }
	D3DXVECTOR3 GetRot(void){ return m_rot; }

	void SetScl(float x, float y, float z){ m_scl.x = x; m_scl.y = y; m_scl.z = z; }
	void SetScl(D3DXVECTOR3 scl){ m_scl = scl; }
	D3DXVECTOR3 GetScl(void){ return m_scl; }

	virtual D3DXVECTOR3 GetVertexMin(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual D3DXVECTOR3 GetVertexMax(void){ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	D3DXMATRIX GetWorldMatrix(void){ return m_mtxWorld; }

	void SetDrawShadowTexture(bool flg){ m_bDrawShadowTexture = flg; }

protected:
	void SetVertexPolygon(void);
	float FixRot(float fRot);

	LPDIRECT3DDEVICE9 m_pDevice;				// Deviceオブジェクト(描画に必要)
	LPDIRECT3DTEXTURE9 m_pD3DTex;			// テクスチャへのポインタ

	D3DXVECTOR3 m_pos;							// 座標
	D3DXVECTOR3 m_rot;							// 回転(角度)
	D3DXVECTOR3 m_scl;

	D3DXMATRIX m_mtxProjection;					// プロジェクションマトリックス
	D3DXMATRIX m_mtxWorld;						// ワールドマトリックス

	LPD3DXMESH m_pD3DXMesh;						// メッシュ情報へのポインタ
	LPD3DXBUFFER m_pD3DXBuffMat;				// マテリアル情報へのポインタ
	DWORD m_nNumMat;							// マテリアル情報の数

	D3DMATERIAL9* m_pMat;
	LPDIRECT3DTEXTURE9* m_ppTex;
	float m_fRadius;

	const char* m_pModelFileName;
	LPDIRECT3DVERTEXDECLARATION9 m_pDec;
	LPDIRECT3DVERTEXDECLARATION9 m_pDecOpt;
	LPD3DXMESH m_pD3DMesh;
	LPD3DXBUFFER m_pD3DMatBuffer;
	LPD3DXBUFFER m_pD3DXAdjacencyBuffer;
	DWORD dwNumMaterials;
	SHADERNO m_nShaderNo;

	CNormalMapShader* m_pNormalMapShader;

	D3DXVECTOR3 m_velPos;
	D3DXVECTOR3 m_destRot;

	bool m_bDrawShadowTexture;

	void DrawNormalMapLighting(void);
};

#endif