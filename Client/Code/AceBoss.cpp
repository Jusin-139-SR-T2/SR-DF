#include "stdafx.h"
#include "AceBoss.h"

CAceBoss::CAceBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceBoss::CAceBoss(const CAceBoss& rhs)
	: Base(rhs)
{
}

CAceBoss::~CAceBoss()
{
}

HRESULT CAceBoss::Ready_GameObject()
{
	SUPER::Ready_GameObject();

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Idle_South");
	m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 12.f;
	m_tFrame.fRepeat = 0.f;
	m_fAwareness = m_tStat.fAwareness = 0.f;
	m_tStat.fMaxAwareness = 15.f;
	m_bDazedState = FALSE;
	m_bDeadState = FALSE;
	m_bDazeToHeal = FALSE;

	// 팀에이전트 셋팅 
	Set_TeamID(ETEAM_BOSS);

	// 조명관련
	m_tFrame.fAge = 0.f;
	m_tFrame.fLifeTime = 0.5f;

	// Status - 사거리는 DISTANCE구조체로 통일
	m_tStat.fAttackFov = 90.f;
	m_tStat.fAttackDistance = m_tDistance.TotalDist;
	m_gHp.Cur = 100.f;
	m_gHp.Max = 100.f;

	// 충돌용
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
	pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
	pShape->vHalfSize = { 1.f, 0.7f, 0.3f };
	
	// 블랙보드 등록 
	Engine::Add_BlackBoard(L"MonsterUnion", CBlackBoard_Monster::Create());

	// Phase - 패턴관련 
	m_ePhase = Engine::MonsterPhase::Intro;
	m_fTriggerHP = 90.f;

#pragma region 목표 상태머신 등록 - (AI) Judge
	m_tState_Obj.Set_State(STATE_OBJ::IDLE);

	// 경계
	m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CAceBoss::AI_Idle);
	m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CAceBoss::AI_Suspicious);
	m_tState_Obj.Add_Func(STATE_OBJ::REST, &CAceBoss::AI_Rest);
	m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CAceBoss::AI_Chase);

	// 추격
	m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CAceBoss::AI_Run);
	m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CAceBoss::AI_Walk);
	m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CAceBoss::AI_Reloading);

	// 전조
	m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CAceBoss::AI_PreAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CAceBoss::AI_SideReady);
	m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CAceBoss::AI_Roll);

	// 공격
	m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CAceBoss::AI_CloseAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CAceBoss::AI_Shoot);
	m_tState_Obj.Add_Func(STATE_OBJ::HEAVYSHOOT, &CAceBoss::AI_HeavyShoot);

	m_tState_Obj.Add_Func(STATE_OBJ::PHASE1_INSTALL, &CAceBoss::AI_InstallPh1);
	m_tState_Obj.Add_Func(STATE_OBJ::PHASE2_INSTALL, &CAceBoss::AI_InstallPh2);
	m_tState_Obj.Add_Func(STATE_OBJ::PHASE2_BUFF, &CAceBoss::AI_SkillBuff);

	m_tState_Obj.Add_Func(STATE_OBJ::FALLING_STONE, &CAceBoss::AI_SkillStone);
	m_tState_Obj.Add_Func(STATE_OBJ::ENERGY_BALL, &CAceBoss::AI_SkillEnergyBall);
	m_tState_Obj.Add_Func(STATE_OBJ::RED_THUNDER, &CAceBoss::AI_SkillThunder);
	m_tState_Obj.Add_Func(STATE_OBJ::ROUND_FIRE, &CAceBoss::AI_SkillFire);

	// 피격
	m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CAceBoss::AI_Hit);
	m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CAceBoss::AI_Dazed);
	m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CAceBoss::AI_FacePunch);
	m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CAceBoss::AI_Falling);
	m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CAceBoss::AI_CrotchHit);

	// 죽음
	m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CAceBoss::AI_Death);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting

	m_tState_Act.Set_State(STATE_ACT::IDLE);

	m_tState_Act.Add_Func(STATE_ACT::IDLE, &CAceBoss::Idle);
	m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CAceBoss::Approach);
	m_tState_Act.Add_Func(STATE_ACT::MOVING, &CAceBoss::Moving);
	m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::SHOOT, &CAceBoss::Shoot);

	m_tState_Act.Add_Func(STATE_ACT::INSTALL, &CAceBoss::LaserInstall);
	m_tState_Act.Add_Func(STATE_ACT::BUFF, &CAceBoss::BuffActive);

	m_tState_Act.Add_Func(STATE_ACT::SKILL_STONE, &CAceBoss::SkillStone);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_ENERGYBALL, &CAceBoss::SkillEnergyBall);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_THUNDER, &CAceBoss::SkillThunder);
	m_tState_Act.Add_Func(STATE_ACT::SKILL_FIRE, &CAceBoss::SkillFire);


#pragma endregion

#pragma region 액션 키 등록

	m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // 멀때 
	m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
	m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
	m_mapActionKey.Add_Action(ACTION_KEY::ROLL); 
	m_mapActionKey.Add_Action(ACTION_KEY::ATTACK);
	m_mapActionKey.Add_Action(ACTION_KEY::CLOSEATK);
	m_mapActionKey.Add_Action(ACTION_KEY::SHOOT);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_LASER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_STONE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_FIRE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_THUNDER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_ENERGYBALL);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_BUFF);

#pragma endregion

	return S_OK;
}

_int CAceBoss::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);

	// 지형타기 
	Height_On_Terrain();

	// 죽는모션이 하나뿐이라서 충돌체 가리지않고 그냥 0 되면 죽음 
	if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
		m_tState_Obj.Set_State(STATE_OBJ::DEATH);

	//보스 페이즈 갱신 
	Change_Phase();

	// 빌보드
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
	    
		if (STATE_OBJ::SHOOTING == m_tState_Obj.Get_State() ||
			STATE_OBJ::RUN == m_tState_Obj.Get_State() )
			m_tFrame.fRepeat += 1;
	}

#pragma region 테스트 장소 

    if (Engine::IsKey_Pressed(DIK_N)) // 1페이즈 
    {
		m_gHp.Cur = 85.f;
    }
	if (Engine::IsKey_Pressed(DIK_M)) // 2페이즈 
    {
		m_gHp.Cur = 45.f;
    }
	if (Engine::IsKey_Pressed(DIK_X))
	{
		m_gHp.Cur -= 5.f;
	}
	if (Engine::IsKey_Pressed(DIK_Z))
	{
		m_tState_Obj.Set_State(STATE_OBJ::RED_THUNDER);
	}
#pragma endregion 

	Update_InternalData(); //블랙보드에 값넣는중 

	//물리 업데이트 코드 - 콜라이더 위치 업데이트 
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); 

	// Renderer 
	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CAceBoss::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

#pragma region 기본 override 

void CAceBoss::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
	m_pBufferComp->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

CAceBoss* CAceBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		Safe_Release(pInstance);

		MSG_BOX("BOSS Monster Create Failed");
		return nullptr;
	}

	pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);

	return pInstance;
}

HRESULT CAceBoss::Add_Component()
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
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSSMONSTER); // 이 클래스가 속할 충돌레이어 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL | LAYER_PLAYER_ATTACK); // 얘랑 충돌해야하는 레이어들 - 투사체랑도 충돌할예정 

	return S_OK;
}

void CAceBoss::Free()
{
	SUPER::Free();
}

#pragma endregion 

#pragma region 충돌 

void CAceBoss::OnCollision(CGameObject* pDst)
{

}
void CAceBoss::OnCollisionEntered(CGameObject* pDst)
{
	if (Get_IsMonsterDeath()) // 죽었으면 더이상 충돌일어나지않게 먼저 막음 
		return;


	else if (25 >= m_gHp.Cur && FALSE == m_bDazedState)
	{
	
		m_tState_Obj.Set_State(STATE_OBJ::DAZED);
	}

	CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

	if (nullptr == pAceObj)
		return;

	// 현재 팀 : 보스팀  적대관계 : 플레이어, 몬스터 
	if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // 적대관계의 경우
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

		if (nullptr == pPlayer)
		{
			CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

			if (nullptr == pPlayerAttack)
				return; 
			//몬스터가 하는 공격은 보스에게 안통함. 따라서 몬스터의 공격은 적대임에도 보스에 넣지않음 
			
			//==== 플레이어 공격체와  충돌 =============================
			if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
			{
				OutputDebugString(L"▷Brown - 충돌판정 DAZED 진입   \n");
				m_tState_Obj.Set_State(STATE_OBJ::DAZED);
				return;
			}
			else if (CPlayer::STATE_RIGHTHAND::KICK == ePlayerRighthand)
				m_tState_Obj.Set_State(STATE_OBJ::FALLING); 
			else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // 앉 + kick = falling 임 
				m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
			else
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
			if (CPlayer::STATE_RIGHTHAND::RUN_HAND == ePlayerRighthand)
				m_tState_Obj.Set_State(STATE_OBJ::FALLING); //달릴때 

		}
	}
	else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // 오브젝트 충돌 
	{
		m_tState_Obj.Set_State(STATE_OBJ::HIT);
	}
}

void CAceBoss::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 

#pragma region 블랙보드 
// 블랙보드로 업데이트 
void CAceBoss::Update_InternalData()
{  
	// 블랙보드 연결 대기, 안전 코드로 필수
	if (!m_wpBlackBoard_Monster.Get_BlackBoard())
	{
		m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
		// 연결 실패
		if (!m_wpBlackBoard_Monster.Get_BlackBoard())
			return;
	}

	// 안전 코드를 거치면 일반 포인터로 접근 허용.
	CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

	// 여기서부터 블랙보드의 정보를 업데이트 한다.
	pBlackBoard->Set_ControlLight(m_bLightOn) ;
}

//블랙보드에서 다운로드 
void CAceBoss::Update_BlackBoard()
{
	//// 블랙보드 연결 대기, 안전 코드로 필수
	//if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//{
	//	m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"BossLight"));
	//	// 연결 실패
	//	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//		return;
	//}

	//// 안전 코드를 거치면 일반 포인터로 접근 허용.
	//CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	//// 여기서부터 블랙보드의 정보를 얻어온다.
	//m_fHp = pBlackBoard->Get_HP().Cur;
}

#pragma endregion 

#pragma region 기타함수 
void CAceBoss::Change_Phase()
{
	if (50.f >= m_gHp.Cur) // 50 이하 = Phase 2
		m_ePhase = Engine::MonsterPhase::Phase2;
	else if (95.f >= m_gHp.Cur && 50.f < m_gHp.Cur) // 50 ~ 95 = Phase 1
		m_ePhase = Engine::MonsterPhase::Phase1;
	else
		m_ePhase = Engine::MonsterPhase::Intro;
}

void CAceBoss::LightControl(const _float& fTimeDelta)
{
	if (m_bLightOn)
	{
		m_tFrame.fAge += 1.f * fTimeDelta;  

		if (m_tFrame.fAge >= m_tFrame.fLifeTime)
		{
			m_bLightOn = false;  //시간경과하면 라이트 끄기 
			m_tFrame.fAge = 0.f;
		}
	}
}


#pragma endregion 

#pragma region 상태머신 : idle ~ Death

void CAceBoss::AI_Idle(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"▷BOSS - 상태머신 : idle 돌입   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;

		Engine::Add_GameObject(L"GameLogic", CBossLight::Create(m_pGraphicDev, this));
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
        //OutputDebugString(L"▷BOSS - 상태머신 : idle 끝 \n");
    }
}

void CAceBoss::AI_Suspicious(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;

		Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
		m_pTransformComp->Get_Pos().x + 0.1f, m_pTransformComp->Get_Pos().y + 1.3f, m_pTransformComp->Get_Pos().z, CAwareness::TYPE::BOSS, this));
	}

	if (m_tState_Obj.Can_Update())
	{
		if (Detect_Player()) 
		{
			m_tStat.fAwareness += fDeltaTime * 4.f;

			if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
			{
				m_tStat.fAwareness = m_tStat.fMaxAwareness; 
				m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
			}
		}
		else 
		{
			m_tStat.fAwareness -= fDeltaTime * 2.f;

			if (0 >= m_tStat.fAwareness)
			{
				m_tStat.fAwareness = 0.f;
				m_tState_Obj.Set_State(STATE_OBJ::IDLE);
			}
		}
	};

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 끝   \n");
	}
}

void CAceBoss::AI_Reloading(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Reloading 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;

		if(Engine::MonsterPhase::Phase2 == m_ePhase) // 속도 상승 
			m_tFrame.fFrameSpeed = 11.f;

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
		//OutputDebugString(L"▷BOSS - 상태머신 : Reloading 끝   \n");
	}
}

void CAceBoss::AI_Rest(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Rest 돌입   \n");
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady"); // ♣ 이미지 바꾸기 ? 
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
		//OutputDebugString(L"▷BOSS - 상태머신 : Rest 끝 \n");
	}
}

void CAceBoss::AI_Chase(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		m_tStat.fAwareness = m_tStat.fMaxAwareness;
		m_AttackOnce = FALSE;
	}

	if (m_tState_Obj.Can_Update())
	{
		_float CurDist = Calc_Distance();

		if (Engine::MonsterPhase::Intro == m_ePhase)
		{

			// 보스맵 전체가 보스 시야에 들어오므로 처음에는 달리거나 하지않음
			if (m_tDistance.INTRO_ShootDist < CurDist) // 15 < a 초과 
			{
				//OutputDebugString(L"★★★★★ Chase - 인트로 - WALK \n");
				m_tState_Obj.Set_State(STATE_OBJ::WALK);
			}
			else if (m_tDistance.INTRO_CloseAtkDist < CurDist && 
					 m_tDistance.INTRO_ShootDist >= CurDist ) // 1.5 < a <= 15이하
			{
				//OutputDebugString(L"★★★★★  Chase - 인트로 - SHOOTING \n");
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.INTRO_CloseAtkDist >= CurDist) // a <= 1.5  
			{
				//OutputDebugString(L"★★★★★  Chase - 인트로 - CLOSEATTACK \n");
				m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
			}
		}
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			_int HP = _int(m_gHp.Cur);

			if (!m_bPhaseStart) //1회용 설치기 
			{
				m_fTriggerHP = 90.f;
				m_bPhaseStart = TRUE;
				m_tState_Obj.Set_State(STATE_OBJ::PHASE1_INSTALL);
			}
			else if(m_fTriggerHP > HP) // 10단위 깎을때마다 나오는 패턴 
			{
				m_fTriggerHP -= 10.f;
				m_tState_Obj.Set_State(STATE_OBJ::ENERGY_BALL); // EnergyBall
			}
			else if (Random_variable(65)) //60% 확률로 스킬공격이 들어간다. 
			{
				m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE); // FallingStone
			}
			// ======= 여기부터는 거리비교 일반공격 =======
			else if (m_tDistance.Ph1_RunDist < CurDist) // 13 < a 초과 
			{							
				m_tState_Obj.Set_State(STATE_OBJ::RUN);
			}
			else if (m_tDistance.Ph1_ShootDist < CurDist &&	m_tDistance.Ph1_RunDist >= CurDist) // 5 < a <= 13이하
			{							
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.Ph1_CloseAtkDist < CurDist &&	m_tDistance.Ph1_RollDist >= CurDist) // 2 < a <= 5이하
			{							
				m_tState_Obj.Set_State(STATE_OBJ::ROLL);
			}
			else if (m_tDistance.Ph1_CloseAtkDist >= CurDist) // a <= 2 
			{							
				m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
			}

		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			_int HP = _int(m_gHp.Cur);

			if (m_bPhaseStart) // 관문 시작스킬 
			{
				m_fTriggerHP = 50.f;
				m_bPhaseStart = FALSE;
				m_tState_Obj.Set_State(STATE_OBJ::PHASE2_BUFF);
			}
			else if (m_fTriggerHP > HP) // 10단위 hp깎을때마다 
			{
				m_fTriggerHP -= 8.f;
				m_tState_Obj.Set_State(STATE_OBJ::RED_THUNDER); // EnergyBall
			}
			//else if (m_gHp.Cur < 25.f) // 레이저 설치할게 마땅치않아서 그냥 주석처리 
			//{
			//	m_tState_Obj.Set_State(STATE_OBJ::PHASE2_INSTALL); // EnergyBall
			//}
			else if (Random_variable(75)) //75% 확률로 스킬공격이 들어간다. 
			{
				// 그중 25프로 확률로 fire, 50프로 확률로 에너지볼, 둘다 안되면 돌떨어짐 
				// 모든 확률은 독립시행 
				if(Random_variable(25)) 
					m_tState_Obj.Set_State(STATE_OBJ::ROUND_FIRE); // FallingStone
				else if(Random_variable(50))
					m_tState_Obj.Set_State(STATE_OBJ::ENERGY_BALL); // FallingStone
				else
					m_tState_Obj.Set_State(STATE_OBJ::FALLING_STONE); // FallingStone
			}
			// ======= 여기부터는 거리비교 일반공격 =======
			else if (m_tDistance.Ph2_RunDist < CurDist) // 15 < a 초과 
			{
				m_tState_Obj.Set_State(STATE_OBJ::RUN);
			}
			else if (m_tDistance.Ph2_SideReadyDist < CurDist && m_tDistance.Ph2_RunDist >= CurDist) // 5 < a <= 13이하
			{
				m_tState_Obj.Set_State(STATE_OBJ::SIDE_READY);
			}
			else if (m_tDistance.Ph2_RollDist < CurDist && m_tDistance.Ph2_SideReadyDist >= CurDist) // 2 < a <= 5이하
			{
				m_tState_Obj.Set_State(STATE_OBJ::ROLL);
			}
			else if (m_tDistance.Ph2_ShootDist < CurDist && m_tDistance.Ph2_RollDist >= CurDist) // 2 < a <= 5이하
			{
				m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
			}
			else if (m_tDistance.Ph1_CloseAtkDist >= CurDist) // a <= 2 
			{
				m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
			}

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Chase 끝   \n");
	}
}

void CAceBoss::AI_Run(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Run 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run"); // ♣ 이미지 바꾸기 ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 11.f;

		if(Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 14.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 Run > Run가상키  \n");
			m_mapActionKey[ACTION_KEY::RUN].Act();
		}

		if (m_tFrame.fRepeat >= 2)
		{
			//OutputDebugString(L"★★★★★ 상태머신 Run -> SHOOTING 다시 진입   \n");
			m_tFrame.fRepeat = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Run 끝 \n");
	}
}

void CAceBoss::AI_Walk(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Walk 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk"); // ♣ 이미지 바꾸기 ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 14.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 Walk > walk가상키  \n");
			m_mapActionKey[ACTION_KEY::WALK].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			//OutputDebugString(L"★★★★★ 상태머신 walk -> Chase 다시 진입   \n");
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Walk 끝 \n");
	}
}

void CAceBoss::AI_PreAttack(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Pre_Attack 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Pre_Attack"); // ♣ 이미지 바꾸기 ? 
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Pre_Attack 끝 \n");
	}
}

void CAceBoss::AI_SideReady(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : SideReady 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"SideReady");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::HEAVYSHOOT);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : CloseAttack 끝 \n");
	}
}

void CAceBoss::AI_Roll(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Roll 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 11.f;

		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 14.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::ROLL].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Roll 끝 \n");
	}
}


void CAceBoss::AI_CloseAttack(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : CloseAttack 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"CloseAttack");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 CloseAttack > CLOSEATK 가상키  \n");
			m_mapActionKey[ACTION_KEY::CLOSEATK].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			//OutputDebugString(L"★★★★★ 상태머신 CloseAttack -> Chase 다시 진입   \n");
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : CloseAttack 끝 \n");
	}
}

void CAceBoss::AI_Shoot(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Shooting 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 13.f;

		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 16.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 Shoot > SHOOT가상키  \n");
			m_mapActionKey[ACTION_KEY::SHOOT].Act();
		}

		if (m_tFrame.fFrame <= 1)
		{
			m_bLightOn = true;
		}

		LightControl(fDeltaTime);
		
		if (4 <= m_tFrame.fRepeat)
		{
			m_bLightOn = FALSE;
			m_tFrame.fRepeat = 0;
			//m_iBulletCnt -= 1; // 장전수 감소 
			//OutputDebugString(L"★★★★★ 상태머신 Shooting -> Chase 다시 진입   \n");
			m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Shooting 끝   \n");
	}
}

void CAceBoss::AI_HeavyShoot(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Shooting 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime) // 겨눈상태로 대기 - HeavyShot이므로 
		{
			m_tFrame.fFrame = 0.f;
		}
		else
		{
			if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
			{
				//OutputDebugString(L"★★★★★ 상태머신 Shoot > SHOOT가상키  \n");
				m_mapActionKey[ACTION_KEY::SHOOT].Act();
			}

			if (m_tFrame.fFrame <= 1)
			{
				m_bLightOn = true;
			}
			LightControl(fDeltaTime);
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_fSideAge = 0.f;
			m_bLightOn = FALSE;
			m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Shooting 끝   \n");
	}
}

void CAceBoss::AI_SkillStone(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Phase1 Skill 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Evasion");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		_int HP = _int(m_gHp.Cur);

		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_STONE].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_SkillEnergyBall(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Phase1 EnergyBall 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Evasion");
		m_tFrame.fFrameEnd = float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 8.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_ENERGYBALL].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_InstallPh1(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"SetOn");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 SKILL_LASER 가상키  \n");
			m_mapActionKey[ACTION_KEY::SKILL_LASER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : CloseAttack 끝 \n");
	}

}
void CAceBoss::AI_SkillThunder(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : Thunder 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 SKILL_THUNDER 가상키  \n");
			m_mapActionKey[ACTION_KEY::SKILL_THUNDER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : SKILL_THUNDER 끝 \n");
	}
}

void CAceBoss::AI_SkillFire(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 :SKILL_FIRE 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 SKILL_FIRE 가상키  \n");
			m_mapActionKey[ACTION_KEY::SKILL_FIRE].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : SKILL_FIRE 끝 \n");
	}
}

void CAceBoss::AI_SkillBuff(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 2관문 버프 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shout");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			//OutputDebugString(L"★★★★★ 상태머신 SKILL_BUFF 가상키  \n");
			m_mapActionKey[ACTION_KEY::SKILL_BUFF].Act();
		}

		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::CHASE);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : CloseAttack 끝 \n");
	}
}

void CAceBoss::AI_InstallPh2(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"SetOn");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
		{
			m_mapActionKey[ACTION_KEY::SKILL_LASER].Act();
		}
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Hit(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 일반피격상태 돌입   \n");
		if (Random_variable(50))
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Hit_A");
		else
			m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Hit_B");

		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Dazed(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Dazed");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_tFrame.fLifeTime = 1.f;
		m_bDazedState = TRUE;
		
		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 12.f;
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
	}
}

void CAceBoss::AI_Falling(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Falling");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_FacePunch(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"FacePunch");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		
		if (Engine::MonsterPhase::Phase2 == m_ePhase)
			m_tFrame.fFrameSpeed = 14.f;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_CrotchHit(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 하단피격 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"CrotchHit");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	}

	if (m_tState_Obj.Can_Update())
	{
		m_fSideAge += 1.f * fDeltaTime;

		if (m_fSideAge > m_fSideTime)
		{
			m_fSideAge = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);
		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

void CAceBoss::AI_Death(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 상태머신 : 1관문 설치기 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Dazed");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;
		m_bDeadState = TRUE;
	}

	if (m_tState_Obj.Can_Update())
	{
		if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
		{
			m_tFrame.fFrame = 0.f;
			m_tState_Obj.Set_State(STATE_OBJ::REST);

		}
	}

	if (m_tState_Obj.IsState_Exit())
	{
	}
}

#pragma endregion 

#pragma region 행동머신
void CAceBoss::Idle(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered()) // 1차 bIsEnter false로 바꿈 
	{
		//OutputDebugString(L"▷BOSS - 행동 : Idle 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Intro == m_ePhase)
		{
			if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
			//else if (m_mapActionKey[ACTION_KEY::ATTACK].IsOnAct()); //CloseAtk
			//{
			//	//OutputDebugString(L"★★★★★ 행동 idle -> ATTACK 진입   \n"); 
			//	m_tState_Act.Set_State(STATE_ACT::ATTACK);
			//}

			// Q. 대체왜 여기서 자꾸 ATTACK으로 ? ACT안했는데도 ?? ??????????
		}// 여기부터 스킬시전루트 
		// ===== 관문 시작 스킬 =====
		else if (m_mapActionKey[ACTION_KEY::SKILL_BUFF].IsOnAct()) // 버프 
			m_tState_Act.Set_State(STATE_ACT::BUFF);
		else if (m_mapActionKey[ACTION_KEY::SKILL_LASER].IsOnAct()) // Laser 설치 
			m_tState_Act.Set_State(STATE_ACT::INSTALL);
		// ===== 일반스킬 ===== 
		else if (m_mapActionKey[ACTION_KEY::SKILL_STONE].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_STONE);
		else if (m_mapActionKey[ACTION_KEY::SKILL_ENERGYBALL].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_ENERGYBALL);
		else if (m_mapActionKey[ACTION_KEY::SKILL_THUNDER].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_THUNDER);
		else if (m_mapActionKey[ACTION_KEY::SKILL_FIRE].IsOnAct())
			m_tState_Act.Set_State(STATE_ACT::SKILL_FIRE);
		// ===== 1페이즈 일반공격라인 
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::MOVING);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//Shooting + CloseAtk 
		{
			if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // WALK
				m_tState_Act.Set_State(STATE_ACT::APPROACH);
			else if (m_mapActionKey[ACTION_KEY::SHOOT].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::SHOOT);
			else if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct()) // Shooting
				m_tState_Act.Set_State(STATE_ACT::MOVING);
			else if (m_mapActionKey[ACTION_KEY::CLOSEATK].IsOnAct()) // CloseAtk
				m_tState_Act.Set_State(STATE_ACT::ATTACK);
		}	
	}
	
	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Idle 끝   \n");
	}
}

void CAceBoss::Approach(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Approach 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
		D3DXVec3Normalize(&vDir, &vDir);

		if (Engine::MonsterPhase::Intro == m_ePhase)// WALK
				m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 4.f);
		else if (Engine::MonsterPhase::Phase1 == m_ePhase) // RUN
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 8.f);
		else if (Engine::MonsterPhase::Phase2 == m_ePhase) //RUN
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 10.f);

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Approaching 끝   \n");
	}
}

void CAceBoss::Moving(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Moving  - Roll 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		vDir = m_pTransformComp->Get_Right();
		D3DXVec3Normalize(&vDir, &vDir);

		if (Engine::MonsterPhase::Phase1 == m_ePhase)//CloseAtk 
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//CloseAtk 
			m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 7.f);

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Attack 끝   \n");
	}
}

void CAceBoss::Attack(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Attack 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Intro == m_ePhase || Engine::MonsterPhase::Phase1 == m_ePhase )//CloseAtk 
		{
			if (STATE_OBJ::CLOSEATTACK == m_tState_Obj.Get_State())
			{
				//OutputDebugString(L" 행동 INTRO - Attack -> CloseAttack 진입   \n");
				if (!m_AttackOnce)
				{
					_vec3 vLook = m_pTransformComp->Get_Look();
					D3DXVec3Normalize(&vLook, &vLook);

					vDir = m_pTransformComp->Get_Pos() + vLook * 1;

					Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
						vDir.x, vDir.y, vDir.z, CMonsterPunch::TYPE::NORMAL, this, (ETEAM_ID)Get_TeamID()));

					m_AttackOnce = TRUE;
				}
			}
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//Shooting + CloseAtk 
		{
			if (STATE_OBJ::CLOSEATTACK == m_tState_Obj.Get_State())
			{
				//OutputDebugString(L" 행동 INTRO - Attack -> CloseAttack 진입   \n");
				if (!m_AttackOnce)
				{
					_vec3 vLook = m_pTransformComp->Get_Look();
					D3DXVec3Normalize(&vLook, &vLook);

					vDir = m_pTransformComp->Get_Pos() + vLook * 1;

					Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev,
						vDir.x, vDir.y, vDir.z, CMonsterPunch::TYPE::HEAVY, this, (ETEAM_ID)Get_TeamID()));

					m_AttackOnce = TRUE;
				}
			}
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Attack 끝   \n");
	}
}

void CAceBoss::Shoot(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Shoot 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Intro == m_ePhase)//Shooting + CloseAtk 
		{
		}
		else if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)//Shooting + CloseAtk 
		{
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : Shoot 끝   \n");
	}
}

void CAceBoss::LaserInstall(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : LaserInstall 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Phase1 == m_ePhase)
		{
			if (!m_AttackOnce)
			{
				_float PosZ = m_pTransformComp->Get_Pos().z;
				_float PosY = m_pTransformComp->Get_Pos().y;

				for (_int i = 0; i < 5; ++i)
				{
					Engine::Add_GameObject(L"GameLogic", CRedLaser::Create(m_pGraphicDev,
						m_pTransformComp->Get_Pos().x,
						Get_RandomFloat(PosY - 1.f, PosY + 5.f),
						Get_RandomFloat(PosZ - 3.f, PosZ + 3.f), this, (ETEAM_ID)Get_TeamID()));
				}
				m_AttackOnce = TRUE;
			}
		}
		else if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			if (!m_AttackOnce)
			{
				//레이저 설치 조건 


				m_AttackOnce = TRUE;
			}
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : LaserInstall 끝   \n");
	}
}

void CAceBoss::BuffActive(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - 행동 : BuffActive 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (Engine::MonsterPhase::Phase2 == m_ePhase)
		{
			if (!m_bBuffActive)
			{
				Engine::Add_GameObject(L"GameLogic", CBlueBuff::Create(m_pGraphicDev,
					m_pTransformComp->Get_Pos().x,
					m_pTransformComp->Get_Pos().y,
					m_pTransformComp->Get_Pos().z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
		
			m_bBuffActive = TRUE;
		}

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
		//OutputDebugString(L"▷BOSS - 행동 : BuffActive 끝   \n");
	}
}

void CAceBoss::SkillStone(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - Skill - SKILL_STONE 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			_int iCnt = 0;

			if (Engine::MonsterPhase::Phase1 == m_ePhase)
				iCnt = 4;
			else if (Engine::MonsterPhase::Phase2 == m_ePhase)
				iCnt = 6;

			for (_int i = 0; i < iCnt; ++i)
			{
				_vec3 randomCenter;
				_vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();

				GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, 6.f);

				Engine::Add_GameObject(L"GameLogic", CFallingStone::Create(m_pGraphicDev,
					randomCenter.x,
					randomCenter.y,
					randomCenter.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
		}
		m_AttackOnce = true;

		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillEnergyBall(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - Skill - EnergyBall 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{	
		if (!m_AttackOnce)
		{
			_int iCnt = 0;

			if (Engine::MonsterPhase::Phase1 == m_ePhase)
				iCnt = 4;
			else if (Engine::MonsterPhase::Phase2 == m_ePhase)
				iCnt = 5;
			
			// 몬스터 위치에서 Right 벡터 방향으로 spacing 간격을 두고 소환 위치 계산
			// 좌우 3개씩 총6개, 보스쪽은 없어서 일종의 딜타임이기도함
			for (_int i = 1; i < iCnt; ++i)
			{
				_vec3 Pos1 = m_pTransformComp->Get_Pos() + m_pTransformComp->Get_Right() * _float(i);
				_vec3 Pos2 = m_pTransformComp->Get_Pos() - m_pTransformComp->Get_Right() * _float(i);

				Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
					Pos1.x, Pos1.y, Pos1.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));

				Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
					Pos2.x, Pos2.y, Pos2.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
			m_AttackOnce = true;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillThunder(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - Skill - SKILL_STONE 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			for (_int i = 0 ; i < 11; ++i)
			{
				_vec3 randomCenter;
				_vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();

				GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, 8.f);

				Engine::Add_GameObject(L"GameLogic", CRedThunder::Create(m_pGraphicDev,
					randomCenter.x,
					randomCenter.y,
					randomCenter.z, m_ePhase, this, (ETEAM_ID)Get_TeamID() ));
			}
			m_AttackOnce = true;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

void CAceBoss::SkillFire(float fDeltaTime)
{
	if (m_tState_Act.IsState_Entered())
	{
		//OutputDebugString(L"▷BOSS - Skill - SKILL_FIRE 진입   \n");
	}

	if (m_tState_Act.Can_Update())
	{
		if (!m_AttackOnce)
		{
			_vec3 Center = m_pTransformComp->Get_Pos();
			_vec3 spawnPosition;

			for (_int i = 0; i < 8; ++i)
			{
				_float angle = static_cast<_float>(i) / static_cast<_float>(8) * D3DX_PI * 2.0f;

				spawnPosition.x = Center.x + 4.f * cosf(angle);
				spawnPosition.y = Center.y;
				spawnPosition.z = Center.z + 4.f * sinf(angle);

				Engine::Add_GameObject(L"GameLogic", CSpawnFire::Create(m_pGraphicDev,
					spawnPosition.x, spawnPosition.y, spawnPosition.z, m_ePhase, this, (ETEAM_ID)Get_TeamID()));
			}
			m_AttackOnce = TRUE;
		}
		m_tState_Act.Set_State(STATE_ACT::IDLE);
	}

	if (m_tState_Act.IsState_Exit())
	{
	}
}

#pragma endregion 