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
    
    m_pTransformComp->Set_Scale({ 1.f, 1.f, 1.f });
    m_fFrameEnd = 0;
    m_fFrameSpeed = 8.f;

    // INFO
    m_iHP = 100;         // ü��

    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    FCollisionSphere* pShape = dynamic_cast<FCollisionSphere*>(m_pColliderComp->Get_Shape());
    pShape->fRadius = 3.f;


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
    m_tState_Obj.Add_Func(STATE_OBJ::BASICATTACK, &CBrown::AI_BasicAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::HEAVYATTACK, &CBrown::AI_HeavyAttack);

    //�ǰ�
    m_tState_Obj.Add_Func(STATE_OBJ::HIT, &CBrown::AI_Hit);
    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CBrown::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::FACEPUNCH, &CBrown::AI_FacePunch);
    m_tState_Obj.Add_Func(STATE_OBJ::FALLING, &CBrown::AI_HitByPitchedBall);

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
    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::GOHOME);

#pragma endregion

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);
    // ���� HP ����� - HP��°� ���¸ӽų����� ���̱⶧���� ��ȭ�� ����� Collision���� �����ϱ����� 
    m_iPreHP = m_iHP;

    // ��ġ�� �������� 
    Get_PlayerPos(fTimeDelta);
    
    // ����Ÿ�� 
    Height_On_Terrain(); 

    // ������
    Billboard(fTimeDelta);
    
    // ���¸ӽ�-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    // ���� �ǰ� �׽�Ʈ�� 
    if (Engine::IsKey_Pressing(DIK_H))
    {
        m_tState_Obj.Set_State(STATE_OBJ::GOHOME);
    }

    if (Engine::IsKey_Pressing(DIK_J))
    {
        m_iHP = 0; // �ǰ� ��� Ȯ�ο� 
        m_tState_Obj.Set_State(STATE_OBJ::DEATH);
    }

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State()
            || STATE_OBJ::DEATH == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    // ���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 
    //_vec3 vTest = m_pTransformComp->Get_Pos();
    //vTest.z += 10.f;
    //list<CGameObject*> listCollision = Engine::IntersectTests_Sphere_GetGameObject(0, vTest, 5.f);
    //for (auto iter = listCollision.begin(); iter != listCollision.end(); ++iter)
    //    (*iter)->Set_Dead();

    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

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

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBrown::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BrownTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
   
    // �ݶ��̴� ������Ʈ
    NULL_CHECK_RETURN(m_pColliderComp = Set_DefaultComponent_FromProto<CColliderComponent>(ID_DYNAMIC, L"Com_Collider", L"Proto_ColliderSphereComp"), E_FAIL);
    // ���� ���� ���
    m_pColliderComp->EnterToPhysics(0);
    // �浹 �Լ� ����
    m_pColliderComp->Set_Collision_Event<ThisClass>(this, &ThisClass::OnCollision);
    m_pColliderComp->Set_CollisionEntered_Event<ThisClass>(this, &ThisClass::OnCollisionEntered);
    m_pColliderComp->Set_CollisionExited_Event<ThisClass>(this, &ThisClass::OnCollisionExited);

    return S_OK;
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

   
    pInstance->m_pTransformComp->Set_Pos(_x, _y, _z);
    pInstance->vPatrolPointZero = { _x, _y, _z};
    return pInstance;
}

void CBrown::OnCollision(CGameObject* pDst) // ��� �浹�� 
{
    //OutputDebugString(L"��Brown�̶� �浹�� \n");

    // �÷��̾ ������ �Ѱ����� �����ؾ���. �⺻������ Collision�� ��������. 
}

void CBrown::OnCollisionEntered(CGameObject* pDst) // ó�� �浿 ���� 
{
    //OutputDebugString(L"��Brown�̶� �浹���� \n");
    // �÷��̾� �������, �÷��̾ ������������ �ʿ��� ���⿡ ��ȭ 
    //����Ʈ�� �ϴ� �־�� 
    m_tState_Obj.Set_State(STATE_OBJ::HIT);

    switch (m_PlayerState)
    {
    case PUNCH:
        m_iHP -= 1;
        m_tState_Obj.Set_State(STATE_OBJ::HIT);
        break;

    case PISTOL:
        m_iHP -= 100;
        m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        break;

    case TOMSON:
        m_iHP -= 100;
        m_tState_Obj.Set_State(STATE_OBJ::HEADLESS);
        break;

    case RUN:
        m_iHP -= 1;
        m_tState_Obj.Set_State(STATE_OBJ::FALLING);
        break;

    }

}

void CBrown::OnCollisionExited(CGameObject* pDst) // �浹 ������ 
{
    //OutputDebugString(L"��Brown�̶� �浹���� \n");

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

void CBrown::Billboard(const _float& fTimeDelta)
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
    }

    if (m_tState_Obj.Can_Update())
    {
        if(Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
           // //OutputDebugString(L"��Brown - ����üũ : �������� �����   \n");
            m_fAwareness += fDeltaTime * 2.f;
            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness; // ���� ���Ҹ� ���� �ִ밪���� ���� 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // �߰����� ���� 
            }
        }
        else // �������� ����
        {
            ////OutputDebugString(L"��Brown - ����üũ : �������� ������   \n");
            m_fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fAwareness)
            {
                m_fAwareness = 0.f;
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
        _float CurDistance = m_fDistance();

        //OutputDebugString(L"�� ����� ã�� : Chease - Update ����   \n");

        if (Detect_Player()) // ��Ÿ��� �����Ǵ°�� 
        {
            //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ���� \n");
            // --------�Ÿ��� ���¸ӽ� -----------
            // �پ �ٰ��� : a > 8
            if (m_fRunDistance < CurDistance)
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ���� - Run ����  \n");
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // �ɾ �ٰ��� : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ���� - Walk ���� \n");
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // ���� : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ���� - Moving ���� \n");
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // ������
            else
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ���� - ���� ����  \n");
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::BASICPlayerLighter);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // ��Ÿ��� �÷��̾ ��������� 
        {
            //OutputDebugString(L"��Brown - ���¸ӽ� : �÷��̾� ��ħ    \n");
            m_fAwareness -= fDeltaTime * 4.f; // ������ ���� 

            if (0 >= m_fAwareness) //�������� �ʱ�ȭ�Ǹ� 
            {
                //OutputDebugString(L"�� ����� ã�� : Chease -�÷��̾� ��ħ - Recon���� �Ѿ  \n");
                m_fAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE); // �� ���� �����ؾ���, ������ġ�� ���ư��� goHOME���� ������ 
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
        if (m_fDistance() <= m_fInchDistance)
        {
            //OutputDebugString(L"�� ����� ã�� : INCHFORWARD - ������ �� ���� \n");
            //OutputDebugString(L"��Brown - ���¸ӽ� : InchForward ��   \n");
            m_tState_Obj.Set_State(STATE_OBJ::BASICATTACK);
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Strafing ��   \n");
    }
}

void CBrown::AI_BasiCPlayerLighter(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : Basic Attack ����   \n");
        m_fFrameSpeed = 8.5f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Multi", L"BasiCPlayerLighter");
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
          //OutputDebugString(L"��Brown - ���¸ӽ� : Basic Attack ��   \n");
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
        // ���ݻ���         
        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : FacePunch ��   \n");
    }
}

void CBrown::AI_HitByPitchedBall(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
          //OutputDebugString(L"��Brown - ���¸ӽ� : HitByPitchedBall ����   \n");

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
          //OutputDebugString(L"��Brown - ���¸ӽ� : HitByPitchedBall ��   \n");
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
        if (1 == m_fCheck)
        {
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"FinalDeath");
        }
       // m_tState_Obj.Set_State(STATE_OBJ::CHASE);
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
            m_fAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 
            if (m_fAwareness >= m_fMaxAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
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
          m_fFrameSpeed = 13.f;
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

//------------------ �ൿ -------------------------

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
        if (m_mapActionKey[ACTION_KEY::BASIC_ATTACK].IsOnAct())
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
        if (STATE_OBJ::INCHFORWARD == m_tState_Obj.Get_State())
        {
            //OutputDebugString(L"��Brown - ����Ű : INCHFORWARD ����   \n");

            // ���Ͱ� �÷��̾ ���º��� = �� - �� 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos(); // �÷��̾� ������ �̵� 
            D3DXVec3Normalize(&vDir, &vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fInchSpeed);
        }

        if (STATE_OBJ::STRAFING == m_tState_Obj.Get_State())
        {
           //OutputDebugString(L"��Brown - ����Ű : STRAFING ����   \n");

           _vec3 MoveDir;
           MoveDir = m_pPlayerTransformcomp->Get_Right(); // �÷��̾� ������ 
           D3DXVec3Normalize(&MoveDir, &MoveDir);
           m_pTransformComp->Move_Pos(&MoveDir, fDeltaTime, m_fStrafingSpeed);

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
        if (STATE_OBJ::BASICPlayerLighter == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"��Brown - ����Ű : BASICATTACK ����   \n");
            // �浹ü ���� �⺻���� ����

        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
              //OutputDebugString(L"��Brown - ����Ű : HEAVYATTACK ����   \n");
            // �浹ü ���� ���� ����
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
