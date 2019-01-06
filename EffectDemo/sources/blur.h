#pragma once
#include "shader.h"
class CBlur :
	public CShader
{
public:
	CBlur();
	~CBlur();

	void Init(void);
	void Uninit(void);

	static CBlur* Create(void);
	void SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix);
	void SetOldWorldViewProjectionMatrix(D3DXMATRIX* pMatrix);
	DWORD GetSamplerIndex(void);
};

