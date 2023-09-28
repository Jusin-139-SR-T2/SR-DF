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


        // ImGui���� �ؽ�ó�� ǥ���� �̹��� ũ�� (��: 200x200 �ȼ�)
        const ImVec2 imageSize(200.0f, 200.0f);

#pragma region �׽�Ʈ1

        static int counter = 0;

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::Button(u8"�̹��� �ε� ��ư (�ӽ�)")) // @@@@@ ���� 2. �̹��� �ε� �ð��� ���������̰� ���. (���� ���� ������ �� �����) @@@@@
        {
            LoadImg(L"../Client/Resource");
        }

#pragma region �� ����
        // ��ġ(Pos) ���� (�����̴�)
        //ImGui::Text(u8"��ġ ����");
        //ImGui::SliderFloat("x", &m_AniInfo.m_vInfo[INFO_POS].x, 0.0f, 300.0f);
        //// �� ��ư ����
        //ImGui::PushItemWidth(50.0f); // �����̴��� �ʺ� 100.0f�� ����
        //ImGui::SameLine(); // ���� ����
        //ImGui::InputFloat("x", &m_AniInfo.m_vInfo[INFO_POS].x, 1.f, 300.f);
        //ImGui::PopItemWidth(); // ��Ÿ�� ������ ������� ����

        //ImGui::SliderFloat("y", &m_AniInfo.m_vInfo[INFO_POS].y, 0.0f, 300.0f);
        //// �� ��ư ����
        //ImGui::PushItemWidth(50.0f); // �����̴��� �ʺ� 100.0f�� ����
        //ImGui::SameLine(); // ���� ����
        //ImGui::InputFloat("y", &m_AniInfo.m_vInfo[INFO_POS].y, 1.f, 300.f);
        //ImGui::PopItemWidth(); // ��Ÿ�� ������ ������� ����

        //ImGui::SliderFloat("z", &m_AniInfo.m_vInfo[INFO_POS].z, 0.0f, 300.0f);
        //// �� ��ư ����
        //ImGui::PushItemWidth(50.0f); // �����̴��� �ʺ� 100.0f�� ����
        //ImGui::SameLine(); // ���� ����
        //ImGui::InputFloat("z", &m_AniInfo.m_vInfo[INFO_POS].z, 1.f, 300.f);
        //ImGui::PopItemWidth(); // ��Ÿ�� ������ ������� ����

        // �� ���̾ƿ� ����
        ImGui::Columns(2); 
        //ImGui::NextColumn();

        
        ImGui::Columns(1);
        // �� ���̾ƿ��� ����

#pragma endregion

        if (ImGui::Button(u8"������ �߰� ��ư"))      // ������ �߰� ��ư
        m_vecAnimationInfo.push_back(m_AniInfo);    // �������� �ִϸ��̼� ���� ���Ϳ� �߰�

        ImGui::SameLine(); // ���� ����
        ImGui::Text(u8"������ ������ = Frame_%d", m_vecAnimationInfo.size());

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


#pragma endregion

#pragma region LisBox 2��
        //for (const auto& objKey : uniqueObj)
        //{
        //    for (const auto& stateKey : m_MapFile[objKey])
        //    {
        //        // �ؽ�ó�� ImGui �ؽ�ó�� ��ȯ
        //        LPDIRECT3DTEXTURE9 pTexture = m_MapTexture[std::make_pair(objKey, stateKey)];
        //        if (pTexture)
        //        {
        //            ImTextureID textureID = reinterpret_cast<ImTextureID>(pTexture);
        //            items[itemIndex] = stateKey.c_str();
        //            if (ImGui::ListBox(items[itemIndex], &selectedItemIndex, itemIndex))
        //            {
        //                // ListBox���� �������� �������� �� ������ �۾� �߰�
        //                // selectedTextureIndex���� ������ �������� �ε����� ���Ե˴ϴ�.
        //            }
        //            itemIndex++;
        //        }
        //    }
        //}
#pragma endregion

        ImGui::InputText("FrameName", "FrameName", IM_ARRAYSIZE("EEEEEEEEEEEE"));

        if (ImGui::Button(u8"���� ��ư"))
        {

        }

        if (ImGui::Button(u8"�ҷ����� ��ư"))
        {

        }

#pragma endregion

#pragma region �׽�Ʈ2
        ImGui::Text(u8"�׽�Ʈ %d", 123);
        if (ImGui::Button(u8"����"))
        {
            // do stuff
        }
        float fSlider2 = 0.f;

        ImGui::InputText("string", "Hi", IM_ARRAYSIZE("Hi"));
        ImGui::SliderFloat("float", &fSlider2, 0.0f, 1.0f);
#pragma endregion


#pragma region �׽�Ʈ3
        _bool   my_tool_active;
        // Create a window called "My First Tool", with a menu bar.

        // �׽�Ʈ �� �� (��ü)
        ImGui::Begin(u8"�׽�Ʈ ��3", &my_tool_active, ImGuiWindowFlags_MenuBar);
        if (ImGui::BeginMenuBar()) // �޴��� ���
        {
            // �޴��ٸ� ������ �� ���
            if (ImGui::BeginMenu(u8"����")) // �޴��� 1�� @@@@@ ���� 1. ������ ������ �ҷ�����, ����, �ݱⰡ �� ����� @@@@@
            {
                // ��ư�� ��� ����
                if (ImGui::MenuItem(u8"�ҷ�����", "Ctrl+O")) { /* Do stuff */ }
                {
                   
                    // �ҷ����� �ۼ�
                    // Open �Լ��� ���� �̹��� �ҷ�����
                    //std::string imagePath = OpenImageFileDialog();
                }
                if (ImGui::MenuItem(u8"����", "Ctrl+S")) { /* Do stuff */ }
                {
                    // ���� �ۼ�
                }
                if (ImGui::MenuItem(u8"�ݱ�", "Ctrl+W")) { my_tool_active = false; }
                {
                    // �ݱ�
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(u8"�׽�Ʈ �޴��� �߰�")) // �޴��� 2��
            {
                // ��ư�� ��� ����
                if (ImGui::MenuItem(u8"�׽�Ʈ ��ư1", u8"����Ű1")) { /* Do stuff */ }
                {
                    // �ҷ����� �ۼ�
                }
                if (ImGui::MenuItem(u8"�׽�Ʈ ��ư2", u8"����Ű2")) { /* Do stuff */ }
                {
                    // ���� �ۼ�
                }
                if (ImGui::MenuItem(u8"�׽�Ʈ ��ư3", u8"����Ű3")) { my_tool_active = false; }
                {
                    // �ݱ�
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        static float my_color[4] = { 0.4f, 0.7f, 0.0f, 0.5f };

        // Edit a color (stored as ~4 floats)
        ImGui::ColorEdit4(u8"����", my_color);

        // Plot some values
        const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
        ImGui::PlotLines(u8"������", my_values, IM_ARRAYSIZE(my_values));

        // Display contents in a scrolling region
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
        ImGui::BeginChild(u8"��ũ��");
        for (int n = 0; n < 50; n++)
            ImGui::Text(u8"%04d: �ؽ�Ʈ", n);
        ImGui::EndChild();
        ImGui::End();
#pragma endregion

#pragma region ��ex
        //if (ImGui::TreeNode(u8"UI ��"))
        //{

        //    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        //    if (ImGui::BeginTabBar("UI ��", tab_bar_flags))
        //    {
        //        if (ImGui::BeginTabItem(u8"UI_1"))
        //        {

        //            ImGui::SeparatorText(u8"�߰��� ������ (�ε�������)");
        //            {
        //                if (ImGui::Button(u8"�� ������ �߰�"))
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
        //                ImGui::BeginListBox(u8"�̹��� �ڽ�", ImVec2(800, 800));
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

        //                if (ImGui::Button(u8"�����ϱ�"))
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
        //                ImGui::BeginListBox(u8"������ �Է�", ImVec2(800, 800));
        //                {

        //                    ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
        //                    ImVec2 uv1 = ImVec2(1.0f, 1.0f);                          // UV coordinates for (32,32) in our texture
        //                    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
        //                    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        //                    ImGui::SameLine();
        //                    ImGui::Image(m_vecUIList[selected_UIIndex]->Get_Info()->pTexture, ImVec2(96.f, 96.f), uv0, uv1, tint_col, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        //                    ImGui::InputFloat3(u8"��ġ", m_vecUIList[selected_UIIndex]->Get_Info()->vPos);
        //                    ImGui::InputFloat3(u8"ũ��", m_vecUIList[selected_UIIndex]->Get_Info()->vSize);

        //                    if (ImGui::Button(u8"���� ���"))
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

    ImGui::Begin(u8"�ִϸ��̼� �� �׽�Ʈ");


    /*if (m_pTexture)
    {
        ImGui::Image((void*)m_pTexture, ImVec2(WINCX, WINCY));
        m_pTexture->Release();
        m_pTexture = nullptr;
    }*/

    ImGui::End();


    return 0;
}
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



// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@������� ������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@������� ������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@������� ������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@������� ������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@������� ������@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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
                /*
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(HUMAN_DEMO).empty()) {

                    // ����
                    Engine::CGameObject* pGameObject = nullptr; // ������Ʈ�� ���� �׸�
                    pGameObject = CHumanDemo::Create(m_pGraphicDev); // ex : CHumanDemo Ŭ���� Create(��ġ); �� ����
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::NONE, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_NONE ,pGameObject }); // �� �����̳ʿ� Ű��, ������Ʈ �߰�
                }
                else {
                    //MSG_BOX("�̹� �����մϴ�");
                }
                */
                break;
            }
            case 2: // ����
            {
                /*
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(HUMAN_DEMO).empty()) {

                    Engine::CGameObject* pGameObject = nullptr;
                    pGameObject = CHumanDemo::Create(m_pGraphicDev);
                    Engine::CManagement::GetInstance()->Add_GameLogic_Object(OBJECTTYPE::RIGHT_OBJECT, pGameObject);
                    m_mapObject.insert({ OBJECTKEY::KEY_GUN ,pGameObject }); // �� �����̳ʿ� Ű��, ������Ʈ �߰�
                }
                else {
                    //MSG_BOX("�̹� �����մϴ�");
                }
                */
                break;
            }
            case 3: // �轼 �����
            {
                    /*
                if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).empty()) {
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
                    */
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

        /*
        if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(selectedObjType).size() != 0)
        {
            // �Ŵ�����Ʈ���� �� ����� ���� ��, ������Ʈ �� �����̳ʿ� ���ϴ� �ε��� ��° �༮�� ����
            Engine::CManagement::GetInstance()->Get_GameLogic_Objects(selectedObjType).front()->Set_Dead();
            m_mapObject.erase((OBJECTKEY)m_iObjectCreatIndex);
        }
        else {
            //MSG_BOX("�ƴϾ�");
        }
        */

    }

    // ?    ���ӷ��� ������Ʈ�� Ÿ��.size �� ������� ���� ���
    //if (Engine::CManagement::GetInstance()->Get_GameLogic_Objects(WEAPON).size() != 0) 
    {
        /*ImGui::InputFloat(u8"ScaleX", &m_ItemTotal->vScale.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleY", &m_ItemTotal->vScale.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"ScaleZ", &m_ItemTotal->vScale.z, 1.0f, 100.f, "%.1f");*/
        ImGui::InputFloat3("Scale", m_AniInfo.m_vScale); // ũ�� ����

        /*ImGui::InputFloat(u8"RotX", &m_ItemTotal->vStartRotation.x, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotY", &m_ItemTotal->vStartRotation.y, 5.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"RotZ", &m_ItemTotal->vStartRotation.z, 5.0f, 100.f, "%.1f");*/
        ImGui::InputFloat3("Rot", m_AniInfo.m_vRot); // ȸ�� ����

        /*ImGui::InputFloat(u8"PosX", &m_ItemTotal->vStartPosition.x, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosY", &m_ItemTotal->vStartPosition.y, 1.0f, 100.f, "%.1f");
        ImGui::InputFloat(u8"PosZ", &m_ItemTotal->vStartPosition.z, 1.0f, 100.f, "%.1f");*/
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

    GetModuleFileName(NULL, lpstrFile, MAX_PATH);
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin\Client.exe
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    PathRemoveFileSpec(lpstrFile);
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin
    lstrcat(lpstrFile, L"\\Data\\ObjectTotalInfo");
    //C:\Users\\Desktop\SR_TeamMine\Client\Bin\Data\Animation
    /*basic_string<TCHAR> converted(m_strItemTotalText.begin(), m_strItemTotalText.end());
    const _tchar* aa = converted.c_str();*/

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
        
        WriteFile(hFile, &m_AniInfo, sizeof(MYANIMATIONINFO), &dwByte, nullptr);

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