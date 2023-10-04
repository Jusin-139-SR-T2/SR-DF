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
            ImVec2 vDockSize = ImGui::GetWindowSize();

            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 250.f / vDockSize.x, NULL, &dockspace_id);
            ImGuiID dock_property_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.5f, NULL, &dock_right_id);

            ImGui::DockBuilderDockWindow(u8"ºä¾î", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"°èÃþ", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"¼Ó¼º", dock_property_id);
            
            
            ImGuiID dock_browser_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 200.f / vDockSize.y, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"ºê¶ó¿ìÀú", dock_browser_scene);

            ImGui::DockBuilderFinish(dockspace_id);
            
            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    
    // ¸®¼Ò½º
    if (ImGui::Begin(u8"ºê¶ó¿ìÀú", NULL, iMain_Flags))
    {
        if (ImGui::BeginTabBar(u8"¿¡¼Â"))
        {
            if (ImGui::BeginTabItem(u8"¾À"))
            {

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(u8"ÁöÇü"))
            {

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(u8"¿ÀºêÁ§Æ®"))
            {

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(u8"Á¶¸í"))
            {

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(u8"Ä«¸Þ¶ó"))
            {

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

    }   ImGui::End();


    // °èÃþ
    if (ImGui::Begin(u8"°èÃþ", NULL, iMain_Flags))
    {


    }   ImGui::End();

    // °èÃþ
    if (ImGui::Begin(u8"¼Ó¼º", NULL, iMain_Flags))
    {


    }   ImGui::End();

    // ºä¾î
    if (ImGui::Begin(u8"ºä¾î", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth, 
                                contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);
        
        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize, 
                        ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
                        ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();

    
    
    ImGui::End();

	return 0;
}
