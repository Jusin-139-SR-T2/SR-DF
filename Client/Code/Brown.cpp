#include "stdafx.h"
#include "Brown.h"

CBrown::CBrown(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CBrown::CBrown(const CBrown& rhs)
    : Base(rhs)
{
}

CBrown::~CBrown()
{
}

CBrown* CBrown::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z); // 시작위치 설정
    pInstance->m_tStat.vPatrolPointZero = { _x, _y, _z }; // 시작위치로 복귀포인트 설정 
    return pInstance;
}

CBrown* CBrown::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CBrown::Ready_GameObject()
{
    SUPER::Ready_GameObject();

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
    m_pTextureComp->Set_Scale({ 3.f, 3.f, 1.f });
    m_pTextureComp->Readjust_Transform();

    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed = 12.f;
    m_tFrame.fRepeat = 0.f;

    // 팀에이전트 셋팅 
    Set_TeamID(ETEAM_MONSTER);

    // Status
    m_tStat.fAttackDistance = 12.f;
    m_tStat.fAttackFov = 90.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;
    m_bDazedState = FALSE;
    m_bDeadState = FALSE;
    m_bDazeToHeal = FALSE;

    // 충돌용
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    pBoxShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());

//#pragma region 블랙보드
//    Engine::Add_BlackBoard(L"Monster", CBlackBoard_Monster::Create());
//#pragma endregion


#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    // 경계
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CBrown::AI_Rest);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);

    // 추격
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    //m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CBrown::AI_Jump);
    
    // 공격
    m_tState_Obj.Add_Func(STATE_OBJ::NORMALATTACK, &CBrown::AI_NormalATTACK);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

    // 피격
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CBrown::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CBrown::AI_CrotchHit);

    // 죽음
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CBrown::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CBrown::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CBrown::AI_Headless);

    // 복귀
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CBrown::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBrown::AI_GoHome);


#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE가 ::A 일때 CBrown클래스의 ::B 함수를 수행하는 상태머신 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CBrown::Moving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    m_tState_Act.Add_Func(STATE_ACT::FALLING, &CBrown::Falling);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBrown::GoHome);
    
#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // 가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y축에 차이가 있을때 
    m_mapActionKey.Add_Action(ACTION_KEY::NORMALATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);
    m_mapActionKey.Add_Action(ACTION_KEY::BEPUSHED); // 밀침당했을때 

#pragma endregion

    //OutputDebugStringW(debugString);

    return S_OK;
}

HRESULT CBrown::Ready_GameObject(const FSerialize_GameObject tObjectSerial)
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

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    //죽는거 셋팅 - 물리 서순문제 발생해결 
    if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
        MonsterDead();

    if (FALSE == m_bDeadState)
        Billboard(fTimeDelta);

    //상태머신
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
    {
        m_tFrame.fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State())
            m_tFrame.fRepeat += 1;
    }

    
    if (IsKey_Pressed(DIK_I))
    {
        //m_tState_Obj.Set_State(STATE_OBJ::FALLING);

        //m_pTransformComp->Get_Look();

   /*     for (_float i = 0; i < D3DX_PI; ++i)
        {
            m_pTransformComp->Set_RotationY(i * fTimeDelta);
        }*/


       //_vec3 look = m_pPlayerTransformcomp->Get_Look();
       //
       //swprintf_s(debugString, L"디버그용 현재 플레이어 LOOK x %f\n", look.x);
       //OutputDebugStringW(debugString);
       //swprintf_s(debugString, L"디버그용 현재 플레이어 LOOK y %f\n", look.y);
       //OutputDebugStringW(debugString);
       //swprintf_s(debugString, L"디버그용 현재 플레이어 LOOK z %f\n", look.z);
       //OutputDebugStringW(debugString);

    }

    // 블랙보드용 
    //Update_InternalData();
    
    //물리 업데이트 코드
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 콜라이더 위치 업데이트 

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region 기본 override 

void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &((*m_pTextureComp->Get_Transform()) * (*m_pTransformComp->Get_Transform())));
    m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
    m_pBufferComp->Render_Buffer();

#pragma region 충돌 메쉬 콜라이더
    MeshBoxColider(pBoxShape->vHalfSize.x, pBoxShape->vHalfSize.y,pBoxShape->vHalfSize.z);
#pragma endregion

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBrown::Add_Component()
{
    // 충돌 컴포넌트 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);
    
    // 물리 세계 등록
    m_pColliderComp->EnterToPhysics(0);
    
    // 충돌 함수 연결
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    
    // 충돌 레이어, 마스크 설정
    m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER); // 이 클래스가 속할 충돌레이어 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_PROJECTILE | LAYER_WALL | LAYER_PLAYER_ATTACK); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

    return S_OK;
}

void CBrown::Free()
{
    SUPER::Free();
}

#pragma endregion 

#pragma region 충돌 - OnCollision 

void CBrown::OnCollision(CGameObject* pDst, const FContact* const pContact) // 계속 충돌중 
{
}

void CBrown::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact) // 처음 충동 진입 
{
    if (Get_IsMonsterDeath())
        return;

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;
    else
    {
        // Pow 생성
        Engine::Add_GameObject(L"GameLogic", CEffect_HitPow::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 0.2f, m_pTransformComp->Get_Pos().z, this));

        // Blood생성
        for (_int i = 0; i < 3; ++i)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_HitBlood::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));
        }

        // Dust 생성
        Engine::Add_GameObject(L"GameLogic", CEffect_HitDust::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));

    }

    // 보스 스킬에 죽을수도 있기때문에 충돌체 먼저 검사한뒤에 dead로 넘어가야함
    // 현재 팀 : 몬스터  적대관계 : 플레이어 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

        if (nullptr == pPlayer)
        {
            CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

            if (nullptr == pPlayerAttack)
                return;

            //==== 플레이어 공격체와  충돌 =============================
            m_eRecentCol = RECENT_COL::PLAYERATK;

            if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
            {
                OutputDebugString(L"▷Brown - 충돌판정 DAZED 진입   \n");
                m_tState_Obj.Set_State(STATE_OBJ::DAZED);
                return;
            }	
            else if (CPlayer::STATE_RIGHTHAND::KICK == ePlayerRighthand)
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); // fist
            else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // 앉 + kick = falling 임 
                m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
            else //일어서야 FacePunch있음 
            {
                if (Random_variable(60))
                    m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
                else
                    m_tState_Obj.Set_State(STATE_OBJ::HIT);
            }
        }
        else
        {
            //==== 플레이어와 충돌 =====================================
            m_eRecentCol = RECENT_COL::PLAYER;

            if (CPlayer::STATE_RIGHTHAND::RUN_HAND == ePlayerRighthand)
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //달릴때 

        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::FRIEND)
    {
        // 친밀관계 - 보스 : 몬스터는 친밀이지만 보스는 적대임 
        // 보스 자체의 충돌은 없고 보스가 소환한 공격체에는 충돌함 

        CMonsterAttackUnion* pBossAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);
       
        if(nullptr == pBossAttack)
            return;
        else
        {
            // ==== 보스스킬과 충돌 ========================================
            m_eRecentCol = RECENT_COL::BOSSATK;

            m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // 오브젝트 충돌 
    {
        m_tState_Obj.Set_State(STATE_OBJ::HIT);
    }
}

void CBrown::OnCollisionExited(CGameObject* pDst) // 충돌 나갈때 
{
}

void CBrown::MonsterDead()
{
    if(RECENT_COL::PLAYER == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else if (RECENT_COL::PLAYERATK == m_eRecentCol)
    {
        if (CPlayer::STATE_RIGHTHAND::GUN == ePlayerRighthand ||
            CPlayer::STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        else
            m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    }
    else if(RECENT_COL::BOSSATK == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
}
#pragma endregion 

#pragma region BlackBoard
void CBrown::Update_InternalData()
{
    // 블랙보드 연결 대기, 안전 코드로 필수
    if (!m_wpBlackBoard_Monster.Get_BlackBoard())
    {
        m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"Monster"));
        // 연결 실패
        if (!m_wpBlackBoard_Monster.Get_BlackBoard())
            return;
    }

    // 안전 코드를 거치면 일반 포인터로 접근 허용.
    CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

    // 여기서부터 블랙보드의 정보를 업데이트 한다.
    //pBlackBoard->Get_BrownAwareness().Cur = m_tStat.fAwareness;
}

#pragma endregion 

#pragma region 환경설정 부속파트 + 상태머신 보조함수 

#pragma endregion 

#pragma region 상태머신 : idle ~ Death

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : idle 돌입   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        m_bArrive = FALSE;
          //OutputDebugString(L"▷Brown - 상태머신 : idle 끝 \n");
    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Suspicious 돌입   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

       Engine::Add_GameObject(L"GameLogic", CEffect_Awareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x + 0.1f, 
           m_pTransformComp->Get_Pos().y + 1.3f,
           m_pTransformComp->Get_Pos().z, CEffect_Awareness::TYPE::BROWN, this));
    }

    if (m_tState_Obj.Can_Update())
    {
        
        if(Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
           // //OutputDebugString(L"▶Brown - 변수체크 : 인지변수 상승중   \n");
            m_tStat.fAwareness += fDeltaTime * 2.f;

            // ＠＠ 수정사항 
            // 중요 << ★ 여기서 awareness와 몬스터의 m_awareness 서로 연결해서 값에따라 증가하게 해야함. 
            // 그리고 몬스터 상태가 Taunt로 가면 쟤 set_dead로 날려야함 
            // 당장은 야매로 값 비슷하게 맞춘거 

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness; // 추후 감소를 위해 최대값으로 고정 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // 추격으로 시작 
            }
        }
        else // 범위밖은 감소
        {
            ////OutputDebugString(L"▶Brown - 변수체크 : 인지변수 감소중   \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    };

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Suspicious 끝   \n");
    }
}

void CBrown::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Taunt 돌입   \n");
        m_tFrame.fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Taunt");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (2 == m_tFrame.fRepeat) // 도발 두번 하고 따라가기 
        {
            m_tFrame.fRepeat = 0; //다른데도 쓰니까 0으로 되돌리기 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Taunt 끝   \n");
    }
}

void CBrown::AI_Chase(float fDeltaTime) // 달리다가 걷다가 잽날리려고함 
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Chease 돌입   \n");
       // m_pTransformComp->m_vScale.x = 0.5f;
        m_tFrame.fFrameSpeed = 10.f; //원상복귀 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        m_AttackOnce = false;
        _float CurDistance = Calc_Distance();

        //OutputDebugString(L"★ Brown 디버깅 : Chease - Update 돌입   \n");

        if (Detect_Player()) // 사거리에 포착되는경우 
        {
            //OutputDebugString(L"★ Brown 디버깅 : Chease -플레이어 포착 \n");
            // --------거리비교 상태머신 -----------
            // 뛰어서 다가옴 : a > 8
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"★ Brown 디버깅 : Chease -플레이어 포착 - Run 실행  \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Brown 디버깅 : Chease -플레이어 포착 - Walk 실행 \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 무빙 : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Brown 디버깅 : Chease -플레이어 포착 - Moving 실행 \n");
                int iCombo = (rand() % 10) + 1;

                if (5 <= iCombo) // 60% / 5~10 
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (5 > iCombo) // 40% / 1~4 
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // 공격함
            else
            {
                //OutputDebugString(L"★ Brown 디버깅 : Chease -플레이어 포착 - 공격 실행  \n");
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // 사거리내 플레이어를 놓쳤을경우 
        {
            //OutputDebugString(L"▷Brown - 상태머신 : 플레이어 놓침    \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f; // 인지값 감소 

            if (0 >= m_tStat.fAwareness) //인지값이 초기화
            {
                //OutputDebugString(L"★ 디버그 찾기 : Chease -플레이어 놓침 - Recon으로 넘어감  \n");
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE); 
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Chease 끝   \n");
    }
}

void CBrown::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Rest 진입   \n");

        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 15.f;
        m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Rest 끝   \n");

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Run 진입   \n");

        m_tFrame.fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (Calc_Distance() < m_fWalkDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Run 끝   \n");
    }
}

void CBrown::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Walk 진입   \n");
        m_tFrame.fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (Calc_Distance() < m_fWalkDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Walk 끝   \n");

    }
}

void CBrown::AI_InchForward(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : InchForward 진입   \n");
        m_tFrame.fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
        {
            //OutputDebugString(L"★ 디버그 찾기 : INCHFORWARD - 가상키 누르기 돌입   \n");
            m_mapActionKey[ACTION_KEY::INCHFORWARD].Act();
        }
        if (Calc_Distance() <= m_fInchDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            //OutputDebugString(L"★ 디버그 찾기 : INCHFORWARD - 프레임 다 돌음 \n");
            //OutputDebugString(L"▷Brown - 상태머신 : InchForward 끝   \n");
            m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : InchForward 끝   \n");

    }
}

void CBrown::AI_Strafing(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Strafing 진입   \n");

        m_tFrame.fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Strafing");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
    }
    if (m_tState_Obj.Can_Update())
    {     
        //행동이 IDLE일때 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::STRAFING].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Strafing 끝   \n");
    }
}

void CBrown::AI_NormalATTACK(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Attack 진입   \n");
        m_tFrame.fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"NormalAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;

    }
    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 가상키 누르기 
        if (m_tFrame.fFrame > 3.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::NORMALATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Attack 끝   \n");
    }
}

void CBrown::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : HeavyAttack 진입   \n");

        m_tFrame.fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"HeavyAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > 4.f && m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CBrown::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Hit 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Hit");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 10.f;
         m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >=  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Hit 끝   \n");
    }
}

void CBrown::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Falling 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Falling");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
        m_tFrame.fFrame = 0.f;
        m_bSecondFall = true;
        m_AttackOnce = true;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BEPUSHED].Act();

        if (m_tFrame.fFrame > 4.f && m_AttackOnce)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y - 1.f, m_pTransformComp->Get_Pos().z, this));
            m_AttackOnce = false;
            m_bSecondFall = false;
        }

        if (m_tFrame.fFrame > 9.f && !m_AttackOnce && !m_bSecondFall)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_FallingDust::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y - 1.f, m_pTransformComp->Get_Pos().z, this));
            m_bSecondFall = true;
        }

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Falling 끝   \n");
    }
}

void CBrown::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : FacePunch 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"FacePunch");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 8.f;
         m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : FacePunch 끝   \n");
    }
}

void CBrown::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Dazed 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"CrotchHit");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 2.f; // 2초후 CHASE 진입 
        m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Dazed 끝   \n");
    }
}

void CBrown::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Dazed 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Dazed");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 1.f;
        m_tFrame.fFrame = 0.f;
        m_bDazedState = TRUE;
        m_AttackOnce = false;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (!m_AttackOnce)
        {
            Engine::Add_GameObject(L"GameLogic", CEffect_DazeSwirl::Create(m_pGraphicDev,
                m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y, m_pTransformComp->Get_Pos().z, this));
            m_AttackOnce = true;
        }

        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_bDazeToHeal = TRUE;
        }
        if (TRUE == m_bDazeToHeal)
        {
            if (m_gHp.Update(fDeltaTime * 10.f, 40.f, TRUE)) // 증가값, 도달하면 bool반환 
            {
                m_gHp.Cur = 40.f;
                m_bDazedState = FALSE;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Dazed 끝   \n");
    }
}

void CBrown::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Chopped 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Chopped");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Chopped 끝   \n");
    }
}

void CBrown::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Headless 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Headless");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 11.f;
         m_tFrame.fFrame = 0.f;
         m_bDeadState = TRUE;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd-1)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Headless 끝   \n");
    }
}

void CBrown::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 상태머신 : Death 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Death");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fFrame = 0.f;
        m_bDeadState = TRUE;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame> m_tFrame.fFrameEnd-1)
        {
            m_tFrame.fFrame =  m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Death 끝   \n");
    }
}

void CBrown::AI_Reconnaissance(float fDeltaTime)
{
    //플레이어 놓쳐서 인지변수 0까지 내려갔을경우 
    if (m_tState_Obj.IsState_Entered())
    {
        m_tFrame.fFrame = 0.f;
          //OutputDebugString(L"▷Brown - 상태머신 : Reconnaissance 진입   \n");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //플레이어 포착하면 다시 돌입 
        {
            m_tStat.fAwareness += fDeltaTime * 6.f; // 이전보다 더 빠르게 증가할것 
            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }
        else // 플레이어 포착x
        {
            m_tStat.fConsider -= fDeltaTime * 3.f;

        }

        if (0.f >= m_tStat.fConsider) 
        {
            // 경계변수까지 완전히 0이 되면 집으로 복귀 
            m_tStat.fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : Reconnaissance 끝   \n");
    }
}

void CBrown::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : GoHome 진입   \n");
           m_tFrame.fFrameSpeed = 15.f;
           m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_North");
           m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
           m_tFrame.fFrame = 0.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // 현재 액션키가 IDLE 이므로 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // 액션키 누르기 
     
        if (m_bArrive && (m_tFrame.fFrame >  m_tFrame.fFrameEnd)) // 프레임 다 돌면 
        {
            m_bArrive = FALSE;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 상태머신 : GoHome 끝   \n");
    }
}

#pragma endregion 

#pragma region 행동머신 Idle ~ GoHome

void CBrown::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::INCHFORWARD].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);
        if (m_mapActionKey[ACTION_KEY::STRAFING].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);

        if (m_mapActionKey[ACTION_KEY::NORMALATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);
        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);

        if (m_mapActionKey[ACTION_KEY::BEPUSHED].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::FALLING);

    }

    if (m_tState_Act.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
    }
}

void CBrown::Approach(float fDeltaTime) // RUN 액션키 들어가면 수행하는곳
{
    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 행동머신 : Approach 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로 - 우선사항 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"▷Brown - 가상키 : RUN 수행   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        { 
            //OutputDebugString(L"▷Brown - 가상키 : WALK 수행   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 3.5f);
    
        }


        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          ////OutputDebugString(L"▷Brown - 행동머신 : Approach 끝   \n");
    }
}
void CBrown::Moving(float fDeltaTime)
{

    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"▷Brown - 행동머신 : MOVING 진입   \n");
    }
    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        if(Calc_Distance() < m_tStat.fAttackDistance && Calc_Distance() > m_fRunDistance ) // Run사이로 걸치면 Run으로 
            m_tState_Obj.Set_State(STATE_OBJ::RUN);

        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"▷Brown - 가상키 : INCHFORWARD 수행   \n");

            // 몬스터가 플레이어를 보는벡터 = 플 - 몬 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos(); // 플레이어 쪽으로 이동 
            D3DXVec3Normalize(&vDir, &vDir);

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 8.f);

            if (Calc_Distance() < m_fInchDistance) //inch 사거리보다 작으면 다른거 수행하기위해서 
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            
        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"▷Brown - 가상키 : STRAFING 수행   \n");

            _vec3 MoveDir = m_pTransformComp->Get_Right() + m_pTransformComp->Get_Look(); // 플레이어 오른쪽 
            D3DXVec3Normalize(&MoveDir, &MoveDir);
            m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, 7.f);

            if (Calc_Distance() < m_fInchDistance) //inch 사거리보다 작으면 다른거 수행하기위해서 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        }
        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          //OutputDebugString(L"▷Brown - 행동머신 : MOVING 끝   \n");
    }
}

void CBrown::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 행동머신 : ATTACK 진입   \n");
    }

    // 실행
    {
        //몬스터가 바라보는쪽으로 생성 
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);

        _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

        if (STATE_OBJ::NORMALATTACK == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"▷Brown - 가상키 : BASICATTACK 수행   \n");
            if (!m_AttackOnce)
            {
                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
                    vDirPos.x, vDirPos.y, vDirPos.z,
                    CMonsterPunch::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));
                
                m_AttackOnce = true;
            }
        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"▷Brown - 가상키 : HEAVYATTACK 수행   \n");
            if (!m_AttackOnce)
            {
                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev, 
                    vDirPos.x, vDirPos.y, vDirPos.z, 
                    CMonsterPunch::TYPE::HEAVY, this, (ETEAM_ID)Get_TeamID()));

                m_AttackOnce = true;
            }
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        // //OutputDebugString(L"▷Brown - 행동머신 : MOVING 끝   \n");
    }
}

void CBrown::Falling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 행동머신 :FALLING 진입   \n");
    }

    // 실행
    {
        _vec3 vDirect = m_pTransformComp->Get_Pos() - m_pPlayerTransformcomp->Get_Pos();


        D3DXVec3Normalize(&vDirect, &vDirect);

        m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 4.f);

        if (STATE_OBJ::FALLING != m_tState_Obj.Get_State())
            m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"▷Brown - 행동머신 : GOHOME 끝   \n");
    }
}


void CBrown::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Brown - 행동머신 : GoHome 진입   \n");
    }

    // 실행
    {
        _vec3 vDirect = m_tStat.vPatrolPointZero - m_pTransformComp->Get_Pos();

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
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 6.f);
        }
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"▷Brown - 행동머신 : GOHOME 끝   \n");
    }
}

#pragma endregion