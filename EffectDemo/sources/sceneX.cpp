//=============================================================================
//
// シーンX処理 [sceneX.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// インクルード
//*****************************************************************************
#include "sceneX.h"
#include "textureManager.h"
#include "Framework.h"
#include "Direct3DDevice.h"
#include "inputKeyboard.h"
#include "meshField.h"

static const int kShaderMax = 3;

// 頂点座標情報の宣言
const D3DVERTEXELEMENT9 aVtxElem[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

// 頂点要素(オプティマイズ)
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
// コンストラクタ
//=============================================================================
CSceneX::CSceneX(LPDIRECT3DDEVICE9 pDevice, int nPritority,OBJTYPE objType) : CScene(nPritority,objType)
{
	//座標の初期化
	m_pos.x = 0;
	m_pos.y = 0;
	m_pos.z = 0;

	//回転量の初期化
	m_rot.x = 0;
	m_rot.y = 0;
	m_rot.z = 0;

	m_scl.x = 1.0f;
	m_scl.y = 1.0f;
	m_scl.z = 1.0f;

	// デバイスの設定
	m_pDevice = pDevice;

	m_fRadius = 50.0f;

	m_pMat = NULL;
	m_ppTex = NULL;
	m_pD3DXAdjacencyBuffer = nullptr;
	m_pD3DMatBuffer = nullptr;
	m_pD3DMesh = nullptr;
	m_nShaderNo = AMB;

	m_velPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_destRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_bDrawShadowTexture = false;
	m_bDrawBlurModel = false;

	D3DXMatrixIdentity(&m_mtxOldWorld);
	D3DXMatrixIdentity(&m_mtxWorld);

}

//=============================================================================
// デストラクタ
//=============================================================================
CSceneX::~CSceneX()
{

}

//=============================================================================
// 初期化処理
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
		NULL,// 最適化された隣接情報
		NULL,// インデックス(面)
		NULL);// インデックス(頂点)

	LPD3DXMESH pOldMesh = m_pD3DMesh;

	opt = pOldMesh->CloneMesh(D3DXMESH_MANAGED,
		aOptElem, m_pDevice,
		&m_pD3DMesh);

	//D3DXComputeNormals(m_pD3DMesh, (DWORD*)m_pD3DXAdjacencyBuffer->GetBufferPointer());

	pOldMesh->Release();
	m_pD3DXAdjacencyBuffer->Release();
	m_pD3DXAdjacencyBuffer = NULL;

	m_pD3DTex = GetTextureManager()->GetTexture("data/TEXTURE/toon.png");

	m_pToonShader = CToonShader::Create();
	m_pShadowTextureShader = CShadowTexture::Create();
	m_pBlurShader = CBlur::Create();

	return S_OK;
}

//=============================================================================
// 終了処理
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

	m_pToonShader->Uninit();
	delete m_pToonShader;

	m_pShadowTextureShader->Uninit();
	delete m_pShadowTextureShader;

	m_pDec->Release();

	// リストから削除
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CSceneX::Update(void)
{
	D3DXMATRIX w, mtxTrans;
	D3DXMatrixIdentity(&m_mtxOldWorld);
	D3DXMatrixRotationY(&w, m_rot.y);
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&w, &w, &mtxTrans);
	D3DXMatrixMultiply(&m_mtxOldWorld, &m_mtxOldWorld, &w);

	m_velPos.y += -0.198f;
	m_pos += m_velPos;

	// 目的の向きまでの差分を計算
	float fDiffRotY = m_destRot.y - m_rot.y;

	// 角度の丸め込み
	fDiffRotY = FixRot(fDiffRotY);

	// 回転量の計算
	m_rot.y += fDiffRotY * 0.03f;

	// 角度の丸め込み
	m_rot.y = FixRot(m_rot.y);


	// 地面の高さを取得
	float fDestPosY = 0.0f;
	if (GetField() != NULL)
	{
		fDestPosY = GetField()->GetHeight(m_pos, NULL);
	}
	else
	{
		m_velPos.y = 0.0f;
	}

	// 自分の座標が地面より下かどうか
	if (m_pos.y < fDestPosY)
	{
		// 下なら引き上げる
		m_pos.y = fDestPosY;

		// y方向の速度を0に戻す
		m_velPos.y = 0.0f;

	}

	m_velPos.x += (0 - m_velPos.x) * 0.5f;
	m_velPos.z += (0 - m_velPos.z) * 0.5f;


	if (CInputKeyboard::IsPress(DIK_Q))
	{
		m_rot.y += 0.01f;
	}

	if (CInputKeyboard::IsPress(DIK_E))
	{
		m_rot.y -= 0.01f;
	}

	float fMoveSpeed = 1.0f;

	bool bLeft = false;
	bool bRight = false;
	bool bUp = false;
	bool bDown = false;

	if (CInputKeyboard::IsPress(DIK_W))
	{
		// 移動量更新
		m_velPos.x += sinf(GetCameraRot().y) * fMoveSpeed;
		m_velPos.z += cosf(GetCameraRot().y) * fMoveSpeed;

		m_destRot.y = -D3DX_PI + GetCameraRot().y;

		bUp = true;
	}

	// 手前移動
	if (CInputKeyboard::IsPress(DIK_S))
	{
		m_velPos.x += -sinf(GetCameraRot().y) * fMoveSpeed;
		m_velPos.z += -cosf(GetCameraRot().y) * fMoveSpeed;

		m_destRot.y = 0 + GetCameraRot().y;

		bDown = true;
	}

	// 右移動
	if (CInputKeyboard::IsPress(DIK_D))
	{
		// 移動量更新
		m_velPos.x += sinf(GetCameraRot().y + D3DX_PI * 0.5f) * fMoveSpeed;
		m_velPos.z += cosf(GetCameraRot().y + D3DX_PI * 0.5f) * fMoveSpeed;

		// 目的の向きをセット
		m_destRot.y = -D3DX_PI / 2 + GetCameraRot().y;

		bRight = true;
	}

	// 左移動
	if (CInputKeyboard::IsPress(DIK_A))
	{
		m_velPos.x += -sinf(GetCameraRot().y + D3DX_PI * 0.5f) * fMoveSpeed;
		m_velPos.z += -cosf(GetCameraRot().y + D3DX_PI * 0.5f) * fMoveSpeed;

		m_destRot.y = D3DX_PI / 2 + GetCameraRot().y;

		bLeft = true;
	}

	// 左斜め奥入力時
	if (bLeft && bUp)
	{
		m_destRot.y = D3DX_PI / 2 + D3DX_PI / 4 + GetCameraRot().y;
	}

	// 左斜め手前入力時
	if (bLeft && bDown)
	{
		m_destRot.y = D3DX_PI / 4 + GetCameraRot().y;
	}

	// 右斜め奥入力時
	if (bRight && bUp)
	{
		m_destRot.y = -D3DX_PI / 2 - D3DX_PI / 4 + GetCameraRot().y;
	}

	// 右斜め手前入力時
	if (bRight && bDown)
	{
		m_destRot.y = -D3DX_PI / 4 + GetCameraRot().y;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CSceneX::Draw(void)
{
	if (m_bDrawShadowTexture == true)
	{
		DrawShadowTexture();
		return;
	}

	if (m_bDrawBlurModel == true)
	{
		DrawBlurModel();
		return;
	}

	DrawToonModel();
}

//=============================================================================
// シーンX生成処理
//=============================================================================
CSceneX* CSceneX::Create(D3DXVECTOR3 pos, LPDIRECT3DDEVICE9 pDevice, int nPriority, const char* pFileName)
{
	// インスタンス化
	CSceneX* p = new CSceneX(pDevice, nPriority,OBJTYPE_X);

	// 初期化
	p->m_pModelFileName = pFileName;
	p->Init();
	p->m_pos = pos;

	return p;
}


//=============================================================================
// 頂点バッファセット処理
//=============================================================================
void CSceneX::SetVertexPolygon(void)
{
	// 丸め込み
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
// 角度修正関数
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

void CSceneX::DrawBlurModel(void)
{
	m_pBlurShader->SetShaderToDevice();

	D3DXMATRIX w, mtxTrans;
	D3DXMATRIX v = GetViewMatrix();
	D3DXMATRIX p = GetProjectionMatrix();

	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	m_pDevice->SetVertexDeclaration(m_pDec);

	D3DXMatrixIdentity(&w);
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixRotationY(&w, m_rot.y);

	// 位置の反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&w, &w, &mtxTrans);

	D3DXMATRIX wvp;
	D3DXMATRIX invWorld;
	D3DXMatrixIdentity(&wvp);
	D3DXMatrixMultiply(&wvp, &w, &v);
	D3DXMatrixMultiply(&wvp, &wvp, &p);

	D3DXMatrixInverse(&invWorld, NULL, &w);

	D3DXVECTOR3 localLightVec;
	D3DXVECTOR3 lightDir = GetLightDir();
	D3DXCOLOR lightColor = GetLightColor();
	D3DXVec3TransformCoord(&localLightVec, &lightDir, &invWorld);

	D3DXVECTOR3 cameraVec = GetCameraVec();

	D3DXVec3Normalize(&cameraVec, &cameraVec);
	D3DXVec3Normalize(&localLightVec, &localLightVec);

	m_pBlurShader->SetWorldViewProjectionMatrix(&wvp);

	D3DXMatrixIdentity(&wvp);
	D3DXMatrixMultiply(&wvp, &m_mtxOldWorld, &v);
	D3DXMatrixMultiply(&wvp, &wvp, &p);

	m_pBlurShader->SetOldWorldViewProjectionMatrix(&wvp);


	// マテリアル情報の取得
	m_pDevice->GetMaterial(&matDef);

	// モデルのマテリアル情報の取得
	pD3DXMat = (D3DXMATERIAL *)m_pD3DMatBuffer->GetBufferPointer();

	// マテリアルごとに描画
	for (unsigned int nCntMat = 0; nCntMat < dwNumMaterials; nCntMat++)
	{
		HRESULT hr;
		// マテリアルの設定
		m_pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// モデルの描画
		m_pD3DMesh->DrawSubset(nCntMat);
	}

	// マテリアルを元に戻す
	m_pDevice->SetMaterial(&matDef);

	m_pBlurShader->EndShader();

	m_pDevice->SetTexture(0, 0);
}

void CSceneX::SetBlurTexture(LPDIRECT3DTEXTURE9 pTex)
{
	HRESULT hr;
	hr = m_pDevice->SetTexture(m_pBlurShader->GetSamplerIndex(), pTex);
}

void CSceneX::DrawToonModel(void)
{
	m_pToonShader->SetShaderToDevice();

	D3DXMATRIX w, mtxTrans;
	D3DXMATRIX v = GetViewMatrix();
	D3DXMATRIX p = GetProjectionMatrix();

	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	m_pDevice->SetVertexDeclaration(m_pDec);

	D3DXMatrixIdentity(&w);
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixRotationY(&w, m_rot.y);

	// 位置の反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&w, &w, &mtxTrans);

	D3DXMATRIX wvp;
	D3DXMATRIX invWorld;
	D3DXMatrixIdentity(&wvp);
	D3DXMatrixIdentity(&invWorld);

	D3DXMatrixMultiply(&wvp, &w, &v);
	D3DXMatrixMultiply(&wvp, &wvp, &p);

	D3DXMatrixInverse(&invWorld, NULL, &w);

	D3DXVECTOR3 localLightVec;
	D3DXVECTOR3 lightDir = GetLightDir();
	D3DXCOLOR lightColor = GetLightColor();
	D3DXVec3TransformCoord(&localLightVec, &lightDir, &invWorld);

	D3DXVECTOR3 cameraVec = GetCameraVec();

	D3DXVec3Normalize(&cameraVec, &cameraVec);
	D3DXVec3Normalize(&localLightVec, &localLightVec);

	m_pToonShader->SetLocalLightDirection(&localLightVec);
	m_pToonShader->SetWorldViewProjectionMatrix(&wvp);
	m_pToonShader->SetWorldMatrix(&w);
	m_pToonShader->SetLightDiffuse(&lightColor);
	m_pToonShader->SetLightAmbient(&D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));


	// マテリアル情報の取得
	m_pDevice->GetMaterial(&matDef);

	// モデルのマテリアル情報の取得
	pD3DXMat = (D3DXMATERIAL *)m_pD3DMatBuffer->GetBufferPointer();

	// マテリアルごとに描画
	for (unsigned int nCntMat = 0; nCntMat < dwNumMaterials; nCntMat++)
	{
		HRESULT hr;

		m_pToonShader->SetMaterialDiffuse(&pD3DXMat[nCntMat].MatD3D.Diffuse);
		m_pToonShader->SetMaterialAmbient(&D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f));

		// マテリアルの設定
		m_pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャーの設定
		m_pDevice->SetTexture(m_pToonShader->GetToonTextureSamplerIndex(), m_pD3DTex);

		// モデルの描画
		m_pD3DMesh->DrawSubset(nCntMat);
	}

	// マテリアルを元に戻す
	m_pDevice->SetMaterial(&matDef);

	m_pToonShader->EndShader();

	m_pDevice->SetTexture(0, 0);
	m_pDevice->SetTexture(m_pToonShader->GetToonTextureSamplerIndex(), 0);

}

void CSceneX::DrawShadowTexture(void)
{
	m_pShadowTextureShader->SetShaderToDevice();

	D3DXMATRIX w, mtxTrans;
	D3DXMATRIX v;
	D3DXMATRIX p;

	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	m_pDevice->SetVertexDeclaration(m_pDec);

	D3DXMatrixIdentity(&w);
	D3DXMatrixIdentity(&mtxTrans);
	D3DXMatrixRotationY(&w, m_rot.y);

	// 位置の反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&w, &w, &mtxTrans);

	D3DXVECTOR3 cameraPosP, cameraPosR, cameraVecV;
	cameraPosP = m_pos;
	cameraPosP.y += 100.0f;
	cameraPosR = m_pos;
	cameraVecV = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	D3DXMatrixIdentity(&v);

	D3DXMatrixLookAtLH(&v,
		&cameraPosP,
		&cameraPosR,
		&cameraVecV);

	D3DXMatrixIdentity(&p);
	D3DXMatrixOrthoLH(&p,256.0f,256.0f,0.0f,200.0f);

	D3DXMATRIX wvp;
	D3DXMATRIX invWorld;
	D3DXMatrixIdentity(&wvp);
	D3DXMatrixIdentity(&invWorld);

	D3DXMatrixMultiply(&wvp, &w, &v);
	D3DXMatrixMultiply(&wvp, &wvp, &p);

	D3DXMatrixInverse(&invWorld, NULL, &w);

	m_pShadowTextureShader->SetWorldViewProjectionMatrix(&wvp);

	// マテリアル情報の取得
	m_pDevice->GetMaterial(&matDef);

	// モデルのマテリアル情報の取得
	pD3DXMat = (D3DXMATERIAL *)m_pD3DMatBuffer->GetBufferPointer();

	// マテリアルごとに描画
	for (unsigned int nCntMat = 0; nCntMat < dwNumMaterials; nCntMat++)
	{
		HRESULT hr;

		// マテリアルの設定
		m_pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャーの設定
		m_pDevice->SetTexture(0, 0);

		// モデルの描画
		m_pD3DMesh->DrawSubset(nCntMat);
	}

	// マテリアルを元に戻す
	m_pDevice->SetMaterial(&matDef);

	m_pShadowTextureShader->EndShader();

	m_pDevice->SetTexture(0, 0);
}