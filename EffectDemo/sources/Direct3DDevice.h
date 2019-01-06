#pragma once

#include <windows.h>
#include <d3d9.h>

bool Direct3DDevice_init(HWND hWnd);
void Direct3DDevice_release(void);
LPDIRECT3DDEVICE9 Direct3DDevice_getDevice(void);
