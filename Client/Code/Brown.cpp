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
    pInstance->vPatrolPointZero = { _x, _y, _z }; // ������ġ�� ��������Ʈ ���� 
    return pInstance;
}

HRESULT CBrown::Ready_GameObject()
{
    srand((_uint)time(NULL));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    
    m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
    m_fFrameEnd = 0;
    m_fFrameSpeed = 8.f;

    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 0.7f, 0.3f };

    // INFO
    m_iHP = m_iMaxHP;
    pPlayerRightState = CPlayer::STATE_RIGHTHAND::NONE;

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);

    //����
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CBrown::AI_Rest);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);

    //�߰�
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CBrown::AI_Jump);
    
    //����
    m_tState_Obj.Add_Func(STATE_OBJ::NORMALATTACK, &CBrown::AI_NormalATTACK);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

    // ================================================================ Ȯ���ʿ� 
    //�ǰ�
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);

    //����
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CBrown::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CBrown::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CBrown::AI_Death);

    //����
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
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBrown::GoHome);
    
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

#pragma endregion

    //swprintf_s(debugString, L"Brown - ���� Ȯ�� m_iHP = %f\n", m_iHP);
    //OutputDebugStringW(debugString);

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // ��ġ�� �������� 
    Get_PlayerPos(fTimeDelta);
    
    // ����Ÿ�� 
    Height_On_Terrain(); 

    
    // ������
    Billboard(fTimeDelta);


     //���¸ӽ�
    m_fFrame += m_fFrameSpeed * fTimeDelta;
    
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ
    
    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;
    
        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State()
            || STATE_OBJ::DEATH == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

#pragma region �׽�Ʈ ��� 

    // ���� �ǰ� �׽�Ʈ�� 
    if (Engine::IsKey_Pressing(DIK_B))
    {
        m_tState_Obj.Set_State(STATE_OBJ::NORMALATTACK);
    }

    if (Engine::IsKey_Pressing(DIK_P))
    {
        Right_Setting();
        swprintf_s(debugString, L"Brown - ���� Ȯ�� pPlayerRightState = %d\n", pPlayerRightState);
        OutputDebugStringW(debugString);
    }
    

     if (Engine::IsKey_Pressing(DIK_J))
     {
         Engine::Add_GameObject(L"GameLogic", CFallingStone::Create(m_pGraphicDev,
             m_pTransformComp->Get_Pos().x + 0.1f, m_pTransformComp->Get_Pos().y + 10.f, m_pTransformComp->Get_Pos().z));

     }


#pragma endregion 

     //���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region �⺻ ȯ�漳�� 
void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

HRESULT CBrown::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BrownTextureComp"), E_FAIL);
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
    m_pColliderComp->Set_CollisionLayer(LAYER_MONSTER); // �� Ŭ������ ���� �浹���̾� 
    m_pColliderComp->Set_CollisionMask(LAYER_PLAYER | LAYER_PROJECTILE | LAYER_WALL); // ��� �浹�ؾ��ϴ� ���̾�� - ����ü���� �浹�ҿ��� 

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
    OutputDebugString(L"��Brown �浹�� \n");

    if (0 == m_iHP)
    {
        //�÷��̾�� �浹�ѰŶ�� 
        switch (pPlayerRightState)
        {
        case CPlayer::STATE_RIGHTHAND::HAND:
                m_tState_Obj.Set_State(STATE_OBJ::HIT);
            break;

        case CPlayer::STATE_RIGHTHAND::RUN_HAND:
            m_tState_Obj.Set_State(STATE_OBJ::FALLING);
            break;

        case CPlayer::STATE_RIGHTHAND::GUN:
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
            break;

        case CPlayer::STATE_RIGHTHAND::THOMPSON:
            m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
            break;

        case CPlayer::STATE_RIGHTHAND::STEELPIPE:
            m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
            break;

        case CPlayer::STATE_RIGHTHAND::BEERBOTLE:
            m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
            break;

        case CPlayer::STATE_RIGHTHAND::FRYINGPAN:
            m_tState_Obj.Set_State(STATE_OBJ::FACEPUNCH);
            break;

        case CPlayer::STATE_RIGHTHAND::KICK:
            m_tState_Obj.Set_State(STATE_OBJ::FALLING);
            break;
        }

        //�浹ü�� �浹�ѰŶ�� 
    }
}

void CBrown::OnCollisionEntered(CGameObject* pDst) // ó�� �浿 ���� 
{
    Right_Setting();

    OutputDebugString(L"��Brown �浹 \n");
    // �÷��̾� �������, �÷��̾ ������������ �ʿ��� ���⿡ ��ȭ 
    // ����Ʈ�� �ϴ� �־�� 
   
    //�浹�Ѱ� �÷��̾��� 
    _int iCombo = (rand() % 2) + 1;

    switch (pPlayerRightState)
    {
    case CPlayer::STATE_RIGHTHAND::HAND:
        break;

    case CPlayer::STATE_RIGHTHAND::RUN_HAND:
        break;

    case CPlayer::STATE_RIGHTHAND::GUN:
        break;

    case CPlayer::STATE_RIGHTHAND::THOMPSON:
        m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        break;

    case CPlayer::STATE_RIGHTHAND::STEELPIPE:
        break;

    case CPlayer::STATE_RIGHTHAND::BEERBOTLE:
        //m_iHp -= 20;
        break;

    case CPlayer::STATE_RIGHTHAND::FRYINGPAN:
        // m_iHp -= 25;
        break;

    case CPlayer::STATE_RIGHTHAND::KICK:
        // m_iHp -= 15;
        break;
    }

    //�浹�Ѱ� ����ü���


    swprintf_s(debugString, L"Brown - ���� Ȯ�� m_iHP = %f\n", m_iHP);
    OutputDebugStringW(debugString);
}

void CBrown::OnCollisionExited(CGameObject* pDst) // �浹 ������ 
{
    //OutputDebugString(L"��Brown �浹���� \n");

}



#pragma endregion 

#pragma region BlackBoard
void CBrown::Update_InternalData()
{
    // ������ ���� ���, ���� �ڵ�� �ʼ�
    if (!m_wpBlackBoard_Player.Get_BlackBoard())
    {
        m_wpBlackBoard_Player.Set_BlackBoard(Engine::Get_BlackBoard(L"Player"));
        // ���� ����
        if (!m_wpBlackBoard_Player.Get_BlackBoard())
            return;
    }

    // ���� �ڵ带 ��ġ�� �Ϲ� �����ͷ� ���� ���.
    CBlackBoard_Player* pBlackBoard = m_wpBlackBoard_Player.Get_BlackBoard();

    // ���⼭���� �������� ������ ���´�.

}


#pragma endregion 

#pragma region ȯ�漳�� �μ���Ʈ + ���¸ӽ� �����Լ� 

_bool CBrown::Detect_Player() // ���� �þ߳� �÷��̾� �ִ��� üũ�ϴ� �Լ� 
{
    _vec3 vMonsterLook;

    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    //���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _vec3 MonToPlayer = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
    _float currdistance = D3DXVec3Length(&MonToPlayer);

    //����ȭ
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

    // �þ߹��� �켱üũ 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // �����Ͽ� RADIAN���ϱ� 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // �þ߹��� + �þ߰��� = ���� 
    if (fradian < m_fMonsterFov * 2 && currdistance < m_fMonsterSightDistance)
        return true;
    else
        return false;
}


void CBrown::Billboard(const _float& fTimeDelta)
{
    // ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);   
}

float CBrown::Calc_Distance()
{
    // �÷��̾� ��ġ - ���� ��ġ = ���Ͱ� �÷��̾� �ٶ󺸴� ����
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

void CBrown::Right_Setting()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"GameLogic", L"Player"));
    pPlayerRightState = pPlayer->Get_PlayerRightHand();
}

HRESULT CBrown::Get_PlayerPos(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    
    return S_OK;
}

void CBrown::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);
                
    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.2f, vPos.z);
}

#pragma endregion 

#pragma region ���¸ӽ� : idle ~ Death

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_iPreHP = m_iHP; // ����üũ�� hp���� 
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }

        // �÷��̾ ������ ����� ��
        // Get_Player()->Get_m_pRightHandComp()->Get_VecTexture()->front()
        // Get_Player()->Get_PlayerGunState()
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : idle �� \n");
    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Suspicious ����   \n");

        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        // ����ü �߻� 
        Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x + 0.1f, m_pTransformComp->Get_Pos().y + 1.3f, m_pTransformComp->Get_Pos().z));

    }

    if (m_tState_Obj.Can_Update())
    {
        if(Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
           // //OutputDebugString(L"��Brown - ����üũ : �������� �����   \n");
            m_fBrownAwareness += fDeltaTime * 2.f;

            // ���� �������� 
            // �߿� << �� ���⼭ awareness�� ������ m_awareness ���� �����ؼ� �������� �����ϰ� �ؾ���. 
            // �׸��� ���� ���°� Taunt�� ���� �� set_dead�� �������� 
            // ������ �߸ŷ� �� ����ϰ� ����� 

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fBrownAwareness)
            {
                m_fBrownAwareness = m_fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // �߰����� ���� 
            }
        }
        else // �������� ����
        {
            ////OutputDebugString(L"��Brown - ����üũ : �������� ������   \n");
            m_fBrownAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fBrownAwareness)
            {
                m_fBrownAwareness = 0.f;
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
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Taunt");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (2 == m_fCheck) // ���� �ι� �ϰ� ���󰡱� 
        {
            m_fCheck = 0; //�ٸ����� ���ϱ� 0���� �ǵ����� 
            m_iPreHP = m_iHP; // �ǰݾ����� pre�� ����hp ���� 
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
        m_fFrameSpeed = 10.f; //���󺹱� 
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
            m_fBrownAwareness -= fDeltaTime * 4.f; // ������ ���� 

            if (0 >= m_fBrownAwareness) //�������� �ʱ�ȭ
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ��ħ - Recon���� �Ѿ  \n");
                m_fBrownAwareness = 0.f;
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

        m_fFrameSpeed = 15.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Rest");
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Rest ��   \n");

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Run ����   \n");

        m_fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (Calc_Distance() < m_fWalkDistance || m_fFrame > m_fFrameEnd)
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
        m_fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (Calc_Distance() < m_fWalkDistance || m_fFrame > m_fFrameEnd)
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
        m_fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ����Ű ������ ����   \n");
            m_mapActionKey[ACTION_KEY::INCHFORWARD].Act();
        }
        if (Calc_Distance() <= m_fInchDistance || m_fFrame > m_fFrameEnd)
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

        m_fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Strafing");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {     
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::STRAFING].Act();

        if (m_fFrame > m_fFrameEnd)
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
        m_fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"NormalAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::NORMALATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
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

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"HeavyAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CBrown::AI_Jump(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Jump ����   \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Jump");
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
        //OutputDebugString(L"��Brown - ���¸ӽ� : Jump ��  \n");
    }  
}

void CBrown::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ����   \n");

        m_fFrameSpeed = 6.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Hit");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST); 
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        //OutputDebugString(L"��Brown - ���¸ӽ� : Hit ��   \n");
    }
}

void CBrown::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ����   \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"FacePunch");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ��   \n");
    }
}

void CBrown::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Dazed ����   \n");

        m_iPreHP = m_iHP;
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Jump");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

    }

    if (m_tState_Obj.Can_Update())
    { 
        if (m_fFrame > m_fFrameEnd)
        {
            m_iHP = int(m_iMaxHP * 0.5);
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

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Chopped");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        DeadSpin = false;
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame >= m_fFrameEnd)
        {
           // Set_Dead();
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
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Headless");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            m_fFrame = m_fFrameEnd - 1;
            Set_Dead();
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

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Death");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame>m_fFrameEnd)
        {
            m_fFrame = m_fFrameEnd - 1;
            Set_Dead();
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
            m_fBrownAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 
            if (m_fBrownAwareness >= m_fMaxAwareness)
            {
                m_fBrownAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }
        else // �÷��̾� ����x
        {
           m_fConsider -= fDeltaTime * 3.f;

        }

        if (0 >= m_fConsider) // ��躯������ ������ 0�� �Ǹ� ������ ���� 
        {
            m_fConsider = 10.f; // �ٽ� �ʱ� �������� 
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
          m_fFrameSpeed = 15.f;
          m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_North");
          m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // �׼�Ű ������ 
     
        if (m_bArrive && m_fFrame > m_fFrameEnd) // ������ �� ���� 
        {
            m_bArrive = false;
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
        {
              //OutputDebugString(L"��Brown - ����Ű : RUN Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::APPROACH);
        }
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : WALK Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::APPROACH);
        }

        if (m_mapActionKey[ACTION_KEY::INCHFORWARD].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : INCHFORWARD Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::MOVING);
        }

        if (m_mapActionKey[ACTION_KEY::STRAFING].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : STRAFING Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::MOVING);
        }
        if (m_mapActionKey[ACTION_KEY::NORMALATTACK].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : ATTACK Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::ATTACK);
        }

        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : HEAVY_ATTACK Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::ATTACK);
        }

        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
        {
              //OutputDebugString(L"��Brown - ����Ű : GoHome Ȯ��    \n");
            m_tState_Act.Set_State(STATE_ACT::GOHOME);
        }

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
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"��Brown - ����Ű : RUN ����   \n");

            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            m_pTransformComp->Set_Look(vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
            
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        { 
              //OutputDebugString(L"��Brown - ����Ű : WALK ����   \n");

            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            m_pTransformComp->Set_Look(vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);
    
        }


        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
    // ����
    {
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
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon����
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        if(Calc_Distance() < m_fMonsterSightDistance && Calc_Distance() > m_fRunDistance ) // Run���̷� ��ġ�� Run���� 
            m_tState_Obj.Set_State(STATE_OBJ::RUN);

        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : INCHFORWARD ����   \n");

            // ���Ͱ� �÷��̾ ���º��� = �� - �� 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos(); // �÷��̾� ������ �̵� 
            D3DXVec3Normalize(&vDir, &vDir);

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fInchSpeed);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            
        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : STRAFING ����   \n");

            _vec3 MoveDir = m_pTransformComp->Get_Right() + m_pTransformComp->Get_Look(); // �÷��̾� ������ 
            D3DXVec3Normalize(&MoveDir, &MoveDir);
            m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, m_fStrafingSpeed);

            if (Calc_Distance() < m_fInchDistance) //inch ��Ÿ����� ������ �ٸ��� �����ϱ����ؼ� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        }



        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // ����
    {

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
        if (STATE_OBJ::NORMALATTACK == m_tState_Obj.Get_State())
        {
            if (!m_AttackOnce)
            {
                //OutputDebugString(L"��Brown - ����Ű : BASICATTACK ����   \n");
                // �浹ü ���� �⺻���� ����

                _vec3 vLook = m_pTransformComp->Get_Look();
                D3DXVec3Normalize(&vLook, &vLook);

                _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev, vDirPos.x, vDirPos.y, vDirPos.z,
                    CMonsterPunch::TYPE::NORMAL));
                
                m_AttackOnce = true;
            }
        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : HEAVYATTACK ����   \n");
            if (!m_AttackOnce)
            {
                //OutputDebugString(L"��Brown - ����Ű : BASICATTACK ����   \n");
                // �浹ü ���� �⺻���� ����

                _vec3 vLook = m_pTransformComp->Get_Look();
                D3DXVec3Normalize(&vLook, &vLook);

                _vec3 vDirPos = m_pTransformComp->Get_Pos() + vLook * 2;

                Engine::Add_GameObject(L"GameLogic", CMonsterPunch::Create(m_pGraphicDev, 
                    vDirPos.x, vDirPos.y, vDirPos.z, CMonsterPunch::TYPE::HEAVY));

                m_AttackOnce = true;
            }
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
        // //OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
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
            //OutputDebugString(L"��Brown - ���� ��Ʈ�� ����Ʈ ������   \n");
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fRunDistance);
        }
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
        ////OutputDebugString(L"��Brown - �ൿ�ӽ� : MOVING ��   \n");
    }
}

#pragma endregion