#include "shader.h"
#include "Direct3DDevice.h"


CShader::CShader()
{
}

CShader::~CShader()
{
}

void CShader::Uninit(void)
{
	m_pPixelShader->Release();
	m_pPixelShaderConstantTable->Release();
	m_pVertexShader->Release();
	m_pVertexShaderConstantTable->Release();
}

void CShader::SetShaderToDevice(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	HRESULT hr;
	hr = pDevice->SetVertexShader(m_pVertexShader);
	hr =pDevice->SetPixelShader(m_pPixelShader);
}

void CShader::EndShader(void)
{
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	HRESULT hr;
	hr = pDevice->SetVertexShader(NULL);
	hr = pDevice->SetPixelShader(NULL);

}
