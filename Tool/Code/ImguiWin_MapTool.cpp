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
    m_vecHierarchi.push_back(pair_string_vector(u8"Environment", vector<string>()));
    m_vecHierarchi[0].second.push_back("Test1");
    m_vecHierarchi[0].second.push_back("Test2");
    m_vecHierarchi[0].second.push_back("Test3");
    m_vecHierarchi.push_back(pair_string_vector(u8"GameLogic", vector<string>()));
    m_vecHierarchi.push_back(pair_string_vector(u8"Camera", vector<string>()));
    m_vecHierarchi.push_back(pair_string_vector(u8"UI", vector<string>()));
    //swap(m_vecHierarchi[1], m_vecHierarchi[3]);


    // 터레인 요소 초기화
    m_vecInput_Terrain.resize(EINPUT_TERRAIN_END);
    for_each(m_vecInput_Terrain.begin(), m_vecInput_Terrain.end(), 
        [](string& str) {
            str.reserve(20);
        });
    fill(m_vecInput_Terrain.begin(), m_vecInput_Terrain.end(), "0");

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

            ImGui::DockBuilderDockWindow(u8"뷰어", dockspace_id);
            ImGui::DockBuilderDockWindow(u8"계층", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"속성", dock_property_id);
            
            
            ImGuiID dock_browser_scene = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 200.f / vDockSize.y, NULL, &dockspace_id);

            ImGui::DockBuilderDockWindow(u8"브라우저", dock_browser_scene);

            ImGui::DockBuilderFinish(dockspace_id);
            
            m_bFirstLoop = false;
        }
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

    Layout_Browser(iMain_Flags);
    Layout_Hierarchy(iMain_Flags);
    Layout_Property(iMain_Flags);
    Layout_Viewer(iMain_Flags);

    ImGui::End();

    return 0;
}

void CImguiWin_MapTool::Layout_Browser(const ImGuiWindowFlags& iMain_Flags)
{
    // 리소스
    if (ImGui::Begin(u8"브라우저", NULL, iMain_Flags))
    {
        if (ImGui::BeginTabBar(u8"에셋"))
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
    if (ImGui::BeginTabItem(u8"씬"))
    {


        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Terrain()
{
    if (ImGui::BeginTabItem(u8"지형"))
    {
        ImGui::BeginGroup();

        // 가로 세팅
        Set_Button_NonActiveColor();
        ImGui::Button(u8"가로", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain1", u8"X",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_HORIZON].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_HORIZON].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }


        ImGui::SameLine();
        // 세로 세팅
        Set_Button_NonActiveColor();
        ImGui::Button(u8"세로", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain2", u8"Y",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_VERTICLE].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_VERTICLE].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }


        ImGui::SameLine();
        // 높이 세팅
        Set_Button_NonActiveColor();
        ImGui::Button(u8"높이", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain3", u8"Z",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_HEIGHT].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_HEIGHT].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }


        // 오프셋 세팅
        Set_Button_NonActiveColor();
        ImGui::Button(u8"오프셋", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain4-1", u8"X",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_X].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_X].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain4-2", u8"Y",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Y].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Y].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain4-3", u8"Z",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Z].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Z].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }



        // 스케일 세팅
        Set_Button_NonActiveColor();
        ImGui::Button(u8"스케일", ImVec2(60, 0));
        Set_Button_ReturnColor();

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain5-1", u8"X",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_X].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_X].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain5-2", u8"Y",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Y].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Y].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }

        ImGui::SameLine();
        if (ImGui::InputTextEx(u8"##InputText_Terrain5-3", u8"Z",
            const_cast<char*>(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Z].c_str()),
            m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Z].capacity(),
            ImVec2(100, 0),
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {

        }


        ImGui::Separator();
        if (ImGui::Button(u8"적용하기", ImVec2(60, 0)))
        {
            Apply_Terrain();
        }



        ImGui::EndGroup();


        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Object()
{
    if (ImGui::BeginTabItem(u8"오브젝트"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Light()
{
    if (ImGui::BeginTabItem(u8"조명"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Browser_Camera()
{
    if (ImGui::BeginTabItem(u8"카메라"))
    {

        ImGui::EndTabItem();
    }
}

void CImguiWin_MapTool::Layout_Hierarchy(const ImGuiWindowFlags& iMain_Flags)
{
    // 계층
    if (ImGui::Begin(u8"계층", NULL, iMain_Flags))
    {
        // 이름
        ImGui::Text(m_strSceneName.c_str());
        ImGui::Separator();

        ImGui::Indent();
        for (_uint i = 0; i < m_vecHierarchi.size(); i++)
        {
            bool bIsSelected_Layer = (m_iSelectedHierarchi_Layer == i);

            if (ImGui::Selectable(m_vecHierarchi[i].first.c_str(),
                bIsSelected_Layer, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (bIsSelected_Layer)
                {
                    
                }
                m_iSelectedHierarchi_Layer = i;
                m_iSelectedHierarchi_Object = -1;
            }

            ImGui::Indent();
            for (_uint j = 0; j < m_vecHierarchi[i].second.size(); j++)
            {
                bool bIsSelected_Object = (m_iSelectedHierarchi_Object == j);

                if (ImGui::Selectable(m_vecHierarchi[i].second[j].c_str(),
                    bIsSelected_Object, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (bIsSelected_Object)
                    {

                    }
                    m_iSelectedHierarchi_Object = j;
                    m_iSelectedHierarchi_Layer = -1;
                }
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();

        ImGui::Separator();
        bool bInput_Apply = false;
        if (ImGui::Button(u8"레이어 추가"))
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
            string strInput = m_arrAddLayer_Buf; // UTF-8 문자열로 변환
            if (strInput.length() > 0)
            {
                auto iter = find_if(m_vecHierarchi.begin(), m_vecHierarchi.end(),
                    [&strInput](pair_string_vector& pairDst) {
                        return strInput == pairDst.first;
                    });
                if (iter == m_vecHierarchi.end())
                {
                    m_vecHierarchi.push_back(pair_string_vector(strInput, vector<string>()));
                }
            }
            memset(m_arrAddLayer_Buf, 0, IM_ARRAYSIZE(m_arrAddLayer_Buf));
        }

    }   ImGui::End();
}

void CImguiWin_MapTool::Layout_Property(const ImGuiWindowFlags& iMain_Flags)
{
    // 계층
    if (ImGui::Begin(u8"속성", NULL, iMain_Flags))
    {


    }   ImGui::End();
}

void CImguiWin_MapTool::Layout_Viewer(const ImGuiWindowFlags& iMain_Flags)
{
    // 뷰어
    if (ImGui::Begin(u8"뷰어", NULL, iMain_Flags | ImGuiWindowFlags_NoNavFocus))
    {
        // 마진 없애기
        ImGuiStyle& style = ImGui::GetStyle();
        style.ItemSpacing = ImVec2(0, 2);

        ImVec2 vAlignMargin = ImGui::GetContentRegionAvail();
        _float fMarginContent = 400.f;

        // 가운데 정렬
        ImGui::Dummy(ImVec2((vAlignMargin.x - fMarginContent) * 0.5f, 0.f));

        // 좌표계
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f); // 버튼을 비활성화 상태로 만듭니다.
        ImGui::SameLine();
        ImGui::Button(u8"좌표계");
        ImGui::PopStyleVar();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##좌표계", m_vecCoord_Item[m_iIndex_CoordItem].c_str(), 
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

        // 옆 마진
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(8.f, 0.f));

        // 피봇
        ImGui::SameLine();
        ImGui::Button(u8"피봇");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##피봇", m_vecPivot_Item[m_iIndex_PivotItem].c_str(),
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

        // 옆 마진
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(8.f, 0.f));

        if (m_bSnap_Active)
            Set_Button_ActiveColor();

        // 스냅
        ImGui::SameLine();
        if (ImGui::Button(u8"스냅"))
        {
            m_bSnap_Active = !m_bSnap_Active;
        }

        Set_Button_ReturnColor();

        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        if (ImGui::BeginCombo(u8"##스냅", m_vecSnap_Item[m_iIndex_SnapItem].c_str(),
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

        // 아이템 간격 복원
        ImGui::GetStyle().ItemSpacing = ImVec2(8, 4);

        // 가운데 정렬
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

void CImguiWin_MapTool::Apply_Terrain()
{
    // 적용버튼이 눌렸을 때 새로운 터레인을 제조하고 맵에 배치해준다.

    // 데이터 구성
    FSerialize_Terrain tTerrain;
    tTerrain.tHeader.strType = ESERIALIZE_TERRAIN;
    tTerrain.tHeader.strName = "Terrain";
    tTerrain.vVertexCount.x = stof(m_vecInput_Terrain[EINPUT_TERRAIN_HORIZON]);
    tTerrain.vVertexCount.y = stof(m_vecInput_Terrain[EINPUT_TERRAIN_HEIGHT]);
    tTerrain.vVertexCount.z = stof(m_vecInput_Terrain[EINPUT_TERRAIN_VERTICLE]);
    tTerrain.vScale.x = stof(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_X]);
    tTerrain.vScale.y = stof(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Y]);
    tTerrain.vScale.z = stof(m_vecInput_Terrain[EINPUT_TERRAIN_SCALE_Z]);
    tTerrain.vInvOffset.x = stof(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_X]);
    tTerrain.vInvOffset.y = stof(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Y]);
    tTerrain.vInvOffset.z = stof(m_vecInput_Terrain[EINPUT_TERRAIN_OFFSET_Z]);


    Export_ParsedTerrain(tTerrain);
}

void CImguiWin_MapTool::Export_ParsedTerrain(const FSerialize_Terrain& tTerrain)
{
    // 생성한 JSON 문서를 가지고 writer에 적용
    Document doc;
    StringBuffer buffer;

    tTerrain.Parse_RapidJSON(doc, buffer, true);

    string strJson = buffer.GetString();
    string strPath = "../Client/Resource/Data/Terrain/";
    string strFileName = "Test";
    ofstream outputFile(strPath + strFileName + m_strSceneExt);
    if (outputFile.is_open())
    {
        outputFile << strJson;
        outputFile.close();
        cout << string("Test") << "파일 저장함\n";
    }
    else
    {
        cerr << "파일을 저장할 수 없소!\n";
    }

    // 파일 읽기 테스트
    ifstream inputFile(strPath + strFileName + m_strSceneExt);
    if (inputFile.is_open())
    {
        // 문자열 쉽게 읽어오는 반복자
        string strIter(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>());
        inputFile.close();
        cout << strJson << "\n파일 불러옴!\n";
    }
    else
    {
        cerr << "파일을 불러들일 수 없소!\n";
    }

    const_cast<FSerialize_Terrain&>(tTerrain).Receive_ByRapidJSON(strJson);
    int t = 0;
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
    // 버튼 활성화 색상 복원
    ImGui::GetStyle().Colors[ImGuiCol_Button] = m_pStyle.Colors[ImGuiCol_Button];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = m_pStyle.Colors[ImGuiCol_ButtonHovered];
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = m_pStyle.Colors[ImGuiCol_ButtonActive];
}
