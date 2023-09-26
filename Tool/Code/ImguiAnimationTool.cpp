#include "ImguiAnimationTool.h"
#include <Windows.h>
#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>
#include "MyTexture.h"

CImguiAnimationTool::CImguiAnimationTool()
{
}

CImguiAnimationTool::~CImguiAnimationTool()
{
}

CImguiAnimationTool* CImguiAnimationTool::Create()
{
    ThisClass* pInstance = new ThisClass;

    if (FAILED(pInstance->Ready_ImguiWin()))
    {
        Safe_Release(pInstance);

        MSG_BOX("BackGround Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CImguiAnimationTool::Free()
{
    SUPER::Free();

    if (m_pTexture != nullptr)
        m_pTexture->Release();
    if (m_pRenderTargetSurface != nullptr)
        m_pRenderTargetSurface->Release();
}

void CImguiAnimationTool::AddImage(const std::string& imagePath)
{

    // imagePath를 Wide 문자열로 변환
    WCHAR wideImagePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, imagePath.c_str(), -1, wideImagePath, MAX_PATH);

    // 이미지를 로드하고 텍스처로 변환
    LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
    if (FAILED(D3DXCreateTextureFromFile(pGraphicDev, wideImagePath, &m_pTexture)))
    {
        // 이미지 로드 실패 처리
        MSG_BOX("Image Load Failed");
    }
}

HRESULT CImguiAnimationTool::Ready_ImguiWin()
{
    return S_OK;
}

//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

//bool show_demo_window = true;
//bool show_another_window = false;

//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
//ImGui::Checkbox("Another Window", &show_another_window);

_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    /*if (m_pTexture != nullptr)
        m_pTexture->Release();
    if (m_pRenderTargetSurface != nullptr)
        m_pRenderTargetSurface->Release();*/

        /*LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
        pGraphicDev->GetRenderTarget(0, &m_pRenderTargetSurface);
        pGraphicDev->CreateTexture(WINCX, WINCY, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL);
        m_pTexture->GetSurfaceLevel(0, &m_pRenderTargetSurface);*/

        //m_pTexture = *CImguiMgr::GetInstance()->Get_EditorTexture();



#pragma region 테스트1

        static int counter = 0;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::Button(u8"이미지 로드 버튼")) // @@@@@ 문제 2. 이미지 로드 시간이 비정상적이게 길다. (폴더 많이 있으면 더 길어짐) @@@@@
        {
            LoadImg(L"../Client/Resource");
        }

        // 위치(Pos) 조정
        ImGui::Text(u8"위치 조절");
        ImGui::SliderFloat("x", &m_AniInfo.m_vInfo[INFO_POS].x, 0.0f, 300.0f);
        ImGui::SliderFloat("y", &m_AniInfo.m_vInfo[INFO_POS].y, 0.0f, 300.0f);
        ImGui::SliderFloat("z", &m_AniInfo.m_vInfo[INFO_POS].z, 0.0f, 300.0f);

        // 회전(Rot) 조정 Test
        ImGui::Text(u8"회전 조절");
        ImGui::SliderFloat("Rot", &m_fX, 0.0f, 300.0f);

        if (ImGui::Button(u8"프레임 추가 버튼"))      // 프레임 추가 버튼
        m_vecAnimationInfo.push_back(m_AniInfo);    // 프레임을 애니메이션 정보 벡터에 추가

        ImGui::SameLine(); // 같은 라인
        ImGui::Text(u8"생성된 프레임 = Frame_%d", m_vecAnimationInfo.size());

#pragma region LisBox 1번
        // 리스트 상자에 표시할 항목 목록
        std::vector<const char*> frameNames;
        for (int i = 0; i < m_vecAnimationInfo.size(); i++) {
            // "Frame_%d" 형식의 문자열을 생성하여 벡터에 추가
            frameNames.push_back(("Frame_" + std::to_string(i)).c_str());
        }

        // ListBox를 생성하고 항목을 표시
        if (ImGui::ListBox("Frames", &selectedItemIndex, frameNames.data(), frameNames.size())) 
        {
            // 선택한 항목에 대한 작업을 수행
            if (selectedItemIndex >= 0 && selectedItemIndex < m_vecAnimationInfo.size()) {
                // selectedItemIndex를 사용하여 선택한 프레임에 대한 작업을 수행

            }
        }
#pragma endregion

#pragma region LisBox 2번
        //for (const auto& objKey : uniqueObj)
        //{
        //    for (const auto& stateKey : m_MapFile[objKey])
        //    {
        //        // 텍스처를 ImGui 텍스처로 변환
        //        LPDIRECT3DTEXTURE9 pTexture = m_MapTexture[std::make_pair(objKey, stateKey)];
        //        if (pTexture)
        //        {
        //            ImTextureID textureID = reinterpret_cast<ImTextureID>(pTexture);
        //            items[itemIndex] = stateKey.c_str();
        //            if (ImGui::ListBox(items[itemIndex], &selectedItemIndex, itemIndex))
        //            {
        //                // ListBox에서 아이템을 선택했을 때 수행할 작업 추가
        //                // selectedTextureIndex에는 선택한 아이템의 인덱스가 포함됩니다.
        //            }
        //            itemIndex++;
        //        }
        //    }
        //}
#pragma endregion

        ImGui::InputText("FrameName", "FrameName", IM_ARRAYSIZE("EEEEEEEEEEEE"));

        if (ImGui::Button(u8"저장 버튼"))
        {

        }

        if (ImGui::Button(u8"불러오기 버튼"))
        {

        }

#pragma endregion

#pragma region 테스트2
        ImGui::Text(u8"테스트 %d", 123);
        if (ImGui::Button(u8"저장"))
        {
            // do stuff
        }
        float fSlider2 = 0.f;

        ImGui::InputText("string", "Hi", IM_ARRAYSIZE("Hi"));
        ImGui::SliderFloat("float", &fSlider2, 0.0f, 1.0f);
#pragma endregion


#pragma region 테스트3
        _bool   my_tool_active;
        // Create a window called "My First Tool", with a menu bar.

        // 테스트 툴 탭 (전체)
        ImGui::Begin(u8"테스트 툴3", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar()) // 메뉴바 목록
        {
            // 메뉴바를 눌렀을 때 목록
            if (ImGui::BeginMenu(u8"파일")) // 메뉴바 1번 @@@@@ 문제 1. 파일을 누르면 불러오기, 저장, 닫기가 다 실행됨 @@@@@
            {
                // 버튼별 기능 구현
                if (ImGui::MenuItem(u8"불러오기", "Ctrl+O")) { /* Do stuff */ }
                {
                   
                    // 불러오기 작성
                    // Open 함수를 통해 이미지 불러오기
                    //std::string imagePath = OpenImageFileDialog();
                }
                if (ImGui::MenuItem(u8"저장", "Ctrl+S")) { /* Do stuff */ }
                {
                    // 저장 작성
                }
                if (ImGui::MenuItem(u8"닫기", "Ctrl+W")) { my_tool_active = false; }
                {
                    // 닫기
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(u8"테스트 메뉴바 추가")) // 메뉴바 2번
            {
                // 버튼별 기능 구현
                if (ImGui::MenuItem(u8"테스트 버튼1", u8"단축키1")) { /* Do stuff */ }
                {
                    // 불러오기 작성
                }
                if (ImGui::MenuItem(u8"테스트 버튼2", u8"단축키2")) { /* Do stuff */ }
                {
                    // 저장 작성
                }
                if (ImGui::MenuItem(u8"테스트 버튼3", u8"단축키3")) { my_tool_active = false; }
                {
                    // 닫기
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        static float my_color[4] = { 0.4f, 0.7f, 0.0f, 0.5f };

        // Edit a color (stored as ~4 floats)
        ImGui::ColorEdit4(u8"색상", my_color);

        // Plot some values
        const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
        ImGui::PlotLines(u8"프레임", my_values, IM_ARRAYSIZE(my_values));

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        ImGui::BeginChild(u8"스크롤");
        for (int n = 0; n < 50; n++)
            ImGui::Text(u8"%04d: 텍스트", n);
        ImGui::EndChild();
        ImGui::End();
#pragma endregion

#pragma region 툴ex
        //if (ImGui::TreeNode(u8"UI 툴"))
        //{

        //    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        //    if (ImGui::BeginTabBar("UI 툴", tab_bar_flags))
        //    {
        //        if (ImGui::BeginTabItem(u8"UI_1"))
        //        {

        //            ImGui::SeparatorText(u8"추가한 데이터 (인덱스기준)");
        //            {
        //                if (ImGui::Button(u8"빈 데이터 추가"))
        //                {
        //                    CMyUI* pMyUI = CMyUI::Create(Engine::Get_GraphicDev(), nullptr, UI_TYPE::BASIC);

        //                    m_vecUIList.push_back(pMyUI);
        //                    m_vecUIName.push_back(L"test");

        //                }
        //                ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);

        //                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));

        //                ImGui::BeginListBox("Red", ImVec2(200, 800));
        //                {
        //                    for (int i = 0; i < m_vecUIList.size(); ++i)
        //                    {
        //                        if (ImGui::Selectable(to_string(i).c_str()))
        //                        {
        //                            selected_UIIndex = i;
        //                        }

        //                    }
        //                }
        //                ImGui::EndListBox();
        //                ImGui::PopStyleColor();
        //            }

        //            ImGui::SameLine();

        //            if (m_vecUIList.size() != 0 && m_vecUIList[selected_UIIndex] && !m_bClick)
        //            {
        //                ImGui::BeginListBox(u8"이미지 박스", ImVec2(800, 800));
        //                {

        //                    int UI_Index = 0;
        //                    for (auto iter : m_mapLoadUI1)
        //                    {
        //                        ImGui::PushID(UI_Index);
        //                        if (ImGui::ImageButton("", iter.second->Get_Info()->pTexture, ImVec2(96.f, 96.f)))
        //                        {
        //                            //lstrcmp(selected_UI_FileName, iter.first);
        //                            m_vecUIName[selected_UIIndex] = iter.first;
        //                            m_vecUIList[selected_UIIndex] = iter.second;
        //                            m_bClick = true;
        //                        }

        //                        ++UI_Index;
        //                        ImGui::PopID();

        //                        if (UI_Index % 6 == 0)
        //                        {
        //                            ImGui::NewLine();
        //                        }
        //                        else
        //                        {
        //                            ImGui::SameLine();
        //                        }
        //                    }
        //                }
        //                ImGui::EndListBox();

        //                if (ImGui::Button(u8"저장하기"))
        //                {
        //                    for (int i = 0; i < m_vecUIList.size(); ++i)
        //                    {
        //                        m_mapChoiceUI.emplace(m_vecUIName[i], m_vecUIList[i]);
        //                    }

        //                    SaveData(L"UI");
        //                }

        //            }
        //            else if (m_bClick)
        //            {
        //                ImGui::SameLine();
        //                ImGui::BeginListBox(u8"데이터 입력", ImVec2(800, 800));
        //                {

        //                    ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
        //                    ImVec2 uv1 = ImVec2(1.0f, 1.0f);                          // UV coordinates for (32,32) in our texture
        //                    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
        //                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        //                    ImGui::SameLine();
        //                    ImGui::Image(m_vecUIList[selected_UIIndex]->Get_Info()->pTexture, ImVec2(96.f, 96.f), uv0, uv1, tint_col, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        //                    ImGui::InputFloat3(u8"위치", m_vecUIList[selected_UIIndex]->Get_Info()->vPos);
        //                    ImGui::InputFloat3(u8"크기", m_vecUIList[selected_UIIndex]->Get_Info()->vSize);

        //                    if (ImGui::Button(u8"선택 취소"))
        //                    {
        //                        m_bClick = false;
        //                    }
        //                }
        //                ImGui::EndListBox();
        //            }
        //            ImGui::EndTabItem();
        //        }
        //        ImGui::EndTabBar();
        //    }
        //    ImGui::Separator();
        //    ImGui::TreePop();
        //}
#pragma endregion

    ImGui::Begin(u8"애니메이션 툴 테스트");


    /*if (m_pTexture)
    {
        ImGui::Image((void*)m_pTexture, ImVec2(WINCX, WINCY));
        m_pTexture->Release();
        m_pTexture = nullptr;
    }*/

    ImGui::End();


    return 0;
}
// 제작중
//std::string CImguiAnimationTool::OpenImageFileDialog()
//{
//    std::string imagePath; // 선택한 이미지의 경로를 저장할 변수
//
//    // 파일 대화 상자 구조체 초기화
//    OPENFILENAME OpenFile;
//
//    // 파일 패스
//    char szFile[MAX_PATH] = "";
//
//    // Wide 문자열로 변환
//    std::wstring w_szFile = ConvertToWideString(szFile);
//  
//    // 이미지 패스 설정
//    std::string ImagePath = "이미지 파일 (*.bmp;*.jpg;*.png;*.gif)\0*.bmp;*.jpg;*.png;*.gif\0모든 파일 (*.*)\0*.*\0";
//
//    // Wide 문자열로 변환
//    std::wstring WideImagePath = ConvertToWideString(ImagePath);
//
//    ZeroMemory(&OpenFile, sizeof(OpenFile));
//    OpenFile.lStructSize = sizeof(OpenFile);
//    OpenFile.hwndOwner = NULL; // 부모 윈도우 핸들, NULL로 설정하면 메인 윈도우를 기준으로 대화 상자가 열립니다.
//    OpenFile.lpstrFilter = WideImagePath.c_str(); 
//    OpenFile.lpstrFile = &w_szFile[0]; // LPWSTR로 변환
//    OpenFile.nMaxFile = sizeof(szFile);
//    OpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
//
//    // 파일 대화 상자 열기
//    if (GetOpenFileName(&OpenFile) == TRUE)
//    {
//        imagePath = szFile; // 선택한 파일 경로를 imagePath 변수에 저장
//    }
//
//    return imagePath;
//}

// 문자열 타입 변환
std::wstring CImguiAnimationTool::ConvertToWideString(const std::string& ansiString)
{
    int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (wideStrLen == 0)
    {
        // 변환 실패 처리
        return L"Fail";
    }

    std::wstring wideString(wideStrLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

    return wideString;
}

//// 파일 불러오기
//HRESULT CImguiAnimationTool::OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev)
//{
//    //파일 및 디렉토리 정보를 저장하기 위한 구조체
//    WIN32_FIND_DATA findData;
//
//    //floderPath의 값을 wFolderPath에 저장 (폴더 경로를 유니코드문자열로 처리)
//    wstring wfolderPath = (wstring)folderPath + L"\\*.*";
//
//    // 폴더 경로 저장 및 핸들 반환
//    HANDLE hFind = FindFirstFileW(wfolderPath.c_str(), &findData);
//
//    if (hFind != INVALID_HANDLE_VALUE)
//    {
//        //폴더 내의 모든 파일과 디렉토리를 검색 FindNextFile함수를 사용해서 다음파일 또는 디렉토리를 찾는다
//        do
//        {
//            //  파일의 속성 정보가 입력된다. (디렉토리인지 파일인지 등)
//            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            {
//                // 디렉토리인 경우, "."(현재 디렉토리)와 ".."(상위 디렉토리)를 제외
//                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
//                {
//                    // 하위 폴더의 경로 생성
//                    wstring subFolderPath = (wstring)folderPath + L"\\" + findData.cFileName;
//                    
//                    // 재귀 호출로 하위 디렉토리 검사
//                    //ReadImgPath(subFolderPath.c_str(), pGraphicDev); 수정필요
//
//                }
//            }
//            else
//            {
//                // 파일인 경우, 이미지 파일인지 확인하고 로드
//                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;
//
//                // 파일 확장자 확인
//                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
//                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga"))
//                {
//                    IDirect3DBaseTexture9* pTexture = nullptr;
//                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pTexture)))
//                    {
//                        // 최종 키값 szKey
//                        const _tchar* szKey = findData.cFileName;
//                        CMyTexture* pMyTexture = nullptr;
//
//                        // 키값을 넘겨 파일을 찾아본다.
//                        if (!FindUI(szKey)) // 있을 경우
//                        {
//                            // szKey를 복사하여 동적으로 할당
//                            _tchar* copiedKey = new _tchar[_tcslen(szKey) + 1];
//                            _tcscpy_s(copiedKey, _tcslen(szKey) + 1, szKey);
//
//                            // 복사본 생성
//                            Engine::Ready_Proto(L"Proto_UITex", CUITex::Create(pGraphicDev));
//                            Engine::Ready_Proto(L"Proto_BaseUI", CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/all wheels.png"));
//
//                            // 첫번째 인자 값의 경로에서, 두번째 인자값 문자열을 찾아준다.
//                            if (wcsstr(findData.cFileName, L"HP")) // 찾은 경우
//                            {
//                                // 찾은 녀석의 타입으로 Create
//                                pMyUI = CMyUI::Create(pGraphicDev, pTexture, UI_TYPE::HP);
//
//                            }
//                            else
//                            {
//                                // BASIC 타입의 텍스처 Create
//                                pMyUI = CMyTexture::Create(pGraphicDev, pTexture, UI_TYPE::BASIC);
//                            }
//
//
//                            // 동적으로 할당한 copiedKey를 맵에 넣어야 합니다.
//                            m_mapLoadUI1.emplace(copiedKey, pMyUI);
//                        }
//
//
//                    }
//                }
//            }
//        } while (FindNextFile(hFind, &findData)); // 다음
//
//        FindClose(hFind); // 파일 핸들을 닫음
//
//    }
//
//    return S_OK;
//}

// dat파일로 정보 저장하기
//HRESULT CImguiAnimationTool::SaveData(const _tchar* mapTag)
//{
//    // dat파일 이름
//    wstring m_strText = L"UIData.dat";
//
//    // 저장할 녀석의 정보를 담을 변수
//    OPENFILENAME    open;
//
//    // 저장할 녀석의 경로
//    TCHAR   lpstrFile[MAX_PATH] = L"";
//
//    // 저장할 녀석의 타입
//    static TCHAR filter[] = L"*.dat";
//
//    // 저장할 녀석의 정보를 담아준다.
//    ZeroMemory(&open, sizeof(OPENFILENAME));
//    open.lStructSize = sizeof(OPENFILENAME);
//    open.lpstrFilter = filter;
//    open.lpstrFile = lpstrFile;
//    open.nMaxFile = 2562;
//    open.lpstrInitialDir = L"";
//
//    // 지정된 모듈이 포함된 파일의 정규화된 경로를 검색
//    GetModuleFileName(NULL, lpstrFile, MAX_PATH);
//    //C:\Users\wnqj4\Desktop\SR_Project\Client\Bin\Client.exe
//
//    PathRemoveFileSpec(lpstrFile);
//    //C:\Users\wnqj4\Desktop\SR_Project\Client\Bin
//
//    lstrcat(lpstrFile, L"\\Data\\UI");
//    //C:\Users\wnqj4\Desktop\SR_Project\Client\Bin\Data\UI
//
//    //basic_string<TCHAR> converted(m_strText.begin(), m_strText.end());
//    const _tchar* aa = m_strText.c_str();
//
//    wcscat_s(lpstrFile, L"\\");
//    wcscat_s(lpstrFile, aa);
//
//
//    if (GetSaveFileName(&open) != 0) {
//
//        SaveUI(m_strText, lpstrFile);
//
//
//        MSG_BOX("저장 완료");
//        return S_OK;
//    }
//}

// 경로마다 실제 값을 저장해준다.
//void CImguiAnimationTool::SaveUI(wstring wstrFileName, wstring wstrFilePath)
//{
//    FILE* op = NULL;
//    //lpstrFile
//
//    _wfopen_s(&op, wstrFileName.c_str(), L"w");
//
//    if (op == NULL)
//        return;
//
//    //        fwprintf(op, L"#KEY             TEXTURE                  SIZE(X,Y,Z)                   POS(X,Y,Z)\n");
//
//    for (const auto& iter : m_mapChoiceUI)
//    {
//
//        wstring wstrkey = iter.first;
//        _vec3                  vSize = iter.second->Get_Info()->vSize;
//        _vec3                  vPos = iter.second->Get_Info()->vPos;
//                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
//
//        fwprintf(op, L"%s,%f,%f,%f,%f,%f,%f\n", wstrkey.c_str(), vSize.x, vSize.y, vSize.z, vPos.x, vPos.y, vPos.z);
//    }
//
//    if (op == NULL)
//    {
//        MSG_BOX("op 널이다~");
//    }
//
//    fclose(op);
//}

//// 찾기
//CMyTexture* CImguiAnimationTool::FindUI(const _tchar* szKey)
//{
//    auto iter = find_if(m_mapLoadUI1.begin(), m_mapLoadUI1.end(), CTag_Finder(szKey));
//
//    if (iter == m_mapLoadUI1.end())
//        return nullptr;
//
//    return iter->second;
//}

// 이미지 로드
void CImguiAnimationTool::LoadImg(const _tchar* folderPath)
{
    // 찾은 이미지 데이터를 받을 변수
    WIN32_FIND_DATA findData;

    // 폴더 경로
    wstring wfolderPath = (wstring)folderPath + L"/*.*";

    // 폴더 경로 저장 및 핸들 반환
    HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

    // 핸들이 있을 경우
    if (hFind != INVALID_HANDLE_VALUE)
    {
        // 하위 디렉토리
        vector<wstring> subDirectories;
        // 이미지 파일
        vector<wstring> imageFiles;
        do
        {
            //  파일의 속성 정보가 입력된다. (디렉토리인지 파일인지 등)
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
                {
                    // 폴더 경로 = 경로 + / + 파일명
                    wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
                    subDirectories.push_back(subFolderPath);
                    LoadImg(subFolderPath.c_str());
                }
            }
            else
            {
                // 파일인 경우, 이미지 파일인지 확인하고 로드
                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;

                // 파일 확장자 확인
                if (wcsstr(findData.cFileName, L".png")|| wcsstr(findData.cFileName, L".bmp"))/*|| wcsstr(findData.cFileName, L".jpg") ||
                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||
                    wcsstr(findData.cFileName, L".dds"))*/
                {
                    // IDirect3DBaseTexture9 인터페이스의 메서드를 사용하여 큐브 및 볼륨 텍스처를 포함한 텍스처 리소스를 조작.
                    IDirect3DBaseTexture9* pBaseTexture = nullptr;

                    // D3DXCreateTextureFromFile(디바이스, 파일경로, 베이스 텍스처) 파일에서 텍스처를 만드는 함수.
                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pBaseTexture)))
                    {
                        LPDIRECT3DTEXTURE9 pTexture = static_cast<LPDIRECT3DTEXTURE9>(pBaseTexture);
                        wstring filePathStr = filePath;

                        Replace(filePathStr, L"\\", L"/");

                        size_t pos = filePathStr.find_last_of(L"/");
                        wstring ObjKey, StateKey;
                        if (pos != wstring::npos)
                        {
                            ObjKey = filePathStr.substr(filePathStr.find_last_of(L"/", pos - 1) + 1, pos - (filePathStr.find_last_of(L"/", pos - 1) + 1));
                            StateKey = filePathStr.substr(pos + 1);
                        }
                        if (find(uniqueObj.begin(), uniqueObj.end(), ObjKey) == uniqueObj.end())
                        {
                            uniqueObj.push_back(ObjKey);
                        }
                        if (find(uniqueState.begin(), uniqueState.end(), StateKey) == uniqueState.end())
                        {
                            uniqueState.push_back(StateKey);
                            m_MapTexture[make_pair(ObjKey, StateKey)] = pTexture;
                        }
                        m_MapFile[ObjKey].push_back(StateKey);
                    }
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

// 찾기
void CImguiAnimationTool::Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace)
{
    wstring temp = strCurrentDirectory;

    size_t start_pos = 0;
    while ((start_pos = temp.find(strSearch, start_pos)) != wstring::npos)
    {
        temp.replace(start_pos, strSearch.length(), strReplace);
        start_pos += strReplace.length();
    }
    strCurrentDirectory = temp;
}

char* CImguiAnimationTool::ConverWStringtoC(const wstring& wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    char* result = new char[size_needed];
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);
    return result;
}

wchar_t* CImguiAnimationTool::ConverCtoWC2(char* str)
{
    _tchar* pStr;
    int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
    pStr = new WCHAR[strSize];
    MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

    return pStr;
}

char* CImguiAnimationTool::ConverWCtoC(wchar_t* str)
{
    char* pStr;
    int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
    pStr = new char[strSize];
    WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
    return pStr;
}

string CImguiAnimationTool::WstringToUTF8(const wstring& wstr)
{
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Length == 0)
    {

        return string();
    }

    string utf8Str(utf8Length + 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], utf8Length + 1, nullptr, nullptr);

    return utf8Str;
}