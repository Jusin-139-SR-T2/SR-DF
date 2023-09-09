//#ifndef IMGUI_DEFINE_MATH_OPERATORS
//#define IMGUI_DEFINE_MATH_OPERATORS
//#endif

#include "framework.h"
#include "MainApp.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = &ImGui::GetIO(); (void)m_io;
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//m_io.ConfigViewportsNoAutoMerge = true;
	//m_io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX9_Init(m_pGraphicDev);

	return S_OK;
}

int CMainApp::Update_MainApp(const _float& fTimeDelta)
{
	m_pManagementClass->Update_Scene(fTimeDelta);

	// Handle window resize (we don't resize directly in the WM_SIZE handler)
	/*if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
	{
		g_d3dpp.BackBufferWidth = g_ResizeWidth;
		g_d3dpp.BackBufferHeight = g_ResizeHeight;
		g_ResizeWidth = g_ResizeHeight = 0;
		ResetDevice();
	}*/

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
		ImGui::End();
	}

	/*if (!ImGui::Begin("Window"))
	{
		ImGui::End();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));*/

	//if (m_io->ConfigFlags)// & ImGuiConfigFlags_DockingEnable)
	//{
	//	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	//}
	//ImDrawList::AddImage();

	//ImGui::End();

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// Rendering
	ImGui::EndFrame();
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	return 0;
}

void CMainApp::LateUpdate_MainApp()
{
	Engine::LateUpdate_Scene();
}

void CMainApp::Render_MainApp()
{
	D3DXCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
	Engine::Render_Begin(clear_col_dx);

	//if (m_pGraphicDev->BeginScene() >= 0)
	{
		//ImTextureID::
		//PDIRECT3DTEXTURE9
		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		m_pGraphicDev->EndScene();
	}

	// Update and Render additional Platform Windows
	if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	HRESULT result = m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && m_pGraphicDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();

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
	// ImGui 끝내기
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

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

void CMainApp::ResetDevice()
{
	/*ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = m_pGraphicDev->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();*/
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