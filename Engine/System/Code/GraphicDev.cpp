#include "Header\GraphicDev.h"


IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev()
	: m_pSDK(nullptr), m_pGraphicDev(nullptr), m_d3dpp(), m_pSprite(nullptr), m_pFont(nullptr)
{

}


CGraphicDev::~CGraphicDev()
{

}

HRESULT CGraphicDev::Ready_GraphicDev(CGraphicDev** ppGraphicClass, HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY)
{
	// 1 : 버전 확인
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	NULL_CHECK_RETURN(m_pSDK, E_FAIL);




	// 2 : HAL, 버텍스 프로세싱
	D3DCAPS9	DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	FAILED_CHECK_RETURN(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps), E_FAIL);

	_ulong	dwFlag = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	else
		dwFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;




	// 3 : 패러미터 세팅
	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	m_d3dpp.BackBufferWidth = iSizeX;
	m_d3dpp.BackBufferHeight = iSizeY;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality = 0;

	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow = hWnd;

	m_d3dpp.Windowed = eMode;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;




	// 4 : 장치 생성
	FAILED_CHECK_RETURN(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwFlag, &m_d3dpp, &m_pGraphicDev), E_FAIL);

	*ppGraphicClass = this;




	// 5 : 스프라이트
	FAILED_CHECK_MSG(D3DXCreateSprite(m_pGraphicDev, &m_pSprite), L"Create Sprite Failed");




	// 6 : 폰트
	D3DXFONT_DESCW			tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESCW));

	tFontInfo.Height = 20;
	tFontInfo.Width = 10;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"궁서");

	FAILED_CHECK_MSG(D3DXCreateFontIndirect(m_pGraphicDev, &tFontInfo, &m_pFont), L"Create Font Failed");

	return m_dwReady = S_OK;
}

HRESULT CGraphicDev::Render_Begin(D3DXCOLOR Color)
{
	HRESULT hr = S_OK;
	hr = m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
						Color, 1.f, 0);

	hr = m_pGraphicDev->BeginScene();

	return hr;
}

HRESULT CGraphicDev::Render_End()
{
	HRESULT hr = S_OK;
	hr = m_pGraphicDev->EndScene();

	//RECT srcRect = { 0, 0, m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight };
	//hr = m_pGraphicDev->Present(&srcRect, NULL, NULL, NULL);
	hr = m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);

	return hr;
}

HRESULT CGraphicDev::Reset_GraphicDev()
{
	return m_pGraphicDev->Reset(&m_d3dpp);
}

void CGraphicDev::Free()
{
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pSDK);
	m_dwReady = E_FAIL;
}
