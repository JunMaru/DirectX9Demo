#pragma once
#include "shader.h"
class CGaussianFilterShader :
	public CShader
{
public:
	CGaussianFilterShader();
	~CGaussianFilterShader();
	void Init(void);
	void Uninit(void);

	static CGaussianFilterShader* Create(void);

	void SetWidthMode(bool bWidth){ m_bWidth = bWidth; }
	void SetShaderToDevice(void);
	DWORD GetTextureSamplerIndex(void);
private:
	LPDIRECT3DPIXELSHADER9 m_pPixelShaderWidthGauss;
	LPD3DXCONSTANTTABLE m_pPixelShaderConstantTableWidthGauss;

	LPDIRECT3DPIXELSHADER9 m_pPixelShaderHeightGauss;
	LPD3DXCONSTANTTABLE m_pPixelShaderConstantTableHeightGauss;

	bool m_bWidth;
};

