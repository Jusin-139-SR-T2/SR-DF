//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif

#include "framework.h"
#include "MainApp.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "ImguiMgr.h"
#include "Logo.h"

CMainApp::CMainApp()
	: m_pDeviceClass(nullptr), m_pGraphicDev(nullptr), m_pManagementClass(nullptr)
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp()
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	FAILED_CHECK_RETURN(CImguiMgr::GetInstance()->Ready_Imgui(&m_pDeviceClass, &m_pGraphicDev), E_FAIL);
	
	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	m_pManagementClass->Update_Scene(fTimeDelta);


	CImguiMgr::GetInstance()->Update_Imgui(fTimeDelta);
	
	
	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	Engine::LateUpdate_Scene();
}

void CMainApp::Render_MainApp()
{
	HRESULT result = S_OK;
	D3DXCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
	
	// [렌더 시작]
	if ((result = Engine::Render_Begin(clear_col_dx)) >= 0)
	{
		// [오브젝트 렌더] 렌더러에 요청해서 그려야할 오브젝트들을 그린다.
		Engine::Render_Scene(m_pGraphicDev);

		CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();
		LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
		LPDIRECT3DSURFACE9 pRenderTarget = nullptr;
		D3DSURFACE_DESC desc;

		pImguiMgr->Get_GraphicDev()->GetBackBuffer(0, 0U, D3DBACKBUFFER_TYPE::D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		pImguiMgr->Get_VecRenderTargetTex(0)->GetSurfaceLevel(0, &pRenderTarget);
		pImguiMgr->Get_GraphicDev()->StretchRect(pBackBuffer, NULL, pRenderTarget, NULL, D3DTEXF_NONE);
		pBackBuffer->GetDesc(&desc);

		Safe_Release(pBackBuffer);
		Safe_Release(pRenderTarget);

		D3DVIEWPORT9 viewport = {0, 0, 2560, 1440, 0.f, 1.f};
		//m_pGraphicDev->GetViewport(&viewport);
		m_pGraphicDev->SetViewport(&viewport);

		// [IMGUI 렌더]
		CImguiMgr::GetInstance()->Render_Imgui();
	}

	// 추가 뷰포트 렌더
	CImguiMgr::GetInstance()->Render_AdditionImgui();

	// [렌더 종료] 스왑 체인을 하여 백버퍼를 앞으로 불러들이고 종료시킨다.
	result = Engine::Render_End();
	
	// 핸들을 잃었을 때 실행시키는 코드
	CImguiMgr::GetInstance()->Render_LossHandle(result);
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement)
{
	Engine::CScene* pScene = nullptr;

	pScene = CLogo::Create(pGraphicDev);
	NULL_CHECK_RETURN(pScene, E_FAIL);

	FAILED_CHECK_RETURN(Engine::Create_Management(pGraphicDev, ppManagement), E_FAIL);
	(*ppManagement)->AddRef();

	FAILED_CHECK_RETURN((*ppManagement)->Set_Scene(pScene), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev)
{
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(&m_pDeviceClass, g_hWnd, MODE_WIN, 2560, 1440), E_FAIL);
	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// InputDev
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// Font
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Default", L"바탕", 15, 20, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Jinji", L"궁서", 30, 30, FW_THIN), E_FAIL);

	// Renderer
	FAILED_CHECK_RETURN(Engine::Ready_Renderer(), E_FAIL);

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	ThisClass* pInstance = new	 ThisClass;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("MainApp Create Failed");
		Engine::Safe_Release(pInstance);

		return nullptr;
	}
	return pInstance;
}

void CMainApp::Free()
{
	CImguiMgr::GetInstance()->DestroyInstance();

	// 장치 제거
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	// 루트 매니저 클래스 제거
	Safe_Release(m_pManagementClass);

	// dll 싱글톤 제거
	Engine::Release_Utility();
	Engine::Release_System();
}

void CMainApp::CleanupDeviceD3D()
{
	if (m_pGraphicDev) { m_pGraphicDev->Release(); m_pGraphicDev = nullptr; }
	if (m_pSDK) { m_pSDK->Release(); m_pSDK = nullptr; }
}

void CMainApp::ResetDevice(_uint dwResizeWidth, _uint dwResizeHeight)
{
	CImguiMgr::GetInstance()->ResetDevice(dwResizeWidth, dwResizeHeight);
}

bool CMainApp::LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight)
{
	LPDIRECT3DTEXTURE9 texture;
	/*HRESULT hr = D3DXCreateTextureFromFileW(m_pGraphicDev, pFileName, &texture);
	if (hr != S_OK)
		return false;

	D3DSURFACE_DESC tImageDesc;
	texture->GetLevelDesc(0, &tImageDesc);
	*pOutTex = texture;
	*pOutWidth = (_int)tImageDesc.Width;
	*pOutHeight = (_int)tImageDesc.Height;*/

	if (FAILED(m_pGraphicDev->CreateTexture(800, 600, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL)))
	{
		return false;
	}

	m_pGraphicDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pOriginalRenderTarget);
	LPDIRECT3DSURFACE9 pRenderTarget = NULL;

	texture->GetSurfaceLevel(0, &pRenderTarget);

	m_pGraphicDev->SetRenderTarget(0, pRenderTarget);
	pRenderTarget->Release();

	D3DLOCKED_RECT lockedRect;
	HRESULT hr = texture->LockRect(0, &lockedRect, NULL, 0);
	
	if (SUCCEEDED(hr))
	{
		texture->UnlockRect(0);
	}
	else
		return false;

	//m_pOriginalRenderTarget->GetDesc(&tImageDesc);
	D3DXSaveTextureToFileW(pFileName, D3DXIFF_BMP, texture, NULL);

	return true;
}