#include "ImguiWin_ProtoTool.h"

#include "Export_System.h"
#include "Export_Utility.h"


#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include <imgui_internal.h>
#include <IconsFontAwesome5.h>

#include <codecvt>
#include <locale>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

CImguiWin_ProtoTool::CImguiWin_ProtoTool()
{
}

CImguiWin_ProtoTool::~CImguiWin_ProtoTool()
{
}

CImguiWin_ProtoTool* CImguiWin_ProtoTool::Create()
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

void CImguiWin_ProtoTool::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_ProtoTool::Ready_ImguiWin()
{

	return S_OK;
}

_int CImguiWin_ProtoTool::Update_ImguiWin(const _float& fTimeDelta)
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
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_MapTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            ImVec2 vDockSize = ImGui::GetWindowSize();

            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 250.f / vDockSize.x, NULL, &dockspace_id);
            ImGuiID dock_property_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.5f, NULL, &dock_right_id);

            ImGui::DockBuilderDockWindow(u8"���", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"������Ʈ", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"�Ӽ�", dock_property_id);


            ImGuiID dock_browser_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 200.f / vDockSize.y, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"������", dock_browser_scene);

            ImGui::DockBuilderFinish(dockspace_id);

            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    Layout_ObjectBrowser(iMain_Flags);
    Layout_Component(iMain_Flags);
    Layout_Property(iMain_Flags);
    Layout_Viewer(iMain_Flags);


    ImGui::End();

	return 0;
}

void CImguiWin_ProtoTool::Layout_ObjectBrowser(const ImGuiWindowFlags& iMain_Flags)
{
    // ���ҽ�
    if (ImGui::Begin(u8"������", NULL, iMain_Flags))
    {
        _bool bAdd_Proto = false;
        if (ImGui::Button(u8"�߰��ϱ�"))
        {
            bAdd_Proto = true;
        }

        ImGui::SameLine();
        char strAdd_Proto[20] = {};
        m_strAdd_Proto.reserve(20);
        strcpy_s(strAdd_Proto, m_strAdd_Proto.c_str());
        if (ImGui::InputTextEx(u8"##InputProtoName", u8"Proto Name",
            strAdd_Proto, IM_ARRAYSIZE(strAdd_Proto),
            ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            bAdd_Proto = true;
        }

        m_strAdd_Proto = strAdd_Proto;

        if (bAdd_Proto)
        {
            if (!m_strAdd_Proto.empty())
                m_mapProtoData.emplace(m_strAdd_Proto, FProtoData());
            m_strAdd_Proto.clear();
        }


        //-------------------------------
        ImGui::Separator();
        for (auto iter = m_mapProtoData.begin(); iter != m_mapProtoData.end(); ++iter)
        {
            // �����ϴ� ������ �߰��ϱ�
            if (ImGui::Selectable((*iter).first.c_str()))
            {

            }
        }

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Component(const ImGuiWindowFlags& iMain_Flags)
{
    // ���ҽ�
    if (ImGui::Begin(u8"������Ʈ", NULL, iMain_Flags))
    {

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Property(const ImGuiWindowFlags& iMain_Flags)
{
    // ���ҽ�
    if (ImGui::Begin(u8"�Ӽ�", NULL, iMain_Flags))
    {

    }   ImGui::End();
}

void CImguiWin_ProtoTool::Layout_Viewer(const ImGuiWindowFlags& iMain_Flags)
{
    // ���ҽ�
    if (ImGui::Begin(u8"���", NULL, iMain_Flags | ImGuiWindowFlags_NoNavFocus))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));


    }   ImGui::End();
}
