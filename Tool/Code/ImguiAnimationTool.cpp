#include "ImguiAnimationTool.h"
#include <Windows.h>
#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <ImguiMgr.h>
#include "PlayerAnimation.h"

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

// 이미지 추가 함수
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

#pragma region 레이아웃
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

        // ImGui에서 텍스처를 표시할 이미지 크기 (예: 200x200 픽셀)
        const ImVec2 imageSize(200.0f, 200.0f);

#pragma region 테스트1

        static int counter = 0;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::Button(u8"이미지 로드 버튼 (임시)")) // @@@@@ 문제 2. 이미지 로드 시간이 비정상적이게 길다. (폴더 많이 있으면 더 길어짐) @@@@@
        {
            LoadImg(L"../Client/Resource");
        }

        ToggleButton("Test1", &m_bTest);

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();

        static const char* ini_to_load = NULL;
        if (ini_to_load)
        {
            ImGui::LoadIniSettingsFromDisk(ini_to_load);
            ini_to_load = NULL;
        }

        ImGui::Begin("Ini Test");
        if (ImGui::Button("Load Ini 1"))
            ini_to_load = "imgui1.ini";
        if (ImGui::Button("Load Ini 2"))
            ini_to_load = "imgui2.ini";
        if (ImGui::Button("Load Ini 3"))
            ini_to_load = "imgui3d.ini";
        if (ImGui::Button("Load Ini 4"))
            ini_to_load = "imgui4d.ini";
        ImGui::End();

#pragma endregion


        // Create a window called "My First Tool", with a menu bar.
        ImGui::Begin(u8"그래프 예제", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"파일"))
            {
                if (ImGui::MenuItem(u8"열기", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem(u8"저장", "Ctrl+S")) { /* Do stuff */ }
                if (ImGui::MenuItem(u8"닫기", "Ctrl+W")) { my_tool_active = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Edit a color stored as 4 floats
        ImGui::ColorEdit4(u8"색상", my_color);

        // Generate samples and plot them
        float fFrame[100];
        for (int n = 0; n < 100; n++)
            fFrame[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
        ImGui::PlotLines(u8"그래프", fFrame, 100);

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"목록");
        ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++)
            ImGui::Text(u8"%04d: 텍스트", n);
        ImGui::EndChild();
        ImGui::End();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
        if (ImGui::Button(u8"프레임 추가 버튼"))      // 프레임 추가 버튼
        m_vecAnimationInfo.push_back(m_AniInfo);    // 프레임을 애니메이션 정보 벡터에 추가

        ImGui::SameLine(); // 같은 라인
        ImGui::Text(u8"생성된 프레임 = Frame_%d", m_vecAnimationInfo.size());

        ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 1.00f), "Data Number");

        // 타임라인 툴
        ImGui::Begin(u8"애니메이션 타임라인");
        RenderTimeline();
        ImGui::End();

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
            if (selectedItemIndex >= 0 && selectedItemIndex < frameNames.size()) {
                // selectedItemIndex를 사용하여 선택한 프레임에 대한 작업을 수행
                //RenderSelectedFrameTexture(selectedItemIndex);
            }
        }

        ImGui::SliderFloat("Animation Time", &g_animationTime, 0.0f, 1.0f);
        ImGui::SliderInt("Current Frame", &g_currentFrame, 0, 100);

        // 애니메이션 타임라인 그리기
        ImGui::Text("Animation Timeline:");
        ImGui::ProgressBar(g_animationTime, ImVec2(-1.0f, 0.0f), "");
#pragma endregion
         
        ImGui::InputText("FrameName", "FrameName", IM_ARRAYSIZE("EEEEEEEEEEEE"));

        if (ImGui::Button(u8"저장 버튼"))
        {

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"불러오기 버튼"))
        {

        }

#pragma endregion

    return 0;
}

#pragma endregion

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
                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||
                    wcsstr(findData.cFileName, L".dds"))
                {
                    // IDirect3DBaseTexture9 인터페이스의 메서드를 사용하여 큐브 및 볼륨 텍스처를 포함한 텍스처 리소스를 조작.
                    IDirect3DBaseTexture9* pBaseTexture = nullptr;

                    // D3DXCreateTextureFromFile(디바이스, 파일경로, 베이스 텍스처) 파일에서 텍스처를 만드는 함수.
                    // 이미지 파일인 경우, 텍스처 로드 및 관리
                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pBaseTexture)))
                    {
                        LPDIRECT3DTEXTURE9 pTexture = static_cast<LPDIRECT3DTEXTURE9>(pBaseTexture);
                        wstring filePathStr = filePath;

                        // 파일 경로를 슬래시(/)로 통일
                        Replace(filePathStr, L"\\", L"/");

                        // 파일 경로에서 객체 키와 상태 키를 추출
                        size_t pos = filePathStr.find_last_of(L"/");
                        wstring ObjKey, StateKey;
                        if (pos != wstring::npos)
                        {
                            ObjKey = filePathStr.substr(filePathStr.find_last_of(L"/", pos - 1) + 1, pos - (filePathStr.find_last_of(L"/", pos - 1) + 1));
                            StateKey = filePathStr.substr(pos + 1);
                        }
                        // 객체 키가 uniqueObj 벡터에 없을 경우 추가
                        if (find(uniqueObj.begin(), uniqueObj.end(), ObjKey) == uniqueObj.end())
                        {
                            uniqueObj.push_back(ObjKey);
                        }
                        // 상태 키가 uniqueState 벡터에 없을 경우 추가
                        if (find(uniqueState.begin(), uniqueState.end(), StateKey) == uniqueState.end())
                        {
                            uniqueState.push_back(StateKey);
                            m_MapTexture[make_pair(ObjKey, StateKey)] = pTexture;
                        }
                        // 객체 키에 해당하는 파일 목록에 상태 키 추가
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

#pragma region 문자열 변환 함수들
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
#pragma endregion

// 오브젝트 설정 및 관리
/*
void CImguiAnimationTool::ObjectSetting()
{
    // 객체 유형 저장
    const char* Object[] = { u8"Human" ,u8"Quadrupedal",u8"Item" };

    // 객체 유형을 선택할 콤보 박스 [선택한 변수의 현재 인덱스]
    ImGui::Combo(u8"Object", &m_iObjectCreatIndex, Object, IM_ARRAYSIZE(Object));

    // 불러오기 버튼
    if (ImGui::Button(u8"Load")) 
    {
        
        switch (m_iObjectCreatIndex)
        {
            case 1: // 주먹
            {       
                if (Engine::CManagement::GetInstance()->Get_GameObject(L"GameLogic", L"Player").empty())
                {
                    // 생성
                    Engine::CGameObject* pGameObject = nullptr; // 오브젝트를 담을 그릇
                    pGameObject = CPlayerAnimation::Create(m_pGraphicDev); // ex : CHumanDemo 클래스 Create(장치); 로 생성
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::NONE, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_NONE ,pGameObject }); // 맵 컨테이너에 키값, 오브젝트 추가
                }
                else {
                    //MSG_BOX("이미 존재합니다");
                }
                break;
            }
            case 2: // 권총
            {
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(HUMAN_DEMO).empty()) 
                {
                    Engine::CGameObject* pGameObject = nullptr;
                    pGameObject = CHumanDemo::Create(m_pGraphicDev);
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::RIGHT_OBJECT, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_GUN ,pGameObject }); // 맵 컨테이너에 키값, 오브젝트 추가
                }
                else {
                    //MSG_BOX("이미 존재합니다");
                }
                break;
            }
            case 3: // 톰슨 기관총
            {
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).empty()) 
                {
                    LoadObjectData();

                    _matrix test;
                    D3DXMatrixIdentity(&test);

                    Engine::CGameObject* pGameObject = nullptr;
                    pGameObject = CItemDemo::Create(m_pGraphicDev, ITEM_USE);
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::TWO_OBJECT, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_THOMPSON ,pGameObject }); // 맵 컨테이너에 키값, 오브젝트 추가

                }
                else {
                    //MSG_BOX("이미 존재합니다");
                }
                break;
            }
            case 4: // 쇠파이프
            {

                break;
            }
            case 5: // 맥주병
            {

                break;
            }
            case 6: // 프라이팬
            {

                break;
            }
        }

    }

    // 삭제 버튼
    if (ImGui::Button(u8"Delete")) {

        OBJ_NAME selectedObjType;

        switch (m_iObjectCreatIndex)
        {
        case 0:
            selectedObjType = OBJ_NAME::NONE;
            break;
        case 1:
            selectedObjType = OBJ_NAME::GUN;
            break;
        case 2:
            selectedObjType = OBJ_NAME::THOMPSON;
            break;
        case 3:
            selectedObjType = OBJ_NAME::THOMPSON;
            break;
        case 4:
            selectedObjType = OBJ_NAME::STEELPIPE;
            break;
        case 5:
            selectedObjType = OBJ_NAME::BEERBOTLE;
            break;
        case 6:
            selectedObjType = OBJ_NAME::FRYINGPAN;
            break;
        default:
            break;
        }

        if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(selectedObjType).size() != 0)
        {
            // 매니지먼트에서 셋 데드로 죽인 후, 오브젝트 맵 컨테이너에 원하는 인덱스 번째 녀석을 삭제
            Engine::CManagement::GetInstance()->Get_GameLogic_Objects(selectedObjType).front()->Set_Dead();
            m_mapObject.erase((OBJECTKEY)m_iObjectCreatIndex);
        }
        else {
            //MSG_BOX("아니야");
        }

    }

    // ?    게임로직 오브젝트의 타입.size 가 비어있지 않을 경우
    //if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).size() != 0) 
    {
        /*ImGui::InputFloat(u8"ScaleX", &m_ItemTotal->vScale.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleY", &m_ItemTotal->vScale.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleZ", &m_ItemTotal->vScale.z, 1.0f, 100.f, "%.1f");*/
        //ImGui::InputFloat3("Scale", m_AniInfo.m_vScale); // 크기 설정
        /*
        ImGui::InputFloat(u8"RotX", &m_ItemTotal->vStartRotation.x, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotY", &m_ItemTotal->vStartRotation.y, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotZ", &m_ItemTotal->vStartRotation.z, 5.0f, 100.f, "%.1f");
        */
        //ImGui::InputFloat3("Rot", m_AniInfo.m_vRot); // 회전 설정

        /*ImGui::InputFloat(u8"PosX", &m_ItemTotal->vStartPosition.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosY", &m_ItemTotal->vStartPosition.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosZ", &m_ItemTotal->vStartPosition.z, 1.0f, 100.f, "%.1f");*/
/*
        ImGui::InputFloat3("Pos", m_AniInfo.m_vPos); // 위치 설정


        //const char* Parts3[] = { u8"Total" ,u8"Head",u8"Body",u8"LeftArm",u8"RightArm",u8"LeftLeg",u8"RightLeg" };
        //static int PartsIndex3 = 0;
        //ImGui::Combo(u8"Parts", &PartsIndex3, Parts3, IM_ARRAYSIZE(Parts3));
        //m_AniInfo.m_eObjectType = (PARTS)PartsIndex3;

        // 객체 정보 저장
        if (ImGui::Button(u8"InfoSave")) {
            SaveObjectInformationData();
        }

        // 객체 정보 불러오기
        if (ImGui::Button(u8"InfoLoad")) {

            LoadObjectInforamtionData();
        }
    }
}
*/

// 데이터 저장
/*
void CImguiAnimationTool::SaveObjectInformationData()
{
    OPENFILENAME    open;
    TCHAR   lpstrFile[MAX_PATH] = L"";
    static TCHAR filter[] = L"*.dat";

    ZeroMemory(&open, sizeof(OPENFILENAME));
    open.lStructSize = sizeof(OPENFILENAME);
    open.lpstrFilter = filter;
    open.lpstrFile = lpstrFile;
    open.nMaxFile = 100;
    open.lpstrInitialDir = L"";

    GetModuleFileName(NULL, lpstrFile, MAX_PATH);
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin\Client.exe
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin
    lstrcat(lpstrFile, L"\\Data\\ObjectTotalInfo");
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin\Data\Animation
    basic_string<TCHAR> converted(m_strItemTotalText.begin(), m_strItemTotalText.end());
    const _tchar* aa = converted.c_str();

    wcscat_s(lpstrFile, L"\\");
    wcscat_s(lpstrFile, L"TOTALINFO_");
    //wcscat_s(lpstrFile, aa);
    wcscat_s(lpstrFile, L".dat");

    if (GetSaveFileName(&open) != 0) {

        HANDLE hFile = CreateFile(lpstrFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
            return;

        DWORD   dwByte = 0;


        /*for (auto& iter : m_vecItemPartsSave) {
            WriteFile(hFile, iter, sizeof(PARTSITEMINFO), &dwByte, nullptr);
        }*/
/*
        
        // (생성된 파일, 저장할 정보 변수, 사이즈, 크기, nullptr);
        WriteFile(hFile, &m_AniInfo, sizeof(MYANIMATIONINFO), &dwByte, nullptr);

        CloseHandle(hFile);
    }
}
*/

// 데이터 로드
void CImguiAnimationTool::LoadObjectInforamtionData()
{
    OPENFILENAME    open;
    TCHAR   lpstrFile[MAX_PATH] = L"";
    static TCHAR filter[] = L"*.*\*.dat";

    ZeroMemory(&open, sizeof(OPENFILENAME));
    open.lStructSize = sizeof(OPENFILENAME);
    open.lpstrFilter = filter;
    open.lpstrFile = lpstrFile;
    open.nMaxFile = 100;
    open.lpstrInitialDir = L"";

    GetModuleFileName(NULL, lpstrFile, MAX_PATH);
    //C:\Users\조우택\Desktop\SR_TeamMine\Client\Bin\Client.exe
    PathRemoveFileSpec(lpstrFile);
    //C:\Users\조우택\Desktop\SR_TeamMine\Client\Bin
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    lstrcat(lpstrFile, L"\\Data\\ObjectTotalInfo\\data");
    //C:\Users\조우택\Desktop\SR_TeamMine\Client\Bin\Data\Animation

    if (GetOpenFileName(&open) != 0) {

        HANDLE hFile = CreateFile(lpstrFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
            return;

        DWORD   dwByte = 0;
        DWORD   dwStrByte = 0;

        //TOTALITEMINFO* pItemPartsInfo = new TOTALITEMINFO;

        ReadFile(hFile, &m_AniInfo, sizeof(MYANIMATIONINFO), &dwByte, nullptr);

        CloseHandle(hFile);
    }
}

// 파일 경로에서 파일 이름을 제거하는 함수
void CImguiAnimationTool::PathRemoveFileSpec(TCHAR* path)
{
    if (path == nullptr || *path == '\0')
        return;

    size_t len = wcslen(path);
    if (len == 0)
        return;

    // 경로의 뒷부분부터 검사하여 '\' 문자를 찾아 제거합니다.
    for (size_t i = len - 1; i > 0; --i)
    {
        if (path[i] == '\\')
        {
            path[i] = '\0';
            break;
        }
    }
}

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

// 토글 버튼
void CImguiAnimationTool::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos(); // 현재 커서위치를 화면 좌표로 반환해주는 함수 (마우스 배치나 피킹등에 활용 가능할듯 하다)
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // 현재 창에 그림을 그리기 위한 포인터 코드
                                                        // ImGui::GetWindowDrawList()는 현재 창에대한 포인터를 얻는다.

    float height = ImGui::GetFrameHeight(); // 기본 크기 값 설정
    float width = height * 1.55f;   // 토글버튼 배경 가로 길이
    float radius = height * 0.50f;  // 토글버튼 안쪽 동그라미 크기(원)

    // 사용자에겐 보이지 않지만, 클릭 이벤트를 감지하거나 상태를 변경하귀 위해 사용하는 함수
    //              (고유식별자(str_id), 버튼의 크기)
    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) // 위젯이 클릭되었을 때 '*v'라는 불리언 포인터 변수를 토글한다. 이 변수는 토글 버튼의 상태를 나타낸다.
        *v = !*v;

    float t = *v ? 1.0f : 0.0f; // '*v' 변수의 값에 따라 't' 값을 설정한다. 
                                // ex : '*v'가 true면 't'는 1.0, false면 0.0

    // 마우스 오버에따른 색상 설정
    ImU32 col_bg;
    if (ImGui::IsItemHovered()) // 마우스가 버튼위에 올라갔는지 판단 
        // ImLerp : 선형 보간 함수 (마우스 오버시 색상)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else                            
        // ImLerp : 선형 보간 함수 (마우스 클릭시 색상)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    // draw_list를 사용하여 버튼의 배경을 그린다.
    // AddRectFilled 함수를 사용하여 사각형을 그린다.
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);

    // 버튼 내부에 동그라미를 그린다. AddCircleFilled 함수를 사용하여 원을 그린다.
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

// 선형 보간 함수 ex1
//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = ImClamp(t, 0.0f, 1.0f); // t 값을 [0, 1] 범위로 클램프합니다.
//    return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
//}

// 선형 보간 함수 ex2
//ImVec4 CImguiAnimationTool::ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t); // t 값을 [0, 1] 범위로 클램프합니다.
//    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
//}

// 타임라인 툴
void CImguiAnimationTool::RenderTimeline() 
{
#pragma region 시간 타임라인

    // 시간
    ImGui::Text("Time: %.2f", currentTime);
    ImGui::SameLine();

    ImGui::PushItemWidth(150);
    // 시간 그래프
    float fFrame[100];
    for (int n = 0; n < 100; n++)
        fFrame[n] = sinf(n * 0.2f + ImGui::GetTime() * currentTime);
    ImGui::PlotLines(u8"타임라인", fFrame, 100);
    ImGui::PopItemWidth();


    // 타임 라인 렌더링
    ImVec2 timelineSize(500, 85); // 타임라인 크기
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 timelinePos = ImGui::GetCursorScreenPos();
    
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0)); // 가로 간격을 추가
    ImGui::SameLine();

#pragma endregion

#pragma region 애니메이션 재생/정지

    // 애니메이션 재생/일시 정지 버튼
    if (isPlaying) {
        if (ImGui::Button(u8"정지"))
        {
            isPlaying = false;
        }
    }
    else {
        if (ImGui::Button(u8"재생"))
        {
            isPlaying = true;
            playbackTime = 0.0f;
        }
    }

    ImGui::SameLine();
    //ImGui::Spacing(); // 간격 추가
    //ImGui::Dummy(ImVec2(0, 20)); // 세로 간격을 추가
    //ImGui::SameLine();

    ImGui::PushItemWidth(100); // 가로 사이즈
    // 애니메이션 재생 속도 조절
    ImGui::SliderFloat(u8"재생 속도", &playbackSpeed, 0.1f, 3.0f);
    ImGui::PopItemWidth();

#pragma endregion

    ImGui::Separator(); // 선 추가

#pragma region 애니메이션 타임 라인

    // 타임 라인 위치 설정
    timelinePos.x += 0.f;
    timelinePos.y += 30.f;

    // 타임 라인 그리기
    draw_list->AddRectFilled(
        timelinePos,
        ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
        IM_COL32(50, 50, 50, 255) // 타임라인 배경 색상
    );

#pragma endregion

#pragma region 키프레임 렌더링 및 편집

    // 키프레임 렌더링 및 편집
    for (int i = 0; i < timeline.size(); ++i) 
    {
        Keyframe& keyframe = timeline[i];
        float xPos = timelinePos.x + timelineSize.x * keyframe.time;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // 키프레임 그리기
        draw_list->AddCircleFilled(
            ImVec2(xPos, yPos),
            5.0f,
            IM_COL32(255, 255, 0, 255)
        );

        // 키프레임 이름 설정
        ImGui::Text(u8"키프레임 %d", i + 1);
        ImGui::SameLine();
        ImGui::InputText((u8"##프레임 이름 : " + std::to_string(i)).c_str(), keyframe.name, 64);

        // 키프레임 색상 설정
        ImGui::SameLine();
        ImGui::ColorEdit3(("##프레임 색상 : " + std::to_string(i)).c_str(), keyframe.color);
        keyframe.color[3] = 1.0f; // 알파 값은 항상 1로 유지

        // 마우스 클릭으로 키프레임 이동
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {
            isDraggingTimeline = true;
            keyframe.value = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;
        }

        // 오른쪽 마우스 클릭으로 키프레임 삭제
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f))
            && ImGui::IsMouseClicked(1))
        {
            timeline.erase(timeline.begin() + i);
            break; // 삭제한 후에 루프를 빠져나갑니다.
        }

        // 키프레임에 이징 설정 UI 추가
        //ImGui::SameLine();
        //ImGui::Checkbox(("In##" + std::to_string(i)).c_str(), &keyframe.isEaseIn);
        //ImGui::SameLine();
        //ImGui::Checkbox(("Out##" + std::to_string(i)).c_str(), &keyframe.isEaseOut);
    }

#pragma endregion

    // 타임 라인 드래그로 현재 시간 이동
    if (isDraggingTimeline && ImGui::IsMouseDragging(0)) 
    {
        currentTime = (ImGui::GetMousePos().x - timelinePos.x) / timelineSize.x;
        currentTime = ImClamp(currentTime, 0.0f, 1.0f);
    }

    // 타임라인에서 키프레임을 드래그하여 순서 변경
    for (int i = 0; i < timeline.size(); ++i) 
    {
        Keyframe& keyframe = timeline[i];
        float xPos = timelinePos.x + timelineSize.x * keyframe.time;

        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, timelinePos.y - 5.0f),
            ImVec2(xPos + 5.0f, timelinePos.y + timelineSize.y + 5.0f))
            && ImGui::IsMouseClicked(0)) 
        {
            isDraggingKeyframe = true;
            draggingKeyframeIndex = i;
        }
    }

    if (isDraggingKeyframe) 
    {
        // 키프레임의 시간을 드래그한 위치로 설정
        Keyframe& keyframe = timeline[draggingKeyframeIndex];
        float xPos = ImGui::GetMousePos().x - timelinePos.x;
        keyframe.time = ImClamp(xPos / timelineSize.x, 0.0f, 1.0f);

        // 키프레임 순서 변경
        if (draggingKeyframeIndex > 0 && keyframe.time < timeline[draggingKeyframeIndex - 1].time) {
            std::swap(timeline[draggingKeyframeIndex], timeline[draggingKeyframeIndex - 1]);
            draggingKeyframeIndex--;
        }
        else if (draggingKeyframeIndex < timeline.size() - 1 && keyframe.time > timeline[draggingKeyframeIndex + 1].time) {
            std::swap(timeline[draggingKeyframeIndex], timeline[draggingKeyframeIndex + 1]);
            draggingKeyframeIndex++;
        }
    }

    if (ImGui::IsMouseReleased(0)) 
    {
        isDraggingTimeline = false;
        isDraggingKeyframe = false;
    }

#pragma region 버튼 (추가, 삭제, 저장, 로드)

    // 키프레임 추가 버튼
    if (ImGui::Button("Add Keyframe")) {
        // 새로운 키프레임을 추가할 때 현재 시간과 가장 가까운 키프레임을 찾습니다.
        float minDist = FLT_MAX;
        int insertIndex = 0;
        for (int i = 0; i < timeline.size(); ++i) {
            float dist = fabsf(currentTime - timeline[i].time);
            if (dist < minDist) {
                minDist = dist;
                insertIndex = i;
            }
        }

        // 새로운 키프레임을 생성하고 삽입합니다.
        Keyframe newKeyframe = { currentTime, 0.5f, 0, false, false };
        timeline.insert(timeline.begin() + insertIndex, newKeyframe);
    }

    ImGui::SameLine();

    // 키프레임 삭제 버튼
    if (ImGui::Button("Delete Keyframe")) {
        if (!timeline.empty()) {
            timeline.pop_back();
        }
    }

    ImGui::SameLine();

    // 애니메이션 저장 및 로드 버튼
    if (ImGui::Button("Save Animation")) {
        SaveAnimationToFile("animation.txt");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load Animation")) {
        LoadAnimationFromFile("animation.txt");
    }
#pragma endregion

    ImGui::SameLine();

#pragma region 타임라인 그래프

    // 타임라인 그래프
    float* values = new float[100];
    for (int i = 0; i < 100; i++) {
        float time = i / 100.0f;
        values[i] = EvaluateAnimationAtTime(time);
    }
    ImGui::PlotLines("Value Curve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(0, 0));

    delete[] values;

#pragma endregion

    //ImGui::Separator();

#pragma region 애니메이션 속성 편집

    // 애니메이션 속성 편집
    if (!timeline.empty()) // 비었는지 확인
    {
        // 타임라인 툴
        ImGui::Begin(u8"애니메이션 속성 편집");

        Keyframe& selectedKeyframe = timeline.back();
        ImGui::Text("Selected Keyframe:");

        // 이징 설정 (Ease In, Ease Out)
        ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
        ImGui::SameLine();
        ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);

        // 애니메이션 타입 설정
        ImGui::Text(u8"애니메이션 타입:");
        ImGui::RadioButton(u8"크기", &selectedKeyframe.type, 0); // 크기 애니메이션
        ImGui::SameLine();
        ImGui::RadioButton(u8"회전", &selectedKeyframe.type, 1); // 회전 애니메이션
        ImGui::SameLine();
        ImGui::RadioButton(u8"이동", &selectedKeyframe.type, 2); // 이동 애니메이션

        // 키프레임 타입에 따른 추가 설정
        if (selectedKeyframe.type == 0) { // 크기 애니메이션
            ImGui::SliderFloat(u8"크기", &selectedKeyframe.size, 0.1f, 2.0f);
        }
        else if (selectedKeyframe.type == 1) { // 회전 애니메이션
            ImGui::SliderFloat(u8"회전", &selectedKeyframe.rotation, 0.0f, 360.0f);
        }
        else if (selectedKeyframe.type == 2) { // 이동 애니메이션
            ImGui::SliderFloat2(u8"이동", selectedKeyframe.translation, -100.0f, 100.0f);
        }

        ImGui::End();
    }

    // 애니메이션 프레임 숫자 입력
    ImGui::InputInt(u8"애니메이션 프레임", &animationFrame);

    // 애니메이션 프레임 변경 버튼
    if (ImGui::Button(u8"프레임 변경")) {
        currentTime = playbackTime;
    }

    // 애니메이션 시간 설정
    if (isPlaying) 
    {
        currentTime += playbackSpeed * ImGui::GetIO().DeltaTime;
        if (currentTime > 10.0f) {
            currentTime = 10.0f;
            isPlaying = false;
        }
    }

    // 미리보기 키프레임 보간
    if (!timeline.empty()) 
    {
        for (int i = 0; i < timeline.size() - 1; ++i) {
            Keyframe& prevKeyframe = timeline[i];
            Keyframe& nextKeyframe = timeline[i + 1];

            if (currentTime >= prevKeyframe.time && currentTime <= nextKeyframe.time) {
                float alpha = (currentTime - prevKeyframe.time) / (nextKeyframe.time - prevKeyframe.time);

                // 보간된 애니메이션 미리보기 값 계산
                float interpolatedValue = Lerp(prevKeyframe.value, nextKeyframe.value, alpha);

                // 이 값을 사용하여 미리보기 애니메이션 렌더링
                RenderPreviewAnimation(interpolatedValue);

                break;
            }
        }
    }
#pragma endregion
}

// 애니메이션 저장
void CImguiAnimationTool::SaveAnimationToFile(const char* fileName) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        return;
    }

    for (const Keyframe& keyframe : timeline) {
        file << keyframe.time << " "
            << keyframe.value << " "
            << keyframe.type << " "
            << keyframe.isEaseIn << " "
            << keyframe.isEaseOut << "\n";
    }

    file.close();
}

// 애니메이션 불러오기
void CImguiAnimationTool::LoadAnimationFromFile(const char* fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        return;
    }

    timeline.clear();
    Keyframe keyframe;
    while (file >> keyframe.time >> keyframe.value >> keyframe.type >> keyframe.isEaseIn >> keyframe.isEaseOut) {
        timeline.push_back(keyframe);
    }

    file.close();
}

// 애니메이션을 계산하여 현재 시간 값을 반환하는 함수
float CImguiAnimationTool::EvaluateAnimationAtTime(float time)
{
    if (timeline.empty()) {
        // 만약 키프레임이 없다면 기본값 0.0을 반환하거나,
        // 다른 처리를 수행할 수 있습니다.
        return 0.0f;
    }

    // 키프레임을 시간에 따라 정렬합니다.
    std::sort(timeline.begin(), timeline.end(), [](const Keyframe& a, const Keyframe& b) {
        return a.time < b.time;
        });

    // 현재 시간과 가장 가까운 이전과 이후의 키프레임을 찾습니다.
    Keyframe* prevKeyframe = nullptr;
    Keyframe* nextKeyframe = nullptr;

    for (int i = 0; i < timeline.size(); ++i) {
        if (timeline[i].time <= time) {
            prevKeyframe = &timeline[i];
        }
        if (timeline[i].time >= time) {
            nextKeyframe = &timeline[i];
            break;
        }
    }

    // 이전과 다음 키프레임을 찾지 못하면 첫 번째 키프레임을 사용합니다.
    if (!prevKeyframe) {
        prevKeyframe = &timeline[0];
    }

    // 다음 키프레임을 찾지 못하면 마지막 키프레임을 사용합니다.
    if (!nextKeyframe) {
        nextKeyframe = &timeline.back();
    }

    // 이전과 다음 키프레임 사이에서 시간을 보간하여 값을 계산합니다.
    float t = (time - prevKeyframe->time) / (nextKeyframe->time - prevKeyframe->time);
    float value = ImLerp(prevKeyframe->value, nextKeyframe->value, t);

    // 이징(Easing)을 적용합니다.
    if (t < 0.5f) {
        if (prevKeyframe->isEaseIn) {
            float easedT = t / 0.5f;
            value = ImEaseInQuad(prevKeyframe->value, nextKeyframe->value, easedT);
        }
    }
    else {
        if (nextKeyframe->isEaseOut) {
            float easedT = (t - 0.5f) / 0.5f;
            value = ImEaseOutQuad(prevKeyframe->value, nextKeyframe->value, easedT);
        }
    }

    return value;
}

// 쿼드라틱 이징(InQuad) 함수 (시작)
float CImguiAnimationTool::ImEaseInQuad(float start, float end, float t)
{
    return start + (end - start) * t * t;
}

// 쿼드라틱 이징(OutQuad) 함수 (끝)
float CImguiAnimationTool::ImEaseOutQuad(float start, float end, float t)
{
    return start + (end - start) * (-t * (t - 2));
}

// 미리보기 애니메이션 렌더링 함수
void CImguiAnimationTool::RenderPreviewAnimation(float value) 
{
    // 이 함수에서 미리보기 애니메이션 렌더링
    // value를 기반으로 크기, 회전, 이동 등을 조절하여 렌더링
}

// 선형 보간 함수
float CImguiAnimationTool::Lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}