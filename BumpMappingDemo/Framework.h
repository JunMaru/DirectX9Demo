#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

#include <windows.h>
#include "textureManager.h"

bool Framework_init(HWND hWnd, HINSTANCE hinstance);
bool Framework_update(void);
void Framework_draw(void);
void Framework_uninit(void);

CTextureManager* GetTextureManager(void);
D3DXCOLOR GetLightColor(void);
D3DXVECTOR3 GetLightDir(void);
D3DXMATRIX GetProjectionMatrix(void);
D3DXMATRIX GetViewMatrix(void);
D3DXVECTOR3 GetCameraVec(void);
D3DXVECTOR3 GetCameraRot(void);
D3DXVECTOR3 GetCameraPos(void);

#endif // _FRAMEWORK_H_
