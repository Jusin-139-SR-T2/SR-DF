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

    // �̹��� ����
    m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
    m_pTransformComp->Set_Scale({ 3.f, 5.f, 1.f });
    m_tFrame.fFrame = 0.f;
    m_tFrame.fFrameEnd= _float(m_pTextureComp->Get_VecTexture()->size());
    m_tFrame.fFrameSpeed= 12.f;
    m_tFrame.fRepeat= 0.f;
    m_tFrame.fAge = 0.f;

    // ��������Ʈ ���� 
    Set_TeamID(ETEAM_MONSTER);

    //Status
    m_tStat.fAttackDistance = 13.f;
    m_tStat.fAttackFov = 80.f;
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;
    m_bDazedState = FALSE;
    m_bDeadState = FALSE;
    m_bDazeToHeal = FALSE;
    
    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 3.f, 3.f, 3.f };

    D3DXCreateBox(m_pGraphicDev, 3.f, 3.f, 3.f, &m_pMesh, NULL);
    
#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // �ʱ���� ���� 
    // �߰�
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CGray::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CGray::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); 
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);
    
    // �⺻����
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CGray::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CGray::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEPEYE, &CGray::AI_KeepEye);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDEWALK, &CGray::AI_SideWalk);
    m_tState_Obj.Add_Func(STATE_OBJ::FRIGHTEN, &CGray::AI_Frighten);
    m_tState_Obj.Add_Func(STATE_OBJ::UPRIGHTRUN, &CGray::AI_UpRightRun);
    
    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::THROW, &CGray::AI_Throw);
    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CGray::AI_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CGray::AI_HeavyAttack);
    
    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CGray::AI_GoHome);
    
    // ����
    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    
    // �ǰ�
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CGray::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CGray::AI_Falling);
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CGray::AI_Hit);
    
    // ���
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);

#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
    m_tState_Act.Add_Func(STATE_ACT::SUDDENATTACK, &CGray::SuddenAttack);
    m_tState_Act.Add_Func(STATE_ACT::SIDEMOVING, &CGray::SideMoving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CGray::GoHome);

#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); //����ﶧ

    m_mapActionKey.Add_Action(ACTION_KEY::KEEPEYE); // ����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::SIDEWALK); // y�࿡ ���̰� ������ 

    m_mapActionKey.Add_Action(ACTION_KEY::UPRIGHT); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::FRIGHTEN); //�߰�

    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

    return S_OK;
}

_int CGray::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // ����Ÿ�� 
    Height_On_Terrain(); 

    if (m_gHp.Cur <= 0 && FALSE == m_bDeadState)
        MonsterDead();

    // ������ 
    if (FALSE == m_bDeadState)
        Billboard(fTimeDelta);

    // ���¸ӽ�
    m_tFrame.fFrame += m_tFrame.fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

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

    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°�

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}


#pragma region �⺻ ȯ�漳�� 

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
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_PROJECTILE | LAYER_WALL | LAYER_PLAYER_ATTACK); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}


#pragma endregion 

#pragma region ȯ�漳�� �μ���Ʈ + ���¸ӽ� �����Լ� 

void CGray::RenderSplitImages()
{
    _vec3 MonsterLook = m_pTransformComp->Get_Look();
    _vec3 PlayerLook = m_pPlayerTransformcomp->Get_Look();

    _float DotProduct = D3DXVec3Dot(&MonsterLook, &PlayerLook);

    //if(DotProduct)

    swprintf_s(debugString, L"Brown - ���� Ȯ�� DotProduct = %f\n", DotProduct);
    //OutputDebugStringW(debugString);
}

#pragma endregion

#pragma region �浹��Ʈ 

void CGray::OnCollision(CGameObject* pDst)
{
    //OutputDebugString(L"��Gray �浹�� \n");
}
void CGray::OnCollisionEntered(CGameObject* pDst)
{
    if (Get_IsMonsterDeath())
        return;

    CAceGameObject* pAceObj = dynamic_cast<CAceGameObject*>(pDst);

    if (nullptr == pAceObj)
        return;

    // ���� ��ų�� �������� �ֱ⶧���� �浹ü ���� �˻��ѵڿ� dead�� �Ѿ����
    // ���� �� : ����  ������� : �÷��̾� 
    if (Check_Relation(pAceObj, this) == ERELATION::HOSTILE) // �������
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pAceObj);

        if (nullptr == pPlayer)
        {
            CPlayerAttackUnion* pPlayerAttack = dynamic_cast<CPlayerAttackUnion*>(pAceObj);

            if (nullptr == pPlayerAttack)
                return;

            //==== �÷��̾� ����ü��  �浹 =============================
            m_eRecentCol = RECENT_COL::PLAYERATK;

            if (m_tStat.iDazedHP >= m_gHp.Cur && FALSE == m_bDazedState)
            {
                OutputDebugString(L"��Brown - �浹���� DAZED ����   \n");
                m_tState_Obj.Set_State(STATE_OBJ::DAZED);
                return;
            }
            else if (CPlayer::STATE_RIGHTHAND::KICK == ePlayerRighthand)
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); // fist
            else if (PSITDONW_ATTACK == m_ePlayer_AttackState) // �� + kick = falling �� 
                m_tState_Obj.Set_State(STATE_OBJ::CROTCHHIT);
            else //�Ͼ�� FacePunch���� 
            {
                if (Random_variable(50))
                    m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
                else
                    m_tState_Obj.Set_State(STATE_OBJ::HIT);
            }
        }
        else
        {
            //==== �÷��̾�� �浹 =====================================
            m_eRecentCol = RECENT_COL::PLAYER;

            if (CPlayer::STATE_RIGHTHAND::RUN_HAND == ePlayerRighthand)
                m_tState_Obj.Set_State(STATE_OBJ::FALLING); //�޸��� 

        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::FRIEND)
    {
        // ģ�а��� - ���� : ���ʹ� ģ�������� ������ ������ 
        // ���� ��ü�� �浹�� ���� ������ ��ȯ�� ����ü���� �浹�� 

        // ==== ������ų�� �浹 ========================================
        CMonsterAttackUnion* pBossAttack = dynamic_cast<CMonsterAttackUnion*>(pAceObj);

        if (nullptr == pBossAttack)
            return;
        else
        {
            // ==== ������ų�� �浹 ========================================
            m_eRecentCol = RECENT_COL::BOSSATK;

            m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }
    }
    else if (Check_Relation(pAceObj, this) == ERELATION::NUETRAL) // ������Ʈ �浹 
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

#pragma region ��ǥ(AI) ���¸ӽ� �κ� 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : idle ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (Detect_Player())
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : idle ��  \n");
    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious ����  \n");
       Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
           m_pTransformComp->Get_Pos().x ,
           m_pTransformComp->Get_Pos().y + 1.4f,
           m_pTransformComp->Get_Pos().z, CAwareness::TYPE::GRAY, this));

    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            //OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious -> �÷��̾� ����   \n");
            
            m_tStat.fAwareness += fDeltaTime * 3.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_tStat.fMaxAwareness <= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = m_tStat.fAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 

               // int iCombo = (rand() % 10) + 1; // 1~10 
               //
               // if (6 <= iCombo) // 6~10
                    m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
               //
               // if (6 > iCombo) // 1~5
               //     m_tState_Obj.Set_State(STATE_OBJ::TAUNT);

            }
        }
        else // �������� ����
        {
            //OutputDebugString(L"��Gray - ����üũ : �������� ������   \n");

            m_tStat.fAwareness -= fDeltaTime * 6.f;

            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ�Ǹ� idle�� back
            if (0 >= m_tStat.fAwareness)
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious ��  \n");

    }
}

void CGray::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Taunt ����  \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Taunt");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 12.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (Detect_Player())
        {
            if (2 == m_tFrame.fRepeat) // ���� �ι� �ϰ� ���󰡱� 
            {
                m_tFrame.fRepeat = 0; //�ٸ����� ���ϱ� 0���� �ǵ����� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Taunt ��  \n");
    }
}

void CGray::AI_YouDie(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : You Die ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 9.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd) // ���� �ι� �ϰ� ���󰡱� 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : You Die ��   \n");
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
            m_tStat.fAwareness += fDeltaTime * 4.f; // �������� �� ������ �����Ұ� 

            if (m_tStat.fAwareness >= m_tStat.fMaxAwareness)
            {
                m_tStat.fAwareness = m_tStat.fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
            }
        }

        if (0 >= m_tStat.fConsider)
        {
            m_tStat.fConsider = 10.f; // �ٽ� �ʱ� �������� 
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
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        if (m_bArrive && m_tFrame.fFrame > m_tFrame.fFrameEnd) // ������ �� ���� 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Chase ����  \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 8.f;

        m_AttackOnce = FALSE;
    }

    if (m_tState_Obj.Can_Update())
    {
        //OutputDebugString(L"�� Gray ����� : Chease - Update ����   \n");

        if (Detect_Player()) // �÷��̾� ���� 
        {
            _float CurDistance = Calc_Distance();

            if (m_bPlayerAttakBool && m_bCollisionEnter == FALSE)
                m_tState_Obj.Set_State(STATE_OBJ::BLOCK);

            // �پ �ٰ��� : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"�� Gray ����� : Chease -RUN �����Լ� Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Gray ����� : Chease - WALK �����Լ� Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // �ֽ��ϸ鼭 ��� : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Gray ����� : Chease - ��� �����Լ� Set    \n");
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //���� �����Ϸ� �� : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
                //OutputDebugString(L"�� Gray ����� : Chease - ���� �����Լ� Set  \n");
                int iCombo = (rand() % 15) + 1;

                if (4 > iCombo)                         // 20���� 1~3
                    m_tState_Obj.Set_State(STATE_OBJ::THROW);
                else if ( 10 > iCombo && 4 <= iCombo)   // 40����  4 ~ 9
                    m_tState_Obj.Set_State(STATE_OBJ::UPRIGHTRUN);
                else                                    // 40���� 10~15
                    m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
            }
            else  // ������
            {
                //OutputDebugString(L"�� Gray ����� : Chease - ���� �����Լ� Set   \n");
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // �i�ٰ��� �þ߿��� ����� ������ �پ�� SUSPICIOUS�� ���ư� 
        {
            //OutputDebugString(L"�� Gray ����� : Chease - ��������   \n");
            m_tStat.fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_tStat.fAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                m_tStat.fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Chase ��   \n");
    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //������ ������ ������ 
        //OutputDebugString(L"��Gray - ���¸ӽ� : Rest ����   \n");

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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Rest ��   \n");
    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Run ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Run");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 17.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� RUN ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Run ��   \n");
    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Walk ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Walk ��   \n");
    }
}

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : KeepEye ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"KeepEye");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 11.f;
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPEYE].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : KeepEye �� \n");
    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : SideWalk ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : SideWalk �� \n");
    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Throw ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_tFrame.fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_tFrame.fFrameSpeed = 10.f;
       
        // ����ü �߻� 
        Engine::Add_GameObject(L"GameLogic", CThrowPipe::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.f, m_pTransformComp->Get_Pos().z, this, (ETEAM_ID)Get_TeamID()));
    }

    if (m_tState_Obj.Can_Update())
    {
        // �浹ü�� ���⼭ ����Ű ������ state���� �ҿ��� 

        if (m_tFrame.fFrame > m_tFrame.fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Throw ��   \n");
    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : UpRightRun ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : UpRightRun ��   \n");
    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Frighten ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Frighten ��   \n");
    }
    
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Attack ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Attack ��   \n");
    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CGray::AI_Block(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Block ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CGray::AI_CrotchHit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : CrotchHit ����   \n");
        m_tStat.fAwareness = m_tStat.fMaxAwareness;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"CrotchHit");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : CrotchHit ��   \n");
    }
}

void CGray::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : FacePunch ����   \n");
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : FacePunch ��   \n");
    }
}

void CGray::AI_Falling(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Falling ����   \n");
        // �Ͼ�°ͱ��� ������ 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Falling ��   \n");
    }
}

void CGray::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Falling ����   \n");
        // �Ͼ�°ͱ��� ������ 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Falling ��   \n");
    }
}

void CGray::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� :  Dazed   \n");
        // �Ͼ�°ͱ��� ������ 
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

            if (m_gHp.Update(fDeltaTime * 6.f, 40.f, TRUE)) // ������, �����ϸ� bool��ȯ 
            {
                m_gHp.Cur = 40.f;
                m_bDazedState = FALSE;
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Dazed ��   \n");
    }
}

void CGray::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� :  Chopped   \n");
        // �Ͼ�°ͱ��� ������ 
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
            // �̻��·� ���ڴ»��� �߰� 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� : Chopped ��   \n");
    }
}

void CGray::AI_HeadShot(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� :  HeadShot   \n");
        // �Ͼ�°ͱ��� ������ 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : HeadShot ��   \n");
    }
}

void CGray::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� :  Headless   \n");
        // �Ͼ�°ͱ��� ������ 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Headless ��   \n");
    }
}

void CGray::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - ���¸ӽ� :  Death   \n");
        // �Ͼ�°ͱ��� ������ 
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
        //OutputDebugString(L"��Gray - ���¸ӽ� : Death ��   \n");
    }
}


#pragma endregion

#pragma region �ൿ ���¸ӽ� �κ� 
void CGray::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Idle ����   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // �Ϲ� �ٰ�������Ʈ 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACH);

        // �ֽ��ϸ� ���
        if (m_mapActionKey[ACTION_KEY::KEEPEYE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        if (m_mapActionKey[ACTION_KEY::SIDEWALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SIDEMOVING);

        // 
        if (m_mapActionKey[ACTION_KEY::UPRIGHT].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        if (m_mapActionKey[ACTION_KEY::FRIGHTEN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SUDDENATTACK);

        // ������Ʈ 
        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

        // ��Ʈ����Ʈ 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);
    }

    if (m_tState_Act.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Idle ��   \n");
    }
}

void CGray::Approach(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Approach ����   \n");
    }

    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        //���Ͱ� �÷��̾� �ٶ󺸴� ���� 
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
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Approach ��   \n");
    }
}

void CGray::SideMoving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Side - KeepEYE  OR  SideWalk ����   \n");
    }

    // ����
    {   
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon����
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
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Side - KeepEYE  OR  SideWalk ��   \n");
    }
}

void CGray::SuddenAttack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Sudden - Upright  OR  Frighten ����   \n");
    }

    // ����
    {
        if (Calc_Distance() > m_tStat.fAttackDistance) // �þ߹��� recon����
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
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Sudden - Upright  OR  Frighten ��   \n");
    }
}

void CGray::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Attack ����   \n");
    }

    // ����
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
        //OutputDebugString(L"��Gray - �ൿ�ӽ� : Attack ��   \n");
    }
}

void CGray::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
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
            ////OutputDebugString(L"��Brown - ���� ��Ʈ�� ����Ʈ ������   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, 3.f);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion