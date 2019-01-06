#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include "common.h"
#include "Direct3DDevice.h"
#include "textureManager.h"
#include "sceneX.h"
#include "inputKeyboard.h"
#include "debugProc.h"
#include "meshField.h"
#include "gaussianFilterShader.h"
#include "blur.h"

// 2Dポリゴン頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct
{
	D3DXVECTOR3 vtx;			//頂点座標
	float rhw;					//値は1.0fで固定
	D3DCOLOR diffuse;			//反射光
	D3DXVECTOR2 tex;			//テクスチャ
}VERTEX_2D;

static HWND _hWnd = NULL;
static const int kModelMax = 4;
static const int kRadialBlurCount = 60;
static const int kFlashCount = 60;

D3DXVECTOR3 g_lightDir(-1,-1,0);
D3DXCOLOR g_lightColor(0.9f, 0.9f, 0.9f, 1.0f);

CTextureManager* g_pTexManager = nullptr;

D3DXMATRIX g_ViewMatrix;
D3DXMATRIX g_ProjectionMatrix;
D3DXVECTOR3 g_posP = D3DXVECTOR3(0, 100, -150);
D3DXVECTOR3 g_posR = D3DXVECTOR3(0, 0, 0);
D3DXVECTOR3 g_vecV = D3DXVECTOR3(0, 1, 0);
D3DXVECTOR3 g_cameraRot = D3DXVECTOR3(0, 0, 0);
D3DXVECTOR3 g_cameraVec;
float g_fCameraRange;
CSceneX* g_pModel;
CSceneX* g_pKuma;
CSceneX* g_pHead;
CSceneX* g_pPenguin;
CSceneX* g_pDog;
CInputKeyboard* g_pKeyboard;
CDebugProc* g_pDebugProc;
CMeshField* g_pField;

LPDIRECT3DTEXTURE9 g_pRenderTexture[2];
LPDIRECT3DSURFACE9 g_pRenderTextureSurface[2];
LPDIRECT3DSURFACE9 g_pRenderDepthStencilSurface[2];
LPDIRECT3DSURFACE9 g_pBackBuffRenderTargetSurface;
LPDIRECT3DSURFACE9 g_pBackBuffDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pEdgeTexture;
LPDIRECT3DSURFACE9 g_pEdgeTextureSurface;
LPDIRECT3DSURFACE9 g_pEdgeDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pEdgeBlurTexture;
LPDIRECT3DSURFACE9 g_pEdgeBlurTextureSurface;
LPDIRECT3DSURFACE9 g_pEdgeBlurDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pShadowTexture;
LPDIRECT3DSURFACE9 g_pShadowTextureSurface;
LPDIRECT3DSURFACE9 g_pShadowTextureDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pNormalDrawTexture;
LPDIRECT3DSURFACE9 g_pNormalDrawTextureSurface;
LPDIRECT3DSURFACE9 g_pNormalDrawDepthStencilSurface;

LPDIRECT3DPIXELSHADER9 g_pEdgePixelShader;
LPD3DXCONSTANTTABLE g_pEdgePixelShaderConstantTable;
LPDIRECT3DPIXELSHADER9 g_pRadialBlurPixelShader;
LPD3DXCONSTANTTABLE g_pRadialBlurPixelShaderConstantTable;

D3DVIEWPORT9 g_backViewport;
D3DVIEWPORT9 g_shadowViewport;

bool g_bRadialBlur = false;
bool g_bRadialBlurMove = false;
int g_nRadialBlurCnt = 0;

void DrawShadow(void);
void DrawGaussian(void);
void DrawBlur(bool bTexWrite);
void DrawRadialBlur(void);
void DrawEdge(void);

CTextureManager* GetTextureManager(void)
{
	return g_pTexManager;
}

D3DXVECTOR3 GetLightDir(void)
{
	return g_lightDir;
}

D3DXCOLOR GetLightColor(void)
{
	return g_lightColor;
}

D3DXMATRIX GetProjectionMatrix(void)
{
	return g_ProjectionMatrix;
}

D3DXMATRIX GetViewMatrix(void)
{
	return g_ViewMatrix;
}

D3DXVECTOR3 GetCameraVec(void)
{
	return g_cameraVec;
}

CMeshField* GetField(void)
{
	return g_pField;
}

D3DXVECTOR3 GetCameraRot(void)
{
	return g_cameraRot;
}

void ChangeModel(void);
void UpdateCamera(void);
float FixRot(float fRot);

HRESULT InitRenderTarget(void);

bool Framework_init(HWND hWnd,HINSTANCE hinstance)
{
	if( !Direct3DDevice_init(hWnd) ) {
		return false;
	}

	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	if( pDevice == NULL ) {
		return false;
	}

	g_pTexManager = new CTextureManager();
	g_pTexManager->Init();

	_hWnd = hWnd;

	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;

	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// テクスチャ縮小フィルタモードを設定
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);		// テクスチャ拡大フィルタモードを設定

	D3DXMatrixIdentity(&g_ProjectionMatrix);		// プロジェクションマトリックスを初期化

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_ProjectionMatrix,
		D3DX_PI / 4,							// 視野角を45度に設定
		float(800) / float(600),		// アスペクト比を設定
		10.0f,											// near値を設定
		10000.0f);										// far値を設定

														// ビューマトリックスを初期化
	D3DXMatrixIdentity(&g_ViewMatrix);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_ViewMatrix,
		&g_posP,
		&g_posR,
		&g_vecV);

	g_cameraVec = g_posR - g_posP;

	g_pPenguin = CSceneX::Create(D3DXVECTOR3(0, 0, 0), Direct3DDevice_getDevice(), 3, "data/MODEL/penguinVer1.2.x");
	g_pModel = g_pPenguin;

	g_pKeyboard = new CInputKeyboard;
	g_pKeyboard->Init(hinstance, hWnd);

	g_fCameraRange = sqrt(pow((g_posP.x - g_posR.x), 2) + pow((g_posP.z - g_posR.z), 2));

	g_pDebugProc = new CDebugProc;
	g_pDebugProc->Init(pDevice);

	InitRenderTarget();

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/edgePS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &g_pEdgePixelShaderConstantTable);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &g_pEdgePixelShader);

	hr = D3DXCompileShaderFromFile("data/EFFECT/radialBlurPS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &g_pRadialBlurPixelShaderConstantTable);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &g_pRadialBlurPixelShader);

	g_pField = CMeshField::Create(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 50, 50, 50.0f, 50.0f, pDevice, 0, "data/TEXTURE/field000.jpg");

	pDevice->GetViewport(&g_backViewport);
	g_shadowViewport = g_backViewport;
	g_shadowViewport.Height = 256;
	g_shadowViewport.Width = 256;

	g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "blurPow",0.0f);

	return true;
}

bool Framework_update(void)
{
	g_pKeyboard->Update();
	CDebugProc::ClearBuffer();

	D3DXMatrixIdentity(&g_ViewMatrix);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_ViewMatrix,
		&g_posP,
		&g_posR,
		&g_vecV);

	g_cameraVec = g_posR - g_posP;

	g_pModel->Update();

	if (g_pKeyboard->IsTrigger(DIK_1) && g_bRadialBlurMove == false)
	{
		g_bRadialBlur = true;
		return true;
	}

	if (g_pKeyboard->IsTrigger(DIK_2) && g_bRadialBlur == false)
	{
		g_bRadialBlurMove = true;
		return true;
	}

	return true;
}

void Framework_draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	if (SUCCEEDED(pDevice->BeginScene()))
	{
		DrawShadow();

		pDevice->SetRenderTarget(0, g_pRenderTextureSurface[0]);
		pDevice->SetRenderTarget(1, g_pRenderTextureSurface[1]);
		pDevice->SetDepthStencilSurface(g_pRenderDepthStencilSurface[0]);

		pDevice->SetViewport(&g_backViewport);
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
		g_pModel->Draw();
		g_pField->SetShadowTexture(g_pShadowTexture);
		g_pField->SetPlayerPos(g_pModel->GetPos());
		g_pField->Draw();

		DrawEdge();

		if (g_bRadialBlur == false && g_bRadialBlurMove == false)
		{
			DrawBlur(false);
		}

		else if (g_bRadialBlur == true)
		{
			DrawBlur(true);
			DrawRadialBlur();
			g_nRadialBlurCnt++;

			g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "blurPow", 5.0f);

			if (g_nRadialBlurCnt > kRadialBlurCount)
			{
				g_nRadialBlurCnt = 0;
				g_bRadialBlur = false;
				g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "blurPow", 0.0f);
			}
		}

		else if (g_bRadialBlurMove == true)
		{
			DrawBlur(true);
			DrawRadialBlur();
			g_nRadialBlurCnt++;

			if (g_nRadialBlurCnt > kRadialBlurCount)
			{
				g_nRadialBlurCnt = 0;
				g_bRadialBlurMove = false;
			}
			g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "blurPow", float(g_nRadialBlurCnt) * 0.4f);

		}

		pDevice->EndScene();
	}
	pDevice->Present(NULL, NULL, _hWnd, NULL);
}

void Framework_uninit(void)
{
	g_pPenguin->Uninit();
	delete g_pPenguin;

	Direct3DDevice_release();
	g_pTexManager->Uninit();
	delete g_pTexManager;

	g_pKeyboard->Uninit();
	delete g_pKeyboard;

	g_pDebugProc->Uninit();
	delete g_pDebugProc;

	g_pField->Uninit();
	delete g_pField;

	g_pRenderTextureSurface[0]->Release();
	g_pRenderTextureSurface[1]->Release();
	g_pRenderTexture[0]->Release();
	g_pRenderTexture[1]->Release();
	g_pRenderDepthStencilSurface[0]->Release();
	g_pRenderDepthStencilSurface[1]->Release();
	g_pBackBuffDepthStencilSurface->Release();
	g_pBackBuffRenderTargetSurface->Release();

	g_pEdgeTexture->Release();
	g_pEdgeTextureSurface->Release();
	g_pEdgeDepthStencilSurface->Release();

	g_pEdgeBlurTexture->Release();
	g_pEdgeBlurTextureSurface->Release();
	g_pEdgeBlurDepthStencilSurface->Release();

	g_pShadowTexture->Release();
	g_pShadowTextureSurface->Release();
	g_pShadowTextureDepthStencilSurface->Release();

	g_pNormalDrawTexture->Release();
	g_pNormalDrawTextureSurface->Release();
	g_pNormalDrawDepthStencilSurface->Release();

	g_pEdgePixelShader->Release();
	g_pEdgePixelShaderConstantTable->Release();
	g_pRadialBlurPixelShader->Release();
	g_pRadialBlurPixelShaderConstantTable->Release();
}

void ChangeModel(void)
{
	static int nCnt = 0;
	CSceneX::SHADERNO shaderNo;

	if (CInputKeyboard::IsTrigger(DIK_LEFTARROW))
	{
		nCnt--;

		if (nCnt < 0)
		{
			nCnt = kModelMax - 1;
		}
		g_pModel->SetRotY(0.0f);
	}

	if (CInputKeyboard::IsTrigger(DIK_RIGHTARROW))
	{
		nCnt++;

		if (nCnt >= kModelMax)
		{
			nCnt = 0;
		}
		g_pModel->SetRotY(0.0f);
	}

	
	switch (nCnt)
	{
	case 0:
		g_pModel = g_pKuma;
		break;
	case 1:
		g_pModel = g_pHead;
		break;
	case 2:
		g_pModel = g_pPenguin;
		break;
	case 3:
		g_pModel = g_pDog;
	default:
		break;
	}
}

void UpdateCamera(void)
{
	if (CInputKeyboard::IsPress(DIK_U))
	{
		g_cameraRot.y -= 0.05f;
	}

	if (CInputKeyboard::IsPress(DIK_O))
	{
		g_cameraRot.y += 0.05f;
	}

	g_cameraRot.y = FixRot(g_cameraRot.y);


	// 視点の更新
	D3DXVECTOR3 destPos = g_pModel->GetPos();
	D3DXVECTOR3 destPosR = g_pModel->GetPos();
	destPos.y += 150.0f;

	destPos.x = destPos.x + -sinf(g_cameraRot.y) * 200.0f;
	destPos.z = destPos.z + -cosf(g_cameraRot.y) * 200.0f;
	
	g_posP.x += (destPos.x - g_posP.x) * 0.9f;
	g_posP.y += (destPos.y - g_posP.y) * 0.9f;
	g_posP.z += (destPos.z - g_posP.z) * 0.9f;

	destPosR.x = destPosR.x - sinf(g_cameraRot.y) * 20.0f;
	destPosR.z = destPosR.z - cosf(g_cameraRot.y) * 20.0f;

	// 注視点の更新
	g_posR.x += (destPosR.x - g_posR.x) * 0.2f;
	g_posR.y += (destPosR.y - g_posR.y) * 0.2f;
	g_posR.z += (destPosR.z - g_posR.z) * 0.2f;

	g_fCameraRange = sqrt(pow((g_posP.x - g_posR.x), 2) + pow((g_posP.z - g_posR.z), 2));

}

float FixRot(float fRot)
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

HRESULT InitRenderTarget(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// バックバッファサーフェス取得
	pDevice->GetRenderTarget(0, &g_pBackBuffRenderTargetSurface);
	pDevice->GetDepthStencilSurface(&g_pBackBuffDepthStencilSurface);

	// レンダーターゲット用のテクスチャ生成
	if (FAILED(pDevice->CreateTexture(256, 256, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pShadowTexture, NULL)))
	{
		return E_FAIL;
	}

	// テクスチャサーフェス取得
	if (FAILED(g_pShadowTexture->GetSurfaceLevel(0, &g_pShadowTextureSurface)))
	{
		return E_FAIL;
	}

	// デプス・ステンシルバッファサーフェス取得
	if (FAILED(pDevice->CreateDepthStencilSurface(256, 256, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pShadowTextureDepthStencilSurface, NULL)))
	{
		return E_FAIL;
	}

	pDevice->SetRenderTarget(0, g_pShadowTextureSurface);
	pDevice->SetDepthStencilSurface(g_pShadowTextureDepthStencilSurface);
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f, 0);


	// レンダーターゲット用のテクスチャ生成
	if (FAILED(pDevice->CreateTexture(640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pNormalDrawTexture, NULL)))
	{
		return E_FAIL;
	}

	// テクスチャサーフェス取得
	if (FAILED(g_pNormalDrawTexture->GetSurfaceLevel(0, &g_pNormalDrawTextureSurface)))
	{
		return E_FAIL;
	}

	// デプス・ステンシルバッファサーフェス取得
	if (FAILED(pDevice->CreateDepthStencilSurface(640, 480, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pNormalDrawDepthStencilSurface, NULL)))
	{
		return E_FAIL;
	}

	pDevice->SetRenderTarget(0, g_pNormalDrawTextureSurface);
	pDevice->SetDepthStencilSurface(g_pNormalDrawDepthStencilSurface);
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f, 0);

	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		// レンダーターゲット用のテクスチャ生成
		if (FAILED(pDevice->CreateTexture(640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRenderTexture[nCnt], NULL)))
		{
			return E_FAIL;
		}

		// テクスチャサーフェス取得
		if (FAILED(g_pRenderTexture[nCnt]->GetSurfaceLevel(0, &g_pRenderTextureSurface[nCnt])))
		{
			return E_FAIL;
		}

		// デプス・ステンシルバッファサーフェス取得
		if (FAILED(pDevice->CreateDepthStencilSurface(640, 480, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pRenderDepthStencilSurface[nCnt], NULL)))
		{
			return E_FAIL;
		}

		// サーフェイスのクリア
		pDevice->SetRenderTarget(0, g_pRenderTextureSurface[nCnt]);
		pDevice->SetDepthStencilSurface(g_pRenderDepthStencilSurface[nCnt]);
		pDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f, 0);
	}

	// レンダーターゲット用のテクスチャ生成
	if (FAILED(pDevice->CreateTexture(640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pEdgeTexture, NULL)))
	{
		return E_FAIL;
	}

	// テクスチャサーフェス取得
	if (FAILED(g_pEdgeTexture->GetSurfaceLevel(0, &g_pEdgeTextureSurface)))
	{
		return E_FAIL;
	}

	// デプス・ステンシルバッファサーフェス取得
	if (FAILED(pDevice->CreateDepthStencilSurface(640, 480, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pEdgeDepthStencilSurface, NULL)))
	{
		return E_FAIL;
	}

	// サーフェイスのクリア
	pDevice->SetRenderTarget(0, g_pEdgeTextureSurface);
	pDevice->SetDepthStencilSurface(g_pEdgeDepthStencilSurface);
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f, 0);


	// レンダーターゲット用のテクスチャ生成
	if (FAILED(pDevice->CreateTexture(640, 480, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pEdgeBlurTexture, NULL)))
	{
		return E_FAIL;
	}

	// テクスチャサーフェス取得
	if (FAILED(g_pEdgeBlurTexture->GetSurfaceLevel(0, &g_pEdgeBlurTextureSurface)))
	{
		return E_FAIL;
	}

	// デプス・ステンシルバッファサーフェス取得
	if (FAILED(pDevice->CreateDepthStencilSurface(640, 480, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pEdgeBlurDepthStencilSurface, NULL)))
	{
		return E_FAIL;
	}

	// サーフェイスのクリア
	pDevice->SetRenderTarget(0, g_pEdgeBlurTextureSurface);
	pDevice->SetDepthStencilSurface(g_pEdgeBlurDepthStencilSurface);
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f, 0);


	// レンダーターゲットを元に戻す
	pDevice->SetRenderTarget(0, g_pBackBuffRenderTargetSurface);
	pDevice->SetDepthStencilSurface(g_pBackBuffDepthStencilSurface);

	return S_OK;
}

void DrawShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	pDevice->SetRenderTarget(0, g_pShadowTextureSurface);
	pDevice->SetDepthStencilSurface(g_pShadowTextureDepthStencilSurface);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
	pDevice->SetViewport(&g_shadowViewport);
	g_pModel->SetDrawShadowTexture(true);
	g_pModel->Draw();
	g_pModel->SetDrawShadowTexture(false);
}

void DrawBlur(bool bTexWrite)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	if (bTexWrite == false)
	{
		pDevice->SetRenderTarget(0, g_pBackBuffRenderTargetSurface);
		pDevice->SetRenderTarget(1, NULL);
		pDevice->SetDepthStencilSurface(g_pBackBuffDepthStencilSurface);
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
	}
	else
	{
		pDevice->SetRenderTarget(0, g_pEdgeBlurTextureSurface);
		pDevice->SetRenderTarget(1, NULL);
		pDevice->SetDepthStencilSurface(g_pEdgeBlurDepthStencilSurface);
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
	}

	pDevice->SetFVF(FVF_VERTEX_2D);
	VERTEX_2D vtx[4] = { { D3DXVECTOR3(0, 0, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 0) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, 0, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 0) },
	{ D3DXVECTOR3(0, (SCREEN_HEIGHT)-0.5f, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 1) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, (SCREEN_HEIGHT)-0.5f, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 1) } };

	pDevice->SetPixelShader(NULL);
	pDevice->SetVertexShader(NULL);
	pDevice->SetTexture(0, g_pEdgeTexture);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vtx[0], sizeof(VERTEX_2D));

	pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);

	g_pModel->SetBlurTexture(g_pEdgeTexture);
	g_pModel->SetBlurFlg(true);
	g_pModel->Draw();
	g_pModel->SetBlurFlg(false);
}

void DrawRadialBlur(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	float TU = 1.0f / 640.0f;
	float TV = 1.0f / 480.0f;
	float fCenter[2] = { 0.5f,0.5f };

	g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "TU", TU);
	g_pRadialBlurPixelShaderConstantTable->SetFloat(pDevice, "TV", TV);
	g_pRadialBlurPixelShaderConstantTable->SetFloatArray(pDevice, "center", fCenter, 2);

	pDevice->SetRenderTarget(0, g_pBackBuffRenderTargetSurface);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetDepthStencilSurface(g_pBackBuffDepthStencilSurface);
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);

	pDevice->SetFVF(FVF_VERTEX_2D);
	VERTEX_2D vtx[4] = { { D3DXVECTOR3(0, 0, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 0) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, 0, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 0) },
	{ D3DXVECTOR3(0, (SCREEN_HEIGHT)-0.5f, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 1) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, (SCREEN_HEIGHT)-0.5f, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 1) } };

	pDevice->SetPixelShader(g_pRadialBlurPixelShader);
	pDevice->SetVertexShader(NULL);
	pDevice->SetTexture(g_pRadialBlurPixelShaderConstantTable->GetSamplerIndex("radialBlurSampler"), g_pEdgeBlurTexture);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vtx[0], sizeof(VERTEX_2D));
	pDevice->SetPixelShader(NULL);
	pDevice->SetVertexShader(NULL);
}

void DrawEdge(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	pDevice->SetRenderTarget(0, g_pEdgeTextureSurface);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetDepthStencilSurface(g_pEdgeDepthStencilSurface);

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 0);
	pDevice->SetFVF(FVF_VERTEX_2D);
	VERTEX_2D vtx[4] = { { D3DXVECTOR3(0, 0, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 0) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, 0, 0.0f),   1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 0) },
	{ D3DXVECTOR3(0, (SCREEN_HEIGHT)-0.5f, 0.0f), 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(0, 1) },
	{ D3DXVECTOR3((SCREEN_WIDTH)-0.5f, (SCREEN_HEIGHT)-0.5f, 0.0f),  1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.9f), D3DXVECTOR2(1, 1) } };

	pDevice->SetPixelShader(g_pEdgePixelShader);
	HRESULT hr = pDevice->SetTexture(g_pEdgePixelShaderConstantTable->GetSamplerIndex("ZSampler"), g_pRenderTexture[1]);
	pDevice->SetTexture(g_pEdgePixelShaderConstantTable->GetSamplerIndex("texSampler"), g_pRenderTexture[0]);
	pDevice->SetTexture(0, g_pRenderTexture[0]);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vtx[0], sizeof(VERTEX_2D));
	pDevice->SetPixelShader(NULL);
}
