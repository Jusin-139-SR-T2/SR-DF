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

    m_eCurrPhase = Phase_End;

    // �浹��
    m_pTransformComp->Readjust_Transform();
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �浹 �ҷ����°� 
    pShape = dynamic_cast<FCollisionBox*>(m_pColliderComp->Get_Shape());
    pShape->vHalfSize = { 1.f, 0.7f, 0.3f };

    // INFO
    m_gHp.Cur = 100.f;
    m_gHp.Max = 100.f;

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // �ʱ���� ���� 

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CBoss::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CBoss::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::RELOADING, &CBoss::AI_Reloading);
    m_tState_Obj.Add_Func(STATE_OBJ::BACKIDLE, &CBoss::AI_BackIdle);

    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CBoss::AI_Chase);
    m_tState_Obj.Add_Func(STATE_OBJ::KEEP_DISTANCE, &CBoss::AI_KeepDistance);
    m_tState_Obj.Add_Func(STATE_OBJ::PRE_ATTACK, &CBoss::AI_Pre_Attack);
    m_tState_Obj.Add_Func(STATE_OBJ::SIDE_READY, &CBoss::AI_Side_Ready);
    m_tState_Obj.Add_Func(STATE_OBJ::WALK, &CBoss::AI_Walk);
    m_tState_Obj.Add_Func(STATE_OBJ::RUN, &CBoss::AI_Run);
    m_tState_Obj.Add_Func(STATE_OBJ::ROLL, &CBoss::AI_Roll);
    m_tState_Obj.Add_Func(STATE_OBJ::CLOSEATTACK, &CBoss::AI_CloseAttack);
    m_tState_Obj.Add_Func(STATE_OBJ::SHOOTING, &CBoss::AI_Shooting);

    m_tState_Obj.Add_Func(STATE_OBJ::GOHOME, &CBoss::AI_GoHome);
    m_tState_Obj.Add_Func(STATE_OBJ::RECONNAISSANCE, &CBoss::AI_GoHome);
    m_tState_Obj.Add_Func(STATE_OBJ::TRACK, &CBoss::AI_Track);

    //����
    m_tState_Obj.Add_Func(STATE_OBJ::PH1_LASER, &CBoss::AI_Ph1_Laser);


#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBoss::Idle);
    m_tState_Act.Add_Func(STATE_ACT::RUN, &CBoss::Run);
    m_tState_Act.Add_Func(STATE_ACT::APPROACHING, &CBoss::Approaching);
    m_tState_Act.Add_Func(STATE_ACT::ROLLING, &CBoss::Rolling);
    m_tState_Act.Add_Func(STATE_ACT::GOHOME, &CBoss::GoHome);
    m_tState_Act.Add_Func(STATE_ACT::KEEPDISTANCE, &CBoss::KeepDistance);
    m_tState_Act.Add_Func(STATE_ACT::TRACK, &CBoss::Track);

    //���Ͻ��� 
    m_tState_Act.Add_Func(STATE_ACT::PHASE1_LASER, &CBoss::Phase1_LaserOn);

#pragma endregion

#pragma region �׼� Ű ���
    m_mapActionKey.Add_Action(ACTION_KEY::IDLE); // �ֶ� 

#pragma endregion
    return S_OK;
}

_int CBoss::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    if (0 == m_gHp.Cur)
        m_bDead = true;

    // ������ üũ�뵵 -> ���ϸ��� üũ�ϱ����� 
    Phase_Check();

    // ��ġ�� �������� 
    Get_PlayerPos(fTimeDelta);

    // ����Ÿ�� 
    Height_On_Terrain();

    // ������
    Billboard(fTimeDelta);

    //���¸ӽ�-------------------------------------
    m_fFrame += m_fFrameSpeed * fTimeDelta;
    m_fPatternAge += 1.f * fTimeDelta;

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

    if (m_fPatternAge > m_fPatternLifeTime)
    {
        TimerReset = TRUE;
        m_fPatternAge = 0.f;
    }

#pragma region �׽�Ʈ ��� 
    if (Engine::IsKey_Pressing(DIK_N))
    {
        m_gHp.Cur = 75.f;
    }

    if (Engine::IsKey_Pressing(DIK_M))
    {
        m_gHp.Cur = 45.f;
        /*
        swprintf_s(debugString, L"BOSS - ���� Ȯ�� m_tState_Obj = %d\n", m_tState_Obj);
        OutputDebugStringW(debugString);*/
    }
#pragma endregion 

    //���� ������Ʈ �ڵ�
    m_pColliderComp->Update_Physics(*m_pTransformComp->Get_Transform()); // �ݶ��̴� ��ġ ������Ʈ 

    // Renderer -----------------------------------
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

#pragma region �⺻ override 

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

#pragma endregion

#pragma region �浹 

void CBoss::OnCollision(CGameObject* pDst)
{

}
void CBoss::OnCollisionEntered(CGameObject* pDst)
{

}
void CBoss::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 

#pragma region ���¸ӽ� �μ���Ʈ 

HRESULT CBoss::Get_PlayerPos(const _float& fTimeDelta)
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    return S_OK;
}

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

_float CBoss::Calc_Distance()
{
    _float fDistance = D3DXVec3Length(&(m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos()));

    return fDistance;
}

void CBoss::Phase_Check()
{
    _float CurHp = m_gHp.Cur;

    if (phase1HP >= CurHp && phase2HP <= CurHp) // 50~80 : Phase 1
        m_eCurrPhase = Phase1;
    else if (phase2HP >= CurHp) // 50���� = phase 2
        m_eCurrPhase = Phase2;

    switch (m_eCurrPhase)
    {
    case Engine::Phase1: // 80�� ���� 
        m_fRunSpeed = 3.f;
        break;

    case Engine::Phase2: // 50�� ���� - �ڹ��� ȹ�� 
        m_fRunSpeed = 5.f;
        break;

    case Engine::Phase_End: // �⺻phase
        m_fRunSpeed = 7.f;
        break;
    default:
        break;
    }
}


void CBoss::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Environment", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos, pTerrainBufferComp->Get_VtxPos());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 1.2f, vPos.z);
}
#pragma endregion

#pragma region ��������
// 1. ������ ������ - �÷��̾� �ݰ����� 
void CBoss::Falling_Stone_Around()
{
    OutputDebugString(L"��BOSS - �������� 1 : ������ ������    \n");
    _vec3 m_pPlayerPos = m_pPlayerTransformcomp->Get_Pos();
    m_pPlayerPos.y = 15.f; // �������� �����ϴ� ���� ���� 

    _int MaxStone = 0; //�� ���� 
    _float Radius = 0.f; // �÷��̾� �ݰ� ������ ��ġ 

    switch (m_eCurrPhase)
    {
    case Engine::Phase1:
        MaxStone = 5.f;
        Radius = 10.f;
        break;
    case Engine::Phase2:
        MaxStone = 7.f;
        Radius = 5.f;
        break;
    }

    for (int i = 0; i < MaxStone; ++i)
    {
        _vec3 randomCenter;
        GetRandomPointInCircle(&randomCenter, &m_pPlayerPos, Radius);

        Engine::Add_GameObject(L"GameLogic", CFallingStone::Create(m_pGraphicDev,
            randomCenter.x, 
            randomCenter.y, 
            randomCenter.z));
    }
}

void CBoss::Pattern_Fire(_int number, _float radius)
{
    _int numPoints = number;
    _float Radius = radius;
    _vec3 Center = m_pTransformComp->Get_Pos();
    _vec3 FirePos;

    for (_int i = 0; i < numPoints; ++i)
    {
        _float angle = static_cast<_float>(i) / static_cast<_float>(numPoints) * D3DX_PI * 2.0f;
        FirePos.x = Center.x + Radius * cosf(angle);
        FirePos.y = Center.y;
        FirePos.z = Center.z + Radius * sinf(angle);

        Engine::Add_GameObject(L"GameLogic", CSpawnFire::Create(m_pGraphicDev,
            FirePos.x, FirePos.y, FirePos.z , m_eCurrPhase ));
    }
}

void CBoss::Pattern_EnergyBall()
{
    for (_int i = 0; i < 7; ++i) //6�� 1
    {
        Engine::Add_GameObject(L"GameLogic", CEnergyBall::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x - 6.f + _float( 2 * i ),
            m_pTransformComp->Get_Pos().y,
            m_pTransformComp->Get_Pos().z + 1.f,
            m_eCurrPhase));
    }
}

void CBoss::Pattern_SlowThunder()
{

}
#pragma endregion 


#pragma region ��ǥ(AI) ���¸ӽ� �κ� 

void CBoss::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : idle ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : idle �� \n");
    }
}

void CBoss::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Suspicious ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

        Engine::Add_GameObject(L"GameLogic", CAwareness::Create(m_pGraphicDev,
            m_pTransformComp->Get_Pos().x + 0.1f, 
            m_pTransformComp->Get_Pos().y + 1.3f, 
            m_pTransformComp->Get_Pos().z, CAwareness::TYPE::BOSS));
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // �þ߰� �̳��� ��ġ + �þ߰Ÿ� �̳� ��ġ 
        {
            m_fBossAwareness += fDeltaTime * 4.f;

            // 2. �������� MAX�� �Ǹ� �÷��̾� �߰� ���� 
            if (m_fMaxAwareness <= m_fBossAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;

                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
            }
        }
        else // �������� ����
        {
            OutputDebugString(L"��Brown - ����üũ : �������� ������   \n");
            m_fBossAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fBossAwareness)
            {
                m_fBossAwareness = 0.f;
                m_tState_Obj.Set_State(STATE_OBJ::IDLE);
            }
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Suspicious ��   \n");
    }
}

void CBoss::AI_Reloading(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Taunt - Reloading ����   \n");
        m_fFrameSpeed = 7.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Reloading");
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : Taunt - Reloading ��   \n");
    }
}

void CBoss::AI_BackIdle(float fDeltaTime) //
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Reconnaissance - BackIdle ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Single", L"BackIdle");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //�÷��̾� �����ϸ� �ٽ� ���� 
        {
            m_fBossAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 
            if (m_fBossAwareness >= m_fMaxAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : Reconnaissance - BackIdle ��   \n");
    }
}

void CBoss::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - �ൿ : Chase ����   \n");
        m_fFrameSpeed = 10.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"IdleReady");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // �ּ����� �������� + ��Ÿ� ������� ���Ͽ켱���� 
        if (m_fCloseAttackDistance >= Calc_Distance()) // ���������� �� �켱�õ� - �Ÿ� �ſ� ����ﶧ�� 
            m_tState_Obj.Set_State(STATE_OBJ::PRE_ATTACK);
        else if (m_fMonsterSightDistance <= Calc_Distance())
            m_tState_Obj.Set_State(STATE_OBJ::BACKIDLE);

        if (m_fFrame > m_fFrameEnd) // ��� �����ϴϱ� �ѹ��� �������� 
        {        // �� Phase�� �������� ���°� ============
            if (Detect_Player())
            {
                if (Engine::Phase_End == m_eCurrPhase) //�⺻phase
                {
                    OutputDebugString(L"�� Brown ����� : Chease - 0����   \n");
                    if (m_fBasicRunDistance <= Calc_Distance()) // ��Ÿ�(9.f) ���� �ڿ� �־�� ������ 
                        m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                    else if (m_fBasicRunDistance > Calc_Distance()) // �Ÿ��α� - �ڷ� �� ����
                        m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);
                }
                else if (Engine::Phase1 == m_eCurrPhase)// hp 80�� ���� - 1���� ���� 
                {
                    OutputDebugString(L"�� Brown ����� : Chease - 1����  \n");

                    // 1. ���ۺ��� ������ 
                    if (m_bPH1_RedLaser) 
                    {
                        if (m_bCheck) //���� m_bCheck = true�� �� / m_bPH1_RedLaser�� false�� ���̻� ��Ÿ�� ������ 
                            m_bPH1_RedLaser = FALSE;
                        else
                        {
                            OutputDebugString(L"�� Brown ����� : Chease - 1���� - RedLaser �ߵ�  \n");
                            m_tState_Obj.Set_State(STATE_OBJ::PH1_LASER);
                            m_bCheck = TRUE; //�ѹ��� �ߵ��ϴ°ɷ� 
                        }
                    } 

                    // 2. 3�ʿ� �ѹ��� �������� 
                    if (TimerReset) 
                    {
                        Falling_Stone_Around();
                        TimerReset = FALSE;
                    }

                    _float CurDist = Calc_Distance();

                    // 3. �⺻ �������� - Shooting & back shooting + Roll 
                    if (FALSE == m_bPH1_RedLaser && m_fPH1basicDistance < CurDist && m_fMonsterSightDistance >= CurDist) //  12< a <=18
                        m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                    else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance < CurDist && m_fPH1basicDistance >= CurDist) // 9 < a <= 12 
                        m_tState_Obj.Set_State(STATE_OBJ::ROLL);
                    else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance >= CurDist) // a <= 9
                        m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);

                }
                else if (Engine::Phase2 == m_eCurrPhase)// hp 50�� ���� 
                {
                    // 1. ���ۺ��� �ڹ��� 
                    if (m_bPH2_Buff)
                    {
                        if (!m_bCheck) // ���� FALSE
                            m_bPH2_Buff = FALSE;
                        else
                        {
                            OutputDebugString(L"�� Brown ����� : Chease - 2����  \n");
                            Engine::Add_GameObject(L"GameLogic", CBlueBuff::Create(m_pGraphicDev, m_pTransformComp->Get_Pos().x, m_pTransformComp->Get_Pos().y,
                                m_pTransformComp->Get_Pos().z, m_eCurrPhase));

                            m_bCheck = FALSE; //�ѹ��� �ߵ�
                        }
                    }

                    // 2. 3�ʸ��� �������� - ������, �� ������������ 
                    if (TimerReset)
                    {
                        Falling_Stone_Around();
                        TimerReset = FALSE;
                    }

                    // 3. �⺻���� + ��ų ���� 
                    if (FALSE == m_bPH2_Buff) // �ϴ� ���� �θ��� ���� ���� 
                    {
                        _int iCombo = (rand() % 4) + 1;

                        if (iCombo <= 2) // 1, 2
                        {
                            _float CurDist = Calc_Distance();
                            Pattern_Fire(12, 4.f);
                            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                            // 3. �⺻ �������� - Shooting & back shooting + Roll + Track
                           // if (FALSE == m_bPH1_RedLaser && m_fChaseDistance < CurDist && m_fMonsterSightDistance >= CurDist) // 15 < a <= 18
                           //     m_tState_Obj.Set_State(STATE_OBJ::TRACK);
                           // else if (FALSE == m_bPH1_RedLaser && m_fPH1basicDistance < CurDist && m_fChaseDistance >= CurDist) //  12< a <=15
                           //     m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
                           // else if (FALSE == m_bPH1_RedLaser && m_fBasicRunDistance < CurDist && m_fPH1basicDistance >= CurDist) // 9 < a <= 12 
                           //     Pattern_Fire(12, 4.f);
                        }
                        else
                        {
                            // -> �⺻ �������� 
                            Pattern_EnergyBall();
                            m_tState_Obj.Set_State(STATE_OBJ::KEEP_DISTANCE);
                        }
                    }
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

void CBoss::AI_KeepDistance(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : KeepDistance ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());

        switch (m_eCurrPhase)
        {
        case Engine::Phase1:
            m_fFrameSpeed = 13.f;
            break;
        case Engine::Phase2:
            m_fFrameSpeed = 16.f;
            break;
        case Engine::Phase_End:
            m_fFrameSpeed = 13.f;
            break;
        }
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::KEEPDISTANCE].Act();
        
        if (m_fFrame > m_fFrameEnd) // ������ ����ٸ� 7.x
        {
            OutputDebugString(L"�� Brown ����� : KeepDistance - �������� �ۿ� �־�� shooting���� ������   \n");
            m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : KeepDistance ��   \n");
    }
}

void CBoss::AI_Run(float fDeltaTime)
{
}

void CBoss::AI_Roll(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Roll ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Roll");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
     
        if (Engine::Phase2 == m_eCurrPhase)
            m_fFrameSpeed = 15.f;
        else
            m_fFrameSpeed = 13.f;
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : Roll ��   \n");
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ����   \n");
            m_fFrameSpeed = 15.f;
        if(Engine::Phase2 == m_eCurrPhase)
            m_fFrameSpeed = 18.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Shooting");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (4 <= m_fCheck)
        {
            OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting 3�� ���� - cHASE ������    \n");
            m_fCheck = 0;
            m_iBulletCnt -= 1; // ������ ���� 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Shooting ��   \n");
    }
}

void CBoss::AI_GoHome(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��Boss - ���¸ӽ� : GoHome ����   \n");
        m_fFrameSpeed = 12.f;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }
    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::GOHOME].Act(); // �׼�Ű ������ 

        if (m_bArrive) // ������ �� ���� 
        {
            m_bArrive = false;
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBoss::AI_Reconnaissance(float fDeltaTime)
{    //�÷��̾� ���ļ� �������� 0���� ����������� 
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Reconnaissance ����   \n");
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) //�÷��̾� �����ϸ� �ٽ� ���� 
        {
            m_fBossAwareness += fDeltaTime * 6.f; // �������� �� ������ �����Ұ� 

            if (m_fBossAwareness >= m_fMaxAwareness)
            {
                m_fBossAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::RELOADING);
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
        OutputDebugString(L"��BOSS - ���¸ӽ� : Reconnaissance ��   \n");
    }
}

void CBoss::AI_Ph1_Laser(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Phase1 - Laser ����   \n");
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"NorthWestWalk");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::NORMAL_LASER].Act(); // �׼�Ű ������ 

       if(m_fFrame > m_fFrameEnd)
       {
           _float PosZ = m_pTransformComp->Get_Pos().z;
           _float PosY = m_pTransformComp->Get_Pos().y;

           for (_int i = 0; i < 5; ++i)
           {
               Engine::Add_GameObject(L"GameLogic", CRedLaser::Create(m_pGraphicDev,
                   m_pTransformComp->Get_Pos().x, 
                   Get_RandomFloat(PosY - 1.f, PosY + 5.f),
                   Get_RandomFloat(PosZ - 3.f, PosZ + 3.f) ));
           }

            m_fConsider = 10.f; // �ٽ� �ʱ� �������� 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
       }
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Phase1 - Laser  ��   \n");
    }
}

void CBoss::AI_Track(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Track ����   \n");
        m_fConsider = 13.f; 
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Boss_Multi", L"Run");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE)) // ���� �׼�Ű�� IDLE �̹Ƿ� 
            m_mapActionKey[ACTION_KEY::TRACK].Act(); // �׼�Ű ������ 

        if (m_fFrame > m_fFrameEnd) //���� 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
    }

    if (m_tState_Obj.IsState_Exit())
    {
        OutputDebugString(L"��BOSS - ���¸ӽ� : Phase1 - Track  ��   \n");
    }
}

#pragma endregion

#pragma region �ൿ ���¸ӽ� �κ� 
void CBoss::Idle(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered()) // 1�� bIsEnter false�� �ٲ� 
    {
        OutputDebugString(L"��BOSS - �ൿ : Idle ����   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        // �ȱ� 
        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::APPROACHING);

        // �Ÿ��������� ������ �ڷ� �ٱ� 
        if (m_mapActionKey[ACTION_KEY::KEEPDISTANCE].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::KEEPDISTANCE);

        // �޸��� 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::RUN);

        // ������ ������
        if (m_mapActionKey[ACTION_KEY::ROLL].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::ROLLING);

        // ����ġ 
        if (m_mapActionKey[ACTION_KEY::GOHOME].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::GOHOME);

        // 
        if (m_mapActionKey[ACTION_KEY::NORMAL_LASER].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::PHASE1_LASER);

        if (m_mapActionKey[ACTION_KEY::TRACK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::TRACK);

    }

    if (m_tState_Act.IsState_Exit())
    {
        //OutputDebugString(L"��BOSS - �ൿ : Idle ��   \n");
    }
}

void CBoss::Approaching(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"��BOSS - �ൿ : Approaching ����   \n");
    }

    if (m_tState_Act.Can_Update())
    {
        if (Calc_Distance() > m_fMonsterSightDistance) // �þ߹��� recon���� - �켱���� 
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);

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

void CBoss::Run(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    if (m_tState_Act.Can_Update())
    {
        if (STATE_OBJ::RUN == m_tState_Obj.Get_State())
        {
            // ���Ͱ� �÷��̾� ���� �ݴ�������� �̳� Ʀ 
            vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
            D3DXVec3Normalize(&vDir, &vDir);

            if (Engine::Phase2 == m_eCurrPhase) // ���信 ������ 
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -(m_fRunSpeed + 3.f));
            else
                m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -m_fRunSpeed);

            if (m_fBasicRunDistance <= Calc_Distance())
                m_tState_Obj.Set_State(STATE_OBJ::SHOOTING);

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

        if (Engine::Phase2 == m_eCurrPhase) // ���信 ������ 
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, (m_fRollingSpeed + 2.f));
        else
            m_pTransformComp->Move_Pos(&Right, fDeltaTime, m_fRollingSpeed);

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
        OutputDebugString(L"��Boss - �ൿ�ӽ� : GoHome ����   \n");
    }

    if (m_tState_Act.Can_Update())
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
            m_pTransformComp->Move_Pos(&vDirect, fDeltaTime, m_fRollingSpeed);
        }
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"��Boss - �ൿ : GOHOME ��   \n");
    }
}

void CBoss::KeepDistance(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"��Boss - �ൿ : KEEP_DISTANCE ����  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
        // ���Ͱ� �÷��̾� ���� �ݴ�������� �̳� Ʀ 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);
     
        if (Engine::Phase1 == m_eCurrPhase || Engine::Phase_End == m_eCurrPhase)
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -m_fRunSpeed);
        else if (Engine::Phase2 == m_eCurrPhase)
            m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -(m_fRunSpeed + 2.f));
        //if (STATE_OBJ::KEEP_DISTANCE == m_tState_Obj.Get_State())

        if (m_fBasicRunDistance <= Calc_Distance())
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);

        m_tState_Act.Set_State(STATE_ACT::IDLE);

    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"��Boss - �ൿ : KEEP_DISTANCE ��   \n");
    }
}

void CBoss::Phase1_LaserOn(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"��Boss - �ൿ : Phase1 - Laser ����  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
         // ���Ͱ� �ϼ��� ���� 
         vDir =  m_pPlayerTransformcomp->Get_Right() - m_pTransformComp->Get_Pos();
         D3DXVec3Normalize(&vDir, &vDir);

         m_pTransformComp->Move_Pos(&vDir, fDeltaTime, -11.f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"��Boss - �ൿ : Phase1 - Laser ��   \n");
    }
}

void CBoss::Track(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
        OutputDebugString(L"��Boss - �ൿ : Track ����  \n");
    }

    if (m_tState_Act.Can_Update()) //PA
    {
        // ���Ͱ� �÷��̾������� 
        vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();
        D3DXVec3Normalize(&vDir, &vDir);

        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, 11.f);

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    if (m_tState_Act.IsState_Exit())
    {
        OutputDebugString(L"��Boss - �ൿ : Track ��   \n");
    }
}

#pragma endregion

