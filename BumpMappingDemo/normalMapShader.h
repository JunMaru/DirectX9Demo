#pragma once
#include "shader.h"
class CNormalMapShader :
	public CShader
{
public:
	CNormalMapShader();
	~CNormalMapShader();
	void Init(void);
	void Uninit(void);
	static CNormalMapShader * Create(void);
	void SetWorldViewProjectionMatrix(D3DXMATRIX * pMatrix);
	void SetLightDiffuse(D3DXCOLOR * pColor);
	void SetMaterialDiffuse(D3DCOLORVALUE * pColor);
	void SetWorldMatrix(D3DXMATRIX * pMatrix);
	void SetWorldInverseTransposeMatrix(D3DXMATRIX * pMatrix);
	void SetWorldLightVector(D3DXVECTOR3 * pWorldLightVector);
	void SetWorldCameraVector(D3DXVECTOR3 * pWorldCameraVector);
	DWORD GetNormalTextureSamplerIndex(void);
	DWORD GetDiffuseTextureSamplerIndex(void);
};

