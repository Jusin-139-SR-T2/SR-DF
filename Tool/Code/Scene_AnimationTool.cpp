#include "Scene_AnimationTool.h"

CScene_AnimationTool::CScene_AnimationTool(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CScene(pGraphicDev)
{
}

CScene_AnimationTool::~CScene_AnimationTool()
{
}

CScene_AnimationTool* CScene_AnimationTool::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

void CScene_AnimationTool::Free()
{
    SUPER::Free();
}

HRESULT CScene_AnimationTool::Ready_Scene()
{
    return S_OK;
}

_int CScene_AnimationTool::Update_Scene(const _float& fTimeDelta)
{
    _int	iExit = SUPER::Update_Scene(fTimeDelta);

    return iExit;
}

void CScene_AnimationTool::LateUpdate_Scene()
{
    SUPER::LateUpdate_Scene();


}

void CScene_AnimationTool::Render_Scene()
{
    // µð¹ö±×

}

HRESULT CScene_AnimationTool::Ready_Layer_Completed()
{
    return S_OK;
}
