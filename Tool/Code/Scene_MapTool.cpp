#include "Scene_MapTool.h"

CScene_MapTool::CScene_MapTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CScene_MapTool::~CScene_MapTool()
{

}

CScene_MapTool* CScene_MapTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_Scene()))
    {
        Safe_Release(pInstance);
        MSG_BOX("MapTool Scene Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CScene_MapTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_MapTool::Ready_Scene()
{
    return S_OK;
}

_int CScene_MapTool::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_MapTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();

}

void CScene_MapTool::Render_Scene()
{
    // µð¹ö±×

}

HRESULT CScene_MapTool::Ready_Layer_Completed()
{
    return S_OK;
}
