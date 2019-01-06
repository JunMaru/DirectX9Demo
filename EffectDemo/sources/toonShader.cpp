#include "toonShader.h"
#include "Direct3DDevice.h"
#include "Framework.h"

CToonShader::CToonShader()
{
}


CToonShader::~CToonShader()
{
}

void CToonShader::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// 頂点シェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/toonVS.hlsl", NULL, NULL, "VS", "vs_2_0", 0, &code, &err, &m_pVertexShaderConstantTable);
	hr = pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &m_pVertexShader);

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/toonPS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTable);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShader);
}

void CToonShader::Uninit(void)
{
	CShader::Uninit();
}

CToonShader* CToonShader::Create(void)
{
	CToonShader* p = new CToonShader;
	p->Init();
	return p;
}

void CToonShader::SetLocalLightDirection(D3DXVECTOR3 * pLocalLightDirection)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "localLightDir", (float*)pLocalLightDirection, 3);
}

void CToonShader::SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gWVP", pMatrix);
}

void CToonShader::SetLightDiffuse(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "LightDiffuse", (float*)pColor, 4);
}

void CToonShader::SetLightAmbient(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "LightAmbient", (float*)pColor, 4);

}

void CToonShader::SetMaterialAmbient(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "MatAmbient", (float*)pColor, 4);

}

void CToonShader::SetMaterialDiffuse(D3DCOLORVALUE * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "MatDiffuse", (float*)pColor, 4);

}

void CToonShader::SetWorldMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gWorld", pMatrix);

}

DWORD CToonShader::GetToonTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("texSampler");
}