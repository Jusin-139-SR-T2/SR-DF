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

    // INFO
    m_iHP = 40;         // 체력
    m_iAttack = 10;     // 공격력

#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBoss::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBoss::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CBoss::AI_Reloading);
    m_tState_Obj.Add_Func(STATE_OBJ::BACKIDLE, &CBoss::AI_BackIdle);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBoss::AI_Chase);
    m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CBoss::AI_Pre_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CBoss::AI_Side_Ready);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBoss::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBoss::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CBoss::AI_Roll);
    m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CBoss::AI_CloseAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CBoss::AI_Shooting);

    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBoss::AI_GoHome);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBoss::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACHING, &CBoss::Approaching);
    m_tState_Act.Add_Func(STATE_ACT::ROLLING, &CBoss::Rolling);
    m_tState_Act.Add_Func(STATE_ACT::CLOSEATTACKING, &CBoss::CloseAttacking);
    m_tState_Act.Add_Func(STATE_ACT::SHOOTING, &CBoss::Shooting);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBoss::GoHome);

#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // 멀때 

#pragma endregion
    return S_OK;
}

_int CBoss::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    Height_On_Terrain(); // 지형타기 

    // ---------- 테스트 빌드 ----------------------

    if (Engine::IsKey_Pressing(DIK_N))
    {
        m_iHP = 0; //즉사 기믹 확인용 
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
        m_iHP = 50; // 피격 기믹 확인용 
    }


    //상태머신-------------------------------------

    m_fFrame += m_fFrameSpeed * fTimeDelta;

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

    // 빌보드 --------------------------------------
    Billboard(fTimeDelta);

    // Renderer -----------------------------------
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

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

void CBoss::Height_On_Terrain()
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

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.2f, vPos.z);
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

#pragma region 상태머신 부속파트 
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
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    _vec3 MonToPlayer = vMonsterPos - vPlayerPos;
    float currdistance = D3DXVec3Length(&MonToPlayer); // 현재 플레이어 위치에서 몬스터까지의 길이 = 시야거리 비교 

    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

    //현재 플레이어 위치가 몬스터 시야거리 외부(밖)에 있다 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // 내부에 있는지 판별 = 몬스터 앞 X (몬스터가 플레이어를 보는 벡터) -> 양수일경우 내적해서 180도 이내 = 시야각안 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // 시야각도 내부 && 시야거리 이내 
    if (fradian < m_fMonsterFov * 2 && currdistance < m_fMonsterSightDistance)
        return true;
    else
        return false;
}

_float CBoss::Calc_Distance()
{
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    float fDistance = D3DXVec3Length(&vDistance);

    return fDistance;
}
#pragma endregion

#pragma region 목표(AI) 상태머신 부분 

void CBoss::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_iPreHP = m_iHP; // 상태체크용 hp저장 
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

    }
}

void CBoss::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            m_fBossAwareness += fDeltaTime * 3.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;

                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // 범위밖은 감소
        {
            m_fBossAwareness -= fDeltaTime * 6.f;

            if (m_fBossAwareness < 0)
                m_fBossAwareness = 0;

            //플레이어가 시야각을 벗어나 인지값이 초기화되면 idle로 back
            if (0 >= m_fBossAwareness)
            {
                m_fBossAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Reloading(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            // 조건 - 플레이어가 시야각으로 들어오면 
            if (TRUE == Detect_Player())
            {
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
            else
            {
                m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_BackIdle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 4.f;
        
        if (Detect_Player())
        {
            m_fBossAwareness += fDeltaTime * 4.f; // 이전보다 더 빠르게 증가할것 

            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }

        if (0 >= m_fConsider)
        {
            m_fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_ChaseTime += fDeltaTime * 14.f;

        _float CurDistance = Calc_Distance();

        if (m_fCloseAttackDistance >= CurDistance) // 근접공격이 최 우선시됨 - 거리 매우 가까울때만 
        {
            m_ChaseTime = 0.f;
            m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
        }

        if (Detect_Player())
        {
            if (0 < m_iBulletCnt)
            {
                if (m_fRunDistance < CurDistance) // 현재 거리가 8 이상일때 
                {
                    m_tState_Obj.Set_State(STATE_OBJ::RUN);
                }
                else
                {
                    int iCombo = (rand() % 10) + 1;

                    if (7 <= iCombo) // 무조건 총쏘는거라서 총알있어야만함 40%
                        m_tState_Obj.Set_State(STATE_OBJ::SIDE_READY); // -> Roll -> Shooting

                    if (7 > iCombo) // 60% 
                        m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK); // Run -> Shooting
                }
            }
            else//총알 없음 
            {
                m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK); // Run -> Shooting
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

void CBoss::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (Calc_Distance() < 5.f)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Roll(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 13.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
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
        m_fFrameSpeed = 16.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SHOOTING].Act();

        if (4 <= m_fCheck)
        {
            m_fCheck = 0;
            m_iBulletCnt -= 1; // 장전수 감소 
            m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        // 조건 - 처음 위치로 돌아갈때까지 
        m_bGoHome = true;
        m_tState_Obj.Set_State(STATE_OBJ::WALK);
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

#pragma endregion

#pragma region 행동 상태머신 부분 
void CBoss::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered()) // 1차 bIsEnter false로 바꿈 
    {
    }

    if (m_tState_Act.Can_Update())
    {
        // 걷기 
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // 달리기 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // 옆으로 구른다!
        if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ROLLING);

        // 근접공격
        if (m_mapActionKey[ACTION_KEY::CLOSE_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::CLOSEATTACKING);

        // 총알 쏘는파트 
        if (m_mapActionKey[ACTION_KEY::SHOOTING].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SHOOTING);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CBoss::Approaching(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        // run + walk 부분 관여 
    }

    if (m_tState_Act.Can_Update())
    {
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            _float DirDistance = D3DXVec3Length(&vDir);
            m_pTransformComp->Set_Look(vDir);

            if (m_iBulletCnt == 0) // 총알다씀 -> 접근
            {
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
            }
            else if (0 < m_iBulletCnt ) //총알 있음
            {
                if (DirDistance >= m_fRunDistance) // 먼경우 
                {
                    m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
                }
                if (DirDistance < m_fRunDistance) // 가까운경우 
                {
                    m_tState_Act.Set_State(STATE_ACT::SHOOTING);
                }
            }
        }

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

void CBoss::Rolling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fRollingSpeed);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::CloseAttacking(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::Shooting(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {

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
    }

    if (m_tState_Act.Can_Update())
    {

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

#pragma endregion

