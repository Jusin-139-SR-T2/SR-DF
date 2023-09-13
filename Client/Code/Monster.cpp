#include "stdafx.h"
#include "Monster.h"

#include "Export_System.h"
#include "Export_Utility.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
    : Base(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    D3DXMatrixIdentity(&m_matRotAxis);
    m_pTransformComp->m_vScale.x = 0.5f;

    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 20.f };

    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    m_fFrame += 6.f * fTimeDelta;

    //if ( m_MonsterInfo.m_fCnt < m_fFrame)
    if (14 < m_fFrame)
        m_fFrame = 0.f;




    SUPER::Update_GameObject(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcTex>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransform>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculator>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // 몬스터 텍스처
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTexture>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

    return S_OK;
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    return pInstance;
}

void CMonster::state_check(MONSTERSTATE _CurrState , MONSTER_DIRECTION _Direction)
{
    m_MonsterInfo.state = _CurrState;
    m_MonsterInfo.dir = _Direction;

    switch (_CurrState)
    {
    case CMonster::IDLE_READY: //ROTATION 존재 
        m_MonsterInfo.m_fCnt = 4;
        break;

    case CMonster::WALK:
        if(SOUTH == _Direction )
            m_MonsterInfo.m_fCnt = 24;
       else if(SOUTH_WEST == _Direction ||NORTH_EAST == _Direction)
            m_MonsterInfo.m_fCnt = 24;
       else if(WEST == _Direction || EAST == _Direction)
            m_MonsterInfo.m_fCnt = 24; 
       else if(NORTH_WEST == _Direction || SOUTH_EAST == _Direction)
            m_MonsterInfo.m_fCnt = 16; 
       else if(NORTH == _Direction )
            m_MonsterInfo.m_fCnt = 32;
        break;

    case CMonster::RUN:
        if (SOUTH == _Direction)
            m_MonsterInfo.m_fCnt = 21;
        else if (SOUTH_WEST == _Direction || NORTH_EAST == _Direction)
            m_MonsterInfo.m_fCnt = 21;
        else if (WEST == _Direction || EAST == _Direction)
            m_MonsterInfo.m_fCnt = 19;
        else if (NORTH_WEST == _Direction || SOUTH_EAST == _Direction)
            m_MonsterInfo.m_fCnt = 18;
        else if (NORTH == _Direction)
            m_MonsterInfo.m_fCnt = 16;
        break;

    case CMonster::BASIC_ATTACK: //ROTATION 존재 
        m_MonsterInfo.m_fCnt = 5;
        break;

    case CMonster::HEAVY_ATTACK:
        m_MonsterInfo.m_fCnt = 5;
        break;
    case CMonster::DAZED:
        m_MonsterInfo.m_fCnt = 14;
        break;
    case CMonster::FACE_PUNCH:
        m_MonsterInfo.m_fCnt = 8;
        break;
    case CMonster::FALLING:
        m_MonsterInfo.m_fCnt = 12;
        break;
    case CMonster::GETUP:
        m_MonsterInfo.m_fCnt = 6;
        break;
    case CMonster::HIT:
        m_MonsterInfo.m_fCnt = 6;
        break;
    case CMonster::TAUNT:
        m_MonsterInfo.m_fCnt = 6;
        break;
    case CMonster::STRAFING:
        m_MonsterInfo.m_fCnt = 6;
        break;
    case CMonster::INCH:
        m_MonsterInfo.m_fCnt = 6;
        break;
    case CMonster::STANDOFF: //rotation있음 
        break;
    case CMonster::POOING:
        m_MonsterInfo.m_fCnt = 3;
        break;
    case CMonster::SHOCKED:
        m_MonsterInfo.m_fCnt = 5;
        break;
    case CMonster::CHOPPED:
        m_MonsterInfo.m_fCnt = 18;
        break;
    case CMonster::DEAD:
        m_MonsterInfo.m_fCnt = 5;
        break;
    case CMonster::THROW_DEAD:
        m_MonsterInfo.m_fCnt = 11;
        break;
    case CMonster::HEADLESS:
        m_MonsterInfo.m_fCnt = 16;
        break;
    case CMonster::MONSTERSTATE_END:
        break;

    default:
//        m_MonsterInfo.m_fCnt = 1;
        break;
    }
}

void CMonster::Monster_Eyerange()
{
    
}

void CMonster::Free()
{
    SUPER::Free();
}
