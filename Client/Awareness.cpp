#include "stdafx.h"
#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
	: CGameObject(m_pGraphicDev)
{
}

CAwareness::~CAwareness()
{
}

CAwareness* CAwareness::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()) )
    {
        Safe_Release(pInstance);

        MSG_BOX("Effect - Monster - Awareness Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CAwareness::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Effect", L"Awareness"), E_FAIL);

    return S_OK;
}

HRESULT CAwareness::Ready_GameObject()
{
	return E_NOTIMPL;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
	return _int();
}

void CAwareness::LateUpdate_GameObject()
{
}

void CAwareness::Render_GameObject()
{
}

void CAwareness::Free()
{
    SUPER::Free();
}
