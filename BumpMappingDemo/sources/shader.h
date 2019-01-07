#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

class CShader
{
public:
	CShader();
	virtual ~CShader();

	virtual void Init(void) = 0;
	virtual void Uninit(void);

	virtual void SetShaderToDevice(void);
	virtual void EndShader(void);

protected:
	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	LPD3DXCONSTANTTABLE m_pPixelShaderConstantTable;
	LPD3DXCONSTANTTABLE m_pVertexShaderConstantTable;
};

