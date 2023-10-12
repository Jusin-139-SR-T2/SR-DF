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

	// �浹��
	m_pTransformComp->Readjust_Transform();
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
	pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
	pShape->vHalfSize = { 1.f, 0.7f, 0.3f };
	
	//������ ��� 
	Engine::Add_BlackBoard(L"MonsterUnion", CBlackBoard_Monster::Create());

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
	m_tState_Obj.Set_State(STATE_OBJ::IDLE);

	// ���
	m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CAceBoss::AI_Idle);
	m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CAceBoss::AI_Suspicious);
	m_tState_Obj.Add_Func(STATE_OBJ::REST, &CAceBoss::AI_Rest);
	m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CAceBoss::AI_Chase);

	// �߰�
	m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CAceBoss::AI_Run);
	m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CAceBoss::AI_Walk);
	m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CAceBoss::AI_Reloading);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CAceBoss::AI_PreAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CAceBoss::AI_SideReady);
	m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CAceBoss::AI_Roll);
	m_tState_Obj.Add_Func(STATE_OBJ::SHOUT, &CAceBoss::AI_Shout);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CAceBoss::AI_CloseAttack);
	m_tState_Obj.Add_Func(STATE_OBJ::SHOOT, &CAceBoss::AI_Shoot);
	m_tState_Obj.Add_Func(STATE_OBJ::SKILLATTACK, &CAceBoss::AI_SkillAttack);

	// �ǰ�
	m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CAceBoss::AI_Hit);
	m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CAceBoss::AI_Dazed);
	m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CAceBoss::AI_FacePunch);
	m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CAceBoss::AI_Falling);
	m_tState_Obj.Add_Func(STATE_OBJ::EVASION, &CAceBoss::AI_Evasion);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CAceBoss::AI_Death);

	// ����
	m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CAceBoss::AI_Reconnaissance);
	m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CAceBoss::AI_GoHome);


#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
	m_tState_Act.Set_State(STATE_ACT::IDLE);
	// STATE�� ::A �϶� CAceBossŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
	m_tState_Act.Add_Func(STATE_ACT::IDLE, &CAceBoss::Idle);
	m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CAceBoss::Approach);
	m_tState_Act.Add_Func(STATE_ACT::MOVING, &CAceBoss::Moving);
	m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::SKILL, &CAceBoss::Attack);
	m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CAceBoss::GoHome);

#pragma endregion

#pragma region �׼� Ű ���
	m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // �ֶ� 
	m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
	m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
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
	
	// ��ġ�� �������� 
	Get_PlayerPos();

	// ����Ÿ�� 
	Height_On_Terrain();

	// ������
	Billboard(fTimeDelta);


	//���¸ӽ�
	m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

	m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
	m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
	m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

	if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
	{
		m_tFrame.fFrame = 0.f;
	    if (STATE_OBJ::WALK == m_tState_Obj.Get_State() ||
	        STATE_OBJ::RUN == m_tState_Obj.Get_State() ||
	        STATE_OBJ::SHOOT == m_tState_Obj.Get_State())
			m_tFrame.fRepeat += 1;
	}


#pragma region �׽�Ʈ ��� 

    if (Engine::IsKey_Pressed(DIK_N))
    {
		m_bLightOn  = !m_bLightOn;
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
		Engine::Add_GameObject(L"GameLogic", CRedThunder::Create(m_pGraphicDev, 5.f, 15.f, 15.f, MonsterPhase::Phase1, this));
		
        swprintf_s(debugString, L"BOSS - ���� Ȯ�� m_tState_Obj = %d\n", m_tState_Obj);
        //OutputDebugStringW(debugString);
    }

#pragma endregion 

	Update_InternalData();

	//���� ������Ʈ �ڵ� - �ݶ��̴� ��ġ ������Ʈ 
	m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); 

	// Renderer 
	Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

	return S_OK;
}

void CAceBoss::LateUpdate_GameObject()
{
	SUPER::LateUpdate_GameObject();
}

#pragma region �⺻ override 

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

	// �ݶ��̴� ������Ʈ
	NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

	// ���� ���� ���
	m_pColliderComp->EnterToPhysics(0);

	// �浹 �Լ� ����
	m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
	m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
	m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

	// �浹 ���̾�, ����ũ ����
	m_pColliderComp->Set_CollisionLayer(LAYER_BOSSMONSTER); // �� Ŭ������ ���� �浹���̾� 
	m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_MONSTER | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

	return S_OK;
}

void CAceBoss::Free()
{
	SUPER::Free();
}

#pragma endregion 

#pragma region �浹 

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

#pragma region ������ 

void CAceBoss::Update_InternalData()
{  
	// ������ ���� ���, ���� �ڵ�� �ʼ�
	if (!m_wpBlackBoard_Monster.Get_BlackBoard())
	{
		m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"MonsterUnion"));
		// ���� ����
		if (!m_wpBlackBoard_Monster.Get_BlackBoard())
			return;
	}

	// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

	// ���⼭���� �������� ������ ������Ʈ �Ѵ�.
	pBlackBoard->Set_ControlLight(m_bLightOn) ;
	pBlackBoard->Get_BossAwareness().Cur = m_tStat.fAwareness;
}

void CAceBoss::Update_BlackBoard()
{
	//// ������ ���� ���, ���� �ڵ�� �ʼ�
	//if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//{
	//	m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"BossLight"));
	//	// ���� ����
	//	if (!m_wpBlackBoard_Player.Get_BlackBoard())
	//		return;
	//}

	//// ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
	//CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

	//// ���⼭���� �������� ������ ���´�.
	//m_fHp = pBlackBoard->Get_HP().Cur;
}

#pragma endregion 

#pragma region ���¸ӽ� : idle ~ Death

void CAceBoss::AI_Idle(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : idle ����   \n");
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : idle �� \n");
    }
}

void CAceBoss::AI_Suspicious(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"��BOSS - ���¸ӽ� : Suspicious ����   \n");

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
		OutputDebugString(L"��BOSS - ���¸ӽ� : Suspicious ��   \n");
	}
}

void CAceBoss::AI_Rest(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"��BOSS - ���¸ӽ� : Rest ����   \n");
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
		OutputDebugString(L"��BOSS - ���¸ӽ� : Rest �� \n");
	}
}

void CAceBoss::AI_Reloading(float fDeltaTime)
{
	if (m_tState_Obj.IsState_Entered())
	{
		OutputDebugString(L"��BOSS - ���¸ӽ� : Reloading ����   \n");
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
		OutputDebugString(L"��BOSS - ���¸ӽ� : Reloading ��   \n");
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

#pragma region �ൿ�ӽ�
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