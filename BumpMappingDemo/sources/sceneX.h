//=============================================================================
//
// �V�[��X���� [sceneX.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h�K�[�h
//*****************************************************************************
#ifndef _SCENEX_H_
#define _SCENEX_H_

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "scene.h"
#include "normalMapShader.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// 3D�|���S�����_�t�H�[�}�b�g( ���_���W[3D] / �@�� / ���ˌ� / �e�N�X�`�����W )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class CSceneX : public CScene
{
public:
	typedef struct
	{
		D3DXVECTOR3 vtx;		// ���_���W
		D3DXVECTOR3 nor;		// �@���x�N�g��
		D3DCOLOR diffuse;		// ���ˌ�
		D3DXVECTOR2 tex;		// �e�N�X�`�����W
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

	LPDIRECT3DDEVICE9 m_pDevice;				// Device�I�u�W�F�N�g(�`��ɕK�v)
	LPDIRECT3DTEXTURE9 m_pD3DTex;			// �e�N�X�`���ւ̃|�C���^

	D3DXVECTOR3 m_pos;							// ���W
	D3DXVECTOR3 m_rot;							// ��](�p�x)
	D3DXVECTOR3 m_scl;

	D3DXMATRIX m_mtxProjection;					// �v���W�F�N�V�����}�g���b�N�X
	D3DXMATRIX m_mtxWorld;						// ���[���h�}�g���b�N�X

	LPD3DXMESH m_pD3DXMesh;						// ���b�V�����ւ̃|�C���^
	LPD3DXBUFFER m_pD3DXBuffMat;				// �}�e���A�����ւ̃|�C���^
	DWORD m_nNumMat;							// �}�e���A�����̐�

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