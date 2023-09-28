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

CGray* CGray::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("GrayMonster Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CGray::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

   // m_pTransformComp->Set_Scale({ 200.f, 100.f, 1.f });
    m_pTransformComp->Set_Pos({ 10.f, 10.f, 25.f });
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;

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

    //m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    //m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    //m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    //m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    //m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

    //m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    //m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    //m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    //m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

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

    // ---------- �׽�Ʈ ���� ---------------------
    if (Engine::IsKey_Pressing(DIK_K))
    {
        m_iHP = 0; //��� ��� Ȯ�ο� 
    }

    if (Engine::IsKey_Pressing(DIK_L))
    {
        m_iHP = 50; // �ǰ� ��� Ȯ�ο� 
    }
    // --------------------------------------------

    Height_On_Terrain(); // ����Ÿ�� 

    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);


    // ���¸ӽ�-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State() || 
            STATE_OBJ::YOUDIE == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    // ������ --------------------------------------
    FaceTurn(fTimeDelta);

    Engine::Add_RenderGroup(RNEDER_ALPHATEST, this);

    return S_OK;
}

void CGray::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGray::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CGray::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.f, vPos.z);
}

HRESULT CGray::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    // ���� �ؽ�ó
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_GrayTextureComp"), E_FAIL);

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}

#pragma region ���¸ӽ� �μ���Ʈ 
void CGray::FaceTurn(const _float& fTimeDelta)
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

    matWorld = *m_pTransformComp->Get_WorldMatrix();

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

_bool CGray::Detect_Player()
{
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    _vec3 MonToPlayer = vMonsterPos - vPlayerPos;
    float currdistance = D3DXVec3Length(&MonToPlayer); // ���� �÷��̾� ��ġ���� ���ͱ����� ���� = �þ߰Ÿ� �� 
   
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

    //���� �÷��̾� ��ġ�� ���� �þ߰Ÿ� �ܺ�(��)�� �ִ� 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // ���ο� �ִ��� �Ǻ� = ���� �� X (���Ͱ� �÷��̾ ���� ����) -> ����ϰ�� �����ؼ� 180�� �̳� = �þ߰��� 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // �þ߰��� ���� && �þ߰Ÿ� �̳� 
    if (fradian < m_fMonsterFov * 2 && currdistance < m_fMonsterSightDistance)
        return true;
    else
        return false;
}

_float CGray::Calc_Distance()
{
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    float fDistance = D3DXVec3Length(&vDistance);

    return fDistance;
}

#pragma endregion


#pragma region ��ǥ(AI) ���¸ӽ� �κ� 
void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
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

    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        // idle���� �״�� �̾ �ؽ�ó ���� 
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            m_fAwareness += fDeltaTime * 3.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 

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
            m_fAwareness -= fDeltaTime * 6.f;

            if (m_fAwareness < 0)
                m_fAwareness = 0;

            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ�Ǹ� idle�� back
            if (0 >= m_fAwareness)
            {
                m_fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

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
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_pTransformComp->Set_ScaleX(0.9f);
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 

        if (m_fFrame > m_fFrameEnd ) // ���� �ι� �ϰ� ���󰡱� 
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
            m_fAwareness += fDeltaTime * 4.f; // �������� �� ������ �����Ұ� 
            if (m_fAwareness >= m_fMaxAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);
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

void CGray::AI_GoHome(float fDeltaTime)
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

void CGray::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 8.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Rest");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // --------�Ÿ��� ���¸ӽ� -----------
                   
        if (Detect_Player())
        {
            _float CurDistance = Calc_Distance();

            // �پ �ٰ��� : 8 < a <= 13
            if (m_fRunDistance < CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7.5 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // �ֽ��ϸ鼭 ��� : 6 < a <= 7.5
            else if (m_fEyesOnYouDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            //���� �����Ϸ� �� : 3 < a <=6
            else if (m_fCloseToYouDistance < CurDistance && m_fEyesOnYouDistance >= CurDistance)
            {
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
                int iCombo = (rand() % 10) + 1;

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                else
                m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }

        }
        else // �i�ٰ��� �þ߿��� ����� ������ �پ�� SUSPICIOUS�� ���ư� 
        {
            m_fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                m_fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
            }

        }
    }
     
    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
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

    }
}

void CGray::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 14.f;
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

    }
}

void CGray::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        if (FALSE == m_bGoHome)
        {
            m_fFrameSpeed = 10.f;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
            m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        }
    }

    if (m_tState_Obj.Can_Update())
    {
        if (TRUE == m_bGoHome)
        {
            // �÷��̾ �ٶ󺸴� ���� : �÷��̾� - ���� 
            _vec3 vDirect = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
            
            if (vDirect.z >= 0) // ��Ʈ�� ���°��� �÷��̾� ���� +z�� ��̸� �ɾ�� 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"WalkNorth");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }
            else if (vDirect.z < 0) // ����Ʈ�� ���°��� �÷��̾� ���� -z�� �׳� �ɾ�� 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Walk");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }

            _float fDistance = D3DXVec3Length(&vDirect);

            if (2.f >= fDistance)
            {
                m_bGoHome = false;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
        else if (FALSE == m_bGoHome)
        {
            if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
                m_mapActionKey[ACTION_KEY::WALK].Act();

            // ���� - �÷��̾ �þ߰����� ������ 
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

void CGray::AI_KeepEye(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
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
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 9.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"SideWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SIDEWALK].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Throw(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Throw");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // �浹ü�� �����ӿ� �°� �ؾ��ؼ� ���⼭ ���鿹�� 


        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"UpRightRun");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::UPRIGHT].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Frighten(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Frighten");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::FRIGHTEN].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"Attack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::BASIC_ATTACK].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"HeavyAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        //�ൿ�� IDLE�϶� WALK ����Ű ������ 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::HEAVY_ATTACK].Act();

        // ���� - �÷��̾ �þ߰����� ������ 
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::REST);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}
/*
void CGray::AI_Block(float fDeltaTime)
{
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
*/

#pragma endregion


#pragma region �ൿ ���¸ӽ� �κ� 
void CGray::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
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

    }
}

void CGray::Approach(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
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
    }
}

void CGray::SideMoving(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // ����
    {
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
        // ���� - zero �ٶ󺸴� ���� ����
        _vec3 vDirect = vPatrolPointZero - m_pTransformComp->Get_Pos();
        
        _float fDistance = D3DXVec3Length(&vDirect);
        
        D3DXVec3Normalize(&vDirect, &vDirect);

        if(2.f >= fDistance)
            m_tState_Act.Set_State(STATE_ACT::IDLE);
        else
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fSideWalkSpeed);
        
    }

    // ����
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}


#pragma endregion