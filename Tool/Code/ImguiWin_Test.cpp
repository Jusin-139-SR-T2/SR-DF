#include "ImguiWin_Test.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

CImguiWin_Test::CImguiWin_Test()
{
}

CImguiWin_Test::~CImguiWin_Test()
{
}

CImguiWin_Test* CImguiWin_Test::Create()
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

void CImguiWin_Test::Free()
{
    SUPER::Free();
}

HRESULT CImguiWin_Test::Ready_ImguiWin()
{
    return S_OK;
}

_int CImguiWin_Test::Update_ImgWin(const _float& fTimeDelta)
{
    ImGui::Begin(u8"응애 머신");




    ImGui::End();

    return 0;
}
