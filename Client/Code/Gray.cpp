#include "stdafx.h"
#include "Gray.h"

CGray::CGray(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CGray::CGray(const CGray& rhs)
	: Base(rhs)
{
}

CGray::~CGray()
{
}

CGray* CGray::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
    pInstance->m_tStat.vPatrolPointZero = { _x, _y, _z };
    return pInstance;
}

HRESULT CGray::Ready_GameObject()
{
    SUPER::Ready_GameObject();

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    // 이미지 관련
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
    m_pTransformComp->Set_Scale({ 3.f, 5.f, 1.f });
    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd= _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed= 12.f;
    m_tFrame.fRepeat= 0.f;
    m_tFrame.fAge = 0.f;

    // 팀에이전트 셋팅 
    Set_TeamID(ETEAM_MONSTER);

    //Status
    m_tStat.fAttackDistance = 13.f;
    m_tStat.fAttackFov = 80.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;
    m_bDazedState = FALSE;
    m_bDeadState = FALSE;
    m_bDazeToHeal = FALSE;
    
    // 충돌용
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 3.f, 3.f, 3.f };

    D3DXCreateBox(m_pGraphicDev, 3.f, 3.f, 3.f, &m_pMesh, NULL);
    
#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 
    // 추격
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CGray::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CGray::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); 
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);
    
    // 기본견제
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CGray::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CGray::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEPEYE, &CGray::AI_KeepEye);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDEWALK, &CGray::AI_SideWalk);
    m_tState_Obj.Add_Func(STATE_OBJ::FRIGHTEN, &CGray::AI_Frighten);
    m_tState_Obj.Add_Func(STATE_OBJ::UPRIGHTRUN, &CGray::AI_UpRightRun);
    
    // 공격
    m_tState_Obj.Add_Func(STATE_OBJ::THROW, &CGray::AI_Throw);
    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CGray::AI_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CGray::AI_HeavyAttack);
    
    // 복귀
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CGray::AI_GoHome);
    
    // 막기
    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    
    // 피격
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CGray::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CGray::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CGray::AI_Hit);
    
    // 사망
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
    m_tState_Act.Add_Func(STATE_ACT::SUDDENATTACK, &CGray::SuddenAttack);
    m_tState_Act.Add_Func(STATE_ACT::SIDEMOVING, &CGray::SideMoving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CGray::GoHome);

#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); //가까울때

    m_mapActionKey.Add_Action(ACTION_KEY::KEEPEYE); // 가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::SIDEWALK); // y축에 차이가 있을때 

    m_mapActionKey.Add_Action(ACTION_KEY::UPRIGHT); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::FRIGHTEN); //중간

    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

    return S_OK;
}

_int CGray::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain(); 

    if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
        MonsterDead();

    // 빌보드 
    if (FALSE == m_bDeadState)
        Billboard(fTimeDelta);

    // 상태머신
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
    {
        m_tFrame.fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() ||
            STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State() ||
            STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State()
            )
            m_tFrame.fRepeat += 1;
    }


#pragma endregion 

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}


#pragma region 기본 환경설정 

void CGray::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGray::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    if (m_pMesh)
        m_pMesh->DrawSubset(0);
    m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CGray::Add_Component()
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

void CGray::Free()
{
    SUPER::Free();
}


#pragma endregion 

#pragma region 환경설정 부속파트 + 상태머신 보조함수 

void CGray::RenderSplitImages()
{
    _vec3 MonsterLook = m_pTransformComp->Get_Look();
    _vec3 PlayerLook = m_pPlayerTransformcomp->Get_Look();

    _float DotProduct = D3DXVec3Dot(&MonsterLook, &PlayerLook);

    //if(DotProduct)

    swprintf_s(debugString, L"Brown - 변수 확인 DotProduct = %f\n", DotProduct);
    //OutputDebugStringW(debugString);
}

#pragma endregion

#pragma region 충돌파트 

void CGray::OnCollision(CGameObject* pDst)
{
    //OutputDebugString(L"▶Gray 충돌중 \n");
}
void CGray::OnCollisionEntered(CGameObject* pDst)
{
    if (Get_IsMonsterDeath())
        return;

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

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
                if (Random_variable(50))
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

        // ==== 보스스킬과 충돌 ========================================
        CMonsterAttackUnion* pBossAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);

        if (nullptr == pBossAttack)
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

void CGray::OnCollisionExited(CGameObject* pDst)
{
    m_bCollisionEnter = FALSE;
}

void CGray::MonsterDead()
{
    if (RECENT_COL::PLAYER == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    else if (RECENT_COL::PLAYERATK == m_eRecentCol)
    {
        if (CPlayer::STATE_RIGHTHAND::GUN == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADSHOT);
        else if(CPlayer::STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        else
            m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    }
    else if (RECENT_COL::BOSSATK == m_eRecentCol)
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
}
#pragma endregion 

#pragma region 목표(AI) 상태머신 부분 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : idle 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : idle 끝  \n");
    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Suspicious 진입  \n");
       Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x ,
           m_pTransformComp->Get_Pos().y + 1.4f,
           m_pTransformComp->Get_Pos().z, CAwareness::TYPE::GRAY, this));

    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            //OutputDebugString(L"▷Gray - 상태머신 : Suspicious -> 플레이어 포착   \n");
            
            m_tStat.fAwareness += fDeltaTime * 3.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fAwareness; // 추후 감소를 위해 최대값으로 고정 

               // int iCombo = (rand() % 10) + 1; // 1~10 
               //
               // if (6 <= iCombo) // 6~10
                    m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
               //
               // if (6 > iCombo) // 1~5
               //     m_tState_Obj.Set_State(STATE_OBJ::TAUNT);

            }
        }
        else // 범위밖은 감소
        {
            //OutputDebugString(L"▶Gray - 변수체크 : 인지변수 감소중   \n");

            m_tStat.fAwareness -= fDeltaTime * 6.f;

            //플레이어가 시야각을 벗어나 인지값이 초기화되면 idle로 back
            if (0 >= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Suspicious 끝  \n");

    }
}

void CGray::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Taunt 진입  \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Taunt");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            if (2 == m_tFrame.fRepeat) // 도발 두번 하고 따라가기 
            {
                m_tFrame.fRepeat = 0; //다른데도 쓰니까 0으로 되돌리기 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Taunt 끝  \n");
    }
}

void CGray::AI_YouDie(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : You Die 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 9.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd) // 도발 두번 하고 따라가기 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : You Die 끝   \n");
    }
}

void CGray::AI_Reconnaissance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {

    }
    if (m_tState_Obj.Can_Update())
    {
        m_tStat.fConsider -= fDeltaTime * 3.f;

        if (Detect_Player())
        {
            m_tStat.fAwareness += fDeltaTime * 4.f; // 이전보다 더 빠르게 증가할것 

            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
            }
        }

        if (0 >= m_tStat.fConsider)
        {
            m_tStat.fConsider = 10.f; // 다시 초기 셋팅으로 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"WalkNorth");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 14.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        if (m_bArrive && m_tFrame.fFrame > m_tFrame.fFrameEnd) // 프레임 다 돌면 
        {
            m_bArrive = FALSE;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CGray::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chase 진입  \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 8.f;

        m_AttackOnce = FALSE;
    }

    if (m_tState_Obj.Can_Update())
    {
        //OutputDebugString(L"★ Gray 디버깅 : Chease - Update 돌입   \n");

        if (Detect_Player()) // 플레이어 포착 
        {
            _float CurDistance = Calc_Distance();

            if (m_bPlayerAttakBool && m_bCollisionEnter == FALSE)
                m_tState_Obj.Set_State(STATE_OBJ::BLOCK);

            // 뛰어서 다가옴 : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease -RUN 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - WALK 상태함수 Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 주시하면서 경계 : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 경계 상태함수 Set    \n");
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //대충 공격하러 옴 : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 접근 상태함수 Set  \n");
                int iCombo = (rand() % 15) + 1;

                if (4 > iCombo)                         // 20프로 1~3
                    m_tState_Obj.Set_State(STATE_OBJ::THROW);
                else if ( 10 > iCombo && 4 <= iCombo)   // 40프로  4 ~ 9
                    m_tState_Obj.Set_State(STATE_OBJ::UPRIGHTRUN);
                else                                    // 40프로 10~15
                    m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
            }
            else  // 공격함
            {
                //OutputDebugString(L"★ Gray 디버깅 : Chease - 공격 상태함수 Set   \n");
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // 쫒다가도 시야에서 벗어나면 게이지 줄어들어서 SUSPICIOUS로 돌아감 
        {
            //OutputDebugString(L"★ Gray 디버깅 : Chease - 포착실패   \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_tStat.fAwareness) //인지값이 초기화되면 
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chase 끝   \n");
    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //일종의 숨고르기 구간임 
        //OutputDebugString(L"▷Gray - 상태머신 : Rest 돌입   \n");

        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Rest 끝   \n");
    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Run 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Run");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 17.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 RUN 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Run 끝   \n");
    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Walk 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 11.f;
        
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Walk 끝   \n");
    }
}

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : KeepEye 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"KeepEye");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 11.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPEYE].Act();

        // 조건 - 플레이어가 시야각으로 들어오면 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : KeepEye 끝 \n");
    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : SideWalk 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"SideWalk");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SIDEWALK].Act();

        if (2 == m_tFrame.fRepeat)
        {
            if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
            {
                m_tFrame.fRepeat = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : SideWalk 끝 \n");
    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Throw 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
       
        // 투사체 발사 
        Engine::Add_GameObject(L"GameLogic", CThrowPipe::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.f, m_pTransformComp->Get_Pos().z, this, (ETEAM_ID)Get_TeamID()));
    }

    if (m_tState_Obj.Can_Update())
    {
        // 충돌체는 여기서 가상키 누르고 state에서 할예정 

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Throw 끝   \n");
    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"UpRightRun");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 13.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::UPRIGHT].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : UpRightRun 끝   \n");
    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Frighten 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Frighten");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::FRIGHTEN].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Frighten 끝   \n");
    }
    
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Attack 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Attack");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Attack 끝   \n");
    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 진입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeavyAttack");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_Block(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Block 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Block");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : HeavyAttack 끝   \n");
    }
}

void CGray::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : CrotchHit 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"CrotchHit");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 2.f; // 2초후 CHASE 진입 
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
        //OutputDebugString(L"▷Gray - 상태머신 : CrotchHit 끝   \n");
    }
}

void CGray::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : FacePunch 진입   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"FacePunch");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : FacePunch 끝   \n");
    }
}

void CGray::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 진입   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Falling"); 
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 끝   \n");
    }
}

void CGray::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 진입   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Coward");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Falling 끝   \n");
    }
}

void CGray::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Dazed   \n");
        // 일어나는것까지 진행함 
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Dazed");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 1.f;
        m_bDazedState = TRUE;
    }

    if (m_tState_Obj.Can_Update())
    {
        m_tFrame.fAge += 1.f * fDeltaTime;

        if (m_tFrame.fAge > m_tFrame.fLifeTime)
        {
            m_tFrame.fLifeTime = 0.f;
            m_tFrame.fAge = 0.f;
            m_bDazeToHeal = TRUE;
        }
        if (TRUE == m_bDazeToHeal)
        {
            if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
                m_tFrame.fFrame = 10.f;

            if (m_gHp.Update(fDeltaTime * 6.f, 40.f, TRUE)) // 증가값, 도달하면 bool반환 
            {
                m_gHp.Cur = 40.f;
                m_bDazedState = FALSE;
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Dazed 끝   \n");
    }
}

void CGray::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Chopped   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Chopped");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_bDeadState = TRUE;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame = m_tFrame.fFrameEnd - 1;
            // 이상태로 잠자는사운드 추가 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 상태머신 : Chopped 끝   \n");
    }
}

void CGray::AI_HeadShot(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  HeadShot   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeadShot");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 7.f;
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
        //OutputDebugString(L"▷Gray - 상태머신 : HeadShot 끝   \n");
    }
}

void CGray::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Headless   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Headless");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed =7.f;
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
        //OutputDebugString(L"▷Gray - 상태머신 : Headless 끝   \n");
    }
}

void CGray::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 상태머신 :  Death   \n");
        // 일어나는것까지 진행함 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Death");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 7.f;
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
        //OutputDebugString(L"▷Gray - 상태머신 : Death 끝   \n");
    }
}


#pragma endregion

#pragma region 행동 상태머신 부분 
void CGray::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 진입   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // 일반 다가오는파트 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        // 주시하며 경계
        if (m_mapActionKey[ACTION_KEY::KEEPEYE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        if (m_mapActionKey[ACTION_KEY::SIDEWALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        // 
        if (m_mapActionKey[ACTION_KEY::UPRIGHT].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        if (m_mapActionKey[ACTION_KEY::FRIGHTEN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        // 공격파트 
        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        // 패트롤파트 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);
    }

    if (m_tState_Act.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Idle 끝   \n");
    }
}

void CGray::Approach(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로 - 우선사항 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        //몬스터가 플레이어 바라보는 벡터 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);
        
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 9.f);
        
        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 6.5f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Approach 끝   \n");
    }
}

void CGray::SideMoving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Side - KeepEYE  OR  SideWalk 진입   \n");
    }

    // 실행
    {   
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        
        if (STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, 3.f);
        }

        if (STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, 4.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Side - KeepEYE  OR  SideWalk 끝   \n");
    }
}

void CGray::SuddenAttack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Sudden - Upright  OR  Frighten 진입   \n");
    }

    // 실행
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // 시야밖은 recon으로
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        else
        {
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
            D3DXVec3Normalize(&vDir, &vDir);

            if (STATE_OBJ::UPRIGHTRUN == m_tState_Obj.Get_State())
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);

            if (STATE_OBJ::FRIGHTEN == m_tState_Obj.Get_State())
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 6.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Sudden - Upright  OR  Frighten 끝   \n");
    }
}

void CGray::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"▷Gray - 행동머신 : Attack 진입   \n");
    }

    // 실행
    {
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);

        _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

        if (STATE_OBJ::ATTACK == m_tState_Obj.Get_State())
        {
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
        //OutputDebugString(L"▷Gray - 행동머신 : Attack 끝   \n");
    }
}

void CGray::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
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
            ////OutputDebugString(L"▷Brown - 기존 패트롤 포인트 복귀중   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 3.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion