#include "ImguiWin_DockingSpace.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>

CImguiWin_DockingSpace::CImguiWin_DockingSpace()
	: m_bOpt_FullScreen(), m_bOpt_Padding(), m_DockSpace_Flags(), m_Window_Flags()
{
}

CImguiWin_DockingSpace::~CImguiWin_DockingSpace()
{
}

CImguiWin_DockingSpace* CImguiWin_DockingSpace::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("ImguiWin_DockingSpace Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_DockingSpace::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_DockingSpace::Ready_ImguiWin()
{
	m_bOpen = true;
	m_bOpt_FullScreen = true;
	m_bOpt_Padding = false;
	m_DockSpace_Flags = ImGuiDockNodeFlags_None;
	m_Window_Flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    return S_OK;
}

_int CImguiWin_DockingSpace::Update_ImguiWin(const _float& fTimeDelta)
{
	SUPER::Update_ImguiWin(fTimeDelta);

	m_DockSpace_Flags = ImGuiDockNodeFlags_None;
	m_Window_Flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	
	if (m_bOpt_FullScreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		m_Window_Flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		m_Window_Flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 1.0f));
	}
	else
	{
		m_DockSpace_Flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}

	// �������̽��� ������ �����ǵ��� ����� �÷���, �Ⱦ�����
	if (m_DockSpace_Flags & ImGuiDockNodeFlags_PassthruCentralNode)
		m_Window_Flags |= ImGuiWindowFlags_NoBackground;

	if (!m_bOpt_Padding)
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &m_bOpen, m_Window_Flags);
	if (!m_bOpt_Padding)
		ImGui::PopStyleVar();

	if (m_bOpt_FullScreen)
	{
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(1);
	}

	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockSpace_Flags);
	}
	else
	{
		//ShowDockingDisabledMessage();
	}

	if (ImGui::BeginMenuBar())
	{
		//if (ImGui::BeginMenu("Options"))
		//{
		//	// Disabling fullscreen would allow the window to be moved to the front of other windows,
		//	// which we can't undo at the moment without finer window depth/z control.
		//	ImGui::MenuItem("Fullscreen", NULL, &m_bOpt_FullScreen);
		//	ImGui::MenuItem("Padding", NULL, &m_bOpt_Padding);
		//	ImGui::Separator();

		//	if (ImGui::MenuItem("Flag: NoSplit", "", (m_DockSpace_Flags & ImGuiDockNodeFlags_NoSplit) != 0)) { m_DockSpace_Flags ^= ImGuiDockNodeFlags_NoSplit; }
		//	if (ImGui::MenuItem("Flag: NoResize", "", (m_DockSpace_Flags & ImGuiDockNodeFlags_NoResize) != 0)) { m_DockSpace_Flags ^= ImGuiDockNodeFlags_NoResize; }
		//	if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (m_DockSpace_Flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { m_DockSpace_Flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
		//	if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (m_DockSpace_Flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { m_DockSpace_Flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
		//	if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (m_DockSpace_Flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, m_bOpt_FullScreen)) { m_DockSpace_Flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
		//	ImGui::Separator();

		//	if (ImGui::MenuItem("Close", NULL, false, m_bOpen != NULL))
		//		m_bOpen = false;
		//	ImGui::EndMenu();
		//}


		// �� ���̾ƿ�
		CImguiMgr::GetInstance()->HelpMarkerEx(u8"   ���� ",
			u8"�����ʿ��� ���ϴ� ������ ������ �� �ֽ��ϴ�");

		if (ImGui::BeginTabBar("Layout"))
		{
			if (ImGui::BeginTabItem(u8"�� ��")) 
			{
				// Tab ����
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(u8"�ؽ�ó ��"))
			{
				// Tab ����
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(u8"�ִϸ��̼� ��"))
			{
				// Tab ����
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		
		ImGui::EndMenuBar();
	}

	ImGui::End();

    return 0;
}
