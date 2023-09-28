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
    srand((_uint)time(NULL));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
    
    //m_pTransformComp->Set_Scale({ 200.f, 100.f, 1.f });
    m_pTransformComp->Set_Pos({ 5.f, 1.f, 25.f });
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;

    // INFO
    m_iHP = 100;         // ü��

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);
    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBrown::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBrown::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CBrown::AI_Taunt);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBrown::AI_Chase);
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CBrown::AI_Rest);

    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBrown::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBrown::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::STRAFING, &CBrown::AI_Strafing);
    m_tState_Obj.Add_Func(STATE_OBJ::INCHFORWARD, &CBrown::AI_InchForward);
    m_tState_Obj.Add_Func(STATE_OBJ::BASICATTACK, &CBrown::AI_BasicAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CBrown::AI_Jump);

    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CBrown::AI_HitByPitchedBall);

    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CBrown::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CBrown::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CBrown::AI_Death);

#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE�� ::A �϶� CBrownŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CBrown::Moving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    
#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // �ֶ� 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //�߰�
    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //����ﶧ
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
    
    // ��ġ�� �������� 
    Get_PlayerPos(fTimeDelta);
    
    // ����Ÿ�� 
    Height_On_Terrain(); 

    // ���¸ӽ�-------------------------------------

    m_fFrame += m_fFrameSpeed * fTimeDelta;


    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;
   
        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    // ������ --------------------------------------

    //FaceTurn(fTimeDelta);
    
    // --------------------------------------------

    
    // ---------- �׽�Ʈ ���� ----------------------

    Monster_Jump(fTimeDelta);
   
    if (Engine::IsKey_Pressing(DIK_H))
    {
        m_iHP = 0; //��� ��� Ȯ�ο� 
    }

    if (Engine::IsKey_Pressing(DIK_J))
    {
        m_iHP = 50; // �ǰ� ��� Ȯ�ο� 
    }

    // --------------------------------------------

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return S_OK;
}

void CBrown::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBrown::Render_GameObject()
{
    DWORD dwTest;
    m_pGraphicDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwTest);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
   // m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    //m_pGraphicDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBrown::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BrownTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
        
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

_bool CBrown::Detect_Player() // ���� �þ߳� �÷��̾� �ִ��� üũ�ϴ� �Լ� 
{
    // �÷��̾� ��ġ ������ 
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
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    float fDistance = D3DXVec3Length(&vDistance);
    
    return fDistance;
}

void CBrown::FaceTurn(const _float& fTimeDelta)
{
    //case1. ȸ����� ����� 
    _matrix		matWorld, matView, matBill, matScale, matChangeScale;

   matWorld = *m_pTransformComp->Get_Transform();

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // ȸ����� ����
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    // case2. ������ �����ϱ� 
    /*_matrix		matWorld, matView, matBill;

    matWorld = *m_pTransformComp->Get_Transform();

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matBill);

    matBill._11 = matView._11;
    matBill._13 = matView._13;
    matBill._31 = matView._31;
    matBill._33 = matView._33;

    D3DXMatrixInverse(&matBill, 0, &matBill);

    m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));*/

    m_pTransformComp->Set_ScaleY(1.9f);
}

HRESULT CBrown::Get_PlayerPos(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    return S_OK;
}

void CBrown::Monster_Jump(const _float& fTimeDelta)
{
    if (Engine::IsKey_Pressed(DIK_SPACE))
    {
        m_bJump = true;
    }

    if (m_bJump)
    {
        _vec3 jump = { 0.f, 50.f , 0.f };

        m_pTransformComp->Move_Pos(&jump, fTimeDelta, -5.f);
       // m_pTransformComp->Move_Pos(&Gravity, fTimeDelta, 0.01f);
    }
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

    m_pTransformComp->Set_Pos(vPos.x, fHeight+1.f, vPos.z);
}

//------------------ AI ---------------------------
void CBrown::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
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
        
        // �ѹ��� �ڿ��� ����� ���������� 
        if (0 == m_iHP) 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHOPPED);
        }
        
        if( m_iHP != m_iMaxHP )
        {
          //CPlayer::STATE_RIGHTHAND::NONE ;
          m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }

        // �÷��̾ ������ ����� ��
        // Get_Player()->Get_m_pRightHandComp()->Get_VecTexture()->front()
        // Get_Player()->Get_PlayerGunState()
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Suspicious");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if(m_iPreHP != m_iHP)
            m_tState_Obj.Set_State(STATE_OBJ::HIT);

        if(Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
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

            if (0 >= m_fAwareness)
            {
                m_fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    };

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Taunt(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
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
    }
}

void CBrown::AI_Chase(float fDeltaTime) // �޸��ٰ� �ȴٰ� �쳯�������� 
{
    if (m_tState_Obj.IsState_Entered())
    {
       // m_pTransformComp->m_vScale.x = 0.5f;
        m_fFrameSpeed = 10.f; //���󺹱� 
    }
    if (m_tState_Obj.Can_Update())
    {
        _float CurDistance = m_fDistance();

        if (Detect_Player()) // ��Ÿ��� �����Ǵ°�� 
        {
            // �þ߰Ÿ��� ���� ���¸ӽź��� �ǰ�/������ �� ����  

            // --------�ǰݻ��� ���¸ӽ� -----------
            if (m_iPreHP != m_iHP)
                m_tState_Obj.Set_State(STATE_OBJ::HIT);

            if (0 == m_iHP)
                m_tState_Obj.Set_State(STATE_OBJ::DEATH);

            // --------�Ÿ��� ���¸ӽ� -----------
            // �پ �ٰ��� : a > 8
            if (m_fRunDistance < CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // ���� : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // ������
            else
            {
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::BASICATTACK);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // �i�ٰ��� �þ߿��� ����� ������ �پ�� SUSPICIOUS�� ���ư� 
        {
            m_fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                m_fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 12.f;
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

    }
}

void CBrown::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 11.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"RunSouth");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� RUN ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
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
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Walk_South");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ - �ٰ����� 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        // �ൿ �߰� 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
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
       // m_pTransformComp->m_vScale.x = 0.4f;
        m_fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"InchForward");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::INCHFORWARD].Act();

        if (m_fFrame > m_fFrameEnd)
        {
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
        m_fFrameSpeed = 11.f;
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

    }
}

void CBrown::AI_BasicAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"BasicAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
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

    }
}

void CBrown::AI_Jump(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
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

    }  
}

void CBrown::AI_Hit(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
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

    }
}

void CBrown::AI_FacePunch(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"FacePunch");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���ݻ���         
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_HitByPitchedBall(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 0.1f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"CrotchHit");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // �÷��̾� �������� + ���ݻ��� 
        m_tState_Obj.Set_State(STATE_OBJ::CHASE);
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Dazed(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_iPreHP = m_iHP;
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Jump");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

    }

    if (m_tState_Obj.Can_Update())
    { 
        //�浹ü ���� HP��� (�����ϸ�) 
        
        // ��Ʈ ������ 
        if (0 == m_iHP)
        {
            if (m_fFrame > m_fFrameEnd)
            {
                m_tState_Obj.Set_State(STATE_OBJ::DEATH);
            }
        }

        //�ƹ��� ��Ʈ�� ���� 
        if(m_iPreHP == m_iHP) 
        {
            if (m_fFrame > m_fFrameEnd)
            {
                m_tState_Obj.Set_State(STATE_OBJ::REST);
            }
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Chopped(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Chopped");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        DeadSpin = false;
    }

    if (m_tState_Obj.Can_Update())
    {
        if(!Dead) //�����ʿ� 
        m_pTransformComp->Rotate(ROT_X, fDeltaTime*0.75f);
        
        if (m_fFrame > m_fFrameEnd)
        {
            m_fFrame = m_fFrameEnd -1;
            Dead = true;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Headless(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Headless");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (!Dead) //�����ʿ� 
            m_pTransformComp->Rotate(ROT_X, fDeltaTime * 0.75f);

        if (m_fFrame > m_fFrameEnd)
        {
            m_fFrame = m_fFrameEnd - 1;
            Dead = true;
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Death(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"Death");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_tState_Obj.Set_State(STATE_OBJ::CHASE);
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBrown::AI_Reconnaissance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 3.f;

        if (Detect_Player())
        {
            m_fAwareness += fDeltaTime * 4.f; // �������� �� ������ �����Ұ� 
            if (m_fAwareness >= m_fMaxAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }

        if (0 >= m_fConsider)
        {
            m_fConsider = 10.f; // �ٽ� �ʱ� �������� 
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        // ���� - ó�� ��ġ�� ���ư������� 
        m_bGoHome = true;
        m_tState_Obj.Set_State(STATE_OBJ::WALK);
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


        if (m_mapActionKey[ACTION_KEY::INCHFORWARD].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);


        if (m_mapActionKey[ACTION_KEY::STRAFING].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::MOVING);

        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);


        if (m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ATTACK);

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
            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            m_pTransformComp->Set_Look(vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
            
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State())
        { 
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
    }
}
void CBrown::Moving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
           // m_bStrafing = true;

            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();

            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fInchSpeed);


        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
           _vec3 MoveDir;
           MoveDir = m_pTransformComp->Get_Right();

           m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, m_fStrafingSpeed);

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

void CBrown::Attack(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
        if (STATE_OBJ::BASICATTACK == m_tState_Obj.Get_State())
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

void CBrown::GoHome(float fDeltaTime)
{
}
