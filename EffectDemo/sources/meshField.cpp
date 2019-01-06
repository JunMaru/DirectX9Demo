//=============================================================================
//
// シーンメッシュフィールド処理 [meshField.cpp]
// Author : JUN MARUYAMA
//
//=============================================================================
//*****************************************************************************
// ワーニング対策
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "meshField.h"
#include "debugProc.h"
#include "textureManager.h"
#include <stdio.h>

#include "inputKeyboard.h"
#include "Framework.h"
#include "shadowShader.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
static bool bReadErr = false;
const D3DVERTEXELEMENT9 aVtxElem[] = {
	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
	{ 0, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

//=============================================================================
// コンストラクタ
//=============================================================================
CMeshField::CMeshField(LPDIRECT3DDEVICE9 pDevice,int nPriority) : CSceneMesh(nPriority)
{
	//座標の初期化
	m_pos.x = 0;
	m_pos.y = 0;
	m_pos.z = 0;

	//回転量の初期化
	m_rot.x = 0;
	m_rot.y = 0;
	m_rot.z = 0;

	// デバイスの設定
	m_pDevice = pDevice;

	FILE* fp = NULL;

	fp = fopen("data/MAP/map.bin", "rb");

	for (int nIdx = 0; nIdx < 4; nIdx++)
	{
		m_aPosCorners[nIdx] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	if (fp == NULL)
	{
		for (int nIdx = 0; nIdx < 51 * 51; nIdx++)
		{
			m_posBuff[nIdx].y = HIGHT_MAP[nIdx];
		}
		bReadErr = true;
	}
	else
	{
		fread(&m_posBuff, sizeof(D3DXVECTOR3), 51 * 51, fp);

		fclose(fp);
	}

	m_pDevice->CreateVertexDeclaration(aVtxElem, &m_pDec);
	m_pShadowShader = nullptr;
}

//=============================================================================
// デストラクタ
//=============================================================================
CMeshField::~CMeshField()
{
	/*
	FILE* fp;
	fp = fopen("data/MAP/map.bin", "wb");

	fwrite(&m_posBuff, sizeof(D3DXVECTOR3), 51 * 51, fp);

	fclose(fp);
*/
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CMeshField::Init(void)
{
	VERTEX_3D *pVtx;
	WORD *pIndex;

	// 例外処理
	if(m_nNumBlockZ == 0 || m_nNumBlockX == 0)
	{
		return E_FAIL;
	}

	// 総頂点数の設定
	m_nNumVertex =  ((m_nNumBlockX + 1) * (m_nNumBlockZ + 1));

	// 頂点の総インデックス数の設定
	m_nNumVertexIndex =(((3 * (m_nNumBlockX - 1)) - m_nNumBlockX + 5) * m_nNumBlockZ) + (2 * (m_nNumBlockZ - 1));

	// 総ポリゴン数の設定
	m_nNumPolygon = ((2 * m_nNumBlockX) * m_nNumBlockZ) + (4 * (m_nNumBlockZ - 1));

	// 地面情報の初期化
	m_scl = D3DXVECTOR3(1.0f,1.0f,1.0f);

	// 頂点バッファを作成
	if(FAILED(m_pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumVertex,
												D3DUSAGE_WRITEONLY,
												FVF_VERTEX_3D,
												D3DPOOL_MANAGED,
												&m_pVtxBuff,
												NULL)))
	{
		// 頂点バッファ作成失敗
		return E_FAIL;
	}

	// インデックスバッファの作成
	if(FAILED(m_pDevice->CreateIndexBuffer(sizeof(WORD) * m_nNumVertexIndex,
										D3DUSAGE_WRITEONLY,
										D3DFMT_INDEX16,
										D3DPOOL_MANAGED,
										&m_pIndexBuff,
										NULL)))
	{
		// インデックスバッファ作成失敗
		return E_FAIL;
	}

	// テクスチャを獲得
	m_pTexture = GetTextureManager()->GetTexture(m_pFileName);

	// 頂点バッファの設定
	m_pVtxBuff->Lock(0,0,(void**)&pVtx,0);

	for(int nIdx = 0,nNumY = 0;nIdx < m_nNumVertex;nIdx++)
	{
		//// 頂点座標の設定
		if (bReadErr)
		{
			pVtx[nIdx].vtx.x = (-m_fSizeBlockX * (m_nNumBlockX >> 1)) + (m_fSizeBlockX * (nIdx % (m_nNumBlockX + 1) ));
			pVtx[nIdx].vtx.z = (m_fSizeBlockZ * (m_nNumBlockZ >> 1)) - (m_fSizeBlockZ * nNumY);
			pVtx[nIdx].vtx.y = HIGHT_MAP[nIdx];

			m_posBuff[nIdx] = pVtx[nIdx].vtx;
		}
		pVtx[nIdx].vtx = m_posBuff[nIdx];

		// 面法線の設定
		if (nNumY > 0 && (nIdx % (m_nNumBlockX + 1) == m_nNumBlockX))
		{
			D3DXVECTOR3 vec1, vec2, vecAns;

			// 一列面分法線設定
			for (int nNorIdx = 0; nNorIdx < m_nNumBlockX; nNorIdx++)
			{
				// ベクトル計算(右下 - 左上)
				vec1 = pVtx[nIdx - m_nNumBlockX + nNorIdx + 1].vtx - pVtx[nIdx - (m_nNumBlockX * 2 + 1) + nNorIdx].vtx;

				// ベクトル計算(左下 - 左上)
				vec2 = pVtx[nIdx - m_nNumBlockX + nNorIdx].vtx - pVtx[nIdx - (m_nNumBlockX * 2 + 1) + nNorIdx].vtx;

				// 外積を求め正規化
				D3DXVec3Cross(&vecAns, &vec1, &vec2);
				D3DXVec3Normalize(&vecAns, &vecAns);

				// 法線バッファに格納
				m_normalBuf[((nNumY - 1) * m_nNumBlockX * 2) + (nNorIdx * 2)] = vecAns;

				// ベクトル計算(右上 - 左上)
				vec1 = pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx + 1].vtx - pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx].vtx;

				// ベクトル計算(右下 - 左上)
				vec2 = pVtx[nIdx - m_nNumBlockX + nNorIdx + 1].vtx - pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx].vtx;

				// 外積を求め正規化
				D3DXVec3Cross(&vecAns, &vec1, &vec2);
				D3DXVec3Normalize(&vecAns, &vecAns);

				// 法線バッファに格納
				m_normalBuf[((nNumY - 1) * m_nNumBlockX * 2) + (nNorIdx * 2) + 1] = vecAns;
			}
		}

		// テクスチャー表示設定
		pVtx[nIdx].tex.x = (1.0f * (nIdx % (m_nNumBlockX+ 1) ));
		pVtx[nIdx].tex.y = (1.0f * nNumY);

		// 列ずらし判定
		if(nIdx % (m_nNumBlockX + 1) == m_nNumBlockX)
		{
			nNumY++;
		}

		// 反射光設定
		pVtx[nIdx].diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);
	}

	// 頂点法線の設定
	for (int nCntZ = 0; nCntZ < m_nNumBlockZ + 1; nCntZ++)
	{
		for (int nCntX = 0; nCntX < m_nNumBlockX + 1; nCntX++)
		{
			D3DXVECTOR3 nor;

			// 一番奥の列のとき
			if (nCntZ == 0)
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[0] + m_normalBuf[1];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[(m_nNumBlockX * 2) - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntX * 2) - 1] + m_normalBuf[(nCntX * 2)] + m_normalBuf[(nCntX * 2) + 1];
				}
			}

			// 一番手前の列のとき
			else if (nCntZ == m_nNumBlockZ)
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[(nCntZ * m_nNumBlockX * 2) - 2] + m_normalBuf[(nCntZ * m_nNumBlockX * 2) - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 2] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2)];
				}
			}

			// それ以外
			else
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2] + m_normalBuf[nCntZ* m_nNumBlockX * 2] + m_normalBuf[nCntZ* m_nNumBlockX * 2 + 1];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[nCntZ * m_nNumBlockX * 2 - 2] + m_normalBuf[nCntZ * m_nNumBlockX * 2 - 1] + m_normalBuf[(nCntZ + 1) * m_nNumBlockX * 2 - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 2] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2)] +
						m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2)] + m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2) + 1];

				}
			}

			// 法線を正規化し格納
			D3DXVec3Normalize(&nor, &nor);
			pVtx[nCntX + (nCntZ * (m_nNumBlockX + 1))].nor = nor;
		}
	}

	// 四隅の座標の保存
	m_posLeftFront = pVtx[0].vtx;
	m_posRightFront = pVtx[m_nNumBlockX].vtx;
	m_posLeftBack = pVtx[(m_nNumVertex - 1) - m_nNumBlockX].vtx;
	m_posRightBack = pVtx[m_nNumVertex - 1].vtx;

	// 頂点バッファの設定終了
	m_pVtxBuff->Unlock();

	// インデックスバッファの設定
	m_pIndexBuff->Lock(0,0,(void **)&pIndex,0);

	for(int nIdx = 0,nNum = 0,nNumGap = 0;nIdx < m_nNumVertexIndex;nIdx++)
	{
		// 頂点が重複していないとき
		if(nIdx % (((m_nNumBlockX + 1)) * 2) != nNumGap || nIdx == 0)
		{
			pIndex[nIdx] = static_cast<WORD>(((nIdx + 1) % 2) * (m_nNumBlockX + 1) + nNum);
			nNum += (nIdx % 2);
		}

		// 頂点が重複しているとき
		else
		{
			// 重複１つめ
			pIndex[nIdx] = pIndex[nIdx - 1];
			nIdx++;

			// 重複２つめ
			pIndex[nIdx] = static_cast<WORD>(((nIdx) % 2) * (m_nNumBlockX + 1) + nNum);
			nIdx++;

			// 通常１つめ
			pIndex[nIdx] = static_cast<WORD>(((nIdx + 1) % 2) * (m_nNumBlockX + 1) + nNum);
			nNum += (nIdx % 2);
			nIdx++;

			// 通常２つめ
			pIndex[nIdx] = static_cast<WORD>(((nIdx + 1) % 2) * (m_nNumBlockX + 1) + nNum);
			nNum += (nIdx % 2);

			// 誤差
			nNumGap += 2;
		}
	}

	// インデックスバッファの設定終了
	m_pIndexBuff->Unlock();

	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールの反映
	D3DXMatrixScaling(&mtxScl, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScl);

	// 回転の反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置の反映
	D3DXMatrixTranslation(&mtxTranslate, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTranslate);

	// 回転した後の四隅の座標を保存
	D3DXVec3TransformCoord(&m_aPosCorners[0], &m_posLeftFront, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[1], &m_posRightFront, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[2], &m_posRightBack, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[3], &m_posLeftBack, &m_mtxWorld);

	SetBuffer();

	m_pShadowShader = CShadowShader::Create();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CMeshField::Uninit(void)
{
	// 頂点バッファの開放
	if(m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// インデックスバッファの開放
	if(m_pIndexBuff != NULL)
	{
		m_pIndexBuff->Release();
		m_pIndexBuff = NULL;
	}

	m_pShadowShader->Uninit();
	delete m_pShadowShader;

	// リストから削除
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshField::Update(void)
{
	// 回転した後の四隅の座標を保存
	m_posLeftFront = D3DXVECTOR3(-745.0f, 0.0f, 745.0f);
	m_posRightFront = D3DXVECTOR3(745.0f, 0.0f, 745.0f);
	m_posLeftBack = D3DXVECTOR3(-745.0f, 0.0f, -745.0f);
	m_posRightBack = D3DXVECTOR3(745.0f, 0.0f, -745.0f);

	D3DXVec3TransformCoord(&m_aPosCorners[0], &m_posLeftFront, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[1], &m_posRightFront, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[2], &m_posRightBack, &m_mtxWorld);
	D3DXVec3TransformCoord(&m_aPosCorners[3], &m_posLeftBack, &m_mtxWorld);
}

//=============================================================================
// 描画処理
//=============================================================================
void CMeshField::Draw(void)
{
	HRESULT hr;
	D3DXMATRIX mtxScl,mtxRot,mtxTranslate;

	D3DXMATRIX proj, view;

	proj = GetProjectionMatrix();
	view = GetViewMatrix();
	hr = m_pDevice->SetTransform(D3DTS_VIEW, &view);

	hr = m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// ワールドマトリックスを初期化
	D3DXMatrixIdentity(&m_mtxWorld);	

	// スケールの反映
	D3DXMatrixScaling(&mtxScl,m_scl.x,m_scl.y,m_scl.z);
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,&mtxScl);

	// 回転の反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,m_rot.y,m_rot.x,m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,&mtxRot);

	// 位置の反映
	D3DXMatrixTranslation(&mtxTranslate,m_pos.x,m_pos.y,m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld,&m_mtxWorld,&mtxTranslate);

	// ワールドマトリックスの設定
	hr = m_pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームにバインド
	hr = m_pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをバインド
	hr = m_pDevice->SetIndices(m_pIndexBuff);

	// 頂点フォーマットの設定
	hr = m_pDevice->SetFVF(FVF_VERTEX_3D);
	//m_pDevice->SetTexture(0, m_pTexture);

	//m_pDevice->SetVertexDeclaration(m_pDec);

	m_pShadowShader->SetShaderToDevice();

	D3DXMATRIX wvp;
	D3DXMATRIX invWorld;
	D3DXMatrixIdentity(&wvp);
	D3DXMatrixIdentity(&invWorld);

	D3DXMatrixMultiply(&wvp, &m_mtxWorld, &view);
	D3DXMatrixMultiply(&wvp, &wvp, &proj);

	D3DXMatrixInverse(&invWorld, NULL, &m_mtxWorld);

	D3DXVECTOR3 localLightVec;
	D3DXVECTOR3 lightDir = GetLightDir();
	D3DXCOLOR lightColor = GetLightColor();
	D3DXVec3TransformCoord(&localLightVec, &lightDir, &invWorld);

	D3DXVECTOR3 cameraVec = GetCameraVec();

	D3DXVec3Normalize(&cameraVec, &cameraVec);
	D3DXVec3Normalize(&localLightVec, &localLightVec);

	m_pShadowShader->SetLocalLightDirection(&localLightVec);
	m_pShadowShader->SetWorldViewProjectionMatrix(&wvp);
	//m_pShadowShader->SetWorldMatrix(&m_mtxWorld);
	m_pShadowShader->SetLightDiffuse(&lightColor);
	m_pShadowShader->SetLightAmbient(&D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
	m_pShadowShader->SetMaterialAmbient(&D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	m_pShadowShader->SetPlayerPos(&m_playerPos);


	m_pDevice->SetTexture(m_pShadowShader->GetShadowTextureSamplerIndex(), m_pShadowTexture);
	m_pDevice->SetTexture(m_pShadowShader->GetTextureSamplerIndex(), m_pTexture);

	m_pDevice->SetSamplerState(m_pShadowShader->GetShadowTextureSamplerIndex(), D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);	// テクスチャアドレッシング方法(U値)を設定
	m_pDevice->SetSamplerState(m_pShadowShader->GetShadowTextureSamplerIndex(), D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);	// テクスチャアドレッシング方法(V値)を設定

	// ポリゴン描画
	hr = m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
								0,
								0,
								m_nNumVertex,
								0,
								m_nNumPolygon);

	m_pDevice->SetSamplerState(m_pShadowShader->GetShadowTextureSamplerIndex(), D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	m_pDevice->SetSamplerState(m_pShadowShader->GetShadowTextureSamplerIndex(), D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定


	m_pShadowShader->EndShader();
	
}

//=============================================================================
// シーン3D生成処理
//=============================================================================
CMeshField* CMeshField::Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot,int nNumBlockX,int nNumBlockZ,float fSizeBlockX,float fSizeBlockZ,
										LPDIRECT3DDEVICE9 pDevice,int nPriority,const char* pFileName)
{
	CMeshField* p = new CMeshField(pDevice,nPriority);

	p->m_pos = pos;
	p->m_rot = rot;
	p->m_nNumBlockX = nNumBlockX;
	p->m_nNumBlockZ = nNumBlockZ;
	p->m_fSizeBlockX = fSizeBlockX;
	p->m_fSizeBlockZ = fSizeBlockZ;
	p->m_pFileName = pFileName;

	p->Init();

	return p;
}

//=============================================================================
// 高さ取得処理
//=============================================================================
float CMeshField::GetHeight(const D3DXVECTOR3& pos, D3DXVECTOR3* pNormal)
{
	int nGridX, nGridZ;
	D3DXVECTOR3 gridCorners[4];

	// 座標から現在のグリッドを求める
	nGridX = static_cast<int>((pos.x + m_nNumBlockX * m_fSizeBlockX * 0.5f) / m_fSizeBlockX);
	nGridZ = static_cast<int>((-pos.z + m_nNumBlockZ * m_fSizeBlockZ * 0.5f) / m_fSizeBlockZ);

	if (nGridZ >= 50)
	{
		nGridZ = 49;
	}

	// 現在の座標から四隅を求める
	gridCorners[1].x = nGridX * m_fSizeBlockX - (m_fSizeBlockX * m_nNumBlockX * 0.5f);
	gridCorners[1].z = -nGridZ * m_fSizeBlockZ + (m_fSizeBlockZ * m_nNumBlockZ * 0.5f);
	gridCorners[1].y = m_posBuff[nGridZ * (m_nNumBlockX + 1) + nGridX].y;

	gridCorners[3].x = nGridX * m_fSizeBlockX - (m_fSizeBlockX * m_nNumBlockX * 0.5f) + m_fSizeBlockX;
	gridCorners[3].z = -nGridZ * m_fSizeBlockZ + (m_fSizeBlockZ * m_nNumBlockZ * 0.5f);
	gridCorners[3].y = m_posBuff[nGridZ * (m_nNumBlockX + 1) + nGridX + 1].y;

	gridCorners[0].x = nGridX * m_fSizeBlockX - (m_fSizeBlockX * m_nNumBlockX * 0.5f);
	gridCorners[0].z = -nGridZ * m_fSizeBlockZ + (m_fSizeBlockZ * m_nNumBlockZ * 0.5f) - m_fSizeBlockZ;
	gridCorners[0].y = m_posBuff[(nGridZ + 1) * (m_nNumBlockX + 1) + nGridX].y;

	gridCorners[2].x = nGridX * m_fSizeBlockX - (m_fSizeBlockX * m_nNumBlockX * 0.5f) + m_fSizeBlockX;
	gridCorners[2].z = -nGridZ * m_fSizeBlockZ + (m_fSizeBlockZ * m_nNumBlockZ * 0.5f) - m_fSizeBlockZ;
	gridCorners[2].y = m_posBuff[(nGridZ + 1) * (m_nNumBlockX + 1) + nGridX + 1].y;

	D3DXVECTOR2 vecLine;					// 四隅の点と点を結んだベクトル(辺)
	D3DXVECTOR2 vecToTarget;				// 弾へのベクトル
	
	int nHitCnt = 0;

	// 場外判定
	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		// ベクトル作成
		vecLine = D3DXVECTOR2(gridCorners[(nCnt + 1) % 3].x - gridCorners[nCnt].x, gridCorners[(nCnt + 1) % 3].z - gridCorners[nCnt].z);
		vecToTarget = D3DXVECTOR2(pos.x - gridCorners[nCnt].x, pos.z - gridCorners[nCnt].z);

		if (Vec2Cross(vecLine, vecToTarget) <= 0)
		{
			nHitCnt++;

			// 左下の場合
			if (nHitCnt == 3)
			{
#if 0
				if (CInputKeyboard::IsPress(DIK_RETURN))
				{
					m_posBuff[nGridX + (nGridZ + 1) * (m_nNumBlockX + 1) + 1].y++;
				}
				if (CInputKeyboard::IsPress(DIK_BACKSPACE))
				{
					m_posBuff[nGridX + (nGridZ + 1) * (m_nNumBlockX + 1) + 1].y--;
				}
#endif
				if (pNormal == NULL)
				{
					return GetHeightPolygon(gridCorners[0], gridCorners[1], gridCorners[2], pos, &m_normalBuf[nGridX * 2 + nGridZ * m_nNumBlockX * 2]);
				}
				else
				{
					return GetHeightPolygon(gridCorners[0], gridCorners[1], gridCorners[2], pos, pNormal);
				}
			}
		}

		else
		{
			break;
		}

	}

	// 右上の場合
#if 0
	if (CInputKeyboard::IsPress(DIK_RETURN))
	{
		m_posBuff[nGridX + (nGridZ + 1) * (m_nNumBlockX + 1) + 1].y++;
	}

	if (CInputKeyboard::IsPress(DIK_BACKSPACE))
	{
		m_posBuff[nGridX + (nGridZ + 1) * (m_nNumBlockX + 1) + 1].y--;
	}
#endif
	if (pNormal == NULL)
	{
		return GetHeightPolygon(gridCorners[2], gridCorners[3], gridCorners[1], pos, &m_normalBuf[nGridX * 2 + nGridZ * m_nNumBlockX * 2 + 1]);
	}
	else
	{
		return GetHeightPolygon(gridCorners[2], gridCorners[3], gridCorners[1], pos, pNormal);
	}
}

//=============================================================================
// 面高さ取得処理
//=============================================================================
float CMeshField::GetHeightPolygon(const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& pos, D3DXVECTOR3* pNormal)
{
	D3DXVECTOR3 vec0, vec1, normal;

	if (pNormal == NULL)
	{
		vec0 = p1 - p0;
		vec1 = p2 - p0;
		D3DXVec3Cross(&normal, &vec0, &vec1);
		D3DXVec3Normalize(&normal, &normal);

		if (normal.y == 0)
		{
			return 0.0f;
		}

		return (p0.y - (normal.x * (pos.x - p0.x) + normal.z * (pos.z - p0.z) / normal.y));
	}
	else
	{
		return p0.y - (pNormal->x * (pos.x - p0.x) + pNormal->z * (pos.z - p0.z)) / pNormal->y;
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void CMeshField::SetBuffer(void)
{
	VERTEX_3D *pVtx;

	// 頂点バッファの設定
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nIdx = 0, nNumY = 0; nIdx < m_nNumVertex; nIdx++)
	{
		pVtx[nIdx].vtx = m_posBuff[nIdx];

		// 面法線の設定
		if (nNumY > 0 && (nIdx % (m_nNumBlockX + 1) == m_nNumBlockX))
		{
			D3DXVECTOR3 vec1, vec2, vecAns;

			// 一列面分法線設定
			for (int nNorIdx = 0; nNorIdx < m_nNumBlockX; nNorIdx++)
			{
				// ベクトル計算(右下 - 左上)
				vec1 = pVtx[nIdx - m_nNumBlockX + nNorIdx + 1].vtx - pVtx[nIdx - (m_nNumBlockX * 2 + 1) + nNorIdx].vtx;

				// ベクトル計算(左下 - 左上)
				vec2 = pVtx[nIdx - m_nNumBlockX + nNorIdx].vtx - pVtx[nIdx - (m_nNumBlockX * 2 + 1) + nNorIdx].vtx;

				// 外積を求め正規化
				D3DXVec3Cross(&vecAns, &vec1, &vec2);
				D3DXVec3Normalize(&vecAns, &vecAns);

				// 法線バッファに格納
				m_normalBuf[((nNumY - 1) * m_nNumBlockX * 2) + (nNorIdx * 2)] = vecAns;

				// ベクトル計算(右上 - 左上)
				vec1 = pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx + 1].vtx - pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx].vtx;

				// ベクトル計算(右下 - 左上)
				vec2 = pVtx[nIdx - m_nNumBlockX + nNorIdx + 1].vtx - pVtx[nIdx - (2 * m_nNumBlockX + 1) + nNorIdx].vtx;

				// 外積を求め正規化
				D3DXVec3Cross(&vecAns, &vec1, &vec2);
				D3DXVec3Normalize(&vecAns, &vecAns);

				// 法線バッファに格納
				m_normalBuf[((nNumY - 1) * m_nNumBlockX * 2) + (nNorIdx * 2) + 1] = vecAns;
			}
		}

		// 列ずらし判定
		if (nIdx % (m_nNumBlockX + 1) == m_nNumBlockX)
		{
			nNumY++;
		}
	}

	// 頂点法線の設定
	for (int nCntZ = 0; nCntZ < m_nNumBlockZ + 1; nCntZ++)
	{
		for (int nCntX = 0; nCntX < m_nNumBlockX + 1; nCntX++)
		{
			D3DXVECTOR3 nor;

			// 一番奥の列のとき
			if (nCntZ == 0)
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[0] + m_normalBuf[1];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[(m_nNumBlockX * 2) - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntX * 2) - 1] + m_normalBuf[(nCntX * 2)] + m_normalBuf[(nCntX * 2) + 1];
				}
			}

			// 一番手前の列のとき
			else if (nCntZ == m_nNumBlockZ)
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[(nCntZ * m_nNumBlockX * 2) - 2] + m_normalBuf[(nCntZ * m_nNumBlockX * 2) - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 2] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2)];
				}
			}

			// それ以外
			else
			{
				// 一番左のとき
				if (nCntX == 0)
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2] + m_normalBuf[nCntZ* m_nNumBlockX * 2] + m_normalBuf[nCntZ* m_nNumBlockX * 2 + 1];
				}

				// 一番右のとき
				else if (nCntX == m_nNumBlockX)
				{
					nor = m_normalBuf[nCntZ * m_nNumBlockX * 2 - 2] + m_normalBuf[nCntZ * m_nNumBlockX * 2 - 1] + m_normalBuf[(nCntZ + 1) * m_nNumBlockX * 2 - 1];
				}

				// それ以外
				else
				{
					nor = m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 2] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[(nCntZ - 1) * m_nNumBlockX * 2 + (nCntX * 2)] +
						m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2) - 1] + m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2)] + m_normalBuf[nCntZ * m_nNumBlockX * 2 + (nCntX * 2) + 1];

				}
			}

			// 法線を正規化し格納
			D3DXVec3Normalize(&nor, &nor);
			pVtx[nCntX + (nCntZ * (m_nNumBlockX + 1))].nor = nor;
		}
	}

	// 頂点バッファの設定終了
	m_pVtxBuff->Unlock();
}