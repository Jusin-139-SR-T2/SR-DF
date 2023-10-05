#include "stdafx.h"
#include "Awareness.h"

CAwareness::CAwareness(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEsystem(pGraphicDev)
{
}

CAwareness::CAwareness(const CAwareness& rhs)
    : CEsystem(rhs)
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
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Effect_AwarenessTextureComp"), E_FAIL);

    return S_OK;
}

HRESULT CAwareness::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 가장먼저 지정하면서 Ready를 호출해야 모든 멤버변수 초기화가 진행된다. 
    CEsystem::Ready_GameObject(Engine::CEsystem::EFFECTTYPE::PER_TURN);

    // 텍스처 지정 
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Effect", L"Awareness");

    // 텍스처를 통해 FrameEnd값 지정가능 
    m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

	return S_OK;
}

_int CAwareness::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);    

    //빌보드 적용 
    Billboard();

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;

}

void CAwareness::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAwareness::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAwareness::Free()
{
    SUPER::Free();
}

//==============================================================

void CAwareness::Billboard()
{
    _matrix		matWorld, matView, matBill;

    matWorld = *m_pTransformComp->Get_Transform();

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matBill);

    matBill._11 = matView._11;
    matBill._13 = matView._13;
    matBill._31 = matView._31;
    matBill._33 = matView._33;

    D3DXMatrixInverse(&matBill, 0, &matBill);

    m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));
}