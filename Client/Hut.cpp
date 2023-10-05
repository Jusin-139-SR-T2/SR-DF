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

    m_pTransformComp->Set_Pos({ 35.f, 1.f, 25.f });
    m_fSize = 4.f;

    FAILED_CHECK_RETURN(Set_BuildingSize(m_fSize, BuildDirection::SOUTH), E_FAIL);

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

    m_pTransformComp->Set_Pos(vPos.x, fHeight + m_fSize, vPos.z);
}

HRESULT CHut::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BuildingTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);
   
    //// 물리 세계 등록
    //m_pColliderComp->EnterToPhysics(0);
    //// 충돌 함수 연결
    //m_pColliderComp->Set_Collision_Event<CHut>(this, &CHut::OnCollision);
    //m_pColliderComp->Set_CollisionEntered_Event<CHut>(this, &CHut::OnCollisionEntered);
    //m_pColliderComp->Set_CollisionExited_Event<CHut>(this, &CHut::OnCollisionExited);

    return S_OK;
}


void CHut::Free()
{
    SUPER::Free();
}

HRESULT CHut::Set_BuildingSize(_float _Size, BuildDirection _eDir)
{
    m_pTransformComp->Set_Scale({ _Size, _Size, _Size });
    
    switch (_eDir)
    {
    case CHut::BuildDirection::EAST:
        break;
    case CHut::BuildDirection::SOUTHEAST:
        m_pTransformComp->Set_RotationY(D3DX_PI / 4);
        break;

    case CHut::BuildDirection::SOUTH:
        m_pTransformComp->Set_RotationY(D3DX_PI / 2);
        break;

    case CHut::BuildDirection::SOUTHWEST:
        m_pTransformComp->Set_RotationY(D3DX_PI / 4 * 3);
        break;

    case CHut::BuildDirection::WEST:
        m_pTransformComp->Set_RotationY(D3DX_PI);
        break;

    case CHut::BuildDirection::NORTHWEST:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 4 * 3);
        break;

    case CHut::BuildDirection::NORTH:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 2);
        break;

    case CHut::BuildDirection::NORTHEAST:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 4);
        break;
    }

    return S_OK;
}
