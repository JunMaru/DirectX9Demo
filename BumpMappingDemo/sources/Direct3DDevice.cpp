#include <d3d9.h>
#include <tchar.h>
#include "config.h"
#include "common.h"

static LPDIRECT3DDEVICE9 _pDevice = NULL; // Direct3DDevice�C���^�[�t�F�[�X


bool Direct3DDevice_init(HWND hWnd)
{
	// Direct3D�̏�����
	LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( !pD3D ) {
		// Direct3D�̃C���^�[�t�F�[�X�̎擾�Ɏ��s
		MessageBox(hWnd, _T("Direct3D�C���^�[�t�F�[�X�̎擾�Ɏ��s"), _T("���s"), MB_OK);
		return false;
	}

	// �f�o�C�X�̃p�����[�^�ݒ�
	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &_pDevice) ) ) {
		// ���̃}�V���ł�Direct3D�͓����Ȃ�...
		MessageBox(hWnd, _T("Direct3D���Ή��ł��I"), _T("�����ł�"), MB_OK);
		return false;
	}

	pD3D->Release();

	return true;
}

void Direct3DDevice_release(void)
{
	SAFE_RELEASE(_pDevice);
}

LPDIRECT3DDEVICE9 Direct3DDevice_getDevice(void)
{
	return _pDevice;
}

