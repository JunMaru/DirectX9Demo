#include "blur.h"
#include "Direct3DDevice.h"


CBlur::CBlur()
{
}


CBlur::~CBlur()
{
}

void CBlur::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// 頂点シェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/blurVS.hlsl", NULL, NULL, "VS", "vs_2_0", 0, &code, &err, &m_pVertexShaderConstantTable);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
	}

	hr = pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &m_pVertexShader);

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/blurPS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTable);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
	}

	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShader);
}

void CBlur::Uninit(void)
{
	m_pPixelShader->Release();
	m_pPixelShaderConstantTable->Release();

	m_pVertexShader->Release();
	m_pVertexShaderConstantTable->Release();
}

CBlur* CBlur::Create(void)
{
	CBlur* p = new CBlur;
	p->Init();
	return p;
}

DWORD CBlur::GetSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("blurSampler");
}

void CBlur::SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gWVP", pMatrix);
}

void CBlur::SetOldWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gOldWVP", pMatrix);
}
