#include "stdafx.h"
//#include "Attack.h"
//
//CAttack::CAttack(LPDIRECT3DDEVICE9 pGraphicDev)
//    : Base(pGraphicDev)
//{
//}
//
//CAttack::CAttack(const CAttack& rhs)
//    : Base(rhs)
//{
//}
//
//CAttack::~CAttack()
//{
//}
//
//HRESULT CAttack::Ready_GameObject()
//{
//    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
//
//    return S_OK;
//}
//
//_int CAttack::Update_GameObject(const _float& fTimeDelta)
//{
//    SUPER::Update_GameObject(fTimeDelta);
//
//    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);
//
//    return S_OK;
//}
//
//void CAttack::LateUpdate_GameObject()
//{
//    SUPER::LateUpdate_GameObject();
//}
//
//void CAttack::Render_GameObject()
//{
//   
//}
//
//HRESULT CAttack::Add_Component()
//{
//    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
//    
//    return S_OK;
//}
//
//void CAttack::Free()
//{
//    SUPER::Free();
//}
//
