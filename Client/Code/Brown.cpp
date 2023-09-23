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
    Free();
}

HRESULT CBrown::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    
    m_pTransformComp->m_vScale.x = 0.4f;
    m_pTransformComp->m_vInfo[INFO_POS] = { 5.f, 10.f, 25.f };
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;

    // INFO
    m_iHP = 40;         // ü��
    m_iAttack = 10;     // ���ݷ�

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);

    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFYING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::BASICATTACK, &CBrown::AI_BasicAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE�� ::A �϶� CBrownŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    
    //m_tState_Act.Add_Func(STATE_ACT::WALK, &CBrown::Walk);
    //m_tState_Act.Add_Func(STATE_ACT::RUN, &CBrown::Run);
    //m_tState_Act.Add_Func(STATE_ACT::INCH, &CBrown::Inch);
    //m_tState_Act.Add_Func(STATE_ACT::JUMP, &CBrown::Jump);

    //m_tState_Act.Add_Func(STATE_ACT::PRE_ATTACK, &CBrown::Prepare_Atk);
    //m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    //m_tState_Act.Add_Func(STATE_ACT::HEAVY, &CBrown::Heavy_Attack);
    //m_tState_Act.Add_Func(STATE_ACT::PARRYING, &CBrown::Parrying);
    //m_tState_Act.Add_Func(STATE_ACT::FALLING, &CBrown::Falling);
    //m_tState_Act.Add_Func(STATE_ACT::DEAD, &CBrown::Dead);
#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
    m_mapActionKey.Add_Action(ACTION_KEY::INCH); //����ﶧ
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // ����ﶧ

    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y�࿡ ���̰� ������ 
  
    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

#pragma endregion

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_fFrame += m_fFrameSpeed * fTimeDelta;

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State()
          || STATE_OBJ::RUN == m_tState_Obj.Get_State()
          || STATE_OBJ::WALK == m_tState_Obj.Get_State()
          || STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State()
          || STATE_OBJ::STRAFYING == m_tState_Obj.Get_State()
          || STATE_OBJ::BASICATTACK == m_tState_Obj.Get_State()
          || STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State() )
            m_fCheck += 1;
    }

    //���¸ӽ�
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    Height_On_Terrain(); // ����Ÿ�� 

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBrown::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // ���� �ؽ�ó
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BrownTextureComp"), E_FAIL);
        
    return S_OK;
}

CBrown* CBrown::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("Monster Create Failed");
        return nullptr;
    }

    return pInstance;
}

_bool CBrown::Monster_Capture() // ���� �þ߳� �÷��̾� �ִ��� üũ�ϴ� �Լ� 
{
    // �÷��̾� ��ġ ������ 
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // �÷��̾��� ��ġ, �ٶ󺸴� ����
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    // ���� ��ġ, �ٶ󺸴� ����
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    // ���Ϳ� �÷��̾� ��ġ ���� ��� + ����ȭ 
    _vec3 MonToPlayer = vMonsterPos - vPlayerPos;
   float currdistance = D3DXVec3Length(&MonToPlayer); // ���� �÷��̾� ��ġ���� ���ͱ����� ���� = �þ߰Ÿ� �� 
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);
   
   //���� �÷��̾� ��ġ�� ���� �þ߰Ÿ� �ܺ�(��)�� �ִ� 
   if (currdistance > m_fMonsterSightDistance) 
       return false;

    // ���ο� �ִ��� �Ǻ� = ���� �� X (���Ͱ� �÷��̾ ���� ����) -> ����ϰ�� �����ؼ� 180�� �̳� = �þ߰��� 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI; 

    // �þ߰��� ���� && �þ߰Ÿ� �̳� 
    if (fradian < m_fMonsterFov * 2 && currdistance <  m_fMonsterSightDistance ) 
        return true;
    else
        return false;
}

float CBrown::m_fDistance()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // �÷��̾��� ��ġ
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    // ���� ��ġ
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    // �ѻ��� ���� 
    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    // �ѻ��� ���� 
    float fDistance = D3DXVec3Length(&vDistance);
    
    return fDistance;
}

HRESULT CBrown::Chase_Player(const _float& fTimeDelta, _float fSpeed)
{ 


    return S_OK;
}

void CBrown::Free()
{
    SUPER::Free();
}

void CBrown::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 1;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (Monster_Capture())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 1;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Monster_Capture()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            m_fAwareness += fDeltaTime * 3.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // �߰����� ���� 
            }
        }
        else // �������� ����
        {
            m_fAwareness -= fDeltaTime * 6.f;

            if (m_fAwareness < 0)
                m_fAwareness = 0;
         
            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ�Ǹ� idle�� back
            if (0 == m_fAwareness)
            {
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 6;
        m_fFrameSpeed = 7.f;
        m_pTransformComp->m_vScale.x = 0.4f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Taunt");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (3 == m_fCheck)
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = �߰ݸ��
           // m_tState_Act.Set_State(STATE_ACT::CHASE); // �ൿ = �߰��ൿ - �÷��̾� ����ٴ� 
        }
        // �÷��̾������� ���ƺ����� 

    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Chase(float fDeltaTime) // �޸��ٰ� �ȴٰ� �쳯�������� 
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f; //���󺹱� 
        m_pTransformComp->m_vScale.x = 0.4f;
    }
    if (m_tState_Obj.Can_Update()) // �þ߹��� 7 
    {
        if (Monster_Capture()) // ��ü ��Ÿ� = m_fMonsterSightDistance = 12.f
        {
            // �پ �ٰ��� : a > 8
            if (m_fRunDistance < m_fDistance())
            {
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7 < a <= 8 
            else if (m_fWalkDistance < m_fDistance() && m_fRunDistance >= m_fDistance())
            {
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // ���� : 4 < a <= 7
            else if (m_fInchDistance < m_fDistance() && m_fWalkDistance >= m_fDistance())
            {
                int iCombo = rand() % 2; // 0 or 1

                if (1 == iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (2 == iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFYING);
            }
            // ������
            else
            {
                int iCombo = rand() % 2; // 0 or 1

                if (1 == iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::BASICATTACK);

                if (2 == iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // �i�ٰ��� �þ߿��� ����� ������ �پ�� SUSPICIOUS�� ���ư� 
        {
            m_fAwareness -= fDeltaTime * 4.f;

            if (m_fAwareness < 0)
                m_fAwareness = 0;
            
            if (0 == m_fAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 21;
        m_fFrameSpeed = 12.f;
        m_pTransformComp->m_vScale.x = 0.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� RUN ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (1 == m_fCheck && m_fRunDistance >= Monster_Capture()) // �ѹ� ������ ���� 
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 24;
        m_fFrameSpeed = 12.f;
        m_pTransformComp->m_vScale.x = 0.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

            // �ൿ �߰� 
        if (1 == m_fCheck && m_fWalkDistance >= Monster_Capture())
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_InchForward(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 4;
        m_fFrameSpeed = 10.f;
        m_pTransformComp->m_vScale.x = 0.4f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
    }
    if (m_tState_Obj.Can_Update())
    {
        // �ൿ �߰� 

        if (1 == m_fCheck) // �ѹ� ������ ���� 
        {
            m_fCheck = 0; 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Strafing(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 6;
        m_fFrameSpeed = 10.f;
        m_pTransformComp->m_vScale.x = 0.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Strafing");
    }
    if (m_tState_Obj.Can_Update())
    {     
        // �ൿ �߰� 

        if (1 == m_fCheck) // �ѹ� ������ ���� 
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_BasicAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameEnd = 5;
        m_fFrameSpeed = 10.f;
        m_pTransformComp->m_vScale.x = 0.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"BasicAttack");
    }
    if (m_tState_Obj.Can_Update())
    {
        if (1 == m_fCheck)
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {
        if (1 == m_fCheck)
        {
            m_fCheck = 0;
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

//------------------ �ൿ -------------------------

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
    }

    if (m_tState_Act.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
       
    }
}

void CBrown::Approach(float fDeltaTime) // RUN �׼�Ű ���� �����ϴ°�
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
            // �÷��̾� ��ġ ������ 
            m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
            //NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

            //����ġ 
            m_pTransformComp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Monster", L"Com_Transform"));
           // NULL_CHECK_RETURN(m_pTransformComp, -1);

            _vec3	vPlayerPos, vMonsterPos, vDir;

            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos); //�÷��̾� ��ġ 
            m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos); //�÷��̾� ��ġ 

            vDir = vPlayerPos - vMonsterPos;

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);

            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        {
           /* Chase_Player(fDeltaTime, m_fWalkSpeed);*/
                // �÷��̾� ��ġ ������ 
            m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
            //NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

            //����ġ 
            m_pTransformComp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Monster", L"Com_Transform"));
            //NULL_CHECK_RETURN(m_pTransformComp, -1);

            _vec3	vPlayerPos, vMonsterPos, vDir;

            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos); //�÷��̾� ��ġ 
            m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos); //�÷��̾� ��ġ 

            vDir = vPlayerPos - vMonsterPos;

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);
            
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        }
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}
/*
void CBrown::Walk(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    ;
    }

    // ����
    {


    }

    // ����
    {
      
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Inch(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Jump(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Prepare_Atk(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }
    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Heavy_Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Parrying(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Falling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBrown::Dead(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    if (m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}
*/
