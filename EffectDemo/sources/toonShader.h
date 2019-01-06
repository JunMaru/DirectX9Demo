#pragma once
#include "shader.h"
class CToonShader :
	public CShader
{
public:
	CToonShader();
	~CToonShader();

	void Init(void);
	void Uninit(void);

	static CToonShader* Create(void);

	void SetLocalLightDirection(D3DXVECTOR3* pLocalLightDirection);
	void SetWorldViewProjectionMatrix(D3DXMATRIX* pMatrix);
	void SetLightDiffuse(D3DXCOLOR* pColor);
	void SetLightAmbient(D3DXCOLOR* pColor);
	void SetMaterialAmbient(D3DXCOLOR* pColor);
	void SetMaterialDiffuse(D3DCOLORVALUE* pColor);
	void SetWorldMatrix(D3DXMATRIX* pMatrix);

	DWORD GetToonTextureSamplerIndex(void);

};

