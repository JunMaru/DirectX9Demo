#pragma once
#include "shader.h"
class CShadowTexture :
	public CShader
{
public:
	CShadowTexture();
	~CShadowTexture();

	void Init(void);
	void Uninit(void);

	static CShadowTexture* Create(void);

	void SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix);
};

