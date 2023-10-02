#include "ImguiWin_MapTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include <imgui_internal.h>

CImguiWin_MapTool::CImguiWin_MapTool()
{
}

CImguiWin_MapTool::~CImguiWin_MapTool()
{
}

CImguiWin_MapTool* CImguiWin_MapTool::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("CImguiWin_TextureTool Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiWin_MapTool::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_MapTool::Ready_ImguiWin()
{
	return S_OK;
}

_int CImguiWin_MapTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    m_DockSpace_Flags = ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_MapTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, m_DockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());

            
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"���", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"����", dock_right_id);

            

            ImGuiID dock_brower_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, NULL, &dockspace_id);
            //ImGui::DockSpace(dock_brower_scene, ImVec2(0.0f, 0.0f), m_DockSpace_Flags);
            ImGuiID dock_brower_terrain = ImGui::DockBuilderAddNode(dock_brower_scene, m_DockSpace_Flags | ImGuiDockNodeFlags_CentralNode);
            ImGuiID dock_brower_object = ImGui::DockBuilderAddNode(dock_brower_scene, m_DockSpace_Flags | ImGuiDockNodeFlags_CentralNode);
            ImGuiID dock_brower_light = ImGui::DockBuilderAddNode(dock_brower_scene, m_DockSpace_Flags | ImGuiDockNodeFlags_CentralNode);

            ImGui::DockBuilderDockWindow(u8"��", dock_brower_scene);
            ImGui::DockBuilderDockWindow(u8"����", dock_brower_terrain);
            ImGui::DockBuilderDockWindow(u8"������Ʈ", dock_brower_object);
            ImGui::DockBuilderDockWindow(u8"����", dock_brower_light);

            ImGui::DockBuilderFinish(dock_brower_scene);
            ImGui::DockBuilderFinish(dockspace_id);
            
            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    
    // ���ҽ�
    if (ImGui::Begin(u8"��", NULL, iMain_Flags))
    {
        

    }   ImGui::End();

    if (ImGui::Begin(u8"����", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin(u8"������Ʈ", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin(u8"����", NULL, iMain_Flags))
    {


    }   ImGui::End();


    // ����
    if (ImGui::Begin(u8"����", NULL, iMain_Flags))
    {


    }   ImGui::End();

    // ���
    if (ImGui::Begin(u8"���", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();

        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize);

    }   ImGui::End();

    
    
    ImGui::End();

	return 0;
}
