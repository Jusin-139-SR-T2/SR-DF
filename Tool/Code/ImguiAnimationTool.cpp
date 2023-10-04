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

#pragma region 레이아웃
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

#pragma region 도킹 스테이션



    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_None;

    // [학준] 도킹 스테이션 바인딩
    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        // 기본 독
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_AnimationTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            
            // 오른쪽에 붙이고, 가로 너비 조정
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, NULL, &dockspace_id);
            // 오른쪽 탭으로 붙이고, 가로 너비를 조절
            ImGuiID dock_right_tap_id = ImGui::DockBuilderSplitNode( dockspace_id, ImGuiDir_Right, 0.3f, &dock_right_id, &dockspace_id);
            // 오른쪽 아래 붙이고, 너비 조정
            ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, NULL, &dock_right_id);
            // 오른쪽 아래 탭으로 붙이고, 너비 조정
            ImGuiID dock_right_down_tap_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.328f, &dock_right_down_id, &dock_right_id);
            // 아래쪽에 붙이고, 너비 조정
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.32f, NULL, &dockspace_id);
            
            
            ImGui::DockBuilderDockWindow(u8"텍스처", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"애니메이션 속성 편집", dock_right_tap_id);
            ImGui::DockBuilderDockWindow(u8"키프레임 수정", dock_right_down_id);
            ImGui::DockBuilderDockWindow(u8"키프레임 정보", dock_right_down_tap_id);
            ImGui::DockBuilderDockWindow(u8"애니메이션 툴", dock_down_id);
            ImGui::DockBuilderDockWindow(u8"뷰어", dockspace_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        m_bFirstLoop = false;
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

#pragma endregion

#pragma region 뷰어
    if (ImGui::Begin(u8"뷰어", NULL, iMain_Flags))
    {
        CImguiMgr* pImguiMgr = CImguiMgr::GetInstance();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 clipSize = ImVec2(contentSize.x / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferWidth,
            contentSize.y / pImguiMgr->Get_DeviceClass()->Get_D3DPP()->BackBufferHeight);

        // 이미지 조정
        ImGui::Image((void*)pImguiMgr->Get_VecRenderTargetTex(0), contentSize,
            ImVec2((1.f - clipSize.x) * 0.5f, (1.f - clipSize.y) * 0.5f),
            ImVec2(clipSize.x + (1.f - clipSize.x) * 0.5f, clipSize.y + (1.f - clipSize.y) * 0.5f));

    }   ImGui::End();
#pragma endregion

    // ImGui에서 텍스처를 표시할 이미지 크기 (예: 200x200 픽셀)
    const ImVec2 imageSize(200.0f, 200.0f);

#pragma region 텍스처 툴
    static int counter = 0;

    if (ImGui::Begin(u8"텍스처"))
    {
        // 프레임
        ImGui::Text(u8"프레임 %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // 문자열 벡터를 const char* 배열로 변환
        std::vector<const char*> charImagePaths = ConvertStringVectorToCharArray(imagePaths);

        int selectedPathIndex = 0; // 선택된 이미지 경로 인덱스

        if (ImGui::CollapsingHeader(u8"이미지 불러오기"))
        {
            if (ImGui::ListBox("Image Paths", &selectedPathIndex, charImagePaths.data(), charImagePaths.size())) {

                // 선택한 이미지 경로를 사용하여 이미지를 로드하는 함수 호출
                const std::string& selectedImagePath = imagePaths[selectedPathIndex];
                //LoadImg(ConverCtoWC(selectedImagePath.c_str()));
                LoadImg(ConverCtoWC(ConverWStringtoC(ConvertToWideString(imagePaths[selectedPathIndex]))));
            }
        }

        ToggleButton(u8"토글", &m_bTest);

        static const char* ini_to_load = NULL;
        if (ini_to_load)
        {
            ImGui::LoadIniSettingsFromDisk(ini_to_load);
            ini_to_load = NULL;
        }

        if (ImGui::Button(u8"저장 버튼"))
        {
            SaveObjectInformationData();

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"불러오기 버튼"))
        {
            LoadObjectInforamtionData();
        }
    }   
    ImGui::End();
#pragma endregion

#pragma region 애니메이션 툴
    // 타임라인 툴
    ImGui::Begin(u8"애니메이션 툴");
    RenderTimeline();
    ImGui::End();
#pragma endregion

    // [학준] Center End
    ImGui::End();
         
    return 0;
}
#pragma endregion

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

wchar_t* CImguiAnimationTool::ConverCtoWC(char* str)
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

// 데이터 저장
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

    wcscat_s(lpstrFile, L"PlayerData");
    wcscat_s(lpstrFile, L".dat");

    if (GetSaveFileName(&open) != 0) {

        HANDLE hFile = CreateFile(lpstrFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
            return;

        DWORD   dwByte = 0;

        for (const Keyframe& keyframe : timeline) {
            WriteFile(hFile, &keyframe, sizeof(Keyframe), &dwByte, nullptr);
        }
        
        // (생성된 파일, 저장할 정보 변수, 사이즈, 크기, nullptr);
        WriteFile(hFile, &timeline, sizeof(Keyframe), &dwByte, nullptr);

        CloseHandle(hFile);
    }
}

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

    wcscat_s(lpstrFile, L"PlayerData");
    wcscat_s(lpstrFile, L".dat");

    if (GetOpenFileName(&open) != 0) {

        HANDLE hFile = CreateFile(lpstrFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (INVALID_HANDLE_VALUE == hFile)
            return;

        DWORD   dwByte = 0;
        DWORD   dwStrByte = 0;

        //TOTALITEMINFO* pItemPartsInfo = new TOTALITEMINFO;

        ReadFile(hFile, &timeline, sizeof(MYANIMATIONINFO), &dwByte, nullptr);

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






//==============================================[애니메이션 툴]==============================================
void CImguiAnimationTool::RenderTimeline() 
{
#pragma region 시간 타임라인

    // 시간
    ImGui::Text(u8"시간 : %.2f", currentTime);
    ImGui::SameLine(80.f);

    ImGui::PushItemWidth(150);
    // 시간 그래프
    float fFrame[100];
    for (int n = 0; n < 100; n++)
        fFrame[n] = sinf(n * 0.2f + ImGui::GetTime() * playbackSpeed);
    ImGui::PlotLines(u8"시간 그래프", fFrame, 100);
    ImGui::PopItemWidth();
    ImGui::SameLine();

    if (ImGui::Button(u8"시간 초기화"))
    {
        currentTime = 0.f;
    }

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

    ImGui::PushItemWidth(150); // 가로 사이즈
    // 애니메이션 재생 속도 조절
    ImGui::SliderFloat(u8"재생 속도", &playbackSpeed, 0.1f, 3.0f);
    ImGui::SameLine();

    // 속도 조절 버튼
    if (ImGui::Button(u8"속도 초기화")) // 속도 기본값으로 초기화
    {
        playbackSpeed = 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("-")) // 속도 -
    {
        playbackSpeed -= 1.f;
    }
    ImGui::SameLine();
    if (ImGui::Button("+")) // 속도 +
    {
        playbackSpeed += 1.f;
    }
    ImGui::PopItemWidth();

#pragma endregion

    ImGui::Separator(); // 선 추가

#pragma region 애니메이션 타임 라인
    // 타임 라인 렌더링
    ImVec2 timelineSize(800, 85); // 애니메이션 타임 라인 크기
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 timelinePos = ImGui::GetCursorScreenPos();

    // 애니메이션 타임 라인 위치 설정
    timelinePos.x += 0.f;
    timelinePos.y += 30.f;

    // 애니메이션 타임 라인 그리기
    draw_list->AddRectFilled(
        timelinePos,
        ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
        IM_COL32(50, 50, 50, 255) // 타임라인 배경 색상
    );

    // 현재 시간을 최소와 최대 시간 비율로 변환
    float minTime = 0.0f; // 최소 시간
    //float maxTime = 100.0f; // 최대 시간

    // 애니메이션 타임 라인 눈금 그리기 (1단위 표시 기준 : 최대 시간 값 만큼)
    for (int i = 0; i <= MaxTime; ++i)
    {
        // 눈금 시간의 간격                                최대 시간값으로 나눠 간격 설정
        float xPos = timelinePos.x + i * (timelineSize.x / MaxTime);

        float relativeStartTimeXPos = timelinePos.x;    //애니메이션 타임 라인 그래프의 시작
        float relativeEndTimeXPos = timelinePos.x + timelineSize.x; // 애니메이션 타임 라인 그래프의 끝

        // 그래프 데이터 배열을 현재 시간에 맞게 업데이트
        for (int n = 0; n < 100; n++) {
            float time = n / 100.0f * MaxTime;
            fFrame[n] = EvaluateAnimationAtTime(time);
        }

        // 현재 시간을 최대 시간 범위 내로 클램핑
        currentTime = ImClamp(currentTime, 0.0f, MaxTime);

        // 현재 시간을 최소와 최대 시간 비율로 변환
        float relativeCurrentTime = (currentTime - minTime) / (MaxTime - minTime);

        // 현재 시간 값으로 애니메이션 타임 라인에 맞는 위치값을 계산
        float relativeCurrentTimeXPos = ImLerp(relativeStartTimeXPos, relativeEndTimeXPos, relativeCurrentTime);

        // 빨간 선(현재 시간 위치) 그리기
        draw_list->AddLine(
            ImVec2(relativeCurrentTimeXPos, timelinePos.y), // 라인 시작점
            ImVec2(relativeCurrentTimeXPos, timelinePos.y + timelineSize.y), // 라인 끝점
            IM_COL32(255, 0, 0, 255) // 빨간 선으로 현재 시간 표시
        );

        // 눈금 아래에 숫자로 시간 표시
        float timeLabel = i * fDisplayTime * (MaxTime / MaxTime); // 0.1 간격으로 0부터 MaxTime까지 표시
        char timeLabelStr[16];
        snprintf(timeLabelStr, sizeof(timeLabelStr), "%.1f", timeLabel);
        ImVec2 textPos(xPos - 10, timelinePos.y + timelineSize.y + 5.f);
        draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), timeLabelStr);
    }

    // graphPos : 애니메이션 타임 라인 그래프를 그리기 위해 계산된 위치를 나타냄
    //  graphPos = ImVec2(왼쪽 위 모서리 화면상의 위치x, y, 타임라인의 크기)
    ImVec2 graphPos = ImVec2(timelinePos.x, timelinePos.y + timelineSize.y + 10);


    //if (ImGui::IsMouseClicked(0) && ImGui::IsMouseHoveringRect(
    //    ImVec2(graphPos.x, graphPos.y),
    //    ImVec2(graphPos.x + timelineSize.x, graphPos.y + 100))) {
    //    // 마우스 클릭될 때
    //    isMouseDragging = true;
    //    mouseClickPos.x = ImGui::GetMousePos().x; // 클릭된 좌표 저장x
    //    mouseClickPos.y = ImGui::GetMousePos().y; // 클릭된 좌표 저장y
    //}

    //if (isMouseDragging && ImGui::IsMouseDown(0)) {
    //    // 마우스 드래그 중
    //    ImVec2 mousePos = ImGui::GetMousePos();
    //    float newTime = (mousePos.x - timelinePos.x) / timelineSize.x;
    //    currentTime = ImClamp(newTime, 0.0f, MaxTime); // 타임 라인 범위 내에서만 클램핑
    //}

    //if (!ImGui::IsMouseDown(0)) {
    //    // 마우스 버튼이 떼지면 드래그 상태 해제
    //    isMouseDragging = false;
    //}

    // 최종 비율 = 최소 시간 값 + 현재 시간 값 * (최대 - 최소);
//float mappedTime = minTime + currentTime * (maxTime - minTime);

// 현재 시간을 타임 라인 그래프 위에 라인으로 표시 (상대적인 좌표로 계산)
//float relativeCurrentTimeXPos = ImLerp(relativeStartTimeXPos, relativeEndTimeXPos, mappedTime); // 현재 시간의 상대적인 x좌표

// 그래프의 시간 범위를 최대 30으로 설정
//float maxTime = 30.0f;

#pragma endregion

#pragma region 타임라인 키프레임 조절

    //// 타임 라인 드래그로 현재 시간 이동
    //if (isDraggingTimeline && ImGui::IsMouseDragging(0)) 
    //{
    //    currentTime = (ImGui::GetMousePos().x - timelinePos.x) / timelineSize.x;
    //    currentTime = ImClamp(currentTime, 0.0f, MaxTime);

    //    //float xPos = timelinePos.x + timelineSize.x * currentTime;
    //    //initialKeyframeX = xPos; // 드래그를 시작하는 위치 저장
    //    //float deltaTime = (ImGui::GetMousePos().x - initialKeyframeX) / timelineSize.x * MaxTime;
    //    //currentTime = (ImGui::GetMousePos().x - timelinePos.x) / timelineSize.x;
    //    //currentTime = ImClamp(currentTime, 0.0f, MaxTime);
    //}


    // 타임라인에서 키프레임을 드래그하여 순서 변경
    for (int i = 0; i < timeline.size(); ++i) 
    {
        Keyframe& keyframe = timeline[i];
        //float xPos = timelinePos.x + timelineSize.x * ImClamp(keyframe.time, 0.0f, MaxTime);

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f), // 좌표 설정
            ImVec2(xPos + 5.0f, yPos + 5.0f)) // 좌표 설정
            && ImGui::IsMouseClicked(0))
        {
            isDraggingKeyframe = true;
            draggingKeyframeIndex = i;
            initialKeyframeX = xPos; // 드래그를 시작하는 위치 저장
        }
    }

    if (isDraggingKeyframe) 
    {
        // 키프레임의 시간을 드래그한 위치로 설정
        Keyframe& keyframe = timeline[draggingKeyframeIndex];
        float xPos = ImGui::GetMousePos().x - timelinePos.x;

        // 마우스 드래그로 키프레임의 시간을 조정
        float newXPos = ImGui::GetMousePos().x;
        float deltaTime = (newXPos - initialKeyframeX) / timelineSize.x * MaxTime;

        // 클릭한 위치에 해당하는 value 값
        keyframe.value = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;

        // 시간 값
        timeline[draggingKeyframeIndex].time += deltaTime;

        // 시간과 값을 적절하게 클램핑할 수도 있습니다.
        // 드래그 했을 때 시간과 벨류 값 조정 (키프레임 time과 value, 최소 값, 최대 값);
        keyframe.time = ImClamp(keyframe.time, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정
        keyframe.value = ImClamp(keyframe.value, 0.0f, MaxTime); // 최대 값을 MaxTime으로 지정

        // 현재 위치를 갱신
        initialKeyframeX = newXPos;

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

#pragma endregion

    // 마우스 릴리즈 시 드래그 종료
    if (ImGui::IsMouseReleased(0)) 
    {
        isDraggingTimeline = false;
        isDraggingKeyframe = false;
    }

#pragma region 버튼 (추가, 삭제, 저장, 로드)

    // 키프레임 추가 버튼
    if (ImGui::Button(u8"추가", ImVec2(30, 0))) {
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
        newKeyframe.time = currentTime;
        timeline.insert(timeline.begin() + insertIndex, newKeyframe);
    }

    // "A" 키 : 현재 마우스 위치에 키프레임을 생성
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
    {
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

        // 현재 마우스 위치
        ImVec2 mousePos = ImGui::GetMousePos();

        // 애니메이션 타임 라인 위치
        ImVec2 timelinePos = ImGui::GetCursorScreenPos();
        timelinePos.x += 0.f;
        timelinePos.y += 30.f;

        // 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseX = (mousePos.x - timelinePos.x) / timelineSize.x;

        // 마우스 위치를 애니메이션 타임 라인 상의 상대적인 위치로 변환합니다.
        float relativeMouseY = (mousePos.y - timelinePos.y) / timelineSize.y;

        // 벨류 값을 계산합니다. 예를 들어, 상대적인 마우스 위치를 벨류 값으로 사용할 수 있습니다.
        float newValue = 0.7f - ImClamp(relativeMouseY, 0.0f, 1.0f); // 벨류 값 범위를 0에서 1로 클램핑합니다.

        // 시간 값을 계산하여 새로운 키프레임을 생성
        float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

        // 새로운 키프레임을 생성하고 삽입합니다.
        Keyframe newKeyframe = { newTime, newValue, 0, false, false };
        newKeyframe.time = newTime;
        newKeyframe.value = newValue;
        timeline.push_back(newKeyframe);

    }

#pragma region 단축키
    // "R" 키 : 현재 시간(시간선)을 0으로 초기화 [처음으로 이동]
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)))
    {
        currentTime = 0.f;
    }

    // "T" 키 : 현재 시간(시간 선)을 마우스 위치로 변경
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T)))
    {
        // 현재 마우스 위치
        ImVec2 mousePos = ImGui::GetMousePos();

        // 마우스 위치X를 애니메이션 타임 라인 상의 상대적인 위치로 변환
        float relativeMouseX = (mousePos.x - timelinePos.x) / timelineSize.x;

        // 시간 값을 계산하여 새로운 키프레임을 생성
        float newTime = relativeMouseX * MaxTime; // MaxTime은 최대 시간 값

        // 시간을 현재 마우스 위치에 맞게 설정합니다.
        currentTime = newTime;   
    }

    // "Space" 키 : 애니메이션 재생/정지
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Space)))
    {
        isPlaying = !isPlaying;
    }

    // "0" 키 : 재생 속도 초기화
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_0)))
    {
        playbackSpeed = 1.f;
    }

    // "-" 키 : 재생 속도 -
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Minus)))
    {
        playbackSpeed -= 1.f;
    }

    // "+" 키 : 재생 속도 +
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Equal)))
    {
        playbackSpeed += 1.f;
    }
#pragma endregion

    ImGui::SameLine();

    // 키프레임 목록 창
    ImGui::Begin(u8"키프레임 정보");

    for (int i = 0; i < timeline.size(); ++i) {
        Keyframe& keyframe = timeline[i];
        char keyframeName[64];
        snprintf(keyframeName, sizeof(keyframeName), u8"키프레임 %d", i);

        // 키프레임 정보를 목록으로 표시
        if (ImGui::TreeNode(keyframeName)) {
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"크기: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"회전:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"이동: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);
            ImGui::Checkbox(u8"이징 In", &keyframe.isEaseIn);
            ImGui::Checkbox(u8"이징 Out", &keyframe.isEaseOut);

            // 키프레임 삭제 버튼
            if (ImGui::Button(u8"삭제")) {
                timeline.erase(timeline.begin() + i);
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();

    ImGui::SameLine();

    // 키프레임 삭제 버튼
    if (ImGui::Button(u8"삭제", ImVec2(30, 0))) {
        if (!timeline.empty()) {
            timeline.pop_back();
        }
    }

    ImGui::SameLine();

    // 애니메이션 저장 및 로드 버튼
    if (ImGui::Button(u8"저장", ImVec2(30, 0))) {
        SaveAnimationToFile("Animation");
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
    ImGui::PlotLines("Value Curve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(300, 0));

    delete[] values;

#pragma endregion

    //ImGui::Separator();

#pragma region 애니메이션 속성 편집

    // 키프렘 목록을 보여주는 ImGui 윈도우를 만든다.
    if (ImGui::Begin(u8"키프레임 수정")) 
    {
        // 키프렘 목록을 루프를 돌며 표시.
        for (int i = 0; i < timeline.size(); ++i) 
        {
            Keyframe& keyframe = timeline[i];

            // 각 키프렘을 버튼 또는 다른 ImGui 위젯으로 표시.
            // 여기에서는 버튼을 사용.
            if (ImGui::Button((u8"키프레임" + std::to_string(i)).c_str())) {
                // 키프렘이 클릭되면 해당 인덱스를 선택된 키프렘 인덱스로 설정.
                selectedKeyframeIndex = i;
            }
        }
    }
    ImGui::End();

    // 선택된 키프렘의 인덱스를 사용하여 키프레임을 편집.
    if (!timeline.empty() && selectedKeyframeIndex >= 0 && selectedKeyframeIndex < timeline.size()) 
    {
        Keyframe& selectedKeyframe = timeline[selectedKeyframeIndex];

        // 선택된 키프렘의 애니메이션 속성 편집
        DrawSelectedKeyframeEditor(selectedKeyframe);
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

    ImGui::Dummy(ImVec2(0.f, 110.f));
    

    ImGui::PushItemWidth(100); // 입력 필드의 길이를 원하는 픽셀로 지정
    // 애니메이션 프레임 숫자 입력
    ImGui::InputInt(u8"애니메이션 프레임", &animationFrame);
    ImGui::PopItemWidth(); // 길이 설정을 되돌림

    ImGui::SameLine();

    // 애니메이션 프레임 변경 버튼
    if (ImGui::Button(u8"프레임 변경")) {
        // 키프레임을 animationFrame 값에 따라 생성하고 편집
        EditKeyframesForAnimation(animationFrame);
        currentTime = playbackTime;
    }


    ImGui::SameLine(690.f);

    if (ImGui::Button(u8"애니메이션 불러오기", ImVec2(120, 0))) {
        LoadAnimationFromFile("Animation");
    }

    // 애니메이션 시간 설정
    if (isPlaying)
    {
        currentTime += playbackSpeed * ImGui::GetIO().DeltaTime;
        if (currentTime > MaxTime) // 현재 시간이 최대 시간 값 보다 클 경우
        {
            currentTime = MaxTime; // 현재 시간을 최대 시간 값으로 고정
            isPlaying = false;  // 플레이 중지
        }
    }

#pragma endregion

#pragma region 키프레임 렌더링 및 편집 (마지막 순서 유지)
    // # 마우스 오버시 나오는 툴팁은 모든 UI위에 그려져야 하기 때문에 순서가 가장 마지막이여야 한다.

    // 키프레임 렌더링 및 편집
    for (int i = 0; i < timeline.size(); ++i)
    {
        Keyframe& keyframe = timeline[i];

        // 타임 라인 상의 위치를 계산
        float xPos = timelinePos.x + (keyframe.time / 20.0f) * timelineSize.x;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // 현재 시간에 해당하는 키프레임 표시 (빨간색 원)
        if (keyframe.time >= currentTime - 0.1 &&
            keyframe.time <= currentTime + 0.1) {
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 0, 0, 255) // 빨간색으로 표시
            );
        }
        else {
            // 다른 시간의 키프레임 표시 (노란색 원)
            draw_list->AddCircleFilled(
                ImVec2(xPos, yPos),
                5.0f,
                IM_COL32(255, 255, 0, 255) // 노란색으로 표시
            );
        }

        // 마우스가 키프레임 위에 있는 경우에만 툴팁 표시 (마우스 오버)
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // 키프레임 정보 표시
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"키프레임 %d", i);
            ImGui::Separator();
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"크기: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"회전:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"이동: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"이징 In: 켜짐");
            else
                ImGui::Text(u8"이징 In: 꺼짐");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"이징 Out: 켜짐");
            else
                ImGui::Text(u8"이징 Out: 꺼짐");
            ImGui::EndTooltip();
        }

        // 마우스 릴리즈 시 드래그 종료
        if (ImGui::IsMouseReleased(0)) 
        {
            isDraggingTimeline = false;
        }

        if (!isDraggingTimeline) // 드래그를 하지 않을 경우만 삭제 가능
        {
            // 오른쪽 마우스 클릭으로 키프레임 삭제
            if (ImGui::IsMouseHoveringRect(
                ImVec2(xPos - 5.0f, yPos - 5.0f),
                ImVec2(xPos + 5.0f, yPos + 5.0f))
                && ImGui::IsMouseClicked(1))
            {
                timeline.erase(timeline.begin() + i);
                break; // 삭제한 후에 루프를 빠져나갑니다.
            }
        }

    }

#pragma endregion

#pragma region 키프레임 자동 생성

    ImGui::Begin("Generate");

    ImGui::Text(u8"키프레임 자동 생성");

    // 입력값을 받는 UI 요소 생성
    ImGui::InputFloat2("Min, Max Time", _v2Time);
    ImGui::InputFloat2("Min, Max Value", _v2Value);
    ImGui::InputFloat3("Min Scale", minScale);
    ImGui::InputFloat3("Max Scale", maxScale);    
    ImGui::InputFloat3("Min Rot", minRot);
    ImGui::InputFloat3("Max Rot", maxRot);
    ImGui::InputFloat3("Min Pos", minPos);
    ImGui::InputFloat3("Max Pos", maxPos);
    ImGui::InputFloat("KeyFrame", &numKeyframes);

    if (ImGui::Button("Generate Keyframes")) {
        // 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
        timeline.clear();

        CreateKeyframesWithLinearInterpolation(timeline,
            _v2Time.x, _v2Time.y,
            _v2Value.x, _v2Value.y,
            minScale, maxScale,
            minRot, maxRot,
            minPos, maxPos,
            numKeyframes);
    }

    if (ImGui::Button("clear")) {
        // 버튼이 클릭되면 timeline 벡터를 비우고 CreateKeyframesWithLinearInterpolation 함수 호출
        timeline.clear();
    }
    ImGui::End();
#pragma endregion

}
//==============================================[애니메이션 툴]==============================================

// 애니메이션 저장
void CImguiAnimationTool::SaveAnimationToFile(const char* fileName) 
{
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기 (문자열 깨짐 방지)
    std::ofstream file(datFileName.c_str(), std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        std::cerr << "Failed to open file: " << datFileName << std::endl;
        return;
    }

    for (const Keyframe& keyframe : timeline) {
        // Keyframe 구조체를 파일에 쓸 때 UTF-8로 인코딩된 문자열로 저장
        file << keyframe.time << " "
            << keyframe.value << " "
            << keyframe.type << " "
            << keyframe.isEaseIn << " "
            << keyframe.isEaseOut << " "
            << keyframe.vScale.x << " "
            << keyframe.vScale.y << " "
            << keyframe.vScale.z << " "
            << keyframe.vRot.x << " "
            << keyframe.vRot.y << " "
            << keyframe.vRot.z << " "
            << keyframe.vPos.x << " "
            << keyframe.vPos.y << " "
            << keyframe.vPos.z << "\n";
    }

    file.close();
    std::cout << "Animation saved to file: " << datFileName << std::endl;
}

// 애니메이션 불러오기
void CImguiAnimationTool::LoadAnimationFromFile(const char* fileName)
 {
    // .dat 파일 확장자를 추가한 파일 이름 생성
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // 파일을 UTF-8로 열기
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // 파일을 열 수 없을 때의 오류 처리
        return;
    }

    timeline.clear();
    Keyframe keyframe;

    while (file >> keyframe.time >> keyframe.value >> keyframe.type >>
        keyframe.isEaseIn >> keyframe.isEaseOut >>
        keyframe.vScale.x >> keyframe.vScale.y >> keyframe.vScale.z >>
        keyframe.vRot.x >> keyframe.vRot.y >> keyframe.vRot.z >>
        keyframe.vPos.x >> keyframe.vPos.y >> keyframe.vPos.z) {
        timeline.push_back(keyframe);
    }

    file.close();
}

// 애니메이션을 계산하여 현재 시간 값을 반환하는 함수
float CImguiAnimationTool::EvaluateAnimationAtTime(float time)
{
    if (timeline.empty()) {
        // 만약 키프레임이 없다면 기본값 0.0을 반환하거나,
        // 다른 처리를 수행.
        return 0.0f;
    }

    // 키프레임을 시간에 따라 정렬.
    std::sort(timeline.begin(), timeline.end(), [](const Keyframe& a, const Keyframe& b) {
        return a.time < b.time;
        });

    // 현재 시간과 가장 가까운 이전과 이후의 키프레임을 찾기
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

    // 이전과 다음 키프레임을 찾지 못하면 첫 번째 키프레임을 사용.
    if (!prevKeyframe) {
        prevKeyframe = &timeline[0];
    }

    // 다음 키프레임을 찾지 못하면 마지막 키프레임을 사용.
    if (!nextKeyframe) {
        nextKeyframe = &timeline.back();
    }

    // 이전과 다음 키프레임 사이에서 시간을 보간하여 값을 계산.
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

    ImGui::SameLine(670.f);
    ImVec4 textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 빨간색
    ImGui::TextColored(textColor, "Preview Animation On");

    // 예제: 원 그리기
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 텍스트 색상 설정

    // 반지름은 value에 따라 조절합니다.
    float radius = 20.0f + value * 30.0f;

    // 회전은 value에 따라 조절합니다.
    float rotation_degrees = value * 360.0f;

    // 이동은 value에 따라 조절합니다.
    float x_position = 100.0f + value * 200.0f;
    float y_position = 100.0f + value * 50.0f;

    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(x_position, y_position),
        radius,
        IM_COL32(255, 0, 0, 255), // 원의 색상
        64); // 원의 분할 수

    ImGui::PopStyleColor();
}

// 선형 보간 함수
float CImguiAnimationTool::Lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}

// std::string 벡터를 const char* 배열로 변환하는 함수
std::vector<const char*> CImguiAnimationTool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
    std::vector<const char*> charArray;
    charArray.reserve(stringVector.size());
    for (const std::string& str : stringVector) {
        charArray.push_back(str.c_str());
    }
    return charArray;
}

// 선택한 키프레임 값을 변경하는 함수
void CImguiAnimationTool::DrawSelectedKeyframeEditor(Keyframe& selectedKeyframe)
{
    // 타임라인 툴
    ImGui::Begin(u8"선택한 키프레임 속성 편집");

    //Keyframe& selectedKeyframe = timeline.back();
    ImGui::Text(u8"현재 선택 : 키프레임%d", selectedKeyframeIndex);

    ImGuiStyle& style = ImGui::GetStyle(); // 스타일 불러오기
    float originalItemWidth = style.ItemInnerSpacing.x; // 원래의 ItemInnerSpacing 값을 저장

    // 원하는 크기로 ItemInnerSpacing을 설정
    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절

    ImGui::SeparatorText(u8"시간 및 벨류 설정");

    // 시간 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
    ImGui::PushItemWidth(40);

    // 시간 입력 필드
    if (ImGui::InputFloat(u8"시간", &newTime, fMin_Time, fMax_Time))
    {
        // 선택된 키프레임시간 값을 변경
        selectedKeyframe.time = newTime;

        // 원래 키프레임의 인덱스 찾기
        for (int i = 0; i < timeline.size(); ++i) {
            if (&timeline[i] == &selectedKeyframe) {
                originalIndex = i;
                break;
            }
        }

        // 키프레임을 시간에 따라 정렬
        std::sort(timeline.begin(), timeline.end(), [](const Keyframe& a, const Keyframe& b) {
            return a.time < b.time;
            });

        // 원래 키프레임의 인덱스 찾기
        for (int i = 0; i < timeline.size(); ++i) {
            if (&timeline[i] == &selectedKeyframe) {
                selectedKeyframeIndex = i;
                break;
            }
        }

        // 키프레임 순서 변경
        if (originalIndex != selectedKeyframeIndex) {
            // 키프레임이 원래 위치에서 이동한 경우
            // 이동한 위치에 따라 인덱스 업데이트
            draggingKeyframeIndex = selectedKeyframeIndex;
        }
    }

    ImGui::PopItemWidth();

    //// 시간 값 저장
    //oldTime = selectedKeyframe.time;

    //// 이제 다른 키프렘들의 시간 값을 변경하지 않도록 조치한다
    //// 시간 값을 변경한 키프렘보다 높은 인덱스의 키프렘들은 이동하지 않아야 한다
    //for (int i = selectedKeyframeIndex + 1; i < timeline.size(); ++i)
    //{
    //    Keyframe& currentKeyframe = timeline[i];

    //    // 이동하지 않도록 기존 시간과의 차이를 더합니다.
    //    float timeDifference = currentKeyframe.time - oldTime;
    //    currentKeyframe.time = newTime + timeDifference;
    //}

    ImGui::SameLine(); // 같은 라인

    // 벨류 입력 필드의 가로 길이를 조절 (ex : 40 픽셀)
    ImGui::PushItemWidth(40);
    // 벨류 입력 필드
    ImGui::InputFloat(u8"벨류", &selectedKeyframe.value, fMin_Value, fMax_Value);
    ImGui::PopItemWidth();
    
    // 원래의 ItemInnerSpacing 값으로 복원
    style.ItemInnerSpacing.x = originalItemWidth;

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    // 애니메이션 타입 설정
    ImGui::SeparatorText(u8"애니메이션 타입 설정");
    ImGui::RadioButton(u8"크기", &selectedKeyframe.type, 0); // 크기 애니메이션
    ImGui::SameLine();
    ImGui::RadioButton(u8"회전", &selectedKeyframe.type, 1); // 회전 애니메이션
    ImGui::SameLine();
    ImGui::RadioButton(u8"이동", &selectedKeyframe.type, 2); // 이동 애니메이션

    ImGui::Dummy(ImVec2(0, 5)); // 공백

    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
    // 키프레임 타입에 따른 추가 설정 (슬라이더)
    if (selectedKeyframe.type == 0) { // 크기 애니메이션
        ImGui::SliderFloat3(u8"크기(슬라이더)", selectedKeyframe.vScale, fMin_Scale, fMax_Scale);
    }
    else if (selectedKeyframe.type == 1) { // 회전 애니메이션
        ImGui::SliderFloat3(u8"회전(슬라이더)", selectedKeyframe.vRot, fMin_Rot, fMax_Rot);
    }
    else if (selectedKeyframe.type == 2) { // 이동 애니메이션
        ImGui::SliderFloat3(u8"이동(슬라이더)", selectedKeyframe.vPos, fMin_Pos, fMax_Pos);
    }
    style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

    //ImGui::Dummy(ImVec2(0, 10));
    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5)); // 공백

    style.ItemInnerSpacing.x = 10.0f; // 수평 간격을 10.0f로 조절
    // 키프레임 타입에 따른 추가 설정 (입력)
    if (selectedKeyframe.type == 0) // 크기 애니메이션
    { 
        // 크기 값 입력
        ImGui::InputFloat3(u8"크기(입력)", selectedKeyframe.vScale);
        // 입력된 값의 범위를 제한(수동)
        selectedKeyframe.vScale.x = (selectedKeyframe.vScale.x < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.x > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.x);
        selectedKeyframe.vScale.y = (selectedKeyframe.vScale.y < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.y > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.y);
        selectedKeyframe.vScale.z = (selectedKeyframe.vScale.z < fMin_Scale) ? fMin_Scale : ((selectedKeyframe.vScale.z > fMax_Scale) ? fMax_Scale : selectedKeyframe.vScale.z);
    }
    else if (selectedKeyframe.type == 1) // 회전 애니메이션
    { 
        // 회전 값 입력
        ImGui::InputFloat3(u8"회전(입력)", selectedKeyframe.vRot);

        // 입력된 값의 범위를 제한(함수)
        selectedKeyframe.vRot.x = ImClamp(selectedKeyframe.vRot.x, fMin_Rot, fMax_Rot); // X 값의 범위 제한
        selectedKeyframe.vRot.y = ImClamp(selectedKeyframe.vRot.y, fMin_Rot, fMax_Rot); // Y 값의 범위 제한
        selectedKeyframe.vRot.z = ImClamp(selectedKeyframe.vRot.z, fMin_Rot, fMax_Rot); // Z 값의 범위 제한
    }
    else if (selectedKeyframe.type == 2) // 이동 애니메이션
    { 
        // 이동 값 입력
        ImGui::InputFloat3(u8"이동(입력)", selectedKeyframe.vPos);

        // 입력된 값의 범위를 제한(함수)
        selectedKeyframe.vPos.x = ImClamp(selectedKeyframe.vPos.x, fMin_Pos, fMax_Pos); // X 값의 범위 제한
        selectedKeyframe.vPos.y = ImClamp(selectedKeyframe.vPos.y, fMin_Pos, fMax_Pos); // Y 값의 범위 제한
        selectedKeyframe.vPos.z = ImClamp(selectedKeyframe.vPos.z, fMin_Pos, fMax_Pos); // Z 값의 범위 제한
    }
    style.ItemInnerSpacing.x = originalItemWidth;// 원래의 ItemInnerSpacing 값으로 복원

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"무기 설정");

    // OBJ_TYPE 선택 목록
    const char* objTypeItems[] = { u8"없음", u8"주먹", u8"양손 무기", u8"한손 무기" };
    int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    ImGui::Combo(u8"타입", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems));
    selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);

    // OBJ_NAME 선택 목록
    const char* objNameItems[] = { u8"없음", u8"권총", u8"톰슨 기관총", u8"쇠파이프", u8"맥주병", u8"프라이팬" };
    int selectedObjName = static_cast<int>(selectedKeyframe.m_eObjectName);
    ImGui::Combo(u8"무기", &selectedObjName, objNameItems, IM_ARRAYSIZE(objNameItems));
    selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);

#pragma region 타입에 따른 선택목록 업데이트 (현재 미사용)
    //// OBJ_TYPE 선택 목록
    //const char* objTypeItems[] = { u8"없음", u8"주먹", u8"양손 무기", u8"한손 무기" };
    //int selectedObjType = static_cast<int>(selectedKeyframe.m_eObjectType);
    //if (ImGui::Combo(u8"타입", &selectedObjType, objTypeItems, IM_ARRAYSIZE(objTypeItems))) {
    //    selectedKeyframe.m_eObjectType = static_cast<OBJ_TYPE>(selectedObjType);
    //}

    //// OBJ_NAME 선택 목록 (타입에 따라 다른 목록 사용)
    //const char* objNameItems[] = { u8"없음" };
    //int numObjNameItems = 1; // 기본적으로 "없음"만 표시
    //int selectedObjName = 0;

    //// 타입에 따라 무기 목록 업데이트
    //if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_HAND) {
    //    const char* fistItems[] = { u8"없음", u8"주먹" };
    //    numObjNameItems = IM_ARRAYSIZE(fistItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, fistItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::TWO_OBJECT) {
    //    const char* twoHandedItems[] = { u8"없음", u8"양손_무기1", u8"양손_무기2" };
    //    numObjNameItems = IM_ARRAYSIZE(twoHandedItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, twoHandedItems, numObjNameItems);
    //}
    //else if (selectedKeyframe.m_eObjectType == OBJ_TYPE::RIGHT_OBJECT) {
    //    const char* oneHandedItems[] = { u8"없음", u8"한손_무기1", u8"한손_무기2" };
    //    numObjNameItems = IM_ARRAYSIZE(oneHandedItems);
    //    ImGui::Combo(u8"무기", &selectedObjName, oneHandedItems, numObjNameItems);
    //}
    //else {
    //    ImGui::Combo(u8"무기", &selectedObjName, objNameItems, numObjNameItems);
    //}

    //selectedKeyframe.m_eObjectName = static_cast<OBJ_NAME>(selectedObjName);
#pragma endregion

    ImGui::Dummy(ImVec2(0, 5)); // 공백
    ImGui::SeparatorText(u8"이징 설정");
    // 이징 설정 (Ease In, Ease Out)
    ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
    ImGui::SameLine();
    ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);
    //ImGui::SameLine();

    ImGui::End();

    // 선택된 키프레임만 업데이트
    timeline[selectedKeyframeIndex] = selectedKeyframe;
}

// _vec3 선형 보간 함수
_vec3 CImguiAnimationTool::Lerp(const _vec3& a, const _vec3& b, float t) {
    _vec3 result;
    result.x = a.x + t * (b.x - a.x);
    result.y = a.y + t * (b.y - a.y);
    result.z = a.z + t * (b.z - a.z);
    return result;
}

// _vec2 선형 보간 함수
_float CImguiAnimationTool::Lerp2(const _float& a, const _float& b, float t) {
    _float result;
    result = a + t * (b - a);
    return result;
}

// 최소, 최대 값 및 크기, 회전, 이동 값을 기반으로 선형 보간된 키프레임을 자동으로 생성하는 함수
void CImguiAnimationTool::CreateKeyframesWithLinearInterpolation(
    std::vector<Keyframe>& timeline, float minTime, float maxTime,
    _float minValue, _float maxValue,
    _vec3 minscaleValue, _vec3 maxscaleValue,
    _vec3 minrotationValue, _vec3 maxrotationValue,
    _vec3 mintranslationValue, _vec3 maxtranslationValue, int numKeyframes)
{
    if (numKeyframes < 2) {
        // 최소 2개 이상의 키프레임이 필요합니다.
        return;
    }

    // 키프레임 간의 시간 간격을 계산
    float timeStep = 1.0f / static_cast<float>(numKeyframes - 1);

    // 첫 번째와 마지막 키프레임을 수동으로 추가
    Keyframe firstKeyframe;

    // 값 보간
    firstKeyframe.time = minTime;
    firstKeyframe.value = minValue;
    firstKeyframe.vScale = minscaleValue;
    firstKeyframe.vRot = minrotationValue;
    firstKeyframe.vPos = mintranslationValue;
    timeline.push_back(firstKeyframe);

    Keyframe lastKeyframe;
   
    // 값 보간
    lastKeyframe.time = maxTime;
    lastKeyframe.value = maxValue;
    lastKeyframe.vScale = maxscaleValue;
    lastKeyframe.vRot = maxrotationValue;
    lastKeyframe.vPos = maxtranslationValue;

    timeline.push_back(lastKeyframe);

    // 중간 키프레임 생성 및 보간
    for (int i = 1; i < numKeyframes - 1; ++i) {
        float time = i * timeStep;
        float t = time;  // 시간을 보간 계수로 사용

        // 각각의 속성에 대해 보간값을 계산
        _vec3 scale = Lerp(minscaleValue, maxscaleValue, t);

        _vec3 rotation = Lerp(minrotationValue, maxrotationValue, t);

        _vec3 translation = Lerp(mintranslationValue, maxtranslationValue, t);

        _float value = Lerp2(minValue, maxValue, t);

        Keyframe keyframe;

        // 값 보간
        keyframe.time = time;
        keyframe.value = value;
        keyframe.vScale = scale;
        keyframe.vRot = rotation;
        keyframe.vPos = translation;

        timeline.push_back(keyframe);
    }

    // 시간에 따라 키프레임 정렬
    std::sort(timeline.begin(), timeline.end(), [](const Keyframe& a, const Keyframe& b) {
        return a.time < b.time;
        });
}

void CImguiAnimationTool::EditKeyframesForAnimation(int numFrames)
{
    //timeline.clear(); // 기존의 키프레임을 모두 지웁니다.

    //float frameTime = MaxTime / static_cast<float>(numFrames);

    //for (int frame = 0; frame <= numFrames; ++frame) {
    //    float time = frame * frameTime;
    //    float value = EvaluateAnimationAtTime(time);

    //    // 키프레임을 생성하고 리스트에 추가합니다.
    //    Keyframe keyframe;
    //    keyframe.time = time;
    //    keyframe.value = value;

    //    // 나머지 키프레임 속성을 여기에 설정합니다.
    //    // 예: keyframe.vScale, keyframe.vPos, keyframe.vRot, keyframe.isEaseIn, keyframe.isEaseOut 등

    //    timeline.push_back(keyframe);
    //}
}

//// 애니메이션 타임라인을 생성하고 애니메이션 값을 계산하는 함수
//void GenerateAnimationTimeline(std::vector<Keyframe>& timeline, AnimationProperties properties, int numFrames) 
//{
//    // 키프레임 간의 시간 간격 계산
//    float timeStep = (properties.maxTime - properties.minTime) / (numFrames - 1);
//
//    // 키프레임을 생성하고 애니메이션 값을 계산하여 타임라인에 추가
//    timeline.clear();
//    for (int i = 0; i < numFrames; ++i) {
//        float time = properties.minTime + i * timeStep;
//        // 여기에서 애니메이션 값을 계산하십시오 (예: 랜덤한 값 또는 특정 함수를 사용하여)
//        float value = CalculateAnimationValue(time, properties);
//        Keyframe keyframe = { time, value, /* 다른 속성 값 */ };
//        timeline.push_back(keyframe);
//    }
//}
//
//// 애니메이션 값을 계산하는 함수 예제
//float CalculateAnimationValue(float time, AnimationProperties properties) {
//    // 예: 시간에 따라 선형으로 값 증가
//    float valueRange = properties.maxValue - properties.minValue;
//    float timeRange = properties.maxTime - properties.minTime;
//    float normalizedTime = (time - properties.minTime) / timeRange;
//    return properties.minValue + normalizedTime * valueRange;
//}
//
//// 사용자로부터 애니메이션 속성 및 키프렘 수를 입력 받는 함수
//void EditAnimationProperties(AnimationProperties& properties, int& numFrames) {
//    ImGui::Text(u8"애니메이션 속성 설정");
//    ImGui::InputFloat(u8"최소 시간", &properties.minTime);
//    ImGui::InputFloat(u8"최대 시간", &properties.maxTime);
//    ImGui::InputFloat(u8"최소 값", &properties.minValue);
//    ImGui::InputFloat(u8"최대 값", &properties.maxValue);
//    ImGui::InputFloat(u8"최소 크기", &properties.minSize);
//    ImGui::InputFloat(u8"최대 크기", &properties.maxSize);
//    ImGui::InputFloat(u8"최소 위치", &properties.minPosition);
//    ImGui::InputFloat(u8"최대 위치", &properties.maxPosition);
//    ImGui::InputFloat(u8"최소 회전", &properties.minRotation);
//    ImGui::InputFloat(u8"최대 회전", &properties.maxRotation);
//
//    ImGui::Text(u8"키프렘 수 입력");
//    ImGui::InputInt(u8"키프렘 수", &numFrames);
//
//    if (ImGui::Button(u8"애니메이션 생성")) {
//        GenerateAnimationTimeline(timeline, properties, numFrames);
//    }
//}

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
