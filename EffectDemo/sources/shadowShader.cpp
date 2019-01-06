#include "shadowShader.h"
#include "Direct3DDevice.h"

CShadowShader::CShadowShader()
{
}


CShadowShader::~CShadowShader()
{
}

void CShadowShader::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// 頂点シェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/shadowVS.hlsl", NULL, NULL, "VS", "vs_2_0", 0, &code, &err, &m_pVertexShaderConstantTable);
	hr = pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &m_pVertexShader);

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/shadowPS.hlsl", NULL, NULL, "PS", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTable);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShader);
}

void CShadowShader::Uninit(void)
{
	CShader::Uninit();
}

CShadowShader* CShadowShader::Create(void)
{
	CShadowShader* p = new CShadowShader;
	p->Init();
	return p;
}

void CShadowShader::SetLocalLightDirection(D3DXVECTOR3 * pLocalLightDirection)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "localLightDir", (float*)pLocalLightDirection, 3);
}

void CShadowShader::SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "gWVP", pMatrix);
}

void CShadowShader::SetLightDiffuse(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "LightDiffuse", (float*)pColor, 4);
}

void CShadowShader::SetLightAmbient(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "LightAmbient", (float*)pColor, 4);

}

void CShadowShader::SetMaterialAmbient(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "MatAmbient", (float*)pColor, 4);

}

void CShadowShader::SetMaterialDiffuse(D3DCOLORVALUE * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "MatDiffuse", (float*)pColor, 4);
}

DWORD CShadowShader::GetShadowTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("shadowSampler");
}

DWORD CShadowShader::GetTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("texSampler");
}

void CShadowShader::SetPlayerPos(D3DXVECTOR3* pPlayerPos)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetFloatArray(pDevice, "playerPos", (float*)pPlayerPos, 3);

}
