#include "stdafx.h"
#include "Hut.h"

CHut::CHut(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CHut::CHut(const CHut& rhs)
	: CGameObject(rhs)
{
}

CHut::~CHut()
{
}

HRESULT CHut::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Pos({ 1.f, 1.f, 25.f });
//    m_pTransformComp->Set_Scale({ 10.f, 10.f, 1.f });
  //  m_pTransformComp->Rotate(ROT_Y, -90);

    return S_OK;
}

_int CHut::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    Height_On_Terrain();


    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CHut::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CHut::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pCubeBufferComp->Render_Buffer();
    //m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CHut* CHut::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Temporary Building Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CHut::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 5.f, vPos.z);
}

HRESULT CHut::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_Temporary_BuildingTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_BuildingTextureComp"), E_FAIL);
   
    //// ���� ���� ���
    //m_pColliderComp->EnterToPhysics(0);
    //// �浹 �Լ� ����
    //m_pColliderComp->Set_Collision_Event<CHut>(this, &CHut::OnCollision);
    //m_pColliderComp->Set_CollisionEntered_Event<CHut>(this, &CHut::OnCollisionEntered);
    //m_pColliderComp->Set_CollisionExited_Event<CHut>(this, &CHut::OnCollisionExited);
}

void CHut::Free()
{
    SUPER::Free();
}
