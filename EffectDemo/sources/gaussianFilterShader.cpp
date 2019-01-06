#include "gaussianFilterShader.h"
#include "Direct3DDevice.h"

CGaussianFilterShader::CGaussianFilterShader()
{
	m_bWidth = false;
}


CGaussianFilterShader::~CGaussianFilterShader()
{
}

void CGaussianFilterShader::Init(void)
{
	HRESULT hr;
	LPD3DXBUFFER err;
	LPD3DXBUFFER code;
	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();

	// ピクセルシェーダーの作成
	hr = D3DXCompileShaderFromFile("data/EFFECT/gaussianFilterPS.hlsl", NULL, NULL, "PSWIDTH", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTableWidthGauss);
	if (FAILED(hr))
	{
		MessageBox(NULL, (LPCSTR)err->GetBufferPointer(), "D3DXCompileShaderFromFile", MB_OK);
		err->Release();
		return;
	}

	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShaderWidthGauss);

	hr = D3DXCompileShaderFromFile("data/EFFECT/gaussianFilterPS.hlsl", NULL, NULL, "PSHEIGHT", "ps_2_0", 0, &code, &err, &m_pPixelShaderConstantTableHeightGauss);
	hr = pDevice->CreatePixelShader((DWORD*)code->GetBufferPointer(), &m_pPixelShaderHeightGauss);
}

void CGaussianFilterShader::Uninit(void)
{
	m_pPixelShaderWidthGauss->Release();
	m_pPixelShaderConstantTableWidthGauss->Release();

	m_pPixelShaderHeightGauss->Release();
	m_pPixelShaderConstantTableHeightGauss->Release();
}

CGaussianFilterShader* CGaussianFilterShader::Create(void)
{
	CGaussianFilterShader* p = new CGaussianFilterShader;
	p->Init();
	return p;
}

void CGaussianFilterShader::SetShaderToDevice(void)
{

	LPDIRECT3DDEVICE9 pDevice = Direct3DDevice_getDevice();
	HRESULT hr;

	if (m_bWidth)
	{
		hr = pDevice->SetPixelShader(m_pPixelShaderWidthGauss);
		return;
	}

	hr = pDevice->SetPixelShader(m_pPixelShaderHeightGauss);
}

DWORD CGaussianFilterShader::GetTextureSamplerIndex(void)
{
	return m_pPixelShaderConstantTableWidthGauss->GetSamplerIndex("texSampler");
}
