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
    pInstance->vPatrolPointZero = { _x, _y, _z };
    return pInstance;
}

HRESULT CGray::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Scale({ 3.f, 5.f, 1.f });
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;
    
    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    FCollisionBox* pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 2.f, 0.1f };

    // INFO
    m_iHP = 40;         // ü��
    m_iAttack = 10;     // ���ݷ�
    
#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // �ʱ���� ���� 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CGray::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CGray::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); 
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);

    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CGray::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CGray::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDEWALK, &CGray::AI_SideWalk);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEPEYE, &CGray::AI_KeepEye);
    m_tState_Obj.Add_Func(STATE_OBJ::UPRIGHTRUN, &CGray::AI_UpRightRun);
    m_tState_Obj.Add_Func(STATE_OBJ::FRIGHTEN, &CGray::AI_Frighten);

    m_tState_Obj.Add_Func(STATE_OBJ::THROW, &CGray::AI_Throw);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CGray::AI_HeavyAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CGray::AI_Attack);

    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CGray::AI_Reconnaissance);
    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CGray::AI_GoHome);

    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

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

    // �÷��̾� ���� ����ؼ� ��� 
    Get_PlayerPos(); 

    // ����Ÿ�� 
    Height_On_Terrain(); 

    // ������ 
    if(m_bBillboard)
        Billboard(fTimeDelta);

    // ���¸ӽ�-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() ||
            STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State() ||
            STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State() 
            )
            m_fCheck += 1;
    }

    

#pragma region �׽�Ʈ ��� 

    // ---------- �׽�Ʈ ���� ---------------------
    if (Engine::IsKey_Pressing(DIK_G))
    {
        m_tState_Obj.Set_State(STATE_OBJ::THROW);
    }

    if (Engine::IsKey_Pressing(DIK_H))
    {
        RenderSplitImages();
    }
    // --------------------------------------------

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

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}


HRESULT CGray::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_GrayTextureComp"), E_FAIL);

    // �ݶ��̴� ������Ʈ
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderBoxComp"), E_FAIL);

    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);

    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    // �浹 ���̾�, ����ũ ����
    m_pColliderComp->Set_CollisionLayer(ELAYER_MONSTER); // �� Ŭ������ ���� �浹���̾� 
    m_pColliderComp->Set_CollisionMask(ELAYER_PLAYER); // ��� �浹�ؾ��ϴ� ���̾�� 

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}

#pragma endregion 

#pragma region ȯ�漳�� �μ���Ʈ + ���¸ӽ� �����Լ� 

void CGray::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.3f , vPos.z);
}

void CGray::Billboard(const _float& fTimeDelta)
{ 
    // ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}

void CGray::RenderSplitImages()
{
    _vec3 MonsterLook = m_pTransformComp->Get_Look();
    _vec3 PlayerLook = m_pPlayerTransformcomp->Get_Look();

    _float DotProduct = D3DXVec3Dot(&MonsterLook, &PlayerLook);

    //if(DotProduct)

    swprintf_s(debugString, L"Brown - ���� Ȯ�� DotProduct = %f\n", DotProduct);
    OutputDebugStringW(debugString);
}

_bool CGray::Detect_Player()
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

_float CGray::Calc_Distance()
{
    // �÷��̾� ��ġ - ���� ��ġ = ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

HRESULT CGray::Get_PlayerPos()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    return S_OK;
}

#pragma endregion

#pragma region �浹��Ʈ 

void CGray::OnCollision(CGameObject* pDst)
{
    OutputDebugString(L"��Gray �浹�� \n");
}
void CGray::OnCollisionEntered(CGameObject* pDst)
{
    OutputDebugString(L"��Gray �浹���� \n");
}

void CGray::OnCollisionExited(CGameObject* pDst)
{
    OutputDebugString(L"��Gray�̶� �浹���� \n");
}


#pragma endregion 

#pragma region ��ǥ(AI) ���¸ӽ� �κ� 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : idle ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
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
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : idle ��  \n");
    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious ����  \n");
        // idle���� �״�� �̾ �ؽ�ó ���� 
        m_bBillboard = true;
        Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x + 0.2f, m_pTransformComp->Get_Pos().y + 1.3f, m_pTransformComp->Get_Pos().z));

    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious -> �÷��̾� ����   \n");

            m_fGrayAwareness += fDeltaTime * 3.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fGrayAwareness)
            {
                m_fGrayAwareness = m_fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 

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
            OutputDebugString(L"��Gray - ����üũ : �������� ������   \n");

            m_fGrayAwareness -= fDeltaTime * 6.f;

            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ�Ǹ� idle�� back
            if (0 >= m_fGrayAwareness)
            {
                m_fGrayAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Suspicious ��  \n");

    }
}

void CGray::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Taunt");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (Detect_Player())
        {
            if (2 == m_fCheck) // ���� �ι� �ϰ� ���󰡱� 
            {
                m_fCheck = 0; //�ٸ����� ���ϱ� 0���� �ǵ����� 
                m_iPreHP = m_iHP; // �ǰݾ����� pre�� ����hp ���� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_YouDie(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : You Die ����   \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd ) // ���� �ι� �ϰ� ���󰡱� 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : You Die ��   \n");
    }
}

void CGray::AI_Reconnaissance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {

    }
    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 3.f;

        if (Detect_Player())
        {
            m_fGrayAwareness += fDeltaTime * 4.f; // �������� �� ������ �����Ұ� 

            if (m_fGrayAwareness >= m_fMaxAwareness)
            {
                m_fGrayAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
            }
        }

        if (0 >= m_fConsider)
        {
            m_fConsider = 10.f; // �ٽ� �ʱ� �������� 
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
        m_fFrameSpeed = 14.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"WalkNorth");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        if (m_bArrive && m_fFrame > m_fFrameEnd) // ������ �� ���� 
        {
            m_bArrive = false;
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
        OutputDebugString(L"��Gray - ���¸ӽ� : Chase ����  \n");

        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        OutputDebugString(L"�� Gray ����� : Chease - Update ����   \n");

        if (Detect_Player()) // �÷��̾� ���� 
        {
            _float CurDistance = Calc_Distance();

            // �پ �ٰ��� : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                OutputDebugString(L"�� Gray ����� : Chease -RUN �����Լ� Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                OutputDebugString(L"�� Gray ����� : Chease - WALK �����Լ� Set   \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // �ֽ��ϸ鼭 ��� : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                OutputDebugString(L"�� Gray ����� : Chease - ��� �����Լ� Set    \n");
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //���� �����Ϸ� �� : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
                OutputDebugString(L"�� Gray ����� : Chease - ���� �����Լ� Set  \n");
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
                OutputDebugString(L"�� Gray ����� : Chease - ���� �����Լ� Set   \n");
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // �i�ٰ��� �þ߿��� ����� ������ �پ�� SUSPICIOUS�� ���ư� 
        {
            OutputDebugString(L"�� Gray ����� : Chease - ��������   \n");
            m_fGrayAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fGrayAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                m_fGrayAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Chase ��   \n");
    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //������ ������ ������ 
        OutputDebugString(L"��Gray - ���¸ӽ� : Rest ����   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
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
        OutputDebugString(L"��Gray - ���¸ӽ� : Rest ��   \n");
    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Run ����   \n");
        m_fFrameSpeed = 16.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� RUN ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Run ��   \n");
    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        
        OutputDebugString(L"��Gray - ���¸ӽ� : Walk ����   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
        
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Walk ��   \n");
    }
}

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : KeepEye ����   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"KeepEye");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPEYE].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::FRIGHTEN);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : KeepEye �� \n");
    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : SideWalk ����   \n");
        m_fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"SideWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SIDEWALK].Act();

        if (2 == m_fCheck)
        {
            if (m_fFrame > m_fFrameEnd)
            {
                m_fCheck = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : SideWalk �� \n");
    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Throw ����   \n");

        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
       
        // ����ü �߻� 
        Engine::Add_GameObject(L"GameLogic", CThrowPipe::Create(m_pGraphicDev, 
            m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y + 1.f , m_pTransformComp->Get_Pos().z));
    }

    if (m_tState_Obj.Can_Update())
    {
        // �浹ü�� ���⼭ ����Ű ������ state���� �ҿ��� 

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Throw ��   \n");
    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : UpRightRun ����   \n");
        m_fFrameSpeed = 13.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"UpRightRun");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::UPRIGHT].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : UpRightRun ��   \n");
    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Frighten ����   \n");
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Frighten");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::FRIGHTEN].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Frighten ��   \n");
    }
    
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Attack ����   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Attack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Attack ��   \n");
    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ����   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeavyAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CGray::AI_Block(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : Block ����   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Block");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        // ���� - ó�� ��ġ�� ���ư������� 
        m_tState_Obj.Set_State(STATE_OBJ::WALK);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��Gray - ���¸ӽ� : HeavyAttack ��   \n");
    }
}

void CGray::AI_CrotchHit(float fDeltaTime)
{
}

void CGray::AI_FacePunch(float fDeltaTime)
{
}

void CGray::AI_Falling(float fDeltaTime)
{
}

void CGray::AI_Dazed(float fDeltaTime)
{
}

void CGray::AI_Chopped(float fDeltaTime)
{
}

void CGray::AI_HeadShot(float fDeltaTime)
{
}

void CGray::AI_Headless(float fDeltaTime)
{
}

void CGray::AI_Death(float fDeltaTime)
{
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
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

        vDir = vPlayerPos - m_pTransformComp->Get_Pos();
        //D3DXVec3Normalize(&vDir, &vDir);
        
        m_pTransformComp->Set_Look(vDir);
        
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
        
        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
    // ����
    {
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
    }

    // ����
    {   
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon����
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
        
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        
        if (STATE_OBJ::KEEPEYE == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fKeepEyeSpeed);
        }

        if (STATE_OBJ::SIDEWALK == m_tState_Obj.Get_State())
        {
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fSideWalkSpeed);
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
    // ����
    {
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::SuddenAttack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon����
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();

        if (STATE_OBJ::UPRIGHTRUN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fUprightSpeed);
       
        if (STATE_OBJ::FRIGHTEN == m_tState_Obj.Get_State())
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
        
        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
        if (STATE_OBJ::ATTACK == m_tState_Obj.Get_State())
        {
            // �浹ü ���� �⺻���� ����

        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            // �浹ü ���� ���� ����
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CGray::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
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
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fSideWalkSpeed);
        }
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion