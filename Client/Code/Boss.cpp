#include "stdafx.h"
#include "Boss.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBoss::CBoss(const CBoss& rhs)
	: Base(rhs)
{
}

CBoss::~CBoss()
{
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
    pInstance->vPatrolPointZero = { _x, _y, _z };

    return pInstance;
}


HRESULT CBoss::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;
    m_fCheck = 0;

    m_eCurrPhase = Phase_End;

    // 충돌용
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 0.7f, 0.3f };

    // INFO
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;

#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBoss::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBoss::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CBoss::AI_Reloading);
    m_tState_Obj.Add_Func(STATE_OBJ::BACKIDLE, &CBoss::AI_BackIdle);

    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBoss::AI_Chase);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEP_DISTANCE, &CBoss::AI_KeepDistance);
    m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CBoss::AI_Pre_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CBoss::AI_Side_Ready);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBoss::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBoss::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CBoss::AI_Roll);
    m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CBoss::AI_CloseAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CBoss::AI_Shooting);

    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBoss::AI_GoHome);
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CBoss::AI_GoHome);
    m_tState_Obj.Add_Func(STATE_OBJ::TRACK, &CBoss::AI_Track);

    //패턴
    m_tState_Obj.Add_Func(STATE_OBJ::PH1_LASER, &CBoss::AI_Ph1_Laser);


#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBoss::Idle);
    m_tState_Act.Add_Func(STATE_ACT::RUN, &CBoss::Run);
    m_tState_Act.Add_Func(STATE_ACT::APPROACHING, &CBoss::Approaching);
    m_tState_Act.Add_Func(STATE_ACT::ROLLING, &CBoss::Rolling);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBoss::GoHome);
    m_tState_Act.Add_Func(STATE_ACT::KEEPDISTANCE, &CBoss::KeepDistance);
    m_tState_Act.Add_Func(STATE_ACT::TRACK, &CBoss::Track);

    //패턴시작 
    m_tState_Act.Add_Func(STATE_ACT::PHASE1_LASER, &CBoss::Phase1_LaserOn);

#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // 멀때 

#pragma endregion
    return S_OK;
}

_int CBoss::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    if (0 == m_gHp.Cur)
        m_bDead = true;

    // 페이즈 체크용도 -> 제일먼저 체크하기위해 
    Phase_Check();

    // 위치값 가져오기 
    Get_PlayerPos(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    // 빌보드
    Billboard(fTimeDelta);

    //상태머신-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;
    m_fPatternAge += 1.f * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State() ||
            STATE_OBJ::RUN == m_tState_Obj.Get_State() ||
            STATE_OBJ::SHOOTING == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    if (m_fPatternAge > m_fPatternLifeTime)
    {
        TimerReset = TRUE;
        m_fPatternAge = 0.f;
    }

#pragma region 테스트 장소 
    if (Engine::IsKey_Pressing(DIK_N))
    {
        m_gHp.Cur = 75.f;
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
        m_gHp.Cur = 45.f;
        /*
        swprintf_s(debugString, L"BOSS - 변수 확인 m_tState_Obj = %d\n", m_tState_Obj);
        OutputDebugStringW(debugString);*/
    }
#pragma endregion 

    //물리 업데이트 코드
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

    // Renderer -----------------------------------
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region 기본 override 

void CBoss::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBoss::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBoss::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BossTextureComp"), E_FAIL);
    
    // 콜라이더 컴포넌트
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);

    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_BOSSMONSTER); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    return S_OK;
}

void CBoss::Free()
{
    SUPER::Free();
}

#pragma endregion

#pragma region 충돌 

void CBoss::OnCollision(CGameObject* pDst)
{

}
void CBoss::OnCollisionEntered(CGameObject* pDst)
{

}
void CBoss::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 

#pragma region 상태머신 부속파트 

HRESULT CBoss::Get_PlayerPos(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    return S_OK;
}

void CBoss::Billboard(const _float& fTimeDelta)
{
    // 몬스터가 플레이어 바라보는 벡터 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}

_bool CBoss::Detect_Player()
{
    _vec3 vMonsterLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    //몬스터가 플레이어 바라보는 벡터 
    _vec3 MonToPlayer = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
    _float currdistance = D3DXVec3Length(&MonToPlayer);

    //정규화
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

    // 시야범위 우선체크 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // 내적하여 RADIAN구하기 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // 시야범위 + 시야각도 = 포착 
    if (fradian < m_fMonsterFov * 2 && currdistance < m_fMonsterSightDistance)
        return true;
    else
        return false;
}

_float CBoss::Calc_Distance()
{
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

void CBoss::Phase_Check()
{
    _float CurHp = m_gHp.Cur;

    if (phase1HP >= CurHp && phase2HP <= CurHp) // 50~80 : Phase 1
        m_eCurrPhase = Phase1;
    else if (phase2HP >= CurHp) // 50이하 = phase 2
        m_eCurrPhase = Phase2;

    switch (m_eCurrPhase)
    {
    case Engine::Phase1: // 80퍼 이하 
        m_fRunSpeed = 3.f;
        break;

    case Engine::Phase2: // 50퍼 이하 - 자버프 획득 
        m_fRunSpeed = 5.f;
        break;

    case Engine::Phase_End: // 기본phase
        m_fRunSpeed = 7.f;
        break;
    default:
        break;
    }
}


void CBoss::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.2f, vPos.z);
}
#pragma endregion

#pragma region 보스패턴
// 1. 돌덩이 떨구기 - 플레이어 반경으로 
void CBoss::Falling_Stone_Around()
{
    OutputDebugString(L"▷BOSS - 보스패턴 1 : 돌덩이 떨구기    \n");
    _vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();
    m_pPlayerPos.y = 15.f; // 떨어지기 시작하는 높이 고정 

    _int MaxStone = 0; //돌 갯수 
    _float Radius = 0.f; // 플레이어 반경 반지름 위치 

    switch (m_eCurrPhase)
    {
    case Engine::Phase1:
        MaxStone = 5.f;
        Radius = 10.f;
        break;
    case Engine::Phase2:
        MaxStone = 7.f;
        Radius = 5.f;
        break;
    }

    for (int i = 0; i < MaxStone; ++i)
    {
        _vec3 randomCenter;
        GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, Radius);

        Engine::Add_GameObject(L"GameLogic", CFallingStone::Create(m_pGraphicDev,
            randomCenter.x, 
            randomCenter.y, 
            randomCenter.z));
    }
}

void CBoss::Pattern_Fire(_int number, _float radius)
{
    _int numPoints = number;
    _float Radius = radius;
    _vec3 Center = m_pTransformComp->Get_Pos();
    _vec3 FirePos;

    for (_int i = 0; i < numPoints; ++i)
    {
        _float angle = static_cast<_float>(i) / static_cast<_float>(numPoints) * D3DX_PI * 2.0f;
        FirePos.x = Center.x + Radius * cosf(angle);
        FirePos.y = Center.y;
        FirePos.z = Center.z + Radius * sinf(angle);

        Engine::Add_GameObject(L"GameLogic", CSpawnFire::Create(m_pGraphicDev,
            FirePos.x, FirePos.y, FirePos.z , m_eCurrPhase ));
    }
}

void CBoss::Pattern_EnergyBall()
{
    for (_int i = 0; i < 7; ++i) //6개 1
    {
        Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x - 6.f + _float( 2 * i ),
            m_pTransformComp->Get_Pos().y,
            m_pTransformComp->Get_Pos().z + 1.f,
            m_eCurrPhase));
    }
}

void CBoss::Pattern_SlowThunder()
{

}
#pragma endregion 


#pragma region 목표(AI) 상태머신 부분 

void CBoss::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : idle 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : idle 끝 \n");
    }
}

void CBoss::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

        Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x + 0.1f, 
            m_pTransformComp->Get_Pos().y + 1.3f, 
            m_pTransformComp->Get_Pos().z, CAwareness::TYPE::BOSS));
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            m_fBossAwareness += fDeltaTime * 4.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;

                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // 범위밖은 감소
        {
            OutputDebugString(L"▶Brown - 변수체크 : 인지변수 감소중   \n");
            m_fBossAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fBossAwareness)
            {
                m_fBossAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 끝   \n");
    }
}

void CBoss::AI_Reloading(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Taunt - Reloading 돌입   \n");
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Taunt - Reloading 끝   \n");
    }
}

void CBoss::AI_BackIdle(float fDeltaTime) //
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Reconnaissance - BackIdle 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //플레이어 포착하면 다시 돌입 
        {
            m_fBossAwareness += fDeltaTime * 6.f; // 이전보다 더 빠르게 증가할것 
            if (m_fBossAwareness >= m_fMaxAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // 플레이어 포착x
        {
            m_fConsider -= fDeltaTime * 3.f;
        }

        if (0 >= m_fConsider) // 경계변수까지 완전히 0이 되면 집으로 복귀 
        {
            m_fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Reconnaissance - BackIdle 끝   \n");
    }
}

void CBoss::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 행동 : Chase 진입   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 최소한의 근접공격 + 사거리 벗어났을때 제일우선순위 
        if (m_fCloseAttackDistance >= Calc_Distance()) // 근접공격이 최 우선시됨 - 거리 매우 가까울때만 
            m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
        else if (m_fMonsterSightDistance <= Calc_Distance())
            m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);

        if (m_fFrame > m_fFrameEnd) // 계속 총질하니까 한번씩 돌려야함 
        {        // 각 Phase별 보스패턴 도는곳 ============
            if (Detect_Player())
            {
                if (Engine::Phase_End == m_eCurrPhase) //기본phase
                {
                    OutputDebugString(L"★ Brown 디버깅 : Chease - 0관문   \n");
                    if (m_fBasicRunDistance <= Calc_Distance()) // 사거리(9.f) 보다 뒤에 있어야 공격함 
                        m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                    else if (m_fBasicRunDistance > Calc_Distance()) // 거리두기 - 뒤로 쭉 빠짐
                        m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);
                }
                else if (Engine::Phase1 == m_eCurrPhase)// hp 80퍼 이하 - 1관문 시작 
                {
                    OutputDebugString(L"★ Brown 디버깅 : Chease - 1관문  \n");

                    // 1. 시작부터 레이저 
                    if (m_bPH1_RedLaser) 
                    {
                        if (m_bCheck) //최종 m_bCheck = true로 끝 / m_bPH1_RedLaser가 false라서 더이상 안타고 마무리 
                            m_bPH1_RedLaser = FALSE;
                        else
                        {
                            OutputDebugString(L"★ Brown 디버깅 : Chease - 1관문 - RedLaser 발동  \n");
                            m_tState_Obj.Set_State(STATE_OBJ::PH1_LASER);
                            m_bCheck = TRUE; //한번만 발동하는걸로 
                        }
                    } 

                    // 2. 3초에 한번씩 돌떨어짐 
                    if (TimerReset) 
                    {
                        Falling_Stone_Around();
                        TimerReset = FALSE;
                    }

                    _float CurDist = Calc_Distance();

                    // 3. 기본 공격패턴 - Shooting & back shooting + Roll 
                    if (FALSE == m_bPH1_RedLaser && m_fPH1basicDistance < CurDist && m_fMonsterSightDistance >= CurDist) //  12< a <=18
                        m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                    else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance < CurDist && m_fPH1basicDistance >= CurDist) // 9 < a <= 12 
                        m_tState_Obj.Set_State(STATE_OBJ::ROLL);
                    else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance >= CurDist) // a <= 9
                        m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);

                }
                else if (Engine::Phase2 == m_eCurrPhase)// hp 50퍼 이하 
                {
                    // 1. 시작부터 자버프 
                    if (m_bPH2_Buff)
                    {
                        if (!m_bCheck) // 최종 FALSE
                            m_bPH2_Buff = FALSE;
                        else
                        {
                            OutputDebugString(L"★ Brown 디버깅 : Chease - 2관문  \n");
                            Engine::Add_GameObject(L"GameLogic", CBlueBuff::Create(m_pGraphicDev, m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y,
                                m_pTransformComp->Get_Pos().z, m_eCurrPhase));

                            m_bCheck = FALSE; //한번만 발동
                        }
                    }

                    // 2. 3초마다 돌떨어짐 - 더많이, 더 좁은범위에서 
                    if (TimerReset)
                    {
                        Falling_Stone_Around();
                        TimerReset = FALSE;
                    }

                    // 3. 기본공격 + 스킬 연계 
                    if (FALSE == m_bPH2_Buff) // 일단 버프 두르고 연계 시작 
                    {
                        _int iCombo = (rand() % 4) + 1;

                        if (iCombo <= 2) // 1, 2
                        {
                            _float CurDist = Calc_Distance();
                            Pattern_Fire(12, 4.f);
                            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                            // 3. 기본 공격패턴 - Shooting & back shooting + Roll + Track
                           // if (FALSE == m_bPH1_RedLaser && m_fChaseDistance < CurDist && m_fMonsterSightDistance >= CurDist) // 15 < a <= 18
                           //     m_tState_Obj.Set_State(STATE_OBJ::TRACK);
                           // else if (FALSE == m_bPH1_RedLaser && m_fPH1basicDistance < CurDist && m_fChaseDistance >= CurDist) //  12< a <=15
                           //     m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                           // else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance < CurDist && m_fPH1basicDistance >= CurDist) // 9 < a <= 12 
                           //     Pattern_Fire(12, 4.f);
                        }
                        else
                        {
                            // -> 기본 공격패턴 
                            Pattern_EnergyBall();
                            m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);
                        }
                    }
                }
            }
            else
            {
                m_fBossAwareness -= fDeltaTime * 6.f;
                if (0 >= m_fBossAwareness)
                {
                    m_fBossAwareness = 0;
                    m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
                }
            }
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBoss::AI_Pre_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Pre_Attack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        _float CurDistance = Calc_Distance();
        
        m_PreAttackTime += fDeltaTime * 8.f;
        
        // 현재 사거리 a <= 2 -근접만 유효 
        if (m_fCloseAttackDistance >= CurDistance)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
        }

        if (m_MaxTime <= m_PreAttackTime)
        {
            m_PreAttackTime = 0.f;

            if (Detect_Player())
            {
                int iCombo = (rand() % 10) + 1;

                if (0 >= m_iBulletCnt && 6 > iCombo) // 1 ~ 5
                {
                    m_tState_Obj.Set_State(STATE_OBJ::WALK);
                }
                else if (0 >= m_iBulletCnt && 6 <= iCombo) // 6 ~ 10 
                {
                    m_tState_Obj.Set_State(STATE_OBJ::RUN);
                }
                else if (1 <= m_iBulletCnt)
                {
                    m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                }

            }
            else
            {
                m_fBossAwareness -= fDeltaTime * 6.f;

                if (0 >= m_fBossAwareness)
                {
                    m_fBossAwareness = 0;
                    m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
                }
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBoss::AI_Side_Ready(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"SideReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_SideAttackTime += fDeltaTime * 12.f;

        if (m_MaxTime <= m_SideAttackTime)
        {
            m_SideAttackTime = 0.f;

            if (Detect_Player())
            {
                m_tState_Obj.Set_State(STATE_OBJ::ROLL);
            }
            else
            {
                m_fBossAwareness -= fDeltaTime * 6.f;

                if (0 >= m_fBossAwareness)
                {
                    m_fBossAwareness = 0.f;
                    m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
                }
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        if (!m_bGoHome)
        {
            m_fFrameSpeed = 14.f;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk");
            m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        }
    }
    if (m_tState_Obj.Can_Update())
    {
        if (TRUE == m_bGoHome)
        {  
            // // 플레이어가 바라보는 몬스터 : 플레이어 - 몬스터 
            _vec3 vDirect = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

            if (vDirect.z >= 0) // 패트롤 가는곳이 플레이어 기준 +z면 등보이며 걸어가기 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"WalkNorth");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }
            else if (vDirect.z < 0) // 몬패트롤 가는곳이 플레이어 기준 -z면 그냥 걸어가기 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }

            _float fDistance = D3DXVec3Length(&vDirect);

            if (2.f >= fDistance)
            {
                m_bGoHome = false;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
        if (FALSE == m_bGoHome)
        {
            if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
                m_mapActionKey[ACTION_KEY::WALK].Act();

            // 조건 - 플레이어가 시야각으로 들어오면 
            if (m_fCheck == 2)
            {
                m_fCheck = 0;
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_KeepDistance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : KeepDistance 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

        switch (m_eCurrPhase)
        {
        case Engine::Phase1:
            m_fFrameSpeed = 13.f;
            break;
        case Engine::Phase2:
            m_fFrameSpeed = 16.f;
            break;
        case Engine::Phase_End:
            m_fFrameSpeed = 13.f;
            break;
        }
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPDISTANCE].Act();
        
        if (m_fFrame > m_fFrameEnd) // 범위를 벗어났다면 7.x
        {
            OutputDebugString(L"★ Brown 디버깅 : KeepDistance - 범위보다 밖에 있어야 shooting으로 갈예정   \n");
            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : KeepDistance 끝   \n");
    }
}

void CBoss::AI_Run(float fDeltaTime)
{
}

void CBoss::AI_Roll(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Roll 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
     
        if (Engine::Phase2 == m_eCurrPhase)
            m_fFrameSpeed = 15.f;
        else
            m_fFrameSpeed = 13.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::ROLL].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Roll 끝   \n");
    }
}

void CBoss::AI_CloseAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"CloseAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::CLOSE_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {

            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Shooting(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Shooting 돌입   \n");
            m_fFrameSpeed = 15.f;
        if(Engine::Phase2 == m_eCurrPhase)
            m_fFrameSpeed = 18.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (4 <= m_fCheck)
        {
            OutputDebugString(L"▷BOSS - 상태머신 : Shooting 3번 돌음 - cHASE 갈예정    \n");
            m_fCheck = 0;
            m_iBulletCnt -= 1; // 장전수 감소 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Shooting 끝   \n");
    }
}

void CBoss::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷Boss - 상태머신 : GoHome 진입   \n");
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // 현재 액션키가 IDLE 이므로 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // 액션키 누르기 

        if (m_bArrive) // 프레임 다 돌면 
        {
            m_bArrive = false;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Reconnaissance(float fDeltaTime)
{    //플레이어 놓쳐서 인지변수 0까지 내려갔을경우 
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Reconnaissance 진입   \n");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //플레이어 포착하면 다시 돌입 
        {
            m_fBossAwareness += fDeltaTime * 6.f; // 이전보다 더 빠르게 증가할것 

            if (m_fBossAwareness >= m_fMaxAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // 플레이어 포착x
        {
            m_fConsider -= fDeltaTime * 3.f;

        }

        if (0 >= m_fConsider) // 경계변수까지 완전히 0이 되면 집으로 복귀 
        {
            m_fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Reconnaissance 끝   \n");
    }
}

void CBoss::AI_Ph1_Laser(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Phase1 - Laser 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWestWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // 현재 액션키가 IDLE 이므로 
            m_mapActionKey[ACTION_KEY::NORMAL_LASER].Act(); // 액션키 누르기 

       if(m_fFrame > m_fFrameEnd)
       {
           _float PosZ = m_pTransformComp->Get_Pos().z;
           _float PosY = m_pTransformComp->Get_Pos().y;

           for (_int i = 0; i < 5; ++i)
           {
               Engine::Add_GameObject(L"GameLogic", CRedLaser::Create(m_pGraphicDev,
                   m_pTransformComp->Get_Pos().x, 
                   Get_RandomFloat(PosY - 1.f, PosY + 5.f),
                   Get_RandomFloat(PosZ - 3.f, PosZ + 3.f) ));
           }

            m_fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
       }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Phase1 - Laser  끝   \n");
    }
}

void CBoss::AI_Track(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Track 진입   \n");
        m_fConsider = 13.f; 
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // 현재 액션키가 IDLE 이므로 
            m_mapActionKey[ACTION_KEY::TRACK].Act(); // 액션키 누르기 

        if (m_fFrame > m_fFrameEnd) //복귀 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : Phase1 - Track  끝   \n");
    }
}

#pragma endregion

#pragma region 행동 상태머신 부분 
void CBoss::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered()) // 1차 bIsEnter false로 바꿈 
    {
        OutputDebugString(L"▷BOSS - 행동 : Idle 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // 걷기 
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // 거리내기위해 빠르게 뒤로 뛰기 
        if (m_mapActionKey[ACTION_KEY::KEEPDISTANCE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::KEEPDISTANCE);

        // 달리기 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::RUN);

        // 옆으로 구르기
        if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ROLLING);

        // 원위치 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);

        // 
        if (m_mapActionKey[ACTION_KEY::NORMAL_LASER].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::PHASE1_LASER);

        if (m_mapActionKey[ACTION_KEY::TRACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::TRACK);

    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷BOSS - 행동 : Idle 끝   \n");
    }
}

void CBoss::Approaching(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 행동 : Approaching 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        if (Calc_Distance() > m_fMonsterSightDistance) // 시야밖은 recon으로 - 우선사항 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State()
            && m_iBulletCnt == 0 )
        {
            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            m_pTransformComp->Set_Look(vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);

        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CBoss::Run(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
            // 몬스터가 플레이어 보는 반대방향으로 겁나 튐 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
            D3DXVec3Normalize(&vDir, &vDir);

            if (Engine::Phase2 == m_eCurrPhase) // 막페에 빨라짐 
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -(m_fRunSpeed + 3.f));
            else
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -m_fRunSpeed);

            if (m_fBasicRunDistance <= Calc_Distance())
                m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);

        }
        m_tState_Act.Set_State(STATE_ACT::IDLE);

    }

    if (m_tState_Act.IsState_Exit())
    {

    }

    
}

void CBoss::Rolling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);

        if (Engine::Phase2 == m_eCurrPhase) // 막페에 빨라짐 
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, (m_fRollingSpeed + 2.f));
        else
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fRollingSpeed);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"▷Boss - 행동머신 : GoHome 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        _vec3 vDirect = vPatrolPointZero - m_pTransformComp->Get_Pos();

        _float fDistance = D3DXVec3Length(&vDirect);

        D3DXVec3Normalize(&vDirect, &vDirect);

        if (2.f >= fDistance)
        {
            m_bArrive = true;
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
        else
        {
            //OutputDebugString(L"▷Brown - 기존 패트롤 포인트 복귀중   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fRollingSpeed);
        }
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"▷Boss - 행동 : GOHOME 끝   \n");
    }
}

void CBoss::KeepDistance(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"▷Boss - 행동 : KEEP_DISTANCE 진입  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
        // 몬스터가 플레이어 보는 반대방향으로 겁나 튐 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);
     
        if (Engine::Phase1 == m_eCurrPhase || Engine::Phase_End == m_eCurrPhase)
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -m_fRunSpeed);
        else if (Engine::Phase2 == m_eCurrPhase)
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -(m_fRunSpeed + 2.f));
        //if (STATE_OBJ::KEEP_DISTANCE == m_tState_Obj.Get_State())

        if (m_fBasicRunDistance <= Calc_Distance())
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        m_tState_Act.Set_State(STATE_ACT::IDLE);

    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"▷Boss - 행동 : KEEP_DISTANCE 끝   \n");
    }
}

void CBoss::Phase1_LaserOn(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"▷Boss - 행동 : Phase1 - Laser 진입  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
         // 몬스터가 북서로 향함 
         vDir =  m_pPlayerTransformcomp->Get_Right() - m_pTransformComp->Get_Pos();
         D3DXVec3Normalize(&vDir, &vDir);

         m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -11.f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"▷Boss - 행동 : Phase1 - Laser 끝   \n");
    }
}

void CBoss::Track(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"▷Boss - 행동 : Track 진입  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
        // 몬스터가 플레이어쪽으로 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 11.f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"▷Boss - 행동 : Track 끝   \n");
    }
}

#pragma endregion

