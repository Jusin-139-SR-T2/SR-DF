#include "stdafx.h"
//#include "Bullet.h"
//
//CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
//    : Base(pGraphicDev)
//{
//}
//
//CBullet::CBullet(const CBullet& rhs)
//    : Base(rhs)
//{
//}
//
//CBullet::~CBullet()
//{
//}
//
//HRESULT CBullet::Ready_GameObject()
//{
//    // 오너 지정
//    //CGameObject::Set_Owner(m_pPlayer);
//
//
//    return E_NOTIMPL;
//}
//
//_int CBullet::Update_GameObject(const _float& fTimeDelta)
//{
//    return _int();
//}
//
//void CBullet::LateUpdate_GameObject()
//{
//}
//
//void CBullet::Render_GameObject()
//{
//    // 행렬 적용
//    //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
//
//    // 랜더 상태 옵션
//    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//}
//
//CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos, CGameObject* Onwer)
//{
//    ThisClass* pInstance = new ThisClass(pGraphicDev);
//
//    if (FAILED(pInstance->Ready_GameObject()))
//    {
//        Safe_Release(pInstance);
//
//        MSG_BOX("Bullet Create Failed");
//        return nullptr;
//    }
//
//    return pInstance;
//}