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
#include <CubeObject.h>

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

    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProj);

    ZeroMemory(m_vCamTranslate, sizeof(_vec3[INFO_END]));

    m_vCamTranslate[INFO_RIGHT] = { 0.f, 0.f, 0.f };
    m_vCamTranslate[INFO_UP] = { 0.f, 1.f, 0.f };
    m_vCamTranslate[INFO_LOOK] = { 10.f, 10.f, 10.f };
    m_vCamTranslate[INFO_POS] = { 0.f, 10.f, 0.f };

    // �� ����� ����� ��ġ���� �Ѱ� ���´�.
    D3DXMatrixLookAtLH(&m_matView, &Get_Pos(), &Get_Look(), &Get_Up());
    CImguiMgr::GetInstance()->Get_GraphicDev()->SetTransform(D3DTS_VIEW, &m_matView);

    // ���� ��ȯ ����� �ѹ� ������ָ� �ٽ� �ȸ���� �־ �ȴ�.
    // �ٸ� �ɼ��� �ٲ�� ����� �־�� �Ѵ�.
    D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
    CImguiMgr::GetInstance()->Get_GraphicDev()->SetTransform(D3DTS_PROJECTION, &m_matProj);

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

            // ������ Ÿ�Ե��� �׻� �ǿ� ������ �ε��Ѵ�.
            Load_ProtoAll();

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

                // ������ �°� ��ü�� �������ش�.
                Load_ObjectToScene();

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
        // ��� �����並 �鿩�´�.
        if (ImGui::Button(u8"������ �ٽ� �ε��ϱ�"))
        {
            Load_ProtoAll();
        }

        ImGui::SameLine();
        ImGui::Dummy(ImVec2(100.f, 0.f));

        ImGui::SameLine();
        // ���� �� ���������� �ε�
        if (ImGui::Button(u8"���� ���̾ �߰��ϱ�"))
        {
            // ������ ���� �ε��ϵ��� �Ѵ�.
            Add_Object();
        }

        ImGui::Separator();

        // ������ ������
        for (size_t i = 0; i < m_vecProto.size(); i++)
        {
            bool bIsSelected_Proto = (m_iSelected_Proto == i);

            if (ImGui::Selectable(m_vecProto[i].strName.c_str(),
                bIsSelected_Proto, ImGuiSelectableFlags_AllowDoubleClick))
            {
                m_iSelected_Proto = i;
            }
        }

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
                m_iSelected_Layer_Remain = i;
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
    _int iSelected_Scene = m_iLoaded_Scene;
    _int iSelected_Layer = m_iSelected_Layer_Remain;
    _int iSelected_Object = m_iSelected_Object;
    
    _bool bIsEdited = false;    // �����Ǿ��� �� ��ȭ �̺�Ʈ

    if (iSelected_Layer == -1 && iSelected_Object == -1 && iSelected_Scene == -1)
        return;

    vector<FLayerData>& vecLayer = m_vecScene[iSelected_Scene].vecLayer;
    vector<FObjectData>& vecObject = vecLayer[iSelected_Layer].vecObject;

    if (ImGui::CollapsingHeader(u8"�̸�"))
    {
        // �̸�
        Set_Button_NonActiveColor();
        ImGui::Button(u8"�̸�", ImVec2(60, 0));
        Set_Button_ReturnColor();

        // �Էº�
        vecObject[iSelected_Object].strName.reserve(20);
        char  strInput[20] = {};
        strcpy_s(strInput, vecObject[iSelected_Object].strName.c_str());
            
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
                vecObject[iSelected_Object].strName = strInput;
            else
            {
                // �ڱ� �ڽ��̸��� ��� �������� ����
                if (vecObject[iSelected_Object].strName != strInput)
                    m_bInput_Warning = true;
            }
        }
        
        ImGui::PopItemWidth();
    }


    ImGui::Separator();
    if (ImGui::CollapsingHeader(u8"Ʈ������"))
    {
        ImGui::Text(u8"�̵�");
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
        ImGui::Button(u8"Z", ImVec2(60, 0));
        Set_Button_ReturnColor();
        

        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Translate", 
            vecObject[iSelected_Object].vPos, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iSelected_Object].vPos, 10000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##TranslateSlider",
            vecObject[iSelected_Object].vPos,
            -10000.f, 10000.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();


        ImGui::Separator();//--------------------------------------------------


        ImGui::Text(u8"ȸ��");
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
        ImGui::Button(u8"Z", ImVec2(60, 0));
        Set_Button_ReturnColor();
        
        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Rotate",
            vecObject[iSelected_Object].vRot, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iSelected_Object].vRot, 360.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##RoateSlider",
            vecObject[iSelected_Object].vRot,
            -360.f, 360.f))
        {
            bIsEdited = true;
        }
        ImGui::PopItemWidth();


        ImGui::Separator();//--------------------------------------------------


        ImGui::Text(u8"ũ��");
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
        ImGui::Button(u8"Z", ImVec2(60, 0));
        Set_Button_ReturnColor();
        
        ImGui::PushItemWidth((60.f + 6.f) * 3.f);
        if (ImGui::InputFloat3("##Scale",
            vecObject[iSelected_Object].vScale,
            "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            Clamp_Vec3(vecObject[iSelected_Object].vScale, 1000.f);
            bIsEdited = true;
        }
        if (ImGui::SliderFloat3("##ScaleSlider",
            vecObject[iSelected_Object].vScale,
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
            &vecObject[iSelected_Object].fPriority[EPRIORITY_OBJECT_UPDATE],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Update",
            &vecObject[iSelected_Object].bUsePriority[EPRIORITY_OBJECT_UPDATE]);
        

        // LateUpdate
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Late", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::PushItemWidth(60.f);
        ImGui::SameLine();
        if (ImGui::InputFloat("##PriorityLate",
            &vecObject[iSelected_Object].fPriority[EPRIORITY_OBJECT_LATE],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Late",
            &vecObject[iSelected_Object].bUsePriority[EPRIORITY_OBJECT_LATE]);

        // Render
        Set_Button_NonActiveColor();
        ImGui::Button(u8"Render", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::PushItemWidth(60.f);
        ImGui::SameLine();
        if (ImGui::InputFloat("##PriorityRender", 
            &vecObject[iSelected_Object].fPriority[EPRIORITY_OBJECT_RENDER],
            0.f, 0.f, "%.3f",
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox(u8"Use##Render",
            &vecObject[iSelected_Object].bUsePriority[EPRIORITY_OBJECT_RENDER]);
    }

    if (bIsEdited)
    {
        FLayerData& tLayerData = vecLayer[m_iSelected_Layer_Remain];
        FObjectData& tObjectData = vecObject[m_iSelected_Object];
        wstring strConvertLayer(tLayerData.strName.begin(), tLayerData.strName.end());
        wstring strConvertObject(tObjectData.strName.begin(), tObjectData.strName.end());

        CGameObject* pObj = Engine::Get_GameObject(strConvertLayer.c_str(), strConvertObject.c_str());
        if (nullptr != pObj)
        {
            CCubeObject* pCube = dynamic_cast<CCubeObject*>(pObj);
            if (pCube)
            {
                pCube->Get_TransformComponent()->Set_Pos(tObjectData.vPos);
                pCube->Get_TransformComponent()->Set_Rotation(tObjectData.vRot);
                pCube->Get_TransformComponent()->Set_Scale(tObjectData.vScale);
            }
        }
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

        if (ImGui::IsItemHovered())
        {
            Input_Camera();
        }

        // �� ����� ����� ��ġ���� �Ѱ� ���´�.
        D3DXMatrixLookAtLH(&m_matView, &Get_Pos(), &Get_Look(), &Get_Up());
        CImguiMgr::GetInstance()->Get_GraphicDev()->SetTransform(D3DTS_VIEW, &m_matView);

        // ī�޶� ������Ʈ
        D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFov, m_fAspect, m_fNear, m_fFar);
        CImguiMgr::GetInstance()->Get_GraphicDev()->SetTransform(D3DTS_PROJECTION, &m_matProj);

    }   ImGui::End();
}

void CImguiWin_MapTool::Load_ObjectToScene()
{
    if (m_iLoaded_Scene == -1)
        return;

    // ���� �ε�� ���� �ִٸ� ���� ���� �޸𸮸� �����Ͽ� ��ü�� �����Ѵ�.

    FSceneData& tSceneData = m_vecScene[m_iLoaded_Scene];


    for (size_t i = 0; i < tSceneData.vecLayer.size(); i++)
    {
        FLayerData& tLayerData = tSceneData.vecLayer[i];

        Create_LayerToScene(tLayerData);

        for (size_t j = 0; j < tLayerData.vecObject.size(); j++)
        {
            FObjectData& tObjectData = tLayerData.vecObject[j];
            wstring strConvert(tLayerData.strName.begin(), tLayerData.strName.end());

            Factory_GameObject(strConvert.c_str(), tObjectData.eObjectID, tObjectData);
        }
    }
}

void CImguiWin_MapTool::Create_LayerToScene(const FLayerData& tLayerData)
{
    wstring strConvert(tLayerData.strName.begin(), tLayerData.strName.end());

    Engine::Add_Layer(strConvert.c_str(), Engine::CLayer::Create(tLayerData.fPriority));
}

void CImguiWin_MapTool::Factory_GameObject(const _tchar* pLayerTag, const EGO_CLASS& eClassID, const FObjectData& tObjectData)
{
    switch (eClassID)
    {
    case Engine::ECLASS_NONE:
        
        break;
    case Engine::ECLASS_PLAYER:
        
        break;
    case Engine::ECLASS_BROWN:

        break;
    case Engine::ECLASS_GREY:

        break;
    case Engine::ECLASS_BOSS:

        break;
    case Engine::ECLASS_FOOD:

        break;
    case Engine::ECLASS_WEAPON:

        break;
    case Engine::ECLASS_THROW:

        break;
    case Engine::ECLASS_INTERACTION:

        break;
    case Engine::ECLASS_BUILDING:
    {
        CGameObject* pObj = static_cast<CGameObject*>(CCubeObject::Create(CImguiMgr::GetInstance()->Get_GraphicDev(),
            tObjectData.vPos, tObjectData.vRot, tObjectData.vScale));
        wstring strConvert(tObjectData.strName.begin(), tObjectData.strName.end());
        pObj->Set_ObjectName(strConvert);
        Engine::Add_GameObject(pLayerTag, pObj);
        break;
    }
    default:
        break;
    }
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
            
            // ������Ʈ �Ľ�
            for (size_t k = 0; k < tLayerData.vecObject.size(); k++)
            {
                FObjectData& tObjectData = tLayerData.vecObject[k];

                FSerialize_GameObject tObjectSerial;
                tObjectSerial.tHeader.eType = ESERIALIZE_OBJECT;
                tObjectSerial.tHeader.strName = tObjectData.strName;

                tObjectSerial.eID = tObjectData.eObjectID;

                tObjectSerial.fPriority_Update = tObjectData.fPriority[0];
                tObjectSerial.fPriority_LateUpdate = tObjectData.fPriority[1];
                tObjectSerial.fPriority_Render = tObjectData.fPriority[2];

                tObjectSerial.bUsePriority_Update = tObjectData.bUsePriority[0];
                tObjectSerial.bUsePriority_LateUpdate = tObjectData.bUsePriority[1];
                tObjectSerial.bUsePriority_Render = tObjectData.bUsePriority[2];

                tObjectSerial.vPos = tObjectData.vPos;
                tObjectSerial.vRotation = tObjectData.vRot;
                tObjectSerial.vScale = tObjectData.vScale;

                // �Ϸ�� ���̾ ����ȭ �߰�
                tLayerSerial.vecGameObject.push_back(tObjectSerial);
            }
            // �Ϸ�� ���� ����ȭ �߰�
            tSceneSerial.vecLayer.push_back(tLayerSerial);
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
        Reset_Hierarchi();
    }
    else
    {
        m_vecScene.clear();
        Reset_Hierarchi();

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

                    tObjectData.strName = tObjectSerial.tHeader.strName;

                    tObjectData.eObjectID = tObjectSerial.eID;

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

void CImguiWin_MapTool::Load_ProtoAll()
{
    // �� ����� ������ �������� �ε��Ѵ�.
    _finddata_t fd;
    intptr_t handle;
    if ((handle = _findfirst((g_strProtoPath + "*" + g_strProtoExt).c_str(), &fd)) == -1L)
    {
        cout << "������ ���� ����!!" << endl;
        m_vecProto.clear();
    }
    else
    {
        m_vecProto.clear();
        m_iSelected_Proto = -1;

        // ��� �ε�
        do
        {
            string strName = fd.name;
            size_t extPos = strName.find_last_of('.');
            if (extPos == string::npos)
                continue;

            // Ȯ���� üũ �� �ε��Ѵ�.
            if (strName.substr(extPos) == g_strProtoExt)
            {
                FSerialize_Proto tProtoSerial;
                FProtoData tProtoData;

                Import_Proto(strName.substr((size_t)0, extPos), tProtoSerial, tProtoData);
            }
        } while (_findnext(handle, &fd) == S_OK);
    }

    _findclose(handle);
}

void CImguiWin_MapTool::Import_Proto(const string& strName, FSerialize_Proto& tProtoSerial, FProtoData& tProtoData)
{
    string strJson;
    ifstream inputFile(g_strProtoPath + strName + g_strProtoExt);
    if (inputFile.is_open())
    {
        // ���ڿ� ���� �о���� �ݺ���
        strJson = string(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
        inputFile.close();
        cout << "\n���� �ҷ���!\n";

        // �Ľ� ������ ������ Data�� ����
        if (tProtoSerial.Receive_ByRapidJSON(strJson))
        {
            tProtoData.strName = tProtoSerial.tHeader.strName;
            tProtoData.eID = tProtoSerial.eID;
            tProtoData.vPos = tProtoSerial.vPos;
            tProtoData.vRot = tProtoSerial.vRot;
            tProtoData.vScale = tProtoSerial.vScale;

            m_vecProto.push_back(tProtoData);
        }
    }
    else
    {
        cerr << "������ �ҷ����� �� ����!\n";
    }
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

    // ���� ��ȸ�� �����͸� ��
    _finddata_t fd;
    intptr_t handle;
    if ((handle = _findfirst((g_strTerrainPath + "*" + g_strTerrainExt).c_str(), &fd)) == -1L)
    {
        cout << "������ ���� ����!!" << endl;
    }
    // �ͷ��� ���� �����Ͱ� ���ٸ� �ε����� �ʰ� ���ο� �����͸� ����Ͽ� �����.
    else
    {
        // ����ã��
        do
        {
            string strFdName = fd.name;
            size_t extPos = strFdName.find_last_of('.');
            if (extPos == string::npos)
                break;

            // �ε��Ϸ��� �̸��� �����̸� Ȯ�� �� ���� �ε�
            if (strFdName.substr((size_t)0, extPos) == strName)
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
    ifstream inputFile(g_strTerrainPath + strName + g_strTerrainExt);
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

void CImguiWin_MapTool::Add_Object()
{
    if (m_iLoaded_Scene == -1 || m_iSelected_Layer_Remain == -1 || m_iSelected_Proto == -1)
        return;

    // �߰��� �� �������� �⺻ �̸��� ������.
    // ���� �Ӽ� �ǿ��� �����ϴ� ����� ������.
    
    const FProtoData& tProtoData = m_vecProto[m_iSelected_Proto];
    FObjectData tObjectData;

    tObjectData.eObjectID = tProtoData.eID;
    tObjectData.vRot = tProtoData.vRot;
    tObjectData.vScale = tProtoData.vScale;
    tObjectData.strName = tProtoData.strName;

    FLayerData& tLayerData = m_vecScene[m_iLoaded_Scene].vecLayer[m_iSelected_Layer_Remain];
    _bool bAdded = false;

    auto iter = find_if(tLayerData.vecObject.begin(), tLayerData.vecObject.end(),
        [&tObjectData](FObjectData& tDstObjectData) {
            return tObjectData.strName == tDstObjectData.strName;
        });
    if (iter == tLayerData.vecObject.end())
    {
        tLayerData.vecObject.push_back(tObjectData);
        bAdded = true;
    }
    // ���� �̸��� ������ �̸��� ���ڸ� �ٿ� �߰��Ѵ�.
    else
    {
        _uint i = 0;
        while(true)
        {
            stringstream ss;
            ss << i;
            string strAdd = tObjectData.strName + ss.str();

            auto iterRe = find_if(tLayerData.vecObject.begin(), tLayerData.vecObject.end(),
                [&strAdd](FObjectData& tDstObjectData) {
                    return strAdd == tDstObjectData.strName;
                });
            if (iterRe == tLayerData.vecObject.end())
            {
                tObjectData.strName = strAdd;
                tLayerData.vecObject.push_back(tObjectData);
                bAdded = true;

                break;
            }

            ++i;
        }
    }

    if (bAdded)
    {
        wstring strConvert(tLayerData.strName.begin(), tLayerData.strName.end());
        Factory_GameObject(strConvert.c_str(), tObjectData.eObjectID, tObjectData);
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

_bool CImguiWin_MapTool::Check_ObjectName_InLayer()
{
    return _bool();
}

void CImguiWin_MapTool::Input_Camera()
{
    // �����ϰ� ����.
    // ī�޶�� ���콺 ��� Ŭ������ �� ���� �̵�
    

    // ���콺 ��Ŭ������ �� ���� ȸ���� �Ѵ�.
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
        ImVec2 fDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
        
        _vec3 vLook = Get_Look() - Get_Pos();
        D3DXVec3Normalize(&vLook, &vLook);
        _vec3 vRight, vUp;
        
        //D3DXVec3Cross(&vLook, &vRight);
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        m_fDrag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    }

    // ���콺 ��Ŭ���� WASD�� ī�޶� ��ü�� �̵���ų �� �ִ�.
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        _vec3 vUp = Get_Up();
        _vec3 vLook = Get_Look() - Get_Pos();
        _vec3 vRight;

        D3DXVec3Normalize(&vLook, &vLook);
        
        D3DXVec3Cross(&vRight, &vUp, &vLook);
        D3DXVec3Normalize(&vRight, &vRight);

        D3DXVec3Cross(&vUp, &vLook, &vRight);
        D3DXVec3Normalize(&vUp, &vUp);

        m_fPrevDrag = m_fDrag;
        ImVec2 fDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        m_fDrag = fDelta;
        cout << m_fDrag.x << ", " << m_fDrag.y << endl;
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            // x�� UpVector�������� ȸ��, y�� RightVector�������� ȸ��
            _matrix matRotX, matRotY, matResult;
            _vec4 vResult;
            D3DXMatrixRotationAxis(&matRotX, &vUp, D3DXToRadian((m_fDrag - m_fPrevDrag).x * 0.34f));
            D3DXMatrixRotationAxis(&matRotY, &vRight, D3DXToRadian((m_fDrag - m_fPrevDrag).y * 0.34f));

            matResult = matRotX * matRotY;

            D3DXVec3Transform(&vResult, &vLook, &matResult);

            vLook = { vResult.x, vResult.y, vResult.z };
            D3DXVec3Normalize(&vLook, &vLook);

            Get_Look() = Get_Pos() + vLook * D3DXVec3Length(&(Get_Look() - Get_Pos()));

            vUp = Get_Up();
            vLook = Get_Look() - Get_Pos();

            D3DXVec3Normalize(&vLook, &vLook);

            D3DXVec3Cross(&vRight, &vUp, &vLook);
            D3DXVec3Normalize(&vRight, &vRight);

            D3DXVec3Cross(&vUp, &vLook, &vRight);
            D3DXVec3Normalize(&vUp, &vUp);
        }

        _float fMul = 10.f;
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
            fMul *= 2.f;

        // Look �������� ȭ���� �̵�
        if (ImGui::IsKeyDown(ImGuiKey_W))
        {
            Get_Pos() += vLook * fMul * 0.034f;
            Get_Look() += vLook * fMul * 0.034f;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_S))
        {
            Get_Pos() -= vLook * fMul * 0.034f;
            Get_Look() -= vLook * fMul * 0.034f;
        }
        if (ImGui::IsKeyDown(ImGuiKey_A))
        {
            Get_Pos() -= vRight * fMul * 0.034f;
            Get_Look() -= vRight * fMul * 0.034f;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_D))
        {
            Get_Pos() += vRight * fMul * 0.034f;
            Get_Look() += vRight * fMul * 0.034f;
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q))
        {
            Get_Pos() -= vUp * fMul * 0.034f;
            Get_Look() -= vUp * fMul * 0.034f;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_E))
        {
            Get_Pos() += vUp * fMul * 0.034f;
            Get_Look() += vUp * fMul * 0.034f;
        }
    }

    // 
}


