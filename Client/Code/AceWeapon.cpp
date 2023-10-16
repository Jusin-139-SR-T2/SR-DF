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

    return S_OK;
}

_int CAceWeapon::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    // 빌보드 
    if(CAceWeapon::WEAPON_NAME::WALLPIPE != m_pCurName) //벽에 박혀있는애라 빌보드 x
    BillBoard(fTimeDelta);

    // 변수에 저장된 enum과 hp로 texture 결정 
    Change_Texture(m_pCurName);

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
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_ObjectTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

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
    _matrix		matWorld;
    _vec3       vPlayerPos;

    matWorld = *m_pTransformComp->Get_Transform();

    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // 회전행렬 생성
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    return S_OK;
}

void CAceWeapon::WeaponName(const _tchar* pObjTag)
{
    if ((wcscmp(pObjTag, L"BOTTLE") == 0) || (wcscmp(pObjTag, L"Bottle") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::BOTTLE;
    else if ((wcscmp(pObjTag, L"PIPE") == 0) || (wcscmp(pObjTag, L"Pipe") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::PIPE;
    else if ((wcscmp(pObjTag, L"WALLPIPE") == 0) || (wcscmp(pObjTag, L"WallPipe") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::WALLPIPE;
    else if ((wcscmp(pObjTag, L"FRYINGPAN") == 0) || (wcscmp(pObjTag, L"FryingPan") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::FRYINGPAN;
    else if ((wcscmp(pObjTag, L"PISTOL") == 0) || (wcscmp(pObjTag, L"Pistol") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::PISTOL;
    else if ((wcscmp(pObjTag, L"TOMMYGUN") == 0) || (wcscmp(pObjTag, L"TommyGun") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::TOMMYGUN;
    else if ((wcscmp(pObjTag, L"GASCANISTER") == 0) || (wcscmp(pObjTag, L"GasCanister") == 0))
        m_pCurName = CAceWeapon::WEAPON_NAME::GASCANISTER;
    else if ((wcscmp(pObjTag, L"BROKENFRYINGPAN") == 0) || (wcscmp(pObjTag, L"BrokenFryingPan") == 0))
    {
        m_fHp = m_fBrokenHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::FRYINGPAN;
    }
    else if ((wcscmp(pObjTag, L"BROKENTOMMYGUN") == 0) || (wcscmp(pObjTag, L"BrokenTommyGun") == 0))
    {
        m_fHp = m_fBrokenHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::TOMMYGUN;
    }
    else if ((wcscmp(pObjTag, L"CRACKEDBOTTLE") == 0) || (wcscmp(pObjTag, L"CrackedBottle") == 0))
    {
        m_fHp = m_fCrackedHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::BOTTLE;
    }
    else if ((wcscmp(pObjTag, L"BROKENBOTTLE") == 0) || (wcscmp(pObjTag, L"BrokenBottle") == 0))
    {
        m_fHp = m_fBrokenHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::BOTTLE;
    }
    else if ((wcscmp(pObjTag, L"BROKENPISTOL") == 0) || (wcscmp(pObjTag, L"BrokenPistol") == 0))
    {
        m_fHp = m_fBrokenHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::PISTOL;
    }
    else if ((wcscmp(pObjTag, L"BROKENPIPE") == 0) || (wcscmp(pObjTag, L"BrokenPipe") == 0))
    {
        m_fHp = m_fBrokenHp;
        m_pCurName = CAceWeapon::WEAPON_NAME::PIPE;
    }
    else
    {
        m_pCurName = CAceWeapon::WEAPON_NAME::WEAPON_END;
    }
}

void CAceWeapon::Change_Texture(WEAPON_NAME eReceiveName)
{
    switch (eReceiveName)
    {
    case CAceWeapon::WEAPON_NAME::BOTTLE:

        if (m_fMaxHP == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 0.25f, 1.f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"Bottle");
            break;
        }
        else if (m_fCrackedHp == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 0.25f, 0.25f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"CrackedBottle");
            break;
        }
        else
        {
            m_pTransformComp->Set_Scale({ 0.25f, 0.25f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenBottle");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::PIPE:
        if (m_fMaxHP == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 1.f, 0.25f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"LeadPipe");
            break;
        }
        else if (m_fBrokenHp == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 1.f, 0.25f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenPipe");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::WALLPIPE:
        if (m_fMaxHP == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 0.25f, 1.f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"WallPipe");
            break;
        }
        else if (m_fBrokenHp == m_fHp)
        {
            m_pTransformComp->Set_Scale({ 1.f, 0.25f, 1.f });
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenPipe");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::FRYINGPAN:
        if (m_fMaxHP == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"FryingPan");
            break;
        }
        else if (m_fBrokenHp == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenFryingPan");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::PISTOL:
        if (m_fMaxHP == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"Pistol");
            break;
        }
        else if (m_fBrokenHp == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenPistol");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::TOMMYGUN:
        if (m_fMaxHP == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"TommyGun");
            break;
        }
        else if (m_fBrokenHp == m_fHp)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"BrokenTommyGun");
            break;
        }
        break;

    case CAceWeapon::WEAPON_NAME::GASCANISTER:
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Weapon", L"GasCanister");
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

