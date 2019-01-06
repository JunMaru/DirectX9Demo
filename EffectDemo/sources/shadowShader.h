#pragma once
#include "shader.h"
class CShadowShader :
	public CShader
{
public:
	CShadowShader();
	~CShadowShader();

	void Init(void);
	void Uninit(void);

	static CShadowShader* Create(void);

	void SetLocalLightDirection(D3DXVECTOR3* pLocalLightDirection);
	void SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix);
	void SetLightDiffuse(D3DXCOLOR* pColor);
	void SetLightAmbient(D3DXCOLOR* pColor);
	void SetMaterialAmbient(D3DXCOLOR* pColor);
	void SetMaterialDiffuse(D3DCOLORVALUE* pColor);
	void SetPlayerPos(D3DXVECTOR3* pPlayerPos);

	DWORD GetShadowTextureSamplerIndex(void);
	DWORD GetTextureSamplerIndex(void);

};