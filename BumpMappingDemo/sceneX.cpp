//=============================================================================
//
// �V�[��X���� [sceneX.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include "sceneX.h"
#include "textureManager.h"
#include "Framework.h"
#include "Direct3DDevice.h"
#include "inputKeyboard.h"

static const int kShaderMax = 3;

// ���_���W���̐錾
const D3DVERTEXELEMENT9 aVtxElem[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	{ 0, 48, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TANGENT,0,},
	D3DDECL_END()
};

// ���_�v�f(�I�v�e�B�}�C�Y)
const D3DVERTEXELEMENT9 aOptElem[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

D3DXVECTOR3 g_pointLightPos(10.0f, 10.0f, 0.0f);
D3DXCOLOR g_pointLightColor(1.0f, 0.5f, 0.0f, 1.0f);
float g_attenuation[] = { 0.1f, 0.05f, 0.0f };

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CSceneX::CSceneX(LPDIRECT3DDEVICE9 pDevice, int nPritority,OBJTYPE objType) : CScene(nPritority,objType)
{
	//���W�̏�����
	m_pos.x = 0;
	m_pos.y = 0;
	m_pos.z = 0;

	//��]�ʂ̏�����
	m_rot.x = 0;
	m_rot.y = 0;
	m_rot.z = 0;

	m_scl.x = 1.0f;
	m_scl.y = 1.0f;
	m_scl.z = 1.0f;

	// �f�o�C�X�̐ݒ�
	m_pDevice = pDevice;

	m_fRadius = 50.0f;

	m_pMat = NULL;
	m_pD3DXAdjacencyBuffer = nullptr;
	m_pD3DMatBuffer = nullptr;
	m_pD3DMesh = nullptr;
	m_nShaderNo = AMB;

	m_velPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_destRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_bDrawShadowTexture = false;
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
CSceneX::~CSceneX()
{

}

//=============================================================================
// ����������
//=============================================================================
HRESULT CSceneX::Init(void)
{
	m_pDevice = Direct3DDevice_getDevice();

	m_pDevice->CreateVertexDeclaration(aVtxElem, &m_pDec);


	D3DXLoadMeshFromX(m_pModelFileName,
		D3DXMESH_SYSTEMMEM,//MANAGED
		m_pDevice,
		&m_pD3DXAdjacencyBuffer,
		&m_pD3DMatBuffer,
		NULL,
		&dwNumMaterials,
		&m_pD3DMesh);


	HRESULT opt = E_FAIL;
	opt = m_pD3DMesh->OptimizeInplace(
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)m_pD3DXAdjacencyBuffer->GetBufferPointer(),
		NULL,// �œK�����ꂽ�אڏ��
		NULL,// �C���f�b�N�X(��)
		NULL);// �C���f�b�N�X(���_)

	LPD3DXMESH pOldMesh = m_pD3DMesh;

	opt = pOldMesh->CloneMesh(D3DXMESH_MANAGED,
		aVtxElem, m_pDevice,
		&m_pD3DMesh);

	//opt = D3DXComputeNormals(m_pD3DMesh, (DWORD*)m_pD3DXAdjacencyBuffer->GetBufferPointer());
	opt = D3DXComputeTangent(m_pD3DMesh, 0, 0, 0, 0, (DWORD*)m_pD3DXAdjacencyBuffer->GetBufferPointer());

	pOldMesh->Release();
	m_pD3DXAdjacencyBuffer->Release();
	m_pD3DXAdjacencyBuffer = NULL;

	m_pNormalMapShader = CNormalMapShader::Create();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void CSceneX::Uninit(void)
{
	if (m_pD3DMesh != NULL)
	{
		m_pD3DMesh->Release();
		m_pD3DMesh = NULL;
	}

	if (m_pD3DMatBuffer != NULL)
	{
		m_pD3DMatBuffer->Release();
		m_pD3DMatBuffer = NULL;
	}

	if (m_pD3DXAdjacencyBuffer != NULL)
	{
		m_pD3DXAdjacencyBuffer->Release();
		m_pD3DXAdjacencyBuffer = NULL;
	}

	m_pNormalMapShader->Uninit();
	delete m_pNormalMapShader;

	m_pDec->Release();

	// ���X�g����폜
	Release();
}

//=============================================================================
// �X�V����
//=============================================================================
void CSceneX::Update(void)
{
	if (CInputKeyboard::IsPress(DIK_A))
	{
		m_rot.y += 0.01f;
	}

	if (CInputKeyboard::IsPress(DIK_D))
	{
		m_rot.y -= 0.01f;
	}

	if (CInputKeyboard::IsPress(DIK_W))
	{
		m_rot.x += 0.01f;
	}

	if (CInputKeyboard::IsPress(DIK_S))
	{
		m_rot.x -= 0.01f;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void CSceneX::Draw(void)
{
	DrawNormalMapLighting();
}

//=============================================================================
// �V�[��X��������
//=============================================================================
CSceneX* CSceneX::Create(D3DXVECTOR3 pos, LPDIRECT3DDEVICE9 pDevice, int nPriority, const char* pFileName)
{
	// �C���X�^���X��
	CSceneX* p = new CSceneX(pDevice, nPriority,OBJTYPE_X);

	// ������
	p->m_pModelFileName = pFileName;
	p->Init();
	p->m_pos = pos;

	return p;
}


//=============================================================================
// ���_�o�b�t�@�Z�b�g����
//=============================================================================
void CSceneX::SetVertexPolygon(void)
{
	// �ۂߍ���
	if (m_rot.z > D3DX_PI)
	{
		m_rot.z = -D3DX_PI + (m_rot.z - D3DX_PI);
	}

	if (m_rot.z < -D3DX_PI)
	{
		m_rot.z = D3DX_PI + (m_rot.z + D3DX_PI);
	}
}

//=============================================================================
// �p�x�C���֐�
//=============================================================================
float CSceneX::FixRot(float fRot)
{
	if (fRot >= D3DX_PI)
	{
		fRot = (-2 * D3DX_PI) + fRot;
	}

	if (fRot <= -D3DX_PI)
	{
		fRot = (2 * D3DX_PI) + fRot;
	}

	return fRot;
}

void CSceneX::DrawNormalMapLighting(void)
{
	m_pNormalMapShader->SetShaderToDevice();

	D3DXMATRIX w, mtxTrans;
	D3DXMATRIX v = GetViewMatrix();
	D3DXMATRIX p = GetProjectionMatrix();

	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	m_pDevice->SetVertexDeclaration(m_pDec);

	D3DXMatrixIdentity(&w);
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixRotationYawPitchRoll(&w, m_rot.y, m_rot.x, m_rot.z);
	// �ʒu�̔��f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&w, &w, &mtxTrans);

	D3DXMATRIX wvp;
	D3DXMATRIX invWorld;
	D3DXMATRIX worldInvTranspose;
	D3DXMatrixIdentity(&wvp);
	D3DXMatrixIdentity(&invWorld);
	D3DXMatrixIdentity(&worldInvTranspose);

	D3DXMatrixMultiply(&wvp, &w, &v);
	D3DXMatrixMultiply(&wvp, &wvp, &p);

	D3DXMatrixInverse(&invWorld, NULL, &w);
	D3DXMatrixInverse(&worldInvTranspose, NULL, &w);
	D3DXMatrixTranspose(&worldInvTranspose, &worldInvTranspose);

	D3DXVECTOR3 localLightVec;
	D3DXVECTOR3 lightDir = GetLightDir();
	D3DXCOLOR lightColor = GetLightColor();
	D3DXVec3TransformCoord(&localLightVec, &lightDir, &invWorld);

	D3DXVECTOR3 cameraVec = GetCameraVec();

	D3DXVec3Normalize(&cameraVec, &cameraVec);
	D3DXVec3Normalize(&localLightVec, &localLightVec);
	D3DXVec3Normalize(&lightDir, &lightDir);

	m_pNormalMapShader->SetWorldLightVector(&lightDir);
	m_pNormalMapShader->SetWorldCameraVector(&cameraVec);
	m_pNormalMapShader->SetWorldViewProjectionMatrix(&wvp);
	m_pNormalMapShader->SetWorldMatrix(&w);
	m_pNormalMapShader->SetLightDiffuse(&lightColor);
	m_pNormalMapShader->SetWorldInverseTransposeMatrix(&worldInvTranspose);
	m_pNormalMapShader->SetWorldCameraVector(&GetCameraVec());

	m_pDevice->SetTexture(m_pNormalMapShader->GetNormalTextureSamplerIndex(), GetTextureManager()->GetTexture("data/TEXTURE/alloy_normal.png"));
	m_pDevice->SetTexture(m_pNormalMapShader->GetDiffuseTextureSamplerIndex(), GetTextureManager()->GetTexture("data/TEXTURE/alloy.png"));

	// �}�e���A�����̎擾
	m_pDevice->GetMaterial(&matDef);

	// ���f���̃}�e���A�����̎擾
	pD3DXMat = (D3DXMATERIAL *)m_pD3DMatBuffer->GetBufferPointer();

	// �}�e���A�����Ƃɕ`��
	for (unsigned int nCntMat = 0; nCntMat < dwNumMaterials; nCntMat++)
	{
		HRESULT hr;

		m_pNormalMapShader->SetMaterialDiffuse(&pD3DXMat[nCntMat].MatD3D.Diffuse);

		// �}�e���A���̐ݒ�
		m_pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// ���f���̕`��
		m_pD3DMesh->DrawSubset(nCntMat);
	}

	// �}�e���A�������ɖ߂�
	m_pDevice->SetMaterial(&matDef);

	m_pNormalMapShader->EndShader();

	m_pDevice->SetTexture(m_pNormalMapShader->GetDiffuseTextureSamplerIndex(), nullptr);
	m_pDevice->SetTexture(m_pNormalMapShader->GetNormalTextureSamplerIndex(), nullptr);
}