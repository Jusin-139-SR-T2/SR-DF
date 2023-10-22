#include "stdafx.h"
#include "AceWeapon.h"

CAceWeapon::CAceWeapon(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CAceWeapon::CAceWeapon(const CAceWeapon& rhs)
    : Base(rhs)
{
}

CAceWeapon::~CAceWeapon()
{
}

CAceWeapon* CAceWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, const _float _fx, const _float _fy, const _float _fz)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("WeaponObject Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz }); //초기위치 여기서 잡아줌 
    pInstance->WeaponName(pObjTag);
    return pInstance;
}

CAceWeapon* CAceWeapon::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("WeaponObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CAceWeapon::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CAceWeapon::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(tObjectSerial.vRotation);
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;

    m_eFactoryClass = OBJECT_CLASS::WEAPON;
    return S_OK;
}

_int CAceWeapon::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    BillBoard(fTimeDelta);

    // 변수에 저장된 enum과 hp로 texture 결정 
    Change_Texture(m_pCurName);

    // 물리바디 업데이트
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform());

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceWeapon::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceWeapon::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture();
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceWeapon::Free()
{
    SUPER::Free();
}

//------------------------------------------------------------------
HRESULT CAceWeapon::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // 충돌 컴포넌트 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_ITEM); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    return S_OK;
}

void CAceWeapon::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 0.6f, vPos.z);
}

HRESULT CAceWeapon::BillBoard(const _float& fTimeDelta)
{
    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);

    return S_OK;
}

void CAceWeapon::WeaponName(const _tchar* pObjTag)
{
        if ((wcscmp(pObjTag, L"BOTTLE") == 0) || (wcscmp(pObjTag, L"Bottle") == 0))
        m_pCurName = CPlayer::OBJECT_NAME::BEERBOTLE;
    else if ((wcscmp(pObjTag, L"PIPE") == 0) || (wcscmp(pObjTag, L"Pipe") == 0))
        m_pCurName = CPlayer::OBJECT_NAME::STEELPIPE;
    else if ((wcscmp(pObjTag, L"FRYINGPAN") == 0) || (wcscmp(pObjTag, L"FryingPan") == 0))
        m_pCurName = CPlayer::OBJECT_NAME::FRYINGPAN;
    else if ((wcscmp(pObjTag, L"PISTOL") == 0) || (wcscmp(pObjTag, L"Pistol") == 0))
        m_pCurName = CPlayer::OBJECT_NAME::GUN;
    else if ((wcscmp(pObjTag, L"TOMMYGUN") == 0) || (wcscmp(pObjTag, L"TommyGun") == 0))
        m_pCurName = CPlayer::OBJECT_NAME::THOMPSON;

}

void CAceWeapon::Change_Texture(CPlayer::OBJECT_NAME eReceiveName)
{
    switch (eReceiveName)
    {
    case CPlayer::OBJECT_NAME::NONE:
        break;

    case CPlayer::OBJECT_NAME::GUN:
        if (m_gHp.Cur > 0)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"Pistol");
            break;
        }
        else if (m_gHp.Cur <= 0)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenPistol");
            break;
        }
        break;
    
    case CPlayer::OBJECT_NAME::THOMPSON:
        if (m_gHp.Cur > 0)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"TommyGun");
            break;
        }
        else if (m_gHp.Cur <= 0)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenTommyGun");
            break;
        }
        break;

    case CPlayer::OBJECT_NAME::STEELPIPE:
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"LeadPipe");
        break;

    case CPlayer::OBJECT_NAME::BEERBOTLE:
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"Bottle");
        break;

    case CPlayer::OBJECT_NAME::FRYINGPAN:

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"FryingPan");
        break;
    }
}

void CAceWeapon::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceWeapon::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceWeapon::OnCollisionExited(CGameObject* pDst)
{
}

//------------------------------------------------------------------

