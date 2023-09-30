#include "ImguiWin_TextureTool.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <ImguiMgr.h>

CImguiWin_TextureTool::CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool::~CImguiWin_TextureTool()
{
}

CImguiWin_TextureTool* CImguiWin_TextureTool::Create()
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

void CImguiWin_TextureTool::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_TextureTool::Ready_ImguiWin()
{


    return S_OK;
}

_int CImguiWin_TextureTool::Update_ImguiWin(const _float& fTimeDelta)
{
    SUPER::Update_ImguiWin(fTimeDelta);

    ImGui::Begin(u8"텍스처 툴");


    ImGui::End();

    return 0;
}
