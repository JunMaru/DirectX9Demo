#include "normalMapShader.h"
#include "Direct3DDevice.h"
#include "Framework.h"



CNormalMapShader::CNormalMapShader()
{
}


CNormalMapShader::~CNormalMapShader()
{
}

void CNormalMapShader::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// 頂点シェーダーの作成
	hr = D3DXCompileShaderFromFile("normalMapVS.hlsl", NULL, NULL, "VS", "vs_3_0", 0, &code, &err, &m_pVertexShaderConstantTable);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
		return;
	}

	hr = pDevice->CreateVertexShader((DWORD*)code->GetBufferPointer(), &m_pVertexShader);

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("normalMapPS.hlsl", NULL, NULL, "PS", "ps_3_0", 0, &code, &err, &m_pPixelShaderConstantTable);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
		return;
	}

	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
		return;
	}

}

void CNormalMapShader::Uninit(void)
{
	CShader::Uninit();
}

CNormalMapShader* CNormalMapShader::Create(void)
{
	CNormalMapShader* p = new CNormalMapShader;
	p->Init();
	return p;
}

void CNormalMapShader::SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "wvp", pMatrix);
}

void CNormalMapShader::SetLightDiffuse(D3DXCOLOR * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pPixelShaderConstantTable->SetFloatArray(pDevice, "lightColor", (float*)pColor, 4);
}


void CNormalMapShader::SetMaterialDiffuse(D3DCOLORVALUE * pColor)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pPixelShaderConstantTable->SetFloatArray(pDevice, "matDiffuse", (float*)pColor, 4);

}

void CNormalMapShader::SetWorldMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "w", pMatrix);
	m_pPixelShaderConstantTable->SetMatrix(pDevice, "w", pMatrix);
}

void CNormalMapShader::SetWorldInverseTransposeMatrix(D3DXMATRIX* pMatrix)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pVertexShaderConstantTable->SetMatrix(pDevice, "wit", pMatrix);
}

void CNormalMapShader::SetWorldLightVector(D3DXVECTOR3* pWorldLightVector)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pPixelShaderConstantTable->SetFloatArray(pDevice, "worldLightVec", (float*)pWorldLightVector, 3);
}

void CNormalMapShader::SetWorldCameraVector(D3DXVECTOR3* pWorldCameraVector)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	m_pPixelShaderConstantTable->SetFloatArray(pDevice, "worldCameraVec", (float*)pWorldCameraVector, 3);
}

DWORD CNormalMapShader::GetNormalTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("normalSampler");
}

DWORD CNormalMapShader::GetDiffuseTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTable->GetSamplerIndex("texSampler");
}