#include "shadowTexture.h"
#include "Direct3DDevice.h"
#include "Framework.h"

CShadowTexture::CShadowTexture()
{
}


CShadowTexture::~CShadowTexture()
{
}

void CShadowTexture::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// 頂点シェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/shadowTextureVS.hlsl", NULL, NULL, "VS", "vs_2_0", 0, &code, &err, &m_pVertexShaderConstantTable);
	hr = pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &m_pVertexShader);

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/shadowTexturePS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTable);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShader);
}

void CShadowTexture::Uninit(void)
{
	CShader::Uninit();
}

CShadowTexture* CShadowTexture::Create(void)
{
	CShadowTexture* p = new CShadowTexture;
	p->Init();
	return p;
}

void CShadowTexture::SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gWVP", pMatrix);
}