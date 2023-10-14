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

    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z); // ������ġ ����
    pInstance->m_tStat.vPatrolPointZero = { _x, _y, _z }; // ������ġ�� ��������Ʈ ���� 
    return pInstance;
}

HRESULT CBrown::Ready_GameObject()
{
    SUPER::Ready_GameObject();

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    Set_TeamID(ETEAM_BETA);
    
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
    m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed = 12.f;
    m_tFrame.fRepeat = 0.f;

    // ��������Ʈ ���� 
    Set_TeamID(ETEAM_MONSTER);

    // Status
    m_tStat.fAttackDistance = 12.f;
    m_tStat.fAttackFov = 90.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;

    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 0.7f, 0.3f };


//#pragma region ������
//
//    Engine::Add_BlackBoard(L"Monster", CBlackBoard_Monster::Create());
//
//#pragma endregion


#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    // ���
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CBrown::AI_Rest);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);

    // �߰�
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    //m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CBrown::AI_Jump);
    
    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::NORMALATTACK, &CBrown::AI_NormalATTACK);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

    // �ǰ�
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CBrown::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CBrown::AI_CrotchHit);

    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CBrown::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CBrown::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CBrown::AI_Headless);

    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CBrown::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBrown::AI_GoHome);


#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE�� ::A �϶� CBrownŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CBrown::Moving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBrown::Falling);
    m_tState_Act.Add_Func(STATE_ACT::FALLING, &CBrown::GoHome);
    
#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // ����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y�࿡ ���̰� ������ 
    m_mapActionKey.Add_Action(ACTION_KEY::NORMALATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);
    m_mapActionKey.Add_Action(ACTION_KEY::BEPUSHED); // ��ħ�������� 

#pragma endregion

    //OutputDebugStringW(debugString);

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // ����Ÿ�� 
    Height_On_Terrain(); 

    // ������
    Billboard(fTimeDelta);

    //Update_InternalData();

     //���¸ӽ�
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;
    
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ
    
    if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
    {
        m_tFrame.fFrame = 0.f;
    
        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State())
            m_tFrame.fRepeat += 1;
    }

     //���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region �⺻ override 

void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_tFrame.fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

HRESULT CBrown::Add_Component()
{
    // �浹 ������Ʈ 
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);
    
    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);
    
    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);
    
    // �浹 ���̾�, ����ũ ����
    m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER); // �� Ŭ������ ���� �浹���̾� 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_PROJECTILE | LAYER_WALL || LAYER_PLAYER_ATTACK); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

    return S_OK;
}

void CBrown::Free()
{
    SUPER::Free();
}

#pragma endregion 

#pragma region �浹 - OnCollision 

void CBrown::OnCollision(CGameObject* pDst) // ��� �浹�� 
{
    
}

void CBrown::OnCollisionEntered(CGameObject* pDst) // ó�� �浿 ���� 
{
    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

    // ���� �� : ����  ������� : �÷��̾� 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // ��������� ���
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

        if (nullptr == pPlayer)
        {
            // �÷��̾��� ����ü�� �浹 
            CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

            if (nullptr == pPlayerAttack)
                return;

            //�÷��̾� �������� ���� Ȯ�� 
            if(PSITDONW_ATTACK ==m_ePlayer_AttackState) 
                m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
            else
            {
                if (0 >= m_gHp.Cur)
                    MonsterDead(); //�ٸ������� ����hp�� �����Ƿ� ���� üũ 
                else
                {
                    if (CPlayer::STATE_RIGHTHAND::GUN == ePlayerRighthand ||
                        CPlayer::STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
                        m_tState_Obj.Set_State(STATE_OBJ::HIT);
                    else 
                        if(Random_variable(60))
                            m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
                        else
                            m_tState_Obj.Set_State(STATE_OBJ::HIT);
                }
            }
        }
        else
        {
            // �÷��̾�� �浹 

            if (PRUN_ATTACK == m_ePlayer_AttackState)
                m_tState_Obj.Set_State(STATE_OBJ::FALLING);
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::FRIEND)
    {
        // ģ�а��� - ���� : ���ʹ� ģ�������� ������ ������ 
        if (0 >= m_gHp.Cur)
            MonsterDead(); // death�� ������ 
        else
            m_tState_Obj.Set_State(STATE_OBJ::HIT);

    }
}

void CBrown::OnCollisionExited(CGameObject* pDst) // �浹 ������ 
{
}

void CBrown::MonsterDead()
{
    if (CPlayer::STATE_RIGHTHAND::GUN == ePlayerRighthand ||
        CPlayer::STATE_RIGHTHAND::THOMPSON == ePlayerRighthand)
        m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
    else
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
}
#pragma endregion 

#pragma region BlackBoard
void CBrown::Update_InternalData()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Monster.Get_BlackBoard())
    {
        m_wpBlackBoard_Monster.Set_BlackBoard(Engine::Get_BlackBoard(L"Monster"));
        // ���� ����
        if (!m_wpBlackBoard_Monster.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Monster* pBlackBoard = m_wpBlackBoard_Monster.Get_BlackBoard();

    // ���⼭���� �������� ������ ������Ʈ �Ѵ�.
    //pBlackBoard->Get_BrownAwareness().Cur = m_tStat.fAwareness;
}

#pragma endregion 

#pragma region ȯ�漳�� �μ���Ʈ + ���¸ӽ� �����Լ� 

#pragma endregion 

#pragma region ���¸ӽ� : idle ~ Death

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle �� \n");
    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Suspicious ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

       Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x + 0.1f, 
           m_pTransformComp->Get_Pos().y + 1.3f,
           m_pTransformComp->Get_Pos().z, CAwareness::TYPE::BROWN, this));
    }

    if (m_tState_Obj.Can_Update())
    {
        
        if(Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
           // //OutputDebugString(L"��Brown - ����üũ : �������� �����   \n");
            m_tStat.fAwareness += fDeltaTime * 2.f;

            // ���� �������� 
            // �߿� << �� ���⼭ awareness�� ������ m_awareness ���� �����ؼ� �������� �����ϰ� �ؾ���. 
            // �׸��� ���� ���°� Taunt�� ���� �� set_dead�� �������� 
            // ������ �߸ŷ� �� ����ϰ� ����� 

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // �߰����� ���� 
            }
        }
        else // �������� ����
        {
            ////OutputDebugString(L"��Brown - ����üũ : �������� ������   \n");
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Suspicious ��   \n");
    }
}

void CBrown::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Taunt ����   \n");
        m_tFrame.fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Taunt");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (2 == m_tFrame.fRepeat) // ���� �ι� �ϰ� ���󰡱� 
        {
            m_tFrame.fRepeat = 0; //�ٸ����� ���ϱ� 0���� �ǵ����� 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Taunt ��   \n");
    }
}

void CBrown::AI_Chase(float fDeltaTime) // �޸��ٰ� �ȴٰ� �쳯�������� 
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chease ����   \n");
       // m_pTransformComp->m_vScale.x = 0.5f;
        m_tFrame.fFrameSpeed = 10.f; //���󺹱� 
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
    }
    if (m_tState_Obj.Can_Update())
    {
        m_AttackOnce = false;
        _float CurDistance = Calc_Distance();

        //OutputDebugString(L"�� Brown ����� : Chease - Update ����   \n");

        if (Detect_Player()) // ��Ÿ��� �����Ǵ°�� 
        {
            //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� \n");
            // --------�Ÿ��� ���¸ӽ� -----------
            // �پ �ٰ��� : a > 8
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Run ����  \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Walk ���� \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // ���� : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - Moving ���� \n");
                int iCombo = (rand() % 10) + 1;

                if (5 <= iCombo) // 60% / 5~10 
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (5 > iCombo) // 40% / 1~4 
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // ������
            else
            {
                //OutputDebugString(L"�� Brown ����� : Chease -�÷��̾� ���� - ���� ����  \n");
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // ��Ÿ��� �÷��̾ ��������� 
        {
            //OutputDebugString(L"��Brown - ���¸ӽ� : �÷��̾� ��ħ    \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f; // ������ ���� 

            if (0 >= m_tStat.fAwareness) //�������� �ʱ�ȭ
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ��ħ - Recon���� �Ѿ  \n");
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE); 
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chease ��   \n");
    }
}

void CBrown::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Rest ����   \n");

        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 15.f;
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Rest ��   \n");

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Run ����   \n");

        m_tFrame.fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Run ��   \n");
    }
}

void CBrown::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Walk ����   \n");
        m_tFrame.fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Walk ��   \n");

    }
}

void CBrown::AI_InchForward(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ����   \n");
        m_tFrame.fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ����Ű ������ ����   \n");
            m_mapActionKey[ACTION_KEY::INCHFORWARD].Act();
        }
        if (Calc_Distance() <= m_fInchDistance || m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ������ �� ���� \n");
            //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ��   \n");
            m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ��   \n");

    }
}

void CBrown::AI_Strafing(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Strafing ����   \n");

        m_tFrame.fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Strafing");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {     
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::STRAFING].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Strafing ��   \n");
    }
}

void CBrown::AI_NormalATTACK(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Attack ����   \n");
        m_tFrame.fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"NormalAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::NORMALATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Attack ��   \n");
    }
}

void CBrown::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : HeavyAttack ����   \n");

        m_tFrame.fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"HeavyAttack");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_tFrame.fFrame >  m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CBrown::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Hit");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size()); 
         m_tFrame.fFrameSpeed = 10.f;
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
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ��   \n");
    }
}

void CBrown::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Falling ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Falling");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BEPUSHED].Act();

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Falling ��   \n");
    }
}

void CBrown::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"FacePunch");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 8.f;
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
        //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ��   \n");
    }
}

void CBrown::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"CrotchHit");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
        m_tFrame.fLifeTime = 2.f; // 2���� CHASE ���� 
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
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ��   \n");
    }
}

void CBrown::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Dazed");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;

    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_gHp.Update(fDeltaTime * 5.f, 40.f, TRUE)) // ������, �����ϸ� bool��ȯ 
        {
            m_gHp.Cur = 40.f;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ��   \n");
    }
}

void CBrown::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chopped ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Chopped");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Chopped ��   \n");
    }
}

void CBrown::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Headless ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Headless");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 10.f;
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Headless ��   \n");
    }
}

void CBrown::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Death ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Death");
         m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
         m_tFrame.fFrameSpeed = 10.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame> m_tFrame.fFrameEnd)
        {
            m_tFrame.fFrame =  m_tFrame.fFrameEnd - 1;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Death ��   \n");
    }
}

void CBrown::AI_Reconnaissance(float fDeltaTime)
{
    //�÷��̾� ���ļ� �������� 0���� ����������� 
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Reconnaissance ����   \n");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //�÷��̾� �����ϸ� �ٽ� ���� 
        {
            m_tStat.fAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 
            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }
        else // �÷��̾� ����x
        {
            m_tStat.fConsider -= fDeltaTime * 3.f;

        }

        if (0.f >= m_tStat.fConsider) 
        {
            // ��躯������ ������ 0�� �Ǹ� ������ ���� 
            m_tStat.fConsider = 10.f; // �ٽ� �ʱ� �������� 
            m_tState_Obj.Set_State(STATE_OBJ::GOHOME); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Reconnaissance ��   \n");
    }
}

void CBrown::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : GoHome ����   \n");
          m_tFrame.fFrameSpeed = 15.f;
          m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_North");
           m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // �׼�Ű ������ 
     
        if (m_bArrive && (m_tFrame.fFrame >  m_tFrame.fFrameEnd)) // ������ �� ���� 
        {
            m_bArrive = FALSE;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : GoHome ��   \n");
    }
}

#pragma endregion 

#pragma region �ൿ�ӽ� Idle ~ GoHome

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

    if (m_tState_Act.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
    }
}

void CBrown::Approach(float fDeltaTime) // RUN �׼�Ű ���� �����ϴ°�
{
    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : Approach ����   \n");
    }

    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"��Brown - ����Ű : RUN ����   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 5.f);
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        { 
            //OutputDebugString(L"��Brown - ����Ű : WALK ����   \n");
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 3.5f);
    
        }


        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          ////OutputDebugString(L"��Brown - �ൿ�ӽ� : Approach ��   \n");
    }
}
void CBrown::Moving(float fDeltaTime)
{

    if (m_tState_Act.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ����   \n");
    }
    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon����
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        if(Calc_Distance() < m_tStat.fAttackDistance && Calc_Distance() > m_fRunDistance ) // Run���̷� ��ġ�� Run���� 
            m_tState_Obj.Set_State(STATE_OBJ::RUN);

        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : INCHFORWARD ����   \n");

            // ���Ͱ� �÷��̾ ���º��� = �� - �� 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos(); // �÷��̾� ������ �̵� 
            D3DXVec3Normalize(&vDir, &vDir);

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 8.f);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            
        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : STRAFING ����   \n");

            _vec3 MoveDir = m_pTransformComp->Get_Right() + m_pTransformComp->Get_Look(); // �÷��̾� ������ 
            D3DXVec3Normalize(&MoveDir, &MoveDir);
            m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, 7.f);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        }
        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
    }
}

void CBrown::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� : ATTACK ����   \n");
    }

    // ����
    {
        //���Ͱ� �ٶ󺸴������� ���� 
        _vec3 vLook = m_pTransformComp->Get_Look();
        D3DXVec3Normalize(&vLook, &vLook);

        _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

        if (STATE_OBJ::NORMALATTACK == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : BASICATTACK ����   \n");
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
            //OutputDebugString(L"��Brown - ����Ű : HEAVYATTACK ����   \n");
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
        // //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
    }
}

void CBrown::Falling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� :FALLING ����   \n");
    }

    // ����
    {
        //�÷��̾ ���� ���� ���� 
        vDir = m_pTransformComp->Get_Pos() - m_pPlayerTransformcomp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 10.f);
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"��Brown - �ൿ�ӽ� : GOHOME ��   \n");
    }
}


void CBrown::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - �ൿ�ӽ� : GoHome ����   \n");
    }

    // ����
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
            //OutputDebugString(L"��Brown - ���� ��Ʈ�� ����Ʈ ������   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 6.f);
        }
    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"��Brown - �ൿ�ӽ� : GOHOME ��   \n");
    }
}

#pragma endregion