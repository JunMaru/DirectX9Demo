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

// 2Dポリゴン頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define	FVF_VERTEX_3D (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct
{
	D3DXVECTOR3 vtx;			//頂点座標
	float rhw;					//値は1.0fで固定
	D3DCOLOR diffuse;			//反射光
	D3DXVECTOR2 tex;			//テクスチャ
}VERTEX_2D;

typedef struct{
	D3DXVECTOR3 xyz;
	D3DXVECTOR3 nor;
	D3DCOLOR	color;
	D3DXVECTOR2 uv;
}VERTEX_3D;


static HWND _hWnd = NULL;
static const int kModelMax = 3;

LPDIRECT3DTEXTURE9 g_pTexture;
LPDIRECT3DTEXTURE9 g_pTexture2;
D3DXVECTOR3 g_lightDir(0,-1,-1);
D3DXCOLOR g_lightColor(0.9f, 0.9f, 0.9f, 1.0f);

CTextureManager* g_pTexManager = nullptr;

D3DXMATRIX g_ViewMatrix;
D3DXMATRIX g_ProjectionMatrix;
D3DXVECTOR3 g_posP = D3DXVECTOR3(0, 0, -2.0f);
D3DXVECTOR3 g_posR = D3DXVECTOR3(0, 0, 0);
D3DXVECTOR3 g_vecV = D3DXVECTOR3(0, 1, 0);
D3DXVECTOR3 g_cameraRot = D3DXVECTOR3(0, 0, 0);
D3DXVECTOR3 g_cameraVec;
D3DXVECTOR3 g_rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
float g_fCameraRange;
CSceneX* g_pModel;
CSceneX* g_pBox;
CSceneX* g_pHead;
CSceneX* g_pPenguin;
CInputKeyboard* g_pKeyboard;
CDebugProc* g_pDebugProc;

LPDIRECT3DTEXTURE9 g_pRenderTexture[2];
LPDIRECT3DSURFACE9 g_pRenderTextureSurface[2];
LPDIRECT3DSURFACE9 g_pRenderDepthStencilSurface[2];
LPDIRECT3DSURFACE9 g_pBackBuffRenderTargetSurface;
LPDIRECT3DSURFACE9 g_pBackBuffDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pShadowTexture;
LPDIRECT3DSURFACE9 g_pShadowTextureSurface;
LPDIRECT3DSURFACE9 g_pShadowTextureDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pNormalDrawTexture;
LPDIRECT3DSURFACE9 g_pNormalDrawTextureSurface;
LPDIRECT3DSURFACE9 g_pNormalDrawDepthStencilSurface;

LPDIRECT3DTEXTURE9 g_pBloomTexture[3];
LPDIRECT3DSURFACE9 g_pBloomTextureSurface[3];
LPDIRECT3DSURFACE9 g_pBloomDepthStencilSurface[3];
D3DVIEWPORT9 g_bloomViewport[3];

LPDIRECT3DPIXELSHADER9 g_pPixelShader;
LPD3DXCONSTANTTABLE g_pPixelShaderConstantTable;

LPDIRECT3DVERTEXSHADER9 g_pBasicVertexShader;
LPD3DXCONSTANTTABLE g_pBasicVertexShaderConstantTable;
LPDIRECT3DPIXELSHADER9 g_pBasicPixelShader;
LPD3DXCONSTANTTABLE g_pBasicPixelShaderConstantTable;


D3DVIEWPORT9 g_backViewport;
D3DVIEWPORT9 g_shadowViewport;

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

D3DXVECTOR3 GetCameraPos(void)
{
	return g_posP;
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
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

																	// プロジェクションマトリックスを初期化
	D3DXMatrixIdentity(&g_ProjectionMatrix);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_ProjectionMatrix,
		D3DX_PI / 4,							// 視野角を45度に設定
		float(800) / float(600),		// アスペクト比を設定
		0.1f,											// near値を設定
		10000.0f);										// far値を設定

														// ビューマトリックスを初期化
	D3DXMatrixIdentity(&g_ViewMatrix);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_ViewMatrix,
		&g_posP,
		&g_posR,
		&g_vecV);

	g_cameraVec = g_posR - g_posP;

	g_pBox = CSceneX::Create(D3DXVECTOR3(0,0,0),Direct3DDevice_getDevice(),3,"data/MODEL/box.x");
	g_pModel = g_pBox;

	g_pKeyboard = new CInputKeyboard;
	g_pKeyboard->Init(hinstance, hWnd);

	g_fCameraRange = sqrt(pow((g_posP.x - g_posR.x), 2) + pow((g_posP.z - g_posR.z), 2));

	g_pDebugProc = new CDebugProc;
	g_pDebugProc->Init(pDevice);

	InitRenderTarget();

	pDevice->GetViewport(&g_backViewport);
	g_shadowViewport = g_backViewport;
	g_shadowViewport.Height = 256;
	g_shadowViewport.Width = 256;

	return true;
}

bool Framework_update(void)
{
	g_pKeyboard->Update();
	CDebugProc::ClearBuffer();
	UpdateCamera();
	//ChangeModel();

	CDebugProc::Print("モデルの回転[w][a][s][d]\n");
	CDebugProc::Print("カメラの回転[u][o]\n");

	D3DXMatrixIdentity(&g_ViewMatrix);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_ViewMatrix,
		&g_posP,
		&g_posR,
		&g_vecV);

	g_cameraVec = g_posR - g_posP;

	g_pModel->Update();

	return true;
}

void Framework_draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	pDevice->BeginScene();

	pDevice->SetRenderTarget(0, g_pBackBuffRenderTargetSurface);
	pDevice->SetRenderTarget(1, NULL);
	pDevice->SetDepthStencilSurface(g_pBackBuffDepthStencilSurface);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.0f, 0.0f,0.0f, 1.0f), 1.0f, 0);

	g_pModel->Draw();

	CDebugProc::Draw();

	pDevice->EndScene();

	pDevice->Present(NULL, NULL, _hWnd, NULL);
}

void Framework_uninit(void)
{
	g_pBox->Uninit();
	delete g_pBox;

	Direct3DDevice_release();
	g_pTexManager->Uninit();
	delete g_pTexManager;

	g_pKeyboard->Uninit();
	delete g_pKeyboard;

	g_pDebugProc->Uninit();
	delete g_pDebugProc;

	g_pRenderTextureSurface[0]->Release();
	g_pRenderTextureSurface[1]->Release();
	g_pRenderTexture[0]->Release();
	g_pRenderTexture[1]->Release();
	g_pRenderDepthStencilSurface[0]->Release();
	g_pRenderDepthStencilSurface[1]->Release();
	g_pBackBuffDepthStencilSurface->Release();
	g_pBackBuffRenderTargetSurface->Release();

	g_pShadowTexture->Release();
	g_pShadowTextureSurface->Release();
	g_pShadowTextureDepthStencilSurface->Release();

	g_pNormalDrawTexture->Release();
	g_pNormalDrawTextureSurface->Release();
	g_pNormalDrawDepthStencilSurface->Release();
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
		g_pModel = g_pBox;
		break;
	case 1:
		g_pModel = g_pHead;
		break;
	case 2:
		g_pModel = g_pPenguin;
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
	destPos.y = 1.0f;
	destPos.x = destPos.x + -sinf(g_cameraRot.y) * 2.0f;
	destPos.z = destPos.z + -cosf(g_cameraRot.y) * 2.0f;
	
	g_posP.x += (destPos.x - g_posP.x) * 0.9f;
	g_posP.y += (destPos.y - g_posP.y) * 0.9f;
	g_posP.z += (destPos.z - g_posP.z) * 0.9f;

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

	for (int nCnt = 0; nCnt < 3; nCnt++)
	{
		int nRate = (nCnt + 1) * (nCnt + 1);

		// レンダーターゲット用のテクスチャ生成
		if (FAILED(pDevice->CreateTexture(640 / nRate, 480 / nRate, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pBloomTexture[nCnt], NULL)))
		{
			return E_FAIL;
		}

		// テクスチャサーフェス取得
		if (FAILED(g_pBloomTexture[nCnt]->GetSurfaceLevel(0, &g_pBloomTextureSurface[nCnt])))
		{
			return E_FAIL;
		}

		// デプス・ステンシルバッファサーフェス取得
		if (FAILED(pDevice->CreateDepthStencilSurface(640 / nRate, 480 / nRate, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &g_pBloomDepthStencilSurface[nCnt], NULL)))
		{
			return E_FAIL;
		}

		// サーフェイスのクリア
		pDevice->SetRenderTarget(0, g_pBloomTextureSurface[nCnt]);
		pDevice->SetDepthStencilSurface(g_pBloomDepthStencilSurface[nCnt]);
		pDevice->Clear(0, NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f, 0);

		g_bloomViewport[nCnt] = g_backViewport;
		g_bloomViewport[nCnt].Height = 640 / nRate;
		g_bloomViewport[nCnt].Width = 480 / nRate;
	}

	// レンダーターゲットを元に戻す
	pDevice->SetRenderTarget(0, g_pBackBuffRenderTargetSurface);
	pDevice->SetDepthStencilSurface(g_pBackBuffDepthStencilSurface);

	return S_OK;
}
