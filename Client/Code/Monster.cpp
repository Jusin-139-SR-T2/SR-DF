#include "stdafx.h"
#include "Monster.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphicDev)
    : Base(pGraphicDev)
{
}

CMonster::CMonster(const CMonster& rhs)
    : Base(rhs)
{
}

CMonster::~CMonster()
{
}

HRESULT CMonster::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    m_pTransformComp->m_vScale.x = 0.5f;

    m_pTransformComp->m_vInfo[INFO_POS] = { 5.f, 10.f, 5.f };
   // m_pTransformComp->m_vInfo[INFO_POS] = { 100.f, 100.f, 100.f };


    // INFO
    m_iHP = 40;         // ü��
    m_iAttack = 10;     // ���ݷ�

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CMonster::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CMonster::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CMonster::AI_Chase);

    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CMonster::AI_Attack);
#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
                    // STATE�� ::A �϶� CMonsterŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CMonster::Idle);
    
    m_tState_Act.Add_Func(STATE_ACT::WALK, &CMonster::Walk);
    m_tState_Act.Add_Func(STATE_ACT::RUN, &CMonster::Run);
    m_tState_Act.Add_Func(STATE_ACT::INCH, &CMonster::Inch);
    m_tState_Act.Add_Func(STATE_ACT::JUMP, &CMonster::Jump);

    m_tState_Act.Add_Func(STATE_ACT::PRE_ATTACK, &CMonster::Prepare_Atk);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CMonster::Attack);
    m_tState_Act.Add_Func(STATE_ACT::HEAVY, &CMonster::Heavy_Attack);
    m_tState_Act.Add_Func(STATE_ACT::PARRYING, &CMonster::Parrying);
    m_tState_Act.Add_Func(STATE_ACT::FALLING, &CMonster::Falling);
    
    m_tState_Act.Add_Func(STATE_ACT::DEAD, &CMonster::Dead);
#pragma endregion


#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::WALK);
    m_mapActionKey.Add_Action(ACTION_KEY::RUN);
    m_mapActionKey.Add_Action(ACTION_KEY::JUMP);

    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::INCH);
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING);

#pragma endregion

    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_fFrame += 6.f * fTimeDelta;
    if (14 < m_fFrame)
        m_fFrame = 0.f;

    //���¸ӽ� ���ư��°� 
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    Height_On_Terrain(); // ����Ÿ�� 

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CMonster::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CMonster::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMonster::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // ���� �ؽ�ó
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);

    return S_OK;
}

CMonster* CMonster::Create(LPDIRECT3DDEVICE9 pGraphicDev)
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

_bool CMonster::Monster_Capture()
{
    // ���� �þ߳� �÷��̾� �ִ��� üũ�ϴ� �Լ� 
    

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

float CMonster::m_fDistance()
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

void CMonster::Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed)
{
    _vec3   vMonsterPos;
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    _vec3	vDir = *pTargetPos - vMonsterPos; // ���Ͱ� ���� ��� ���� 

    vMonsterPos += *D3DXVec3Normalize(&vDir, &vDir) * fTimeDelta * fSpeed;
}

void CMonster::Free()
{
    SUPER::Free();
}

void CMonster::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

//------------------ AI ---------------------------
void CMonster::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
       
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

void CMonster::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //Taunt�̹��� ��� - �̱���
       // m_iFrameEnd = 1; 
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Monster_Capture()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            // ���� ���� �������� 
            m_fAwareness += fDeltaTime * 10.f;

            // 1. �������� �������� �̻��̸� forward���͸� ȸ�����Ѽ� �÷��̾������� ���Ը��� 
            if (8 <= m_fAwareness && m_fMaxAwareness > m_fAwareness && SpinCheck)
            {
                // �÷��̾� ��ġ
                m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
                _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

                m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
                m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

                // ���� ��ġ, �ٶ󺸴� ����
                m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
                m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

                // ���Ͱ� �÷��̾� ���� ����
                _vec3 vMonLookPlayer = vPlayerPos - vMonsterPos;

                // ���Ͱ� �÷��̾ �ٶ󺸱� ���� ��ǥ ���� (���Ͷ� ���� - ����� �޶󵵵�)
                _float targetAngle = atan2(vMonLookPlayer.z, vMonLookPlayer.x);

                _float ftheta = acos(D3DXVec3Dot(&vPlayerLook, &vMonsterLook));

                if (ftheta == targetAngle)
                {
                    SpinCheck = false;
                }
                else
                    m_pTransformComp->Rotation(ROT_Y, targetAngle * fDeltaTime);

            }

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fAwareness || 15.f <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
                // ���� �����°� üũ �Ϸ� 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); //�� ���� ������ 
            }
        }
        else // �������� ����
        {
            m_fAwareness -= fDeltaTime * 30.f;

            if (m_fAwareness < 0)
                m_fAwareness = 0;
         
            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ��
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

void CMonster::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        // �߰��Ҷ� �̹��� end - 8���� * �������� 
       // m_iFrameEnd = 
    }

    if (m_tState_Obj.Can_Update()) // �þ߹��� 7 
    {
        // �Ÿ��� �ʹ��ִ� - �޸��� 
        //if (7.f < m_fDistance())
        //{
        //    m_mapActionKey[ACTION_KEY::RUN].Act();
        //}

        // �ѻ��� �Ÿ� 3~7  - �ȱ� 
        if (3.f < m_fDistance() && 7.f > m_fDistance())
        {
            m_mapActionKey[ACTION_KEY::WALK].Act();
        }

        // �Ÿ��� ���� �Ÿ��϶� - ���� // �� ������ �� �� ����. ã�Ƴ�����
        //if ( )
        //{
        //    m_mapActionKey[ACTION_KEY::JUMP].Act();
        //}

        // �Ÿ��� ���� �Ÿ��϶� ���ݸ������ �ٲ��. 
        //if ( 3.f > m_fDistance()  )
        //{
        //    m_tState_Obj.Set_State(STATE_OBJ::ATTACK); 
        //}

    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CMonster::AI_Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        //Set_FrameKey(0, L"NBT_Mettaur_Prepare_Atk");
       // CAnimationTable::Get_Instance()->Load_AnimData(L"1", Get_FrameList()[0]);
    }

    if (m_tState_Act.Can_Update())
    {
        //����Ȧ¦�� ���� ������ ����� ������ 
        if (1 == int(fDeltaTime) % 2 + 1)
        {
            int a = 3;
        }
        if (2 == int(fDeltaTime) % 2 + 1)
        {
            int b = 3;
        }

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

//------------------ �ൿ -------------------------

void CMonster::Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered()) 
    {
    }

    if (m_tState_Obj.Can_Update()) 
    {
        // �׼�Ű�� �������ºб� 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // �� �׼�Ű�� ������        
            m_tState_Act.Set_State(STATE_ACT::RUN); // �ൿ���¸ӽſ��� �� Ŭ������ ����� �Լ��� �����ض� -> 52�� ���κ��� walk�Լ� ���� = 441����    

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::WALK);

        if (m_mapActionKey[ACTION_KEY::JUMP].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::JUMP);
    }

    if (m_tState_Obj.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
    }
}


void CMonster::Walk(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    // ����
    {
        _vec3 vPlayerPos, vMonsterPos;
        m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));

        // ��ġ
        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
        
        _vec3	vDir = vPlayerPos - vMonsterPos; // ���Ͱ� ���� ��ǥ�� ���� 
        D3DXVec3Normalize(&vDir, &vDir);
        
        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);
       
        // vMonsterPos += vDir * fDeltaTime * m_fWalkSpeed;

    }

    // ����
    {
      
    }

    if (m_tState_Act.IsState_Exit())
    {

        //m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
}

void CMonster::Run(float fDeltaTime) // RUN �׼�Ű ���� �����ϴ°�
{
    //�¿�� �����鼭 �÷��̾���� �Ÿ� ����  
    int a = 3;
}

void CMonster::Inch(float fDeltaTime)
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

void CMonster::Jump(float fDeltaTime)
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

void CMonster::Prepare_Atk(float fDeltaTime)
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

void CMonster::Attack(float fDeltaTime)
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

void CMonster::Heavy_Attack(float fDeltaTime)
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

void CMonster::Parrying(float fDeltaTime)
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

void CMonster::Falling(float fDeltaTime)
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

void CMonster::Dead(float fDeltaTime)
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
