#include "ImguiMgr.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "ImguiWin.h"
#include "ImguiWin_Test.h"
#include "ImguiAnimationTool.h"

IMPLEMENT_SINGLETON(CImguiMgr)

CImguiMgr::CImguiMgr()
{

}

CImguiMgr::~CImguiMgr()
{
	Free();
}

void CImguiMgr::Free()
{
	// ��ġ ����, RefCount ����
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pDeviceClass);

	// ImguiWin �������� ����
	for (auto item : m_mapImguiWin)
		Safe_Release(item.second);
}

HRESULT CImguiMgr::Ready_Imgui(CGraphicDev** ppGraphicClass, LPDIRECT3DDEVICE9* ppGraphicDev)
{
	// �̹� ������ ��ġ���� �޾� ����
	m_pDeviceClass = (*ppGraphicClass);
	m_pDeviceClass->AddRef();
	m_pGraphicDev = (*ppGraphicDev);
	m_pGraphicDev->AddRef();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_pIO = &ImGui::GetIO(); (void)m_pIO;
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_pIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//m_pIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//m_io.ConfigViewportsNoAutoMerge = true;
	//m_io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	//�ҿ� - �ѱ���Ʈ�߰�
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("wanju.ttf", 15.f, NULL, io.Fonts->GetGlyphRangesKorean());

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX9_Init(m_pGraphicDev);

	//m_mapImguiWin.emplace(L"Test",  CImguiWin_Test::Create());

	// + ���� : �׽�Ʈ Ŭ���� �߰�
	m_mapImguiWin.emplace(L"Test AnimationTool", CImguiAnimationTool::Create());

	return S_OK;
}

HRESULT CImguiMgr::Update_Imgui(const _float& fTimeDelta)
{
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
	if (m_bShow_DemoWindow)
		ImGui::ShowDemoWindow(&m_bShow_DemoWindow);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	if (m_bShow_DemoWindow)
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_bShow_DemoWindow);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &m_bShow_AnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&m_vClearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_pIO->Framerate, m_pIO->Framerate);
		ImGui::End();
	}

	// �ҿ� - ����������
	//if (m_bShow_MapWindow)
	//	ImGui::ShowDemoWindow(&m_bShow_MapWindow);

	//{
	//	ImGui::Begin("Map Tool", &m_bShow_MapWindow, ImGuiWindowFlags_MenuBar);
	//	if (ImGui::BeginMenuBar())
	//	{
	//		if (ImGui::BeginMenu("File")) // ���� �޴���
	//		{
	//			if (ImGui::MenuItem("Open..", "Ctrl+O"))
	//			{ /* Do stuff */
	//				int a = 0;
	//			}
	//			if (ImGui::MenuItem("Save", "Ctrl+S"))
	//			{ /* Do stuff */
	//				int a = 0;
	//			}
	//			if (ImGui::MenuItem("Close", "Ctrl+W"))
	//			{// my_tool_active = false;
	//				int a = 0;
	//			}
	//			ImGui::EndMenu();
	//		}
	//		ImGui::EndMenuBar();
	//	}
	//	ImGui::Text(u8"������ �ؽ�Ʈ�� �Է�"); // �ѱ��� ���϶��� u8"����"
	//	ImGui::Text(u8"�ѱ۰���");

	//	ImGui::End();
	//}
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
	if (m_bShow_AnotherWindow)
	{
		ImGui::Begin("Another Window", &m_bShow_AnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			m_bShow_AnotherWindow = false;
		ImGui::End();
	}


	// �����̳ʿ� ����� ������ ����
	for (auto item : m_mapImguiWin)
		item.second->Update_ImguiWin(fTimeDelta);


	// Rendering
	ImGui::EndFrame();
	m_pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	return S_OK;
}

HRESULT CImguiMgr::Render_Imgui()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

HRESULT CImguiMgr::Render_AdditionImgui(HRESULT dwDeviceState)
{
	// Update and Render additional Platform Windows
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// �ڵ��� ������ ����̽��� �ʱ�ȭ�Ѵ�.
	if (dwDeviceState == D3DERR_DEVICELOST && m_pGraphicDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();

	return S_OK;
}

void CImguiMgr::ResetDevice(_uint dwResizeWidth, _uint dwResizeHeight)
{
	if (dwResizeWidth == 0 && dwResizeHeight == 0)
		return;

	m_pDeviceClass->Get_D3DPP()->BackBufferWidth = dwResizeWidth;
	m_pDeviceClass->Get_D3DPP()->BackBufferHeight = dwResizeHeight;

	ImGui_ImplDX9_InvalidateDeviceObjects();
	if (m_pDeviceClass->Reset_GraphicDev() == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

bool CImguiMgr::LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight)
{
	return false;
}
