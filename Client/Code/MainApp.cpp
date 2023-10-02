#include "stdafx.h"
#include "MainApp.h"

#include "Logo.h"


CMainApp::CMainApp()
	: m_pDeviceClass(nullptr), m_pGraphicDev(nullptr), m_pManagementClass(nullptr)
{
}

CMainApp::~CMainApp()
{
}

CMainApp* CMainApp::Create()
{
	ThisClass* pInstance = new ThisClass;

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MSG_BOX("MainApp Create Failed");
		Engine::Safe_Release(pInstance);

		return nullptr;
	}
	return pInstance;
}


HRESULT CMainApp::Ready_MainApp()
{
	FAILED_CHECK_RETURN(SetUp_DefaultSetting(&m_pGraphicDev), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Scene(m_pGraphicDev, &m_pManagementClass), E_FAIL);

	// ��ġ���� ���÷��� ����� �� �ְ� �����ϴ� �ɼ� (��Ƽ �ٸ����)
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphicDev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	// ���̷�Ʈ ��ǲ ������Ʈ
	Engine::Update_KeyMgr();

	// �� ������Ʈ
	m_pManagementClass->Update_Scene(fTimeDelta);

	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	Engine::LateUpdate_Scene();
	Engine::LateUpdate_KeyMgr();		// ����Ű �ý��� ������Ʈ
}

void CMainApp::Render_MainApp()
{
	Engine::Render_Begin(D3DXCOLOR(0.f, 0.f, 1.f, 1.f));

	Engine::Render_Scene(m_pGraphicDev);

	Engine::Render_End();
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
	FAILED_CHECK_RETURN(Engine::Ready_GraphicDev(&m_pDeviceClass, g_hWnd, MODE_WIN), E_FAIL);
	m_pDeviceClass->AddRef();

	(*ppGraphicDev) = m_pDeviceClass->Get_GraphicDev();
	(*ppGraphicDev)->AddRef();

	(*ppGraphicDev)->SetRenderState(D3DRS_LIGHTING, FALSE);

	// InputDev
	FAILED_CHECK_RETURN(Engine::Ready_InputDev(g_hInst, g_hWnd), E_FAIL);

	// Font
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Default", L"����", 15, 20, FW_HEAVY), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Font((*ppGraphicDev), L"Font_Jinji", L"�ü�", 30, 30, FW_THIN), E_FAIL);

	// Renderer
	FAILED_CHECK_RETURN(Engine::Ready_Renderer(), E_FAIL);

	// TextureMgr
	FAILED_CHECK_RETURN(Engine::Ready_TextureMgr(*ppGraphicDev), E_FAIL);

	// PhysicsMgr
	FAILED_CHECK_RETURN(Engine::Ready_PhysicsMgr(1), E_FAIL);

	return S_OK;
}

void CMainApp::Free()
{
	// ��ġ ����
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	// ��Ʈ �Ŵ��� Ŭ���� ����
	Safe_Release(m_pManagementClass);

	// dll �̱��� ����
	Engine::Release_Utility();
	Engine::Release_System();
}