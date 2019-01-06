//=============================================================================
//
// �V�[�����b�V������ [sceneMesh.h]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h�K�[�h
//*****************************************************************************
#ifndef _SCENEMESH_H_
#define _SCENEMESH_H_

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "scene.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// 3D�|���S�����_�t�H�[�}�b�g( ���_���W[3D] / �@�� / ���ˌ� / �e�N�X�`�����W )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

//*****************************************************************************
// �N���X��`
//*****************************************************************************
class CSceneMesh : public CScene
{
public:
	typedef struct
	{
		D3DXVECTOR3 vtx;		// ���_���W
		D3DXVECTOR3 nor;		// �@���x�N�g��
		D3DCOLOR diffuse;		// ���ˌ�
		D3DXVECTOR2 tex;		// �e�N�X�`�����W
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
	LPDIRECT3DTEXTURE9 m_pTexture;				// �e�N�X�`���ւ̃|�C���^
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuff;		// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	D3DXMATRIX m_mtxWorld;

	D3DXVECTOR3 m_pos;							// ���W
	D3DXVECTOR3 m_rot;							// ��]
	D3DXVECTOR3 m_scl;							// �X�P�[��

	int m_nNumVertex;							// �����_��
	int m_nNumPolygon;							// ���|���S����
	int m_nNumVertexIndex;						// ���_�̑��C���f�b�N�X��
};
#endif