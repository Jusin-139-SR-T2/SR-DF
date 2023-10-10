#include "stdafx.h"
#include "AceBuilding.h"

CAceBuilding::CAceBuilding(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CAceBuilding::CAceBuilding(const CAceBuilding& rhs)
    : CGameObject(rhs)
{
}

CAceBuilding::~CAceBuilding()
{
}

CAceBuilding* CAceBuilding::Create(LPDIRECT3DDEVICE9 pGraphicDev, 
                                    const _tchar* pObjTag, 
                                    const _float _fx, const _float _fy, const _float _fz, 
                                    CAceObjectFactory::OBJECT_DIRECTION pDirection)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("FoodObject Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz });
    pInstance->Set_BuildingDir(pDirection);
    pInstance->ObjectName(pObjTag); // 처음 들어온값을 enum 으로 변경 
    return pInstance;
}

HRESULT CAceBuilding::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

_int CAceBuilding::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceBuilding::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceBuilding::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pCubeBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CAceBuilding::Add_Component()
{
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BuildingTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCubeBufferComp = Set_DefaultComponent_FromProto<CCubeBufferComp>(ID_STATIC, L"Com_CubeTex", L"Proto_CubeBufferComp"), E_FAIL);

    return S_OK;
}

void CAceBuilding::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + m_fHeight, vPos.z);
}

void CAceBuilding::ObjectName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"A") == 0) || (wcscmp(pObjTag, L"a") == 0))
    {
        m_fHeight = 3.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::A;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"A");
        m_pTransformComp->Set_Scale({ 3.f, m_fHeight, 3.f });
    }
    else if ((wcscmp(pObjTag, L"B") == 0) || (wcscmp(pObjTag, L"b") == 0))
    {
        m_fHeight = 5.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::B;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"B");
        m_pTransformComp->Set_Scale({ 5.f, m_fHeight, 5.f });
    }
    else if ((wcscmp(pObjTag, L"C") == 0) || (wcscmp(pObjTag, L"c") == 0))
    {
        m_fHeight = 7.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::C;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"C");
        m_pTransformComp->Set_Scale({ 4.f, m_fHeight, 4.f });
    }
    else if ((wcscmp(pObjTag, L"D") == 0) || (wcscmp(pObjTag, L"d") == 0))
    {
        m_fHeight = 6.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::D;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"D");
        m_pTransformComp->Set_Scale({ 5.f, m_fHeight, 5.f });
    }
    else if ((wcscmp(pObjTag, L"E") == 0) || (wcscmp(pObjTag, L"e") == 0))
    {
        m_fHeight = 6.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::E;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"E");
        m_pTransformComp->Set_Scale({ 5.f, m_fHeight, 5.f });
    }
    else if ((wcscmp(pObjTag, L"F") == 0) || (wcscmp(pObjTag, L"f") == 0))
    {
        m_fHeight = 8.f;
        m_pCurName = CAceBuilding::BUILDING_NAME::F;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Building", L"F");
        m_pTransformComp->Set_Scale({ 3.f, m_fHeight, 11.f });
    }
}

HRESULT CAceBuilding::Set_BuildingDir(CAceObjectFactory::OBJECT_DIRECTION _eDir)
{
    switch (_eDir)
    {
    case CAceObjectFactory::OBJECT_DIRECTION::EAST:
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::SOUTHEAST:
        m_pTransformComp->Set_RotationY(D3DX_PI / 4);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::SOUTH:
        m_pTransformComp->Set_RotationY(D3DX_PI / 2);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::SOUTHWEST:
        m_pTransformComp->Set_RotationY(D3DX_PI / 4 * 3);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::WEST:
        m_pTransformComp->Set_RotationY(D3DX_PI);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::NORTHWEST:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 4 * 3);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::NORTH:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 2);
        break;
    case CAceObjectFactory::OBJECT_DIRECTION::NORTHEAST:
        m_pTransformComp->Set_RotationY(-D3DX_PI / 4);
        break;
    }

    return S_OK;
}


void CAceBuilding::Free()
{
    SUPER::Free();
}