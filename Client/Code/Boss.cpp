#include "stdafx.h"
#include "Boss.h"

CBoss::CBoss(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CBoss::CBoss(const CBoss& rhs)
	: Base(rhs)
{
}

CBoss::~CBoss()
{
}

CBoss* CBoss::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _x, _float _y, _float _z)
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


HRESULT CBoss::Ready_GameObject()
{
    srand(_ulong(time(NULL)));

    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    m_pTransformComp->Set_Scale({ 3.f, 2.5f, 1.f });
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;
    m_fCheck = 0;

    // INFO
    m_iHP = 40;         // ü��
    m_iAttack = 10;     // ���ݷ�

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // �ʱ���� ���� 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBoss::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBoss::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CBoss::AI_Reloading);
    m_tState_Obj.Add_Func(STATE_OBJ::BACKIDLE, &CBoss::AI_BackIdle);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBoss::AI_Chase);
    m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CBoss::AI_Pre_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CBoss::AI_Side_Ready);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBoss::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBoss::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CBoss::AI_Roll);
    m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CBoss::AI_CloseAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CBoss::AI_Shooting);

    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBoss::AI_GoHome);

#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBoss::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACHING, &CBoss::Approaching);
    m_tState_Act.Add_Func(STATE_ACT::ROLLING, &CBoss::Rolling);
    m_tState_Act.Add_Func(STATE_ACT::CLOSEATTACKING, &CBoss::CloseAttacking);
    m_tState_Act.Add_Func(STATE_ACT::SHOOTING, &CBoss::Shooting);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBoss::GoHome);

#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // �ֶ� 

#pragma endregion
    return S_OK;
}

_int CBoss::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    Height_On_Terrain(); // ����Ÿ�� 

    // ---------- �׽�Ʈ ���� ----------------------

    if (Engine::IsKey_Pressing(DIK_N))
    {
        m_iHP = 0; //��� ��� Ȯ�ο� 
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
        m_iHP = 50; // �ǰ� ��� Ȯ�ο� 
    }


    //���¸ӽ�-------------------------------------

    m_fFrame += m_fFrameSpeed * fTimeDelta;

    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State() ||
            STATE_OBJ::RUN == m_tState_Obj.Get_State() ||
            STATE_OBJ::SHOOTING == m_tState_Obj.Get_State())

           m_fCheck += 1;
    }

    // ������ --------------------------------------
    Billboard(fTimeDelta);

    // Renderer -----------------------------------
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CBoss::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CBoss::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CBoss::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.2f, vPos.z);
}

HRESULT CBoss::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_BossTextureComp"), E_FAIL);
    
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

void CBoss::Free()
{
    SUPER::Free();
}

#pragma region ���¸ӽ� �μ���Ʈ 
void CBoss::Billboard(const _float& fTimeDelta)
{
    // ���Ͱ� �÷��̾� �ٶ󺸴� ���� 
    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);
}

_bool CBoss::Detect_Player()
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

_float CBoss::Calc_Distance()
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

void CBoss::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
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

void CBoss::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            m_fBossAwareness += fDeltaTime * 3.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;

                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // �������� ����
        {
            m_fBossAwareness -= fDeltaTime * 6.f;

            if (m_fBossAwareness < 0)
                m_fBossAwareness = 0;

            //�÷��̾ �þ߰��� ��� �������� �ʱ�ȭ�Ǹ� idle�� back
            if (0 >= m_fBossAwareness)
            {
                m_fBossAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Reloading(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_fFrame > m_fFrameEnd)
        {
            // ���� - �÷��̾ �þ߰����� ������ 
            if (TRUE == Detect_Player())
            {
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
            else
            {
                m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_BackIdle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 4.f;
        
        if (Detect_Player())
        {
            m_fBossAwareness += fDeltaTime * 4.f; // �������� �� ������ �����Ұ� 

            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
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

void CBoss::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_ChaseTime += fDeltaTime * 14.f;

        _float CurDistance = Calc_Distance();

        if (m_fCloseAttackDistance >= CurDistance) // ���������� �� �켱�õ� - �Ÿ� �ſ� ����ﶧ�� 
        {
            m_ChaseTime = 0.f;
            m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
        }

        if (Detect_Player())
        {
            if (0 < m_iBulletCnt)
            {
                if (m_fRunDistance < CurDistance) // ���� �Ÿ��� 8 �̻��϶� 
                {
                    m_tState_Obj.Set_State(STATE_OBJ::RUN);
                }
                else
                {
                    int iCombo = (rand() % 10) + 1;

                    if (7 <= iCombo) // ������ �ѽ�°Ŷ� �Ѿ��־�߸��� 40%
                        m_tState_Obj.Set_State(STATE_OBJ::SIDE_READY); // -> Roll -> Shooting

                    if (7 > iCombo) // 60% 
                        m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK); // Run -> Shooting
                }
            }
            else//�Ѿ� ���� 
            {
                m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK); // Run -> Shooting
            }
        }
        else
        {
            m_fBossAwareness -= fDeltaTime * 6.f;

            if (0 >= m_fBossAwareness)
            {
                m_fBossAwareness = 0;
                m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
            }
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBoss::AI_Pre_Attack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"Pre_Attack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        _float CurDistance = Calc_Distance();
        
        m_PreAttackTime += fDeltaTime * 8.f;
        
        // ���� ��Ÿ� a <= 2 -������ ��ȿ 
        if (m_fCloseAttackDistance >= CurDistance)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CLOSEATTACK);
        }

        if (m_MaxTime <= m_PreAttackTime)
        {
            m_PreAttackTime = 0.f;

            if (Detect_Player())
            {
                int iCombo = (rand() % 10) + 1;

                if (0 >= m_iBulletCnt && 6 > iCombo) // 1 ~ 5
                {
                    m_tState_Obj.Set_State(STATE_OBJ::WALK);
                }
                else if (0 >= m_iBulletCnt && 6 <= iCombo) // 6 ~ 10 
                {
                    m_tState_Obj.Set_State(STATE_OBJ::RUN);
                }
                else if (1 <= m_iBulletCnt)
                {
                    m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                }

            }
            else
            {
                m_fBossAwareness -= fDeltaTime * 6.f;

                if (0 >= m_fBossAwareness)
                {
                    m_fBossAwareness = 0;
                    m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
                }
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CBoss::AI_Side_Ready(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"SideReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        m_SideAttackTime += fDeltaTime * 12.f;

        if (m_MaxTime <= m_SideAttackTime)
        {
            m_SideAttackTime = 0.f;

            if (Detect_Player())
            {
                m_tState_Obj.Set_State(STATE_OBJ::ROLL);
            }
            else
            {
                m_fBossAwareness -= fDeltaTime * 6.f;

                if (0 >= m_fBossAwareness)
                {
                    m_fBossAwareness = 0.f;
                    m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);
                }
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Walk(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        if (!m_bGoHome)
        {
            m_fFrameSpeed = 14.f;
            m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk");
            m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        }
    }
    if (m_tState_Obj.Can_Update())
    {
        if (TRUE == m_bGoHome)
        {  
            // // �÷��̾ �ٶ󺸴� ���� : �÷��̾� - ���� 
            _vec3 vDirect = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

            if (vDirect.z >= 0) // ��Ʈ�� ���°��� �÷��̾� ���� +z�� ��̸� �ɾ�� 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"WalkNorth");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }
            else if (vDirect.z < 0) // ����Ʈ�� ���°��� �÷��̾� ���� -z�� �׳� �ɾ�� 
            {
                m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Walk");
                m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
            }

            _float fDistance = D3DXVec3Length(&vDirect);

            if (2.f >= fDistance)
            {
                m_bGoHome = false;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
        if (FALSE == m_bGoHome)
        {
            if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
                m_mapActionKey[ACTION_KEY::WALK].Act();

            // ���� - �÷��̾ �þ߰����� ������ 
            if (m_fCheck == 2)
            {
                m_fCheck = 0;
                m_tState_Obj.Set_State(STATE_OBJ::CHASE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Run(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::RUN].Act();

        if (Calc_Distance() < 5.f)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Roll(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 13.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::ROLL].Act();

        if (m_fFrame > m_fFrameEnd)
        {
            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_CloseAttack(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"CloseAttack");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::CLOSE_ATTACK].Act();

        if (m_fFrame > m_fFrameEnd)
        {

            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Shooting(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_fFrameSpeed = 16.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::SHOOTING].Act();

        if (4 <= m_fCheck)
        {
            m_fCheck = 0;
            m_iBulletCnt -= 1; // ������ ���� 
            m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
    }

    if (m_tState_Obj.Can_Update())
    {
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

#pragma endregion

#pragma region �ൿ ���¸ӽ� �κ� 
void CBoss::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered()) // 1�� bIsEnter false�� �ٲ� 
    {
    }

    if (m_tState_Act.Can_Update())
    {
        // �ȱ� 
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // �޸��� 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // ������ ������!
        if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ROLLING);

        // ��������
        if (m_mapActionKey[ACTION_KEY::CLOSE_ATTACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::CLOSEATTACKING);

        // �Ѿ� �����Ʈ 
        if (m_mapActionKey[ACTION_KEY::SHOOTING].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::SHOOTING);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CBoss::Approaching(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        // run + walk �κ� ���� 
    }

    if (m_tState_Act.Can_Update())
    {
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            _float DirDistance = D3DXVec3Length(&vDir);
            m_pTransformComp->Set_Look(vDir);

            if (m_iBulletCnt == 0) // �Ѿ˴پ� -> ����
            {
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
            }
            else if (0 < m_iBulletCnt ) //�Ѿ� ����
            {
                if (DirDistance >= m_fRunDistance) // �հ�� 
                {
                    m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fRunSpeed);
                }
                if (DirDistance < m_fRunDistance) // ������� 
                {
                    m_tState_Act.Set_State(STATE_ACT::SHOOTING);
                }
            }
        }

        if (STATE_OBJ::WALK == m_tState_Obj.Get_State()
            && m_iBulletCnt == 0 )
        {
            m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

            vDir = vPlayerPos - m_pTransformComp->Get_Pos();
            m_pTransformComp->Set_Look(vDir);
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);

        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CBoss::Rolling(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        _vec3 Right = m_pTransformComp->Get_Right();
        D3DXVec3Normalize(&Right, &Right);
        m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fRollingSpeed);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::CloseAttacking(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::Shooting(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CBoss::GoHome(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

#pragma endregion

