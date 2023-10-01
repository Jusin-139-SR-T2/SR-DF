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

//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

//bool show_demo_window = true;
//bool show_another_window = false;

//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
//ImGui::Checkbox("Another Window", &show_another_window);

#pragma region ���̾ƿ�
_int CImguiAnimationTool::Update_ImguiWin(const _float& fTimeDelta)
{
    if (!m_bOpen)
        return 0;

    SUPER::Update_ImguiWin(fTimeDelta);

        // ImGui���� �ؽ�ó�� ǥ���� �̹��� ũ�� (��: 200x200 �ȼ�)
        const ImVec2 imageSize(200.0f, 200.0f);

#pragma region �׽�Ʈ1

        static int counter = 0;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::Button(u8"�̹��� �ε� ��ư (�ӽ�)")) // @@@@@ ���� 2. �̹��� �ε� �ð��� ���������̰� ���. (���� ���� ������ �� �����) @@@@@
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
        ImGui::Begin(u8"�׷��� ����", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"����"))
            {
                if (ImGui::MenuItem(u8"����", "Ctrl+O")) { /* Do stuff */ }
                if (ImGui::MenuItem(u8"����", "Ctrl+S")) { /* Do stuff */ }
                if (ImGui::MenuItem(u8"�ݱ�", "Ctrl+W")) { my_tool_active = false; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Edit a color stored as 4 floats
        ImGui::ColorEdit4(u8"����", my_color);

        // Generate samples and plot them
        float fFrame[100];
        for (int n = 0; n < 100; n++)
            fFrame[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
        ImGui::PlotLines(u8"�׷���", fFrame, 100);

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"���");
        ImGui::BeginChild("Scrolling");
        for (int n = 0; n < 50; n++)
            ImGui::Text(u8"%04d: �ؽ�Ʈ", n);
        ImGui::EndChild();
        ImGui::End();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
        if (ImGui::Button(u8"������ �߰� ��ư"))      // ������ �߰� ��ư
        m_vecAnimationInfo.push_back(m_AniInfo);    // �������� �ִϸ��̼� ���� ���Ϳ� �߰�

        ImGui::SameLine(); // ���� ����
        ImGui::Text(u8"������ ������ = Frame_%d", m_vecAnimationInfo.size());

        ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 1.00f), "Data Number");

        // Ÿ�Ӷ��� ��
        ImGui::Begin(u8"�ִϸ��̼� Ÿ�Ӷ���");
        RenderTimeline();
        ImGui::End();

#pragma region LisBox 1��
        // ����Ʈ ���ڿ� ǥ���� �׸� ���
        std::vector<const char*> frameNames;
        for (int i = 0; i < m_vecAnimationInfo.size(); i++) {
            // "Frame_%d" ������ ���ڿ��� �����Ͽ� ���Ϳ� �߰�
            frameNames.push_back(("Frame_" + std::to_string(i)).c_str());
        }

        // ListBox�� �����ϰ� �׸��� ǥ��
        if (ImGui::ListBox("Frames", &selectedItemIndex, frameNames.data(), frameNames.size())) 
        {
            // ������ �׸� ���� �۾��� ����
            if (selectedItemIndex >= 0 && selectedItemIndex < frameNames.size()) {
                // selectedItemIndex�� ����Ͽ� ������ �����ӿ� ���� �۾��� ����
                //RenderSelectedFrameTexture(selectedItemIndex);
            }
        }

        ImGui::SliderFloat("Animation Time", &g_animationTime, 0.0f, 1.0f);
        ImGui::SliderInt("Current Frame", &g_currentFrame, 0, 100);

        // �ִϸ��̼� Ÿ�Ӷ��� �׸���
        ImGui::Text("Animation Timeline:");
        ImGui::ProgressBar(g_animationTime, ImVec2(-1.0f, 0.0f), "");
#pragma endregion
         
        ImGui::InputText("FrameName", "FrameName", IM_ARRAYSIZE("EEEEEEEEEEEE"));

        if (ImGui::Button(u8"���� ��ư"))
        {

        }

        ImGui::SameLine();

        if (ImGui::Button(u8"�ҷ����� ��ư"))
        {

        }

#pragma endregion

    return 0;
}

#pragma endregion

// ������
//std::string CImguiAnimationTool::OpenImageFileDialog()
//{
//    std::string imagePath; // ������ �̹����� ��θ� ������ ����
//
//    // ���� ��ȭ ���� ����ü �ʱ�ȭ
//    OPENFILENAME OpenFile;
//
//    // ���� �н�
//    char szFile[MAX_PATH] = "";
//
//    // Wide ���ڿ��� ��ȯ
//    std::wstring w_szFile = ConvertToWideString(szFile);
//  
//    // �̹��� �н� ����
//    std::string ImagePath = "�̹��� ���� (*.bmp;*.jpg;*.png;*.gif)\0*.bmp;*.jpg;*.png;*.gif\0��� ���� (*.*)\0*.*\0";
//
//    // Wide ���ڿ��� ��ȯ
//    std::wstring WideImagePath = ConvertToWideString(ImagePath);
//
//    ZeroMemory(&OpenFile, sizeof(OpenFile));
//    OpenFile.lStructSize = sizeof(OpenFile);
//    OpenFile.hwndOwner = NULL; // �θ� ������ �ڵ�, NULL�� �����ϸ� ���� �����츦 �������� ��ȭ ���ڰ� �����ϴ�.
//    OpenFile.lpstrFilter = WideImagePath.c_str(); 
//    OpenFile.lpstrFile = &w_szFile[0]; // LPWSTR�� ��ȯ
//    OpenFile.nMaxFile = sizeof(szFile);
//    OpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
//
//    // ���� ��ȭ ���� ����
//    if (GetOpenFileName(&OpenFile) == TRUE)
//    {
//        imagePath = szFile; // ������ ���� ��θ� imagePath ������ ����
//    }
//
//    return imagePath;
//}

//// ���� �ҷ�����
//HRESULT CImguiAnimationTool::OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev)
//{
//    //���� �� ���丮 ������ �����ϱ� ���� ����ü
//    WIN32_FIND_DATA findData;
//
//    //floderPath�� ���� wFolderPath�� ���� (���� ��θ� �����ڵ幮�ڿ��� ó��)
//    wstring wfolderPath = (wstring)folderPath + L"\\*.*";
//
//    // ���� ��� ���� �� �ڵ� ��ȯ
//    HANDLE hFind = FindFirstFileW(wfolderPath.c_str(), &findData);
//
//    if (hFind != INVALID_HANDLE_VALUE)
//    {
//        //���� ���� ��� ���ϰ� ���丮�� �˻� FindNextFile�Լ��� ����ؼ� �������� �Ǵ� ���丮�� ã�´�
//        do
//        {
//            //  ������ �Ӽ� ������ �Էµȴ�. (���丮���� �������� ��)
//            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//            {
//                // ���丮�� ���, "."(���� ���丮)�� ".."(���� ���丮)�� ����
//                if (lstrcmp(findData.cFileName, L".") != 0 && lstrcmp(findData.cFileName, L"..") != 0)
//                {
//                    // ���� ������ ��� ����
//                    wstring subFolderPath = (wstring)folderPath + L"\\" + findData.cFileName;
//                    
//                    // ��� ȣ��� ���� ���丮 �˻�
//                    //ReadImgPath(subFolderPath.c_str(), pGraphicDev); �����ʿ�
//
//                }
//            }
//            else
//            {
//                // ������ ���, �̹��� �������� Ȯ���ϰ� �ε�
//                wstring filePath = (wstring)folderPath + L"\\" + findData.cFileName;
//
//                // ���� Ȯ���� Ȯ��
//                if (wcsstr(findData.cFileName, L".png") || wcsstr(findData.cFileName, L".jpg") ||
//                    wcsstr(findData.cFileName, L".bmp") || wcsstr(findData.cFileName, L".tga"))
//                {
//                    IDirect3DBaseTexture9* pTexture = nullptr;
//                    if (SUCCEEDED(D3DXCreateTextureFromFile(Engine::Get_GraphicDev(), filePath.c_str(), (LPDIRECT3DTEXTURE9*)&pTexture)))
//                    {
//                        // ���� Ű�� szKey
//                        const _tchar* szKey = findData.cFileName;
//                        CMyTexture* pMyTexture = nullptr;
//
//                        // Ű���� �Ѱ� ������ ã�ƺ���.
//                        if (!FindUI(szKey)) // ���� ���
//                        {
//                            // szKey�� �����Ͽ� �������� �Ҵ�
//                            _tchar* copiedKey = new _tchar[_tcslen(szKey) + 1];
//                            _tcscpy_s(copiedKey, _tcslen(szKey) + 1, szKey);
//
//                            // ���纻 ����
//                            Engine::Ready_Proto(L"Proto_UITex", CUITex::Create(pGraphicDev));
//                            Engine::Ready_Proto(L"Proto_BaseUI", CTexture::Create(pGraphicDev, TEX_NORMAL, L"../Bin/Resource/Texture/UI/all wheels.png"));
//
//                            // ù��° ���� ���� ��ο���, �ι�° ���ڰ� ���ڿ��� ã���ش�.
//                            if (wcsstr(findData.cFileName, L"HP")) // ã�� ���
//                            {
//                                // ã�� �༮�� Ÿ������ Create
//                                pMyUI = CMyUI::Create(pGraphicDev, pTexture, UI_TYPE::HP);
//
//                            }
//                            else
//                            {
//                                // BASIC Ÿ���� �ؽ�ó Create
//                                pMyUI = CMyTexture::Create(pGraphicDev, pTexture, UI_TYPE::BASIC);
//                            }
//
//
//                            // �������� �Ҵ��� copiedKey�� �ʿ� �־�� �մϴ�.
//                            m_mapLoadUI1.emplace(copiedKey, pMyUI);
//                        }
//
//
//                    }
//                }
//            }
//        } while (FindNextFile(hFind, &findData)); // ����
//
//        FindClose(hFind); // ���� �ڵ��� ����
//
//    }
//
//    return S_OK;
//}

// dat���Ϸ� ���� �����ϱ�
//HRESULT CImguiAnimationTool::SaveData(const _tchar* mapTag)
//{
//    // dat���� �̸�
//    wstring m_strText = L"UIData.dat";
//
//    // ������ �༮�� ������ ���� ����
//    OPENFILENAME    open;
//
//    // ������ �༮�� ���
//    TCHAR   lpstrFile[MAX_PATH] = L"";
//
//    // ������ �༮�� Ÿ��
//    static TCHAR filter[] = L"*.dat";
//
//    // ������ �༮�� ������ ����ش�.
//    ZeroMemory(&open, sizeof(OPENFILENAME));
//    open.lStructSize = sizeof(OPENFILENAME);
//    open.lpstrFilter = filter;
//    open.lpstrFile = lpstrFile;
//    open.nMaxFile = 2562;
//    open.lpstrInitialDir = L"";
//
//    // ������ ����� ���Ե� ������ ����ȭ�� ��θ� �˻�
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
//        MSG_BOX("���� �Ϸ�");
//        return S_OK;
//    }
//}

// ��θ��� ���� ���� �������ش�.
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
//        MSG_BOX("op ���̴�~");
//    }
//
//    fclose(op);
//}

//// ã��
//CMyTexture* CImguiAnimationTool::FindUI(const _tchar* szKey)
//{
//    auto iter = find_if(m_mapLoadUI1.begin(), m_mapLoadUI1.end(), CTag_Finder(szKey));
//
//    if (iter == m_mapLoadUI1.end())
//        return nullptr;
//
//    return iter->second;
//}

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

// ������Ʈ ���� �� ����
/*
void CImguiAnimationTool::ObjectSetting()
{
    // ��ü ���� ����
    const char* Object[] = { u8"Human" ,u8"Quadrupedal",u8"Item" };

    // ��ü ������ ������ �޺� �ڽ� [������ ������ ���� �ε���]
    ImGui::Combo(u8"Object", &m_iObjectCreatIndex, Object, IM_ARRAYSIZE(Object));

    // �ҷ����� ��ư
    if (ImGui::Button(u8"Load")) 
    {
        
        switch (m_iObjectCreatIndex)
        {
            case 1: // �ָ�
            {       
                if (Engine::CManagement::GetInstance()->Get_GameObject(L"GameLogic", L"Player").empty())
                {
                    // ����
                    Engine::CGameObject* pGameObject = nullptr; // ������Ʈ�� ���� �׸�
                    pGameObject = CPlayerAnimation::Create(m_pGraphicDev); // ex : CHumanDemo Ŭ���� Create(��ġ); �� ����
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::NONE, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_NONE ,pGameObject }); // �� �����̳ʿ� Ű��, ������Ʈ �߰�
                }
                else {
                    //MSG_BOX("�̹� �����մϴ�");
                }
                break;
            }
            case 2: // ����
            {
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(HUMAN_DEMO).empty()) 
                {
                    Engine::CGameObject* pGameObject = nullptr;
                    pGameObject = CHumanDemo::Create(m_pGraphicDev);
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::RIGHT_OBJECT, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_GUN ,pGameObject }); // �� �����̳ʿ� Ű��, ������Ʈ �߰�
                }
                else {
                    //MSG_BOX("�̹� �����մϴ�");
                }
                break;
            }
            case 3: // �轼 �����
            {
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).empty()) 
                {
                    LoadObjectData();

                    _matrix test;
                    D3DXMatrixIdentity(&test);

                    Engine::CGameObject* pGameObject = nullptr;
                    pGameObject = CItemDemo::Create(m_pGraphicDev, ITEM_USE);
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::TWO_OBJECT, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_THOMPSON ,pGameObject }); // �� �����̳ʿ� Ű��, ������Ʈ �߰�

                }
                else {
                    //MSG_BOX("�̹� �����մϴ�");
                }
                break;
            }
            case 4: // ��������
            {

                break;
            }
            case 5: // ���ֺ�
            {

                break;
            }
            case 6: // ��������
            {

                break;
            }
        }

    }

    // ���� ��ư
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
            // �Ŵ�����Ʈ���� �� ����� ���� ��, ������Ʈ �� �����̳ʿ� ���ϴ� �ε��� ��° �༮�� ����
            Engine::CManagement::GetInstance()->Get_GameLogic_Objects(selectedObjType).front()->Set_Dead();
            m_mapObject.erase((OBJECTKEY)m_iObjectCreatIndex);
        }
        else {
            //MSG_BOX("�ƴϾ�");
        }

    }

    // ?    ���ӷ��� ������Ʈ�� Ÿ��.size �� ������� ���� ���
    //if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).size() != 0) 
    {
        /*ImGui::InputFloat(u8"ScaleX", &m_ItemTotal->vScale.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleY", &m_ItemTotal->vScale.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleZ", &m_ItemTotal->vScale.z, 1.0f, 100.f, "%.1f");*/
        //ImGui::InputFloat3("Scale", m_AniInfo.m_vScale); // ũ�� ����
        /*
        ImGui::InputFloat(u8"RotX", &m_ItemTotal->vStartRotation.x, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotY", &m_ItemTotal->vStartRotation.y, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotZ", &m_ItemTotal->vStartRotation.z, 5.0f, 100.f, "%.1f");
        */
        //ImGui::InputFloat3("Rot", m_AniInfo.m_vRot); // ȸ�� ����

        /*ImGui::InputFloat(u8"PosX", &m_ItemTotal->vStartPosition.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosY", &m_ItemTotal->vStartPosition.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosZ", &m_ItemTotal->vStartPosition.z, 1.0f, 100.f, "%.1f");*/
/*
        ImGui::InputFloat3("Pos", m_AniInfo.m_vPos); // ��ġ ����


        //const char* Parts3[] = { u8"Total" ,u8"Head",u8"Body",u8"LeftArm",u8"RightArm",u8"LeftLeg",u8"RightLeg" };
        //static int PartsIndex3 = 0;
        //ImGui::Combo(u8"Parts", &PartsIndex3, Parts3, IM_ARRAYSIZE(Parts3));
        //m_AniInfo.m_eObjectType = (PARTS)PartsIndex3;

        // ��ü ���� ����
        if (ImGui::Button(u8"InfoSave")) {
            SaveObjectInformationData();
        }

        // ��ü ���� �ҷ�����
        if (ImGui::Button(u8"InfoLoad")) {

            LoadObjectInforamtionData();
        }
    }
}
*/

// ������ ����
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
        
        // (������ ����, ������ ���� ����, ������, ũ��, nullptr);
        WriteFile(hFile, &m_AniInfo, sizeof(MYANIMATIONINFO), &dwByte, nullptr);

        CloseHandle(hFile);
    }
}
*/

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

    GetModuleFileName(NULL, lpstrFile, MAX_PATH);
    //C:\Users\������\Desktop\SR_TeamMine\Client\Bin\Client.exe
    PathRemoveFileSpec(lpstrFile);
    //C:\Users\������\Desktop\SR_TeamMine\Client\Bin
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    lstrcat(lpstrFile, L"\\Data\\ObjectTotalInfo\\data");
    //C:\Users\������\Desktop\SR_TeamMine\Client\Bin\Data\Animation

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

// Ÿ�Ӷ��� ��
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

        // Ű������ �̸� ����
        ImGui::Text(u8"Ű������ %d", i + 1);
        ImGui::SameLine();
        ImGui::InputText((u8"##������ �̸� : " + std::to_string(i)).c_str(), keyframe.name, 64);

        // Ű������ ���� ����
        ImGui::SameLine();
        ImGui::ColorEdit3(("##������ ���� : " + std::to_string(i)).c_str(), keyframe.color);
        keyframe.color[3] = 1.0f; // ���� ���� �׻� 1�� ����

        // ���콺 Ŭ������ Ű������ �̵�
        if (ImGui::IsMouseHoveringRect(
            ImVec2(xPos - 5.0f, yPos - 5.0f),
            ImVec2(xPos + 5.0f, yPos + 5.0f)))
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

    if (ImGui::IsMouseReleased(0)) 
    {
        isDraggingTimeline = false;
        isDraggingKeyframe = false;
    }

#pragma region ��ư (�߰�, ����, ����, �ε�)

    // Ű������ �߰� ��ư
    if (ImGui::Button("Add Keyframe")) {
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

    // Ű������ ���� ��ư
    if (ImGui::Button("Delete Keyframe")) {
        if (!timeline.empty()) {
            timeline.pop_back();
        }
    }

    ImGui::SameLine();

    // �ִϸ��̼� ���� �� �ε� ��ư
    if (ImGui::Button("Save Animation")) {
        SaveAnimationToFile("animation.txt");
    }

    ImGui::SameLine();

    if (ImGui::Button("Load Animation")) {
        LoadAnimationFromFile("animation.txt");
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
    ImGui::PlotLines("Value Curve", values, 100, 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(0, 0));

    delete[] values;

#pragma endregion

    //ImGui::Separator();

#pragma region �ִϸ��̼� �Ӽ� ����

    // �ִϸ��̼� �Ӽ� ����
    if (!timeline.empty()) // ������� Ȯ��
    {
        // Ÿ�Ӷ��� ��
        ImGui::Begin(u8"�ִϸ��̼� �Ӽ� ����");

        Keyframe& selectedKeyframe = timeline.back();
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

        // Ű������ Ÿ�Կ� ���� �߰� ����
        if (selectedKeyframe.type == 0) { // ũ�� �ִϸ��̼�
            ImGui::SliderFloat(u8"ũ��", &selectedKeyframe.size, 0.1f, 2.0f);
        }
        else if (selectedKeyframe.type == 1) { // ȸ�� �ִϸ��̼�
            ImGui::SliderFloat(u8"ȸ��", &selectedKeyframe.rotation, 0.0f, 360.0f);
        }
        else if (selectedKeyframe.type == 2) { // �̵� �ִϸ��̼�
            ImGui::SliderFloat2(u8"�̵�", selectedKeyframe.translation, -100.0f, 100.0f);
        }

        ImGui::End();
    }

    // �ִϸ��̼� ������ ���� �Է�
    ImGui::InputInt(u8"�ִϸ��̼� ������", &animationFrame);

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
#pragma endregion
}

// �ִϸ��̼� ����
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

// �ִϸ��̼� �ҷ�����
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
}

// ���� ���� �Լ�
float CImguiAnimationTool::Lerp(float a, float b, float t) 
{
    return a + t * (b - a);
}