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

// �̹��� �߰� �Լ�
void CImguiAnimationTool::AddImage(const std::string& imagePath)
{

    // imagePath�� Wide ���ڿ��� ��ȯ
    WCHAR wideImagePath[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, imagePath.c_str(), -1, wideImagePath, MAX_PATH);

    // �̹����� �ε��ϰ� �ؽ�ó�� ��ȯ
    LPDIRECT3DDEVICE9 pGraphicDev = CImguiMgr::GetInstance()->Get_GraphicDev();
    if (FAILED(D3DXCreateTextureFromFile(pGraphicDev, wideImagePath, &m_pTexture)))
    {
        // �̹��� �ε� ���� ó��
        MSG_BOX("Image Load Failed");
    } 
}

HRESULT CImguiAnimationTool::Ready_ImguiWin()
{
    return S_OK;
}

#pragma region ���̾ƿ�
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

#pragma region ��ŷ �����̼�



    ImGuiWindowFlags iMain_Flags = ImGuiWindowFlags_NoMove;
    ImGuiDockNodeFlags iDockSpace_Flags = ImGuiDockNodeFlags_NoDockingInCentralNode;

    // [����] ��ŷ �����̼� ���ε�
    ImGui::Begin("Center", NULL, iMain_Flags);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        // �⺻ ��
        ImGuiID dockspace_id = ImGui::GetID("DockSpace_AnimationTool");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), iDockSpace_Flags);

        if (m_bFirstLoop)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, iDockSpace_Flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());
            
            ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, NULL, &dockspace_id);
            ImGuiID dock_right_tap_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.2f, &dock_right_id, &dockspace_id);
            ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.2f, NULL, &dock_right_id);
            ImGuiID dock_right_down_tap_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.2f, &dock_right_down_id, &dock_right_id);
            ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, NULL, &dockspace_id);
            
            
            ImGui::DockBuilderDockWindow(u8"�ؽ�ó", dock_right_id);
            ImGui::DockBuilderDockWindow(u8"�ִϸ��̼� �Ӽ� ����", dock_right_tap_id);
            ImGui::DockBuilderDockWindow(u8"Keyframe List", dock_right_down_id);
            ImGui::DockBuilderDockWindow(u8"Ű������ ���", dock_right_down_tap_id);
            ImGui::DockBuilderDockWindow(u8"�ִϸ��̼� ��", dock_down_id);
            ImGui::DockBuilderDockWindow(u8"���", dockspace_id);

            ImGui::DockBuilderFinish(dockspace_id);
        }

        m_bFirstLoop = false;
    }
    else
    {
        //ShowDockingDisabledMessage();
    }

#pragma endregion


#pragma region ���
    if (ImGui::Begin(u8"���", NULL, iMain_Flags))
    {

    }
    ImGui::End();
#pragma endregion

    // ImGui���� �ؽ�ó�� ǥ���� �̹��� ũ�� (��: 200x200 �ȼ�)
    const ImVec2 imageSize(200.0f, 200.0f);

#pragma region �ؽ�ó ��
    static int counter = 0;

    if (ImGui::Begin(u8"�ؽ�ó"))
    {
        // ������
        ImGui::Text(u8"������ %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        // ���ڿ� ���͸� const char* �迭�� ��ȯ
        std::vector<const char*> charImagePaths = ConvertStringVectorToCharArray(imagePaths);

        int selectedPathIndex = 0; // ���õ� �̹��� ��� �ε���

        if (ImGui::CollapsingHeader(u8"�̹��� �ҷ�����"))
        {
            if (ImGui::ListBox("Image Paths", &selectedPathIndex, charImagePaths.data(), charImagePaths.size())) {

                // ������ �̹��� ��θ� ����Ͽ� �̹����� �ε��ϴ� �Լ� ȣ��
                const std::string& selectedImagePath = imagePaths[selectedPathIndex];
                //LoadImg(ConverCtoWC(selectedImagePath.c_str()));
                LoadImg(ConverCtoWC(ConverWStringtoC(ConvertToWideString(imagePaths[selectedPathIndex]))));
            }
        }

        ToggleButton(u8"���", &m_bTest);

        static const char* ini_to_load = NULL;
        if (ini_to_load)
        {
            ImGui::LoadIniSettingsFromDisk(ini_to_load);
            ini_to_load = NULL;
        }

        if (ImGui::Button(u8"���� ��ư"))
        {
            SaveObjectInformationData();

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"�ҷ����� ��ư"))
        {
            LoadObjectInforamtionData();
        }
    }   
    ImGui::End();
#pragma endregion

#pragma region �ִϸ��̼� ��
    // Ÿ�Ӷ��� ��
    ImGui::Begin(u8"�ִϸ��̼� ��");
    RenderTimeline();
    ImGui::End();
#pragma endregion

    // [����] Center End
    ImGui::End();
         
    return 0;
}
#pragma endregion

// �̹��� �ε�
void CImguiAnimationTool::LoadImg(const _tchar* folderPath)
{
    // ã�� �̹��� �����͸� ���� ����
    WIN32_FIND_DATA findData;

    // ���� ���
    wstring wfolderPath = (wstring)folderPath + L"/*.*";

    // ���� ��� ���� �� �ڵ� ��ȯ
    HANDLE hFind = FindFirstFile(wfolderPath.c_str(), &findData);

    // �ڵ��� ���� ���
    if (hFind != INVALID_HANDLE_VALUE)
    {
        // ���� ���丮
        vector<wstring> subDirectories;
        // �̹��� ����
        vector<wstring> imageFiles;
        do
        {
            //  ������ �Ӽ� ������ �Էµȴ�. (���丮���� �������� ��)
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
                {
                    // ���� ��� = ��� + / + ���ϸ�
                    wstring subFolderPath = (wstring)folderPath + L"/" + findData.cFileName;
                    subDirectories.push_back(subFolderPath);
                    LoadImg(subFolderPath.c_str());
                }
            }
            else
            {
                // ������ ���, �̹��� �������� Ȯ���ϰ� �ε�
                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;

                // ���� Ȯ���� Ȯ��
                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga") ||
                    wcsstr(findData.cFileName, L".dds"))
                {
                    // IDirect3DBaseTexture9 �������̽��� �޼��带 ����Ͽ� ť�� �� ���� �ؽ�ó�� ������ �ؽ�ó ���ҽ��� ����.
                    IDirect3DBaseTexture9* pBaseTexture = nullptr;

                    // D3DXCreateTextureFromFile(����̽�, ���ϰ��, ���̽� �ؽ�ó) ���Ͽ��� �ؽ�ó�� ����� �Լ�.
                    // �̹��� ������ ���, �ؽ�ó �ε� �� ����
                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pBaseTexture)))
                    {
                        LPDIRECT3DTEXTURE9 pTexture = static_cast<LPDIRECT3DTEXTURE9>(pBaseTexture);
                        wstring filePathStr = filePath;

                        // ���� ��θ� ������(/)�� ����
                        Replace(filePathStr, L"\\", L"/");

                        // ���� ��ο��� ��ü Ű�� ���� Ű�� ����
                        size_t pos = filePathStr.find_last_of(L"/");
                        wstring ObjKey, StateKey;
                        if (pos != wstring::npos)
                        {
                            ObjKey = filePathStr.substr(filePathStr.find_last_of(L"/", pos - 1) + 1, pos - (filePathStr.find_last_of(L"/", pos - 1) + 1));
                            StateKey = filePathStr.substr(pos + 1);
                        }
                        // ��ü Ű�� uniqueObj ���Ϳ� ���� ��� �߰�
                        if (find(uniqueObj.begin(), uniqueObj.end(), ObjKey) == uniqueObj.end())
                        {
                            uniqueObj.push_back(ObjKey);
                        }
                        // ���� Ű�� uniqueState ���Ϳ� ���� ��� �߰�
                        if (find(uniqueState.begin(), uniqueState.end(), StateKey) == uniqueState.end())
                        {
                            uniqueState.push_back(StateKey);
                            m_MapTexture[make_pair(ObjKey, StateKey)] = pTexture;
                        }
                        // ��ü Ű�� �ش��ϴ� ���� ��Ͽ� ���� Ű �߰�
                        m_MapFile[ObjKey].push_back(StateKey);
                    }
                }
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }
}

// ã��
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

#pragma region ���ڿ� ��ȯ �Լ���
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

// ������ ����
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
        
        // (������ ����, ������ ���� ����, ������, ũ��, nullptr);
        WriteFile(hFile, &timeline, sizeof(Keyframe), &dwByte, nullptr);

        CloseHandle(hFile);
    }
}

// ������ �ε�
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

// ���� ��ο��� ���� �̸��� �����ϴ� �Լ�
void CImguiAnimationTool::PathRemoveFileSpec(TCHAR* path)
{
    if (path == nullptr || *path == '\0')
        return;

    size_t len = wcslen(path);
    if (len == 0)
        return;

    // ����� �޺κк��� �˻��Ͽ� '\' ���ڸ� ã�� �����մϴ�.
    for (size_t i = len - 1; i > 0; --i)
    {
        if (path[i] == '\\')
        {
            path[i] = '\0';
            break;
        }
    }
}

// ���ڿ� Ÿ�� ��ȯ
std::wstring CImguiAnimationTool::ConvertToWideString(const std::string& ansiString)
{
    int wideStrLen = MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, nullptr, 0);
    if (wideStrLen == 0)
    {
        // ��ȯ ���� ó��
        return L"Fail";
    }

    std::wstring wideString(wideStrLen, L'\0');
    MultiByteToWideChar(CP_ACP, 0, ansiString.c_str(), -1, &wideString[0], wideStrLen);

    return wideString;
}

// ��� ��ư
void CImguiAnimationTool::ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos(); // ���� Ŀ����ġ�� ȭ�� ��ǥ�� ��ȯ���ִ� �Լ� (���콺 ��ġ�� ��ŷ� Ȱ�� �����ҵ� �ϴ�)
    ImDrawList* draw_list = ImGui::GetWindowDrawList(); // ���� â�� �׸��� �׸��� ���� ������ �ڵ�
                                                        // ImGui::GetWindowDrawList()�� ���� â������ �����͸� ��´�.

    float height = ImGui::GetFrameHeight(); // �⺻ ũ�� �� ����
    float width = height * 1.55f;   // ��۹�ư ��� ���� ����
    float radius = height * 0.50f;  // ��۹�ư ���� ���׶�� ũ��(��)

    // ����ڿ��� ������ ������, Ŭ�� �̺�Ʈ�� �����ϰų� ���¸� �����ϱ� ���� ����ϴ� �Լ�
    //              (�����ĺ���(str_id), ��ư�� ũ��)
    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) // ������ Ŭ���Ǿ��� �� '*v'��� �Ҹ��� ������ ������ ����Ѵ�. �� ������ ��� ��ư�� ���¸� ��Ÿ����.
        *v = !*v;

    float t = *v ? 1.0f : 0.0f; // '*v' ������ ���� ���� 't' ���� �����Ѵ�. 
                                // ex : '*v'�� true�� 't'�� 1.0, false�� 0.0

    // ���콺 ���������� ���� ����
    ImU32 col_bg;
    if (ImGui::IsItemHovered()) // ���콺�� ��ư���� �ö󰬴��� �Ǵ� 
        // ImLerp : ���� ���� �Լ� (���콺 ������ ����)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else                            
        // ImLerp : ���� ���� �Լ� (���콺 Ŭ���� ����)
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    // draw_list�� ����Ͽ� ��ư�� ����� �׸���.
    // AddRectFilled �Լ��� ����Ͽ� �簢���� �׸���.
    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);

    // ��ư ���ο� ���׶�̸� �׸���. AddCircleFilled �Լ��� ����Ͽ� ���� �׸���.
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

// ���� ���� �Լ� ex1
//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = ImClamp(t, 0.0f, 1.0f); // t ���� [0, 1] ������ Ŭ�����մϴ�.
//    return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t));
//}

// ���� ���� �Լ� ex2
//ImVec4 CImguiAnimationTool::ImLerp(const ImVec4& a, const ImVec4& b, float t)
//{
//    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t); // t ���� [0, 1] ������ Ŭ�����մϴ�.
//    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
//}

// �ִϸ��̼� ��
void CImguiAnimationTool::RenderTimeline() 
{
#pragma region �ð� Ÿ�Ӷ���

    // �ð�
    ImGui::Text("Time: %.2f", currentTime);
    ImGui::SameLine();

    ImGui::PushItemWidth(150);
    // �ð� �׷���
    float fFrame[100];
    for (int n = 0; n < 100; n++)
        fFrame[n] = sinf(n * 0.2f + ImGui::GetTime() * currentTime);
    ImGui::PlotLines(u8"Ÿ�Ӷ���", fFrame, 100);
    ImGui::PopItemWidth();


    // Ÿ�� ���� ������
    ImVec2 timelineSize(500, 85); // Ÿ�Ӷ��� ũ��
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 timelinePos = ImGui::GetCursorScreenPos();
    
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0)); // ���� ������ �߰�
    ImGui::SameLine();

#pragma endregion

#pragma region �ִϸ��̼� ���/����

    // �ִϸ��̼� ���/�Ͻ� ���� ��ư
    if (isPlaying) {
        if (ImGui::Button(u8"����"))
        {
            isPlaying = false;
        }
    }
    else {
        if (ImGui::Button(u8"���"))
        {
            isPlaying = true;
            playbackTime = 0.0f;
        }
    }

    ImGui::SameLine();
    //ImGui::Spacing(); // ���� �߰�
    //ImGui::Dummy(ImVec2(0, 20)); // ���� ������ �߰�
    //ImGui::SameLine();

    ImGui::PushItemWidth(100); // ���� ������
    // �ִϸ��̼� ��� �ӵ� ����
    ImGui::SliderFloat(u8"��� �ӵ�", &playbackSpeed, 0.1f, 3.0f);
    ImGui::PopItemWidth();

#pragma endregion

    ImGui::Separator(); // �� �߰�

#pragma region �ִϸ��̼� Ÿ�� ����

    // Ÿ�� ���� ��ġ ����
    timelinePos.x += 0.f;
    timelinePos.y += 30.f;

    // Ÿ�� ���� �׸���
    draw_list->AddRectFilled(
        timelinePos,
        ImVec2(timelinePos.x + timelineSize.x, timelinePos.y + timelineSize.y),
        IM_COL32(50, 50, 50, 255) // Ÿ�Ӷ��� ��� ����
    );

#pragma endregion

#pragma region Ű������ ������ �� ����

    // Ű������ ������ �� ����
    for (int i = 0; i < timeline.size(); ++i) 
    {
        Keyframe& keyframe = timeline[i];
        float xPos = timelinePos.x + timelineSize.x * keyframe.time;
        float yPos = timelinePos.y + timelineSize.y - timelineSize.y * keyframe.value;

        // Ű������ �׸���
        draw_list->AddCircleFilled(
            ImVec2(xPos, yPos),
            5.0f,
            IM_COL32(255, 255, 0, 255)
        );

        // ���콺�� Ű������ ���� �ִ� ��쿡�� ���� ǥ��
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
        {

            // Ű������ ���� ǥ��
            ImGui::SetCursorScreenPos(ImVec2(xPos + 10.0f, yPos - 20.0f));
            ImGui::BeginTooltip();
            ImGui::Text(u8"Ű������ %d", i + 1);
            ImGui::Separator();
            ImGui::Text(u8"�ð�: %.2f", keyframe.time);
            ImGui::Text(u8"��: %.2f", keyframe.value);
            ImGui::Text(u8"����: (%.2f, %.2f, %.2f)", keyframe.color[0], keyframe.color[1], keyframe.color[2]);

            if (keyframe.isEaseIn)
                ImGui::Text(u8"��¡ In: ����");
            else
                ImGui::Text(u8"��¡ In: ����");
            if (keyframe.isEaseOut)
                ImGui::Text(u8"��¡ Out: ����");
            else
                ImGui::Text(u8"��¡ Out: ����");
            ImGui::EndTooltip();
        }

        // ���콺 Ŭ������ Ű������ �̵�
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f))
            && ImGui::IsMouseClicked(0))
        {
            isDraggingTimeline = true;
            keyframe.value = 1.0f - (ImGui::GetMousePos().y - timelinePos.y) / timelineSize.y;
        }

        // ������ ���콺 Ŭ������ Ű������ ����
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f))
            && ImGui::IsMouseClicked(1))
        {
            timeline.erase(timeline.begin() + i);
            break; // ������ �Ŀ� ������ ���������ϴ�.
        }

        // Ű�����ӿ� ��¡ ���� UI �߰�
        //ImGui::SameLine();
        //ImGui::Checkbox(("In##" + std::to_string(i)).c_str(), &keyframe.isEaseIn);
        //ImGui::SameLine();
        //ImGui::Checkbox(("Out##" + std::to_string(i)).c_str(), &keyframe.isEaseOut);
    }

#pragma endregion

#pragma region Ÿ�Ӷ��� Ű������ ����

    // Ÿ�� ���� �巡�׷� ���� �ð� �̵�
    if (isDraggingTimeline && ImGui::IsMouseDragging(0)) 
    {
        currentTime = (ImGui::GetMousePos().x - timelinePos.x) / timelineSize.x;
        currentTime = ImClamp(currentTime, 0.0f, 1.0f);
    }


    // Ÿ�Ӷ��ο��� Ű�������� �巡���Ͽ� ���� ����
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
        // Ű�������� �ð��� �巡���� ��ġ�� ����
        Keyframe& keyframe = timeline[draggingKeyframeIndex];
        float xPos = ImGui::GetMousePos().x - timelinePos.x;
        keyframe.time = ImClamp(xPos / timelineSize.x, 0.0f, 1.0f);

        // Ű������ ���� ����
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

#pragma region ��ư (�߰�, ����, ����, �ε�)

    // Ű������ �߰� ��ư
    if (ImGui::Button(u8"�߰�", ImVec2(30, 0))) {
        // ���ο� Ű�������� �߰��� �� ���� �ð��� ���� ����� Ű�������� ã���ϴ�.
        float minDist = FLT_MAX;
        int insertIndex = 0;
        for (int i = 0; i < timeline.size(); ++i) {
            float dist = fabsf(currentTime - timeline[i].time);
            if (dist < minDist) {
                minDist = dist;
                insertIndex = i;
            }
        }

        // ���ο� Ű�������� �����ϰ� �����մϴ�.
        Keyframe newKeyframe = { currentTime, 0.5f, 0, false, false };
        timeline.insert(timeline.begin() + insertIndex, newKeyframe);
    }

    ImGui::SameLine();

    // Ű������ ��� â
    ImGui::Begin(u8"Ű������ ���");

    for (int i = 0; i < timeline.size(); ++i) {
        Keyframe& keyframe = timeline[i];
        char keyframeName[64];
        snprintf(keyframeName, sizeof(keyframeName), u8"Ű������ %d", i + 1);

        // Ű������ ������ ������� ǥ��
        if (ImGui::TreeNode(keyframeName)) {
            ImGui::Text(u8"��: %.2f", keyframe.value);
            ImGui::Text(u8"�ð�: %.2f", keyframe.time);
            ImGui::Text(u8"ũ��: (%.2f, %.2f, %.2f)", keyframe.vScale.x, keyframe.vScale.y, keyframe.vScale.z);
            ImGui::Text(u8"ȸ��:(%.2f, %.2f, %.2f)", keyframe.vRot.x, keyframe.vRot.y, keyframe.vRot.z);
            ImGui::Text(u8"�̵�: (%.2f, %.2f, %.2f)", keyframe.vPos.x, keyframe.vPos.y, keyframe.vPos.z);
            ImGui::Checkbox(u8"��¡ In", &keyframe.isEaseIn);
            ImGui::Checkbox(u8"��¡ Out", &keyframe.isEaseOut);

            // Ű������ ���� ��ư
            if (ImGui::Button(u8"����")) {
                timeline.erase(timeline.begin() + i);
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();

    ImGui::SameLine();

    // Ű������ ���� ��ư
    if (ImGui::Button(u8"����", ImVec2(30, 0))) {
        if (!timeline.empty()) {
            timeline.pop_back();
        }
    }

    ImGui::SameLine();

    // �ִϸ��̼� ���� �� �ε� ��ư
    if (ImGui::Button(u8"����", ImVec2(30, 0))) {
        SaveAnimationToFile("Animation");
    }

    ImGui::SameLine();

    if (ImGui::Button(u8"�ε�", ImVec2(30, 0))) {
        LoadAnimationFromFile("Animation");
    }

#pragma endregion

    ImGui::SameLine();

#pragma region Ÿ�Ӷ��� �׷���

    // Ÿ�Ӷ��� �׷���
    float* values = new float[100];
    for (int i = 0; i < 100; i++) {
        float time = i / 100.0f;
        values[i] = EvaluateAnimationAtTime(time);
    }
    ImGui::PlotLines("VCurve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(300, 0));

    delete[] values;

#pragma endregion

    //ImGui::Separator();

#pragma region �ִϸ��̼� �Ӽ� ����

    // Ű���� ����� �����ִ� ImGui �����츦 ����ϴ�.
    if (ImGui::Begin("Keyframe List")) {
        // Ű���� ����� ������ ���� ǥ���մϴ�.
        for (int i = 0; i < timeline.size(); ++i) {
            Keyframe& keyframe = timeline[i];

            // �� Ű������ ��ư �Ǵ� �ٸ� ImGui �������� ǥ���մϴ�.
            // ���⿡���� ��ư�� ����մϴ�.
            if (ImGui::Button(("Keyframe " + std::to_string(i)).c_str())) {
                // Ű������ Ŭ���Ǹ� �ش� �ε����� ���õ� Ű���� �ε����� �����մϴ�.
                selectedKeyframeIndex = i;
            }
        }
    }
    ImGui::End();

    // ���õ� Ű������ �ε����� ����Ͽ� Ű������ �����մϴ�.
    if (!timeline.empty() && selectedKeyframeIndex >= 0 && selectedKeyframeIndex < timeline.size()) 
    {
        Keyframe& selectedKeyframe = timeline[selectedKeyframeIndex];

        // ���õ� Ű������ �ִϸ��̼� �Ӽ� ����
        DrawSelectedKeyframeEditor(selectedKeyframe);
    }

    // �̸����� Ű������ ����
    if (!timeline.empty()) 
    {
        for (int i = 0; i < timeline.size() - 1; ++i) {
            Keyframe& prevKeyframe = timeline[i];
            Keyframe& nextKeyframe = timeline[i + 1];

            if (currentTime >= prevKeyframe.time && currentTime <= nextKeyframe.time) {
                float alpha = (currentTime - prevKeyframe.time) / (nextKeyframe.time - prevKeyframe.time);

                // ������ �ִϸ��̼� �̸����� �� ���
                float interpolatedValue = Lerp(prevKeyframe.value, nextKeyframe.value, alpha);
                
                // �� ���� ����Ͽ� �̸����� �ִϸ��̼� ������
                RenderPreviewAnimation(interpolatedValue);

                break;
            }
        }
    }

    ImGui::Dummy(ImVec2(0.f, 90.f));
    

    ImGui::PushItemWidth(100); // �Է� �ʵ��� ���̸� ���ϴ� �ȼ��� ����
    // �ִϸ��̼� ������ ���� �Է�
    ImGui::InputInt(u8"�ִϸ��̼� ������", &animationFrame);
    ImGui::PopItemWidth(); // ���� ������ �ǵ���

    ImGui::SameLine();

    // �ִϸ��̼� ������ ���� ��ư
    if (ImGui::Button(u8"������ ����")) {
        currentTime = playbackTime;
    }

    // �ִϸ��̼� �ð� ����
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

// �ִϸ��̼� ����
void CImguiAnimationTool::SaveAnimationToFile(const char* fileName) 
{
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ���� (���ڿ� ���� ����)
    std::ofstream file(datFileName.c_str(), std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        // ������ �� �� ���� ���� ���� ó��
        std::cerr << "Failed to open file: " << datFileName << std::endl;
        return;
    }

    for (const Keyframe& keyframe : timeline) {
        // Keyframe ����ü�� ���Ͽ� �� �� UTF-8�� ���ڵ��� ���ڿ��� ����
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

// �ִϸ��̼� �ҷ�����
void CImguiAnimationTool::LoadAnimationFromFile(const char* fileName)
 {
    // .dat ���� Ȯ���ڸ� �߰��� ���� �̸� ����
    std::string datFileName = "../Data/" + std::string(fileName) + ".dat";

    // ������ UTF-8�� ����
    std::ifstream file(datFileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        // ������ �� �� ���� ���� ���� ó��
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

// �ִϸ��̼��� ����Ͽ� ���� �ð� ���� ��ȯ�ϴ� �Լ�
float CImguiAnimationTool::EvaluateAnimationAtTime(float time)
{
    if (timeline.empty()) {
        // ���� Ű�������� ���ٸ� �⺻�� 0.0�� ��ȯ�ϰų�,
        // �ٸ� ó���� ������ �� �ֽ��ϴ�.
        return 0.0f;
    }

    // Ű�������� �ð��� ���� �����մϴ�.
    std::sort(timeline.begin(), timeline.end(), [](const Keyframe& a, const Keyframe& b) {
        return a.time < b.time;
        });

    // ���� �ð��� ���� ����� ������ ������ Ű�������� ã���ϴ�.
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

    // ������ ���� Ű�������� ã�� ���ϸ� ù ��° Ű�������� ����մϴ�.
    if (!prevKeyframe) {
        prevKeyframe = &timeline[0];
    }

    // ���� Ű�������� ã�� ���ϸ� ������ Ű�������� ����մϴ�.
    if (!nextKeyframe) {
        nextKeyframe = &timeline.back();
    }

    // ������ ���� Ű������ ���̿��� �ð��� �����Ͽ� ���� ����մϴ�.
    float t = (time - prevKeyframe->time) / (nextKeyframe->time - prevKeyframe->time);
    float value = ImLerp(prevKeyframe->value, nextKeyframe->value, t);

    // ��¡(Easing)�� �����մϴ�.
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

// �����ƽ ��¡(InQuad) �Լ� (����)
float CImguiAnimationTool::ImEaseInQuad(float start, float end, float t)
{
    return start + (end - start) * t * t;
}

// �����ƽ ��¡(OutQuad) �Լ� (��)
float CImguiAnimationTool::ImEaseOutQuad(float start, float end, float t)
{
    return start + (end - start) * (-t * (t - 2));
}

// �̸����� �ִϸ��̼� ������ �Լ�
void CImguiAnimationTool::RenderPreviewAnimation(float value) 
{
    // �� �Լ����� �̸����� �ִϸ��̼� ������
    // value�� ������� ũ��, ȸ��, �̵� ���� �����Ͽ� ������

    ImGui::Text("Preview Animation:");

    // ����: �� �׸���
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // �ؽ�Ʈ ���� ����

    // �������� value�� ���� �����մϴ�.
    float radius = 20.0f + value * 30.0f;

    // ȸ���� value�� ���� �����մϴ�.
    float rotation_degrees = value * 360.0f;

    // �̵��� value�� ���� �����մϴ�.
    float x_position = 100.0f + value * 200.0f;
    float y_position = 100.0f + value * 50.0f;

    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(x_position, y_position),
        radius,
        IM_COL32(255, 0, 0, 255), // ���� ����
        64); // ���� ���� ��

    ImGui::PopStyleColor();
}

// ���� ���� �Լ�
float CImguiAnimationTool::Lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}

// std::string ���͸� const char* �迭�� ��ȯ�ϴ� �Լ�
std::vector<const char*> CImguiAnimationTool::ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector)
{
    std::vector<const char*> charArray;
    charArray.reserve(stringVector.size());
    for (const std::string& str : stringVector) {
        charArray.push_back(str.c_str());
    }
    return charArray;
}

// ������ Ű������ ���� �����ϴ� �Լ�
void CImguiAnimationTool::DrawSelectedKeyframeEditor(Keyframe& selectedKeyframe)
{
    // Ÿ�Ӷ��� ��
    ImGui::Begin(u8"�ִϸ��̼� �Ӽ� ����");

    //Keyframe& selectedKeyframe = timeline.back();
    ImGui::Text("Selected Keyframe:");

    // ��¡ ���� (Ease In, Ease Out)
    ImGui::Checkbox("Ease In", &selectedKeyframe.isEaseIn);
    ImGui::SameLine();
    ImGui::Checkbox("Ease Out", &selectedKeyframe.isEaseOut);

    // �ִϸ��̼� Ÿ�� ����
    ImGui::Text(u8"�ִϸ��̼� Ÿ��:");
    ImGui::RadioButton(u8"ũ��", &selectedKeyframe.type, 0); // ũ�� �ִϸ��̼�
    ImGui::SameLine();
    ImGui::RadioButton(u8"ȸ��", &selectedKeyframe.type, 1); // ȸ�� �ִϸ��̼�
    ImGui::SameLine();
    ImGui::RadioButton(u8"�̵�", &selectedKeyframe.type, 2); // �̵� �ִϸ��̼�

    // Ű������ Ÿ�Կ� ���� �߰� ���� (�����̴�)
    if (selectedKeyframe.type == 0) { // ũ�� �ִϸ��̼�
        ImGui::SliderFloat3(u8"ũ��", selectedKeyframe.vScale, 0.1f, 2.0f);
    }
    else if (selectedKeyframe.type == 1) { // ȸ�� �ִϸ��̼�
        ImGui::SliderFloat3(u8"ȸ��", selectedKeyframe.vRot, 0.0f, 360.0f);
    }
    else if (selectedKeyframe.type == 2) { // �̵� �ִϸ��̼�
        ImGui::SliderFloat3(u8"�̵�", selectedKeyframe.vPos, -300.0f, 300.0f);
    }

    //ImGui::Dummy(ImVec2(0, 10));
    ImGui::SeparatorText(u8"�Է�");


    // Ű������ Ÿ�Կ� ���� �߰� ���� (�Է�)
    if (selectedKeyframe.type == 0) { // ũ�� �ִϸ��̼�
        ImGui::InputFloat3(u8"ũ��", selectedKeyframe.vScale);
    }
    else if (selectedKeyframe.type == 1) { // ȸ�� �ִϸ��̼�
        ImGui::InputFloat3(u8"ȸ��", selectedKeyframe.vRot);
    }
    else if (selectedKeyframe.type == 2) { // �̵� �ִϸ��̼�
        ImGui::InputFloat3(u8"�̵�", selectedKeyframe.vPos);
    }

    ImGui::End();

    // ���õ� Ű������ ������Ʈ
    timeline[selectedKeyframeIndex] = selectedKeyframe;
}