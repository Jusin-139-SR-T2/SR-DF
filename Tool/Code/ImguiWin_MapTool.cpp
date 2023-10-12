#include "ImguiWin_MapTool.h"

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
    /*m_vecHierarchi.push_back(FLayerProperty({ u8"Environment", 0.0f, vector<FObjectProperty>() }));
    m_vecHierarchi[0].vecObject.push_back(FObjectProperty({ "Test1" }));
    m_vecHierarchi[0].vecObject.push_back(FObjectProperty({ "Test2" }));
    m_vecHierarchi[0].vecObject.push_back(FObjectProperty({ "Test3" }));
    m_vecHierarchi.push_back(FLayerProperty({ u8"GameLogic", 0.0f }));
    m_vecHierarchi.push_back(FLayerProperty({ u8"Camera", 0.0f }));
    m_vecHierarchi.push_back(FLayerProperty({ u8"UI", 0.0f }));*/
    //swap(m_vecHierarchi[1], m_vecHierarchi[3]);


    // �� ������
    m_strAdd_SceneName.reserve(20);

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

            ImGui::DockBuilderDockWindow(u8"���", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"����", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"�Ӽ�", dock_property_id);
            
            
            ImGuiID dock_browser_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 200.f / vDockSize.y, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"������", dock_browser_scene);

            ImGui::DockBuilderFinish(dockspace_id);
            
            m_bFirstLoop = false;

            if (m_bScene_Init)
            {
                Load_SceneAll();
                m_bScene_Init = !m_bScene_Init;
            }
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    Warning();
    Layout_Browser(iMain_Flags);
    Layout_Hierarchi(iMain_Flags);
    Layout_Property(iMain_Flags);
    Layout_Viewer(iMain_Flags);

    ImGui::End();

    return 0;
}

void CImguiWin_MapTool::Warning()
{
    if (m_bInput_Warning)
    {
        ImGui::OpenPopup(u8"�̸��� �ߺ���!");
        if (ImGui::BeginPopupModal(u8"�̸��� �ߺ���!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"���̾� ���� ������Ʈ�� �̸��� �ߺ���!");

            if (ImGui::Button("Ok", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                m_bInput_Warning = false;
            }
        }
        ImGui::EndPopup();
    }
}

void CImguiWin_MapTool::Layout_Browser(const ImGuiWindowFlags& iMain_Flags)
{
    // ���ҽ�
    if (ImGui::Begin(u8"������", NULL, iMain_Flags))
    {
        if (ImGui::BeginTabBar(u8"����"))
        {
            Layout_Browser_Scene();
            Layout_Browser_Terrain();
            Layout_Browser_Object();
            Layout_Browser_Light();
            Layout_Browser_Camera();

            ImGui::EndTabBar();
        }

    }   ImGui::End();
}

void CImguiWin_MapTool::Layout_Browser_Scene()
{
    if (ImGui::BeginTabItem(u8"��"))
    {
        _bool bAdd_Scene = false;
        if (ImGui::Button(u8"�߰��ϱ�"))
        {
            bAdd_Scene = true;
        }

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputSceneName", u8"Scene Name",
            const_cast<char*>(m_strAdd_SceneName.c_str()),
            (_int)m_strAdd_SceneName.capacity(),
            ImVec2(200, 0), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            bAdd_Scene = true;
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(100.f, 0.f));

        ImGui::SameLine();
        // ��� ���� �鿩�´�.
        if (ImGui::Button(u8"�� ��ü �ε��ϱ�"))
        {
            Load_SceneAll();
        }

        ImGui::SameLine();
        // ��� ���� ������ �������� �Ѵ�.
        if (ImGui::Button(u8"�� ��ü �����ϱ�"))
        {
            Save_SceneAll();
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(100.f, 0.f));

        ImGui::SameLine();
        // ���� �� ���������� �ε�
        if (ImGui::Button(u8"���� �� �ε��ϱ�"))
        {
            // ������ ���� �ε��ϵ��� �Ѵ�.
            if (m_iSelected_Scene != -1)
            {
                // �� �ε�� ���� ���� ����
                Reset_Hierarchi();

                // �ε�� ���� ��ȣ�� �Է����ش�.
                m_iLoaded_Scene = m_iSelected_Scene;

                // �ͷ��� ������ �ε��Ѵ�.
                Load_Terrain(m_iLoaded_Scene, m_vecScene[m_iLoaded_Scene].strName);
            }
        }

        // �� �߰�
        if (bAdd_Scene)
        {
            m_strAdd_SceneName = m_strAdd_SceneName.c_str();
            auto iter = find_if(m_vecScene.begin(), m_vecScene.end(),
                [this](FSceneData& refScene) {
                    return refScene.strName == m_strAdd_SceneName;
                });
            
            if (iter == m_vecScene.end() && !m_strAdd_SceneName.empty())
            {
                FSceneData tSceneData;
                tSceneData.strName = m_strAdd_SceneName;
                m_vecScene.push_back(tSceneData);
            }

            m_strAdd_SceneName.clear();
        }

        ImGui::Separator();

        // �� ������
        for (size_t i = 0; i < m_vecScene.size(); i++)
        {
            bool bIsSelected_Scene = (m_iSelected_Scene == i);

            if (ImGui::Selectable(m_vecScene[i].strName.c_str(),
                bIsSelected_Scene, ImGuiSelectableFlags_AllowDoubleClick))
            {
                m_iSelected_Scene = i;
            }
        }

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Terrain()
{
    if (ImGui::BeginTabItem(u8"����"))
    {
        if (m_iLoaded_Scene == -1)
        {
            ImGui::EndTabItem();
            return;
        }

        FTerrainData& tTerrain = m_vecScene[m_iLoaded_Scene].tTerrain;

        _bool bIsEdited = false;

        ImGui::BeginGroup();

        // ���� ����
        Set_Button_NonActiveColor();
        ImGui::Button(u8"����", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        ImGui::PushItemWidth(100.f);
        _float fInputHorizontal = tTerrain.vVertexCnt.x;
        if (ImGui::InputFloat(u8"##InputText_Terrain1", &fInputHorizontal,
            -1000.f, 1000.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            tTerrain.vVertexCnt.x = fInputHorizontal;
        }
        ImGui::PopItemWidth();



        ImGui::SameLine();
        // ���� ����
        Set_Button_NonActiveColor();
        ImGui::Button(u8"����", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        ImGui::PushItemWidth(100.f);
        _float fInputVertical = tTerrain.vVertexCnt.z;
        if (ImGui::InputFloat(u8"##InputText_Terrain2", &fInputVertical,
            -1000.f, 1000.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            tTerrain.vVertexCnt.z = fInputVertical;
        }
        ImGui::PopItemWidth();



        ImGui::SameLine();
        // ���� ����
        Set_Button_NonActiveColor();
        ImGui::Button(u8"����", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        ImGui::PushItemWidth(100.f);
        _float fInputHeight = tTerrain.vVertexCnt.y;
        if (ImGui::InputFloat(u8"##InputText_Terrain3", &fInputHeight,
            -100.f, 100.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            tTerrain.vVertexCnt.y = fInputHeight;
        }
        ImGui::PopItemWidth();



        // ������ ����
        // X
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ X", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Y
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ Y", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Z
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ Z", ImVec2(60, 0));
        Set_Button_ReturnColor();


        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Offset", tTerrain.vOffset, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(tTerrain.vOffset, 10000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##OffsetSlider",
            tTerrain.vOffset,
            -10000.f, 10000.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();



        // ������ ����
        // X
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ X", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Y
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ Y", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Z
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"������ Z", ImVec2(60, 0));
        Set_Button_ReturnColor();


        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Scale", tTerrain.vScale, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(tTerrain.vScale, 10000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##ScaleSlider",
            tTerrain.vScale,
            -10000.f, 10000.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();



        ImGui::Separator();
        if (ImGui::Button(u8"�����ϱ�", ImVec2(60, 0)))
        {
            auto iter = find_if(m_vecScene.begin(), m_vecScene.end(),
                [this](FSceneData& tScene) {
                    return tScene.strName == m_vecScene[m_iLoaded_Scene].strName;
                });
            if (iter != m_vecScene.end())
                Apply_Terrain(iter->strName);
        }


        ImGui::EndGroup();

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Object()
{
    if (ImGui::BeginTabItem(u8"������Ʈ"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Light()
{
    if (ImGui::BeginTabItem(u8"����"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Camera()
{
    if (ImGui::BeginTabItem(u8"ī�޶�"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Hierarchi(const ImGuiWindowFlags& iMain_Flags)
{
    // ����
    if (ImGui::Begin(u8"����", NULL, iMain_Flags))
    {
        if (m_iLoaded_Scene == -1)
        {
            ImGui::End();
            return;
        }

        // �̸�
        ImGui::Text(m_vecScene[m_iLoaded_Scene].strName.c_str());
        ImGui::Separator();

        ImGui::Indent();
        vector<FLayerData>& vecLayer = m_vecScene[m_iLoaded_Scene].vecLayer;
        for (_uint i = 0; i < vecLayer.size(); i++)
        {
            
            bool bIsSelected_Layer = (m_iSelected_Layer == i);

            if (ImGui::Selectable(vecLayer[i].strName.c_str(),
                bIsSelected_Layer, ImGuiSelectableFlags_AllowDoubleClick))
            {
                // ���ý� ���̾� ���� ���
                //if (bIsSelected_Layer)
                {
                    m_eSelectedProperty_Type = ESELECTED_TYPE_LAYER;
                }
                m_iSelected_Layer = i;
                m_iSelected_Object = -1;
            }

            ImGui::Indent();
            for (_uint j = 0; j < vecLayer[i].vecObject.size(); j++)
            {
                bool bIsSelected_Object = (m_iSelected_Object == j);

                if (ImGui::Selectable(vecLayer[i].vecObject[j].strName.c_str(),
                    bIsSelected_Object, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    // ���ý� ������Ʈ ���� ���
                    //if (bIsSelected_Object)
                    {
                        m_eSelectedProperty_Type = ESELECTED_TYPE_OBJECT;
                    }

                    m_iSelected_Object = j;
                    m_iSelected_Layer = -1;
                    m_iSelected_Layer_Remain = i;
                }
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();

        ImGui::Separator();
        bool bInput_Apply = false;
        if (ImGui::Button(u8"���̾� �߰�"))
        {
            bInput_Apply = true;
            m_bFocusedLayer_Edit = false;
        }
        ImGui::SameLine();

        ImGui::InputText("##LayerEdit", m_arrAddLayer_Buf, IM_ARRAYSIZE(m_arrAddLayer_Buf));

        if (ImGui::IsItemFocused())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                bInput_Apply = true;
                m_bFocusedLayer_Edit = false;
            }
        }

        if (bInput_Apply)
        {
            string strInput = m_arrAddLayer_Buf; // UTF-8 ���ڿ��� ��ȯ
            if (strInput.length() > 0)
            {
                auto iter = find_if(vecLayer.begin(), vecLayer.end(),
                    [&strInput](FLayerData& Dst) {
                        return strInput == Dst.strName;
                    });
                if (iter == vecLayer.end())
                {
                    vecLayer.push_back(FLayerData({strInput, 0.f}));
                }
            }
            memset(m_arrAddLayer_Buf, 0, IM_ARRAYSIZE(m_arrAddLayer_Buf));
        }

    }   ImGui::End();
}

void CImguiWin_MapTool::Layout_Property(const ImGuiWindowFlags& iMain_Flags)
{
    // ����
    if (ImGui::Begin(u8"�Ӽ�", NULL, iMain_Flags))
    {
        switch (m_eSelectedProperty_Type)
        {
        case CImguiWin_MapTool::ESELECTED_TYPE_SCENE:
            Layout_Property_Scene();
            break;
        case CImguiWin_MapTool::ESELECTED_TYPE_LAYER:
            Layout_Property_Layer();
            break;
        case CImguiWin_MapTool::ESELECTED_TYPE_OBJECT:
            Layout_Property_Object();
            break;
        case CImguiWin_MapTool::ESELECTED_TYPE_LIGHT:
            Layout_Property_Light();
            break;
        }

    }   ImGui::End();
}

void CImguiWin_MapTool::Layout_Property_Scene()
{
    
}

void CImguiWin_MapTool::Layout_Property_Layer()
{
    if (m_iLoaded_Scene == -1 || m_iSelected_Layer == -1)
        return;

    if (ImGui::CollapsingHeader(u8"��ǥ"))
    {
        FLayerData& tLayerData = m_vecScene[m_iLoaded_Scene].vecLayer[m_iSelected_Layer];

        // �̸�
        Set_Button_NonActiveColor();
        ImGui::Button(u8"�̸�", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // �Էº�
        tLayerData.strName.reserve(20);
        ImGui::SameLine();
        ImGui::PushItemWidth(140.f);
        if (ImGui::InputText("##Name", 
            const_cast<char*>(tLayerData.strName.c_str()),
            tLayerData.strName.capacity(),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }
        tLayerData.strName = tLayerData.strName;
        ImGui::PopItemWidth();
    }
}

void CImguiWin_MapTool::Layout_Property_Object()
{
    _uint iSelected_Scene = m_iLoaded_Scene;
    _uint iHierarchi_Layer = m_iSelected_Layer_Remain;
    _uint iHierarchi_Object = m_iSelected_Object;
    
    _bool bIsEdited = false;    // �����Ǿ��� �� ��ȭ �̺�Ʈ

    if (iHierarchi_Layer == -1 && iHierarchi_Object == -1 && iSelected_Scene == -1)
        return;

    vector<FLayerData>& vecLayer = m_vecScene[iSelected_Scene].vecLayer;
    vector<FObjectData>& vecObject = vecLayer[iHierarchi_Layer].vecObject;

    if (ImGui::CollapsingHeader(u8"�̸�"))
    {
        // �̸�
        Set_Button_NonActiveColor();
        ImGui::Button(u8"�̸�", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // �Էº�
        vecObject[iHierarchi_Object].strName.reserve(20);
        char  strInput[20] = {};
        strcpy_s(strInput, vecObject[iHierarchi_Object].strName.c_str());
            
        ImGui::SameLine();
        ImGui::PushItemWidth(140.f);
        if (ImGui::InputText("##Name",
            strInput, IM_ARRAYSIZE(strInput),
            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            string strCheck = strInput;
            auto iter = find_if(vecObject.begin(), vecObject.end(),
                [&strInput](FObjectData& refObject) {
                    return refObject.strName == strInput;
                });

            if (iter == vecObject.end())
                vecObject[iHierarchi_Object].strName = strInput;
            else
            {
                // �ڱ� �ڽ��̸��� ��� �������� ����
                if (vecObject[iHierarchi_Object].strName != strInput)
                    m_bInput_Warning = true;
            }
        }
        
        ImGui::PopItemWidth();
    }


    ImGui::Separator();
    if (ImGui::CollapsingHeader(u8"��ǥ"))
    {
        // X
        Set_Button_NonActiveColor();
        ImGui::Button(u8"X", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Y
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Z
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();
        

        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Translate", 
            vecObject[iHierarchi_Object].vPos, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iHierarchi_Object].vPos, 10000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##TranslateSlider",
            vecObject[iHierarchi_Object].vPos,
            -10000.f, 10000.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();
    }

    if (ImGui::CollapsingHeader(u8"ȸ��"))
    {
        // X
        Set_Button_NonActiveColor();
        ImGui::Button(u8"X", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Y
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Z
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();
        
        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Rotate",
            vecObject[iHierarchi_Object].vRot, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iHierarchi_Object].vRot, 360.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##RoateSlider",
            vecObject[iHierarchi_Object].vRot,
            -360.f, 360.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();
    }

    if (ImGui::CollapsingHeader(u8"ũ��"))
    {
        // X
        Set_Button_NonActiveColor();
        ImGui::Button(u8"X", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Y
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // Z
        ImGui::SameLine();
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Y", ImVec2(60, 0));
        Set_Button_ReturnColor();
        
        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Scale",
            vecObject[iHierarchi_Object].vScale,
            "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iHierarchi_Object].vScale, 1000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##ScaleSlider",
            vecObject[iHierarchi_Object].vScale,
            -1000.f, 1000.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();
    }


    ImGui::Separator();
    if (ImGui::CollapsingHeader(u8"�켱��"))
    {
        // Update
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Update", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::PushItemWidth(60.f);
        ImGui::SameLine();
        if (ImGui::InputFloat("##PriorityUpdate",
            &vecObject[iHierarchi_Object].fPriority[EPRIORITY_OBJECT_UPDATE],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Update",
            &vecObject[iHierarchi_Object].bUsePriority[EPRIORITY_OBJECT_UPDATE]);
        

        // LateUpdate
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Late", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::PushItemWidth(60.f);
        ImGui::SameLine();
        if (ImGui::InputFloat("##PriorityLate",
            &vecObject[iHierarchi_Object].fPriority[EPRIORITY_OBJECT_LATE],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Late",
            &vecObject[iHierarchi_Object].bUsePriority[EPRIORITY_OBJECT_LATE]);

        // Render
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Render", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::PushItemWidth(60.f);
        ImGui::SameLine();
        if (ImGui::InputFloat("##PriorityRender", 
            &vecObject[iHierarchi_Object].fPriority[EPRIORITY_OBJECT_RENDER],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Render",
            &vecObject[iHierarchi_Object].bUsePriority[EPRIORITY_OBJECT_RENDER]);
    }
}

void CImguiWin_MapTool::Layout_Property_Light()
{
}

void CImguiWin_MapTool::Layout_Viewer(const ImGuiWindowFlags& iMain_Flags)
{
    // ���
    if (ImGui::Begin(u8"���", NULL, iMain_Flags | ImGuiWindowFlags_NoNavFocus))
    {
        // ���� ���ֱ�
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemSpacing = ImVec2(0, 2);

        ImVec2 vAlignMargin = ImGui::GetContentRegionAvail();
        _float fMarginContent = 400.f;

        // ��� ����
        ImGui::Dummy(ImVec2((vAlignMargin.x - fMarginContent) * 0.5f, 0.f));

        // ��ǥ��
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); // ��ư�� ��Ȱ��ȭ ���·� ����ϴ�.
        ImGui::SameLine();
        ImGui::Button(u8"��ǥ��");
        ImGui::PopStyleVar();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##��ǥ��", m_vecCoord_Item[m_iIndex_CoordItem].c_str(), 
            ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton))
        {
            for (int i = 0; i < m_vecCoord_Item.size(); i++) 
            {
                bool isSelected = (m_iIndex_CoordItem == i);
                if (ImGui::Selectable(m_vecCoord_Item[i].c_str(), isSelected)) 
                {
                    m_iIndex_CoordItem = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // �� ����
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(8.f, 0.f));

        // �Ǻ�
        ImGui::SameLine();
        ImGui::Button(u8"�Ǻ�");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##�Ǻ�", m_vecPivot_Item[m_iIndex_PivotItem].c_str(),
            ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton))
        {
            for (int i = 0; i < m_vecPivot_Item.size(); i++) {
                bool isSelected = (m_iIndex_PivotItem == i);
                if (ImGui::Selectable(m_vecPivot_Item[i].c_str(), isSelected)) {
                    m_iIndex_PivotItem = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // �� ����
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(8.f, 0.f));

        if (m_bSnap_Active)
            Set_Button_ActiveColor();

        // ����
        ImGui::SameLine();
        if (ImGui::Button(u8"����"))
        {
            m_bSnap_Active = !m_bSnap_Active;
        }

        Set_Button_ReturnColor();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##����", m_vecSnap_Item[m_iIndex_SnapItem].c_str(),
            ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton))
        {
            for (int i = 0; i < m_vecSnap_Item.size(); i++) {
                bool isSelected = (m_iIndex_SnapItem == i);
                if (ImGui::Selectable(m_vecSnap_Item[i].c_str(), isSelected)) {
                    m_iIndex_SnapItem = i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // ������ ���� ����
        ImGui::GetStyle().ItemSpacing = ImVec2(8, 4);

        // ��� ����
        ImGui::SameLine();
        ImGui::Dummy(ImVec2((vAlignMargin.x - fMarginContent) * 0.5f, 0.f));


        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();
}

void CImguiWin_MapTool::Save_SceneAll()
{
    // �� ����� ������ ������ ���Ϸ� �����Ѵ�.
    for (size_t i = 0; i < m_vecScene.size(); i++)
    {
        FSceneData& tSceneData = m_vecScene[i];

        FSerialize_Scene tSceneSerial;
        tSceneSerial.tHeader.eType = ESERIALIZE_SCENE;
        tSceneSerial.tHeader.strName = tSceneData.strName;
        tSceneSerial.refTerrainName = tSceneData.strName;

        // ���̾� �Ľ�
        for (size_t j = 0; j < tSceneData.vecLayer.size(); j++)
        {
            FLayerData& tLayerData = tSceneData.vecLayer[j];

            FSerialize_Layer tLayerSerial;
            tLayerSerial.tHeader.eType = ESERIALIZE_LAYER;
            tLayerSerial.tHeader.strName = tLayerData.strName;
            tLayerSerial.fPriority = tLayerData.fPriority;
            // �Ϸ�� ���� ����ȭ �߰�
            tSceneSerial.vecLayer.push_back(tLayerSerial);

            // ������Ʈ �Ľ�
            for (size_t k = 0; k < tLayerData.vecObject.size(); k++)
            {
                FObjectData& tObjectData = tLayerData.vecObject[k];

                FSerialize_GameObject tObject;
                tObject.tHeader.eType = ESERIALIZE_LAYER;
                tObject.tHeader.strName = tObjectData.strName;
                // �Ϸ�� ���̾ ����ȭ �߰�
                tLayerSerial.vecGameObject.push_back(tObject);

                // ������� ���߿� �߰�, ������Ʈ�� ����
                //tObject.bTag.emplace();
            }
        }

        // �� �� ���� ������ ������
        Export_Scene(tSceneSerial);

        // �ͷ��������� ����
        Save_Terrain(i);
    }
}

void CImguiWin_MapTool::Export_Scene(const FSerialize_Scene& tSceneSerial)
{
    Document doc;
    StringBuffer buffer;

    tSceneSerial.Parse_RapidJSON(doc, buffer, ESERIALIZE_PROCESS_IMMEDIATE, true);

    ofstream ofs(g_strScenePath + tSceneSerial.tHeader.strName + g_strSceneExt);
    if (ofs.is_open())
    {
        ofs << buffer.GetString();
        ofs.close();
        cout << "���� ���� �Ϸ�!" << endl;
    }
    else
        cout << "������ ���� ����..." << endl;
}

void CImguiWin_MapTool::Load_SceneAll()
{
    // �� ����� ������ �������� �ε��Ѵ�.
    _finddata_t fd;
    intptr_t handle;
    if ((handle = _findfirst((g_strScenePath + "*" + g_strSceneExt).c_str(), &fd)) == -1L)
    {
        cout << "������ ���� ����!!" << endl;
        m_vecScene.clear();
    }
    else
    {
        m_vecScene.clear();
        m_iSelected_Scene = -1;
        m_iLoaded_Scene = -1;

        // ��� �ε�
        do
        {
            string strName = fd.name;
            size_t extPos = strName.find_last_of('.');
            if (extPos == string::npos)
                continue;

            // Ȯ���� üũ �� �ε��Ѵ�.
            if (strName.substr(extPos) == g_strSceneExt)
            {
                FSerialize_Scene tSceneSerial;
                FSceneData tSceneData;

                Import_Scene(strName.substr((size_t)0, extPos), tSceneSerial, tSceneData);
            }
        } while (_findnext(handle, &fd) == S_OK);
    }

    _findclose(handle);
}

void CImguiWin_MapTool::Import_Scene(const string& strName, FSerialize_Scene& tSceneSerial, FSceneData& tSceneData)
{
    string strJson;
    ifstream inputFile(g_strScenePath + strName + g_strSceneExt);
    if (inputFile.is_open())
    {
        // ���ڿ� ���� �о���� �ݺ���
        strJson = string(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
        inputFile.close();
        cout << "���� �ҷ���!" << endl;

        // �Ľ� ������ ������ Data�� ����
        if (tSceneSerial.Receive_ByRapidJSON(strJson))
        {
            tSceneData.tTerrain.strName = tSceneSerial.refTerrainName;
            tSceneData.strName = tSceneSerial.tHeader.strName;

            for (size_t i = 0; i < tSceneSerial.vecLayer.size(); i++)
            {
                FSerialize_Layer& tLayerSerial = tSceneSerial.vecLayer[i];
                FLayerData tLayerData;
                tLayerData.fPriority = tLayerSerial.fPriority;
                tLayerData.strName = tLayerSerial.tHeader.strName;

                for (size_t j = 0; j < tLayerSerial.vecGameObject.size(); j++)
                {
                    FSerialize_GameObject& tObjectSerial = tLayerSerial.vecGameObject[j];
                    FObjectData tObjectData;

                    tObjectData.fPriority[EPRIORITY_OBJECT_UPDATE] = tObjectSerial.fPriority_Update;
                    tObjectData.fPriority[EPRIORITY_OBJECT_LATE] = tObjectSerial.fPriority_LateUpdate;
                    tObjectData.fPriority[EPRIORITY_OBJECT_RENDER] = tObjectSerial.fPriority_Render;

                    tObjectData.bUsePriority[EPRIORITY_OBJECT_UPDATE] = tObjectSerial.bUsePriority_Update;
                    tObjectData.bUsePriority[EPRIORITY_OBJECT_LATE] = tObjectSerial.bUsePriority_LateUpdate;
                    tObjectData.bUsePriority[EPRIORITY_OBJECT_RENDER] = tObjectSerial.bUsePriority_Render;

                    tObjectData.vPos = tObjectSerial.vPos;
                    tObjectData.vRot = tObjectSerial.vRotation;
                    tObjectData.vScale = tObjectSerial.vScale;

                    tLayerData.vecObject.push_back(tObjectData);
                }
                tSceneData.vecLayer.push_back(tLayerData);
            }
            m_vecScene.push_back(tSceneData);

            // �ͷ��� �ε�
            Load_Terrain((_int)(m_vecScene.size() - (size_t)1), tSceneData.strName);
        }
    }
    else
        cerr << "������ �ҷ����� �� ����!\n";
}

void CImguiWin_MapTool::Apply_Terrain(const string& strTerrainName)
{
    // �����ư�� ������ �� ���ο� �ͷ����� �����ϰ� �ʿ� ��ġ���ش�.

}

void CImguiWin_MapTool::Save_Terrain(const _int iSelected_Scene)
{
    if (iSelected_Scene == -1)
        return;

    FTerrainData& tTerrain = m_vecScene[iSelected_Scene].tTerrain;

    // ������ ����
    FSerialize_Terrain tTerrainSerial;
    tTerrainSerial.tHeader.eType = ESERIALIZE_TERRAIN;
    tTerrainSerial.tHeader.strName = tTerrain.strName;
    tTerrainSerial.vVertexCount = tTerrain.vVertexCnt;
    tTerrainSerial.vScale = tTerrain.vScale;
    tTerrainSerial.vInvOffset = tTerrain.vOffset;

    // ���� ����
    Export_Terrain(tTerrainSerial);
}

void CImguiWin_MapTool::Export_Terrain(const FSerialize_Terrain& tTerrainSerial)
{
    // ������ JSON ������ ������ writer�� ����
    Document doc;
    StringBuffer buffer;

    tTerrainSerial.Parse_RapidJSON(doc, buffer, true);

    string strJson = buffer.GetString();
    ofstream outputFile(g_strTerrainPath + tTerrainSerial.tHeader.strName + g_strTerrainExt);
    if (outputFile.is_open())
    {
        outputFile << strJson;
        outputFile.close();
        cout << "���� ������" << endl;
    }
    else
    {
        cerr << "������ ������ �� ����!\n";
    }
}

void CImguiWin_MapTool::Load_Terrain(const _int iSelected_Scene, const string& strName)
{
    if (iSelected_Scene == -1)
        return;

    FTerrainData& tTerrain = m_vecScene[iSelected_Scene].tTerrain;

    // ���� ��ȸ�� �����͸� ��
    _finddata_t fd;
    intptr_t handle;
    if ((handle = _findfirst((g_strTerrainPath + "*" + g_strTerrainExt).c_str(), &fd)) == -1L)
    {
        cout << "������ ���� ����!!" << endl;
        tTerrain = {};
    }
    // �ͷ��� ���� �����Ͱ� ���ٸ� �ε����� �ʰ� ���ο� �����͸� ����Ͽ� �����.
    else
    {
        // ����ã��
        do
        {
            string strFdName = fd.name;
            size_t extPos = strName.find_last_of('.');
            if (extPos == string::npos)
                break;

            // �ε��Ϸ��� �̸��� �����̸� Ȯ�� �� ���� �ε�
            if (strFdName == strName)
            {
                FSerialize_Terrain tTerrainSerial;

                Import_Terrain(iSelected_Scene, strName, tTerrainSerial);
            }

        } while (_findnext(handle, &fd) == S_OK);
    }

    _findclose(handle);
}

void CImguiWin_MapTool::Import_Terrain(const _int iSelected_Scene, const string& strName, FSerialize_Terrain& tTerrainSerial)
{
    if (iSelected_Scene == -1)
        return;

    FTerrainData& tTerrain = m_vecScene[iSelected_Scene].tTerrain;

    string strJson;
    ifstream inputFile(g_strTerrainPath + strName);
    if (inputFile.is_open())
    {
        // ���ڿ� ���� �о���� �ݺ���
        strJson = string(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
        inputFile.close();
        cout << "\n���� �ҷ���!\n";

        // �Ľ� ������ ������ Data�� ����
        if (tTerrainSerial.Receive_ByRapidJSON(strJson))
        {
            tTerrain.strName = tTerrainSerial.tHeader.strName;
            tTerrain.vVertexCnt = tTerrainSerial.vVertexCount;
            tTerrain.vScale = tTerrainSerial.vScale;
            tTerrain.vOffset = tTerrainSerial.vInvOffset;
        }
    }
    else
    {
        cerr << "������ �ҷ����� �� ����!\n";
    }
}

void CImguiWin_MapTool::Set_Button_ActiveColor()
{
    m_pStyle = ImGui::GetStyle();
    ImGuiStyle& style = ImGui::GetStyle();
    m_pStyle.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    m_pStyle.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonHovered];
    m_pStyle.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_ButtonActive];

    style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.56f, 1.00f, 1.00f); 
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.70f, 1.00f, 1.00f); 
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.40f, 0.90f, 1.00f); 
}

void CImguiWin_MapTool::Set_Button_NonActiveColor()
{
    m_pStyle = ImGui::GetStyle();
    ImGuiStyle& style = ImGui::GetStyle();
    m_pStyle.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    m_pStyle.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_ButtonHovered];
    m_pStyle.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_ButtonActive];

    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_Button];
}

void CImguiWin_MapTool::Set_Button_ReturnColor()
{
    // ��ư Ȱ��ȭ ���� ����
    ImGui::GetStyle().Colors[ImGuiCol_Button] = m_pStyle.Colors[ImGuiCol_Button];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = m_pStyle.Colors[ImGuiCol_ButtonHovered];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = m_pStyle.Colors[ImGuiCol_ButtonActive];
}


