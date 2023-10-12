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
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Idle_South");
	m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
	m_tFrame.fFrame = 0.f;
	m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
	m_tFrame.fFrameSpeed = 12.f;
	m_tFrame.fRepeat = 0.f;
	m_tStat.fAwareness = 0.f;
	m_tStat.fMaxAwareness = 15.f;


	// Status
	m_tStat.fAttackDistance = 16.f;
	m_tStat.fAttackFov = 90.f;
	m_gHp.Cur = 100.f;
	m_gHp.Max = 100.f;

	// 충돌용
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // 충돌 불러오는곳 
	pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
	pShape->vHalfSize = { 1.f, 0.7f, 0.3f };
	
	//블랙보드 등록 
	Engine::Add_BlackBoard(L"MonsterUnion", CBlackBoard_Monster::Create());

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
	m_tState_Obj.Add_Func(STATE_OBJ::SHOUT, &CAceBoss::AI_Shout);

	// 공격
	m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CAceBoss::AI_CloseAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SHOOT, &CAceBoss::AI_Shoot);
	m_tState_Obj.Add_Func(STATE_OBJ::SKILLATTACK, &CAceBoss::AI_SkillAttack);

	// 피격
	m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CAceBoss::AI_Hit);
	m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CAceBoss::AI_Dazed);
	m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CAceBoss::AI_FacePunch);
	m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CAceBoss::AI_Falling);
	m_tState_Obj.Add_Func(STATE_OBJ::EVASION, &CAceBoss::AI_Evasion);

	// 죽음
	m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CAceBoss::AI_Death);

	// 복귀
	m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CAceBoss::AI_Reconnaissance);
	m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CAceBoss::AI_GoHome);


#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
	m_tState_Act.Set_State(STATE_ACT::IDLE);
	// STATE가 ::A 일때 CAceBoss클래스의 ::B 함수를 수행하는 상태머신 
	m_tState_Act.Add_Func(STATE_ACT::IDLE, &CAceBoss::Idle);
	m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CAceBoss::Approach);
	m_tState_Act.Add_Func(STATE_ACT::MOVING, &CAceBoss::Moving);
	m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::SKILL, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CAceBoss::GoHome);

#pragma endregion

#pragma region 액션 키 등록
	m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // 멀때 
	m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
	m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
	m_mapActionKey.Add_Action(ACTION_KEY::ROLL); 
	m_mapActionKey.Add_Action(ACTION_KEY::ATTACK);
	m_mapActionKey.Add_Action(ACTION_KEY::SHOOT);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_LASER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_STONE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_FIRE);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_THUNDER);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_ENERGYBALL);
	m_mapActionKey.Add_Action(ACTION_KEY::SKILL_BUFF);
	m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

	return S_OK;
}

_int CAceBoss::Update_GameObject(const _float& fTimeDelta)
{
	SUPER::Update_GameObject(fTimeDelta);
	
	// 위치값 가져오기 
	Get_PlayerPos();

	// 지형타기 
	Height_On_Terrain();

	// 빌보드
	Billboard(fTimeDelta);


	//상태머신
	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
	m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
	m_mapActionKey.Update();	// 액션키 초기화

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;
	    if (STATE_OBJ::WALK == m_tState_Obj.Get_State() ||
	        STATE_OBJ::RUN == m_tState_Obj.Get_State() ||
	        STATE_OBJ::SHOOT == m_tState_Obj.Get_State())
			m_tFrame.fRepeat += 1;
	}


#pragma region 테스트 장소 

    if (Engine::IsKey_Pressed(DIK_N))
    {
		m_bLightOn  = !m_bLightOn;
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
		Engine::Add_GameObject(L"GameLogic", CRedThunder::Create(m_pGraphicDev, 5.f, 15.f, 15.f, MonsterPhase::Phase1, this));
		
        swprintf_s(debugString, L"BOSS - 변수 확인 m_tState_Obj = %d\n", m_tState_Obj);
        //OutputDebugStringW(debugString);
    }

#pragma endregion 

	Update_InternalData();

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
	NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
	NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

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
}

void CAceBoss::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 

#pragma region 블랙보드 

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
	pBlackBoard->Get_BossAwareness().Cur = m_tStat.fAwareness;
}

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

#pragma region 상태머신 : idle ~ Death

void CAceBoss::AI_Idle(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"▷BOSS - 상태머신 : idle 돌입   \n");
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
        OutputDebugString(L"▷BOSS - 상태머신 : idle 끝 \n");
    }
}

void CAceBoss::AI_Suspicious(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 돌입   \n");

		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 10.f;

		Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
		m_pTransformComp->Get_Pos().x + 0.1f, m_pTransformComp->Get_Pos().y + 1.3f, m_pTransformComp->Get_Pos().z, CAwareness::TYPE::BROWN, this));
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
		OutputDebugString(L"▷BOSS - 상태머신 : Suspicious 끝   \n");
	}
}

void CAceBoss::AI_Rest(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"▷BOSS - 상태머신 : Rest 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
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
		OutputDebugString(L"▷BOSS - 상태머신 : Rest 끝 \n");
	}
}

void CAceBoss::AI_Reloading(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"▷BOSS - 상태머신 : Reloading 돌입   \n");
		m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
		m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
		m_tFrame.fFrameSpeed = 7.f;
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
		OutputDebugString(L"▷BOSS - 상태머신 : Reloading 끝   \n");
	}
}

void CAceBoss::AI_Chase(float fDeltaTime)
{
}

void CAceBoss::AI_Run(float fDeltaTime)
{
}

void CAceBoss::AI_Walk(float fDeltaTime)
{
}

void CAceBoss::AI_PreAttack(float fDeltaTime)
{
}

void CAceBoss::AI_SideReady(float fDeltaTime)
{
}

void CAceBoss::AI_Roll(float fDeltaTime)
{
}

void CAceBoss::AI_Shout(float fDeltaTime)
{
}

void CAceBoss::AI_CloseAttack(float fDeltaTime)
{
}

void CAceBoss::AI_Shoot(float fDeltaTime)
{
}

void CAceBoss::AI_SkillAttack(float fDeltaTime)
{
}

void CAceBoss::AI_Hit(float fDeltaTime)
{
}

void CAceBoss::AI_Dazed(float fDeltaTime)
{
}

void CAceBoss::AI_Falling(float fDeltaTime)
{
}

void CAceBoss::AI_Evasion(float fDeltaTime)
{
}

void CAceBoss::AI_FacePunch(float fDeltaTime)
{
}

void CAceBoss::AI_Death(float fDeltaTime)
{
}

void CAceBoss::AI_Reconnaissance(float fDeltaTime)
{
}

void CAceBoss::AI_GoHome(float fDeltaTime)
{
}

#pragma endregion 

#pragma region 행동머신
void CAceBoss::Idle(float fDeltaTime)
{
}

void CAceBoss::Approach(float fDeltaTime)
{
}

void CAceBoss::Moving(float fDeltaTime)
{
}

void CAceBoss::Attack(float fDeltaTime)
{
}

void CAceBoss::Skill(float fDeltaTime)
{
}

void CAceBoss::GoHome(float fDeltaTime)
{
}

#pragma endregion 