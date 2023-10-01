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

        // ImGui에서 텍스처를 표시할 이미지 크기 (예: 200x200 픽셀)
        const ImVec2 imageSize(200.0f, 200.0f);

#pragma region 텍스처 툴
        static int counter = 0;

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

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();

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
#pragma endregion

#pragma region 애니메이션 툴
        // 타임라인 툴
        ImGui::Begin(u8"애니메이션 툴");
        RenderTimeline();
        ImGui::End();
#pragma endregion
         
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

// 애니메이션 툴
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

        // 마우스가 키프레임 위에 있는 경우에만 툴팁 표시
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // 키프레임 정보 표시
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"키프레임 %d", i + 1);
            ImGui::Separator();
            ImGui::Text(u8"시간: %.2f", keyframe.time);
            ImGui::Text(u8"값: %.2f", keyframe.value);
            ImGui::Text(u8"색상: (%.2f, %.2f, %.2f)", keyframe.color[0], keyframe.color[1], keyframe.color[2]);

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

        // 마우스 클릭으로 키프레임 이동
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f))
            && ImGui::IsMouseClicked(0))
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

#pragma region 타임라인 키프레임 조절

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

#pragma endregion

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
        timeline.insert(timeline.begin() + insertIndex, newKeyframe);
    }

    ImGui::SameLine();

    // 키프레임 목록 창
    ImGui::Begin(u8"키프레임 목록");

    for (int i = 0; i < timeline.size(); ++i) {
        Keyframe& keyframe = timeline[i];
        char keyframeName[64];
        snprintf(keyframeName, sizeof(keyframeName), u8"키프레임 %d", i + 1);

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

    ImGui::SameLine();

    if (ImGui::Button(u8"로드", ImVec2(30, 0))) {
        LoadAnimationFromFile("Animation");
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
    ImGui::PlotLines("VCurve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(300, 0));

    delete[] values;

#pragma endregion

    //ImGui::Separator();

#pragma region 애니메이션 속성 편집

    // 키프렘 목록을 보여주는 ImGui 윈도우를 만듭니다.
    if (ImGui::Begin("Keyframe List")) {
        // 키프렘 목록을 루프를 돌며 표시합니다.
        for (int i = 0; i < timeline.size(); ++i) {
            Keyframe& keyframe = timeline[i];

            // 각 키프렘을 버튼 또는 다른 ImGui 위젯으로 표시합니다.
            // 여기에서는 버튼을 사용합니다.
            if (ImGui::Button(("Keyframe " + std::to_string(i)).c_str())) {
                // 키프렘이 클릭되면 해당 인덱스를 선택된 키프렘 인덱스로 설정합니다.
                selectedKeyframeIndex = i;
            }
        }
    }
    ImGui::End();

    // 선택된 키프렘의 인덱스를 사용하여 키프렘을 편집합니다.
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

    ImGui::Dummy(ImVec2(0.f, 90.f));
    

    ImGui::PushItemWidth(100); // 입력 필드의 길이를 원하는 픽셀로 지정
    // 애니메이션 프레임 숫자 입력
    ImGui::InputInt(u8"애니메이션 프레임", &animationFrame);
    ImGui::PopItemWidth(); // 길이 설정을 되돌림

    ImGui::SameLine();

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

#pragma endregion
}

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

    ImGui::Text("Preview Animation:");

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
    ImGui::Begin(u8"애니메이션 속성 편집");

    //Keyframe& selectedKeyframe = timeline.back();
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

    // 키프레임 타입에 따른 추가 설정 (슬라이더)
    if (selectedKeyframe.type == 0) { // 크기 애니메이션
        ImGui::SliderFloat3(u8"크기", selectedKeyframe.vScale, 0.1f, 2.0f);
    }
    else if (selectedKeyframe.type == 1) { // 회전 애니메이션
        ImGui::SliderFloat3(u8"회전", selectedKeyframe.vRot, 0.0f, 360.0f);
    }
    else if (selectedKeyframe.type == 2) { // 이동 애니메이션
        ImGui::SliderFloat3(u8"이동", selectedKeyframe.vPos, -300.0f, 300.0f);
    }

    //ImGui::Dummy(ImVec2(0, 10));
    ImGui::SeparatorText(u8"입력");


    // 키프레임 타입에 따른 추가 설정 (입력)
    if (selectedKeyframe.type == 0) { // 크기 애니메이션
        ImGui::InputFloat3(u8"크기", selectedKeyframe.vScale);
    }
    else if (selectedKeyframe.type == 1) { // 회전 애니메이션
        ImGui::InputFloat3(u8"회전", selectedKeyframe.vRot);
    }
    else if (selectedKeyframe.type == 2) { // 이동 애니메이션
        ImGui::InputFloat3(u8"이동", selectedKeyframe.vPos);
    }

    ImGui::End();

    // 선택된 키프렘만 업데이트
    timeline[selectedKeyframeIndex] = selectedKeyframe;
}