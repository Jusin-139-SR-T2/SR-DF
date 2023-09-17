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

    Height_On_Terrain();


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
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // 몬스터 텍스처
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

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

void CMonster::Monster_Capture()
{
    /*몬스터위치, 플레이어 위치 가져옴
     
    CHECK - 1차적으로 둘 사이 거리 몬스터 기준 몇 이상이내여야함  - 여기서 거르기 


    A : 몬스터 바라보는 방향벡터
    B : 몬스터가 플레이어 바라보는 벡터 만들기

    A~B 사이각도 재기 = 내적 = angle


    angle이 대충 30도 ? 45도?  사이이면 감지하고 상태머신 변화 돌입 
    */
}

void CMonster::Free()
{
    SUPER::Free();
}

void CMonster::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}