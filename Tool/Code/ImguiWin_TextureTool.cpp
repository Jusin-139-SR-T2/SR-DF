#include "ImguiWin_TextureTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include <imgui_internal.h>

CImguiWin_TextureTool::CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool::~CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool* CImguiWin_TextureTool::Create()
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

void CImguiWin_TextureTool::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_TextureTool::Ready_ImguiWin()
{


    return S_OK;
}

_int CImguiWin_TextureTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_TextureTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());


            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.3f, NULL, &dockspace_id);
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, NULL, &dockspace_id);
            ImGui::DockBuilderDockWindow("TextureList", dock_right_id);
            ImGui::DockBuilderDockWindow("Browser", dock_down_id);
            ImGui::DockBuilderDockWindow("Viewer", dockspace_id);

            ImGui::DockBuilderFinish(dockspace_id);

            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    if (ImGui::Begin("Viewer", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin("Browser", NULL, iMain_Flags))
    {


    }   ImGui::End();

    if (ImGui::Begin("TextureList", NULL, iMain_Flags))
    {


    }   ImGui::End();

    ImGui::End();

    return 0;
}
