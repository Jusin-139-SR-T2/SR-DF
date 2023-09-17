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

    D3DXMatrixIdentity(&m_matRotAxis);
    m_pTransformComp->m_vScale.x = 0.5f;

    // Transform Component - Tool���� ���� ����
    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 20.f };

    // INFO
    m_iHP = 40;         // ü��
    m_iAwareness = 15;  // �˾������°� ������
    m_iAttack = 10;     // ���ݷ�

#pragma region ��ǥ ���¸ӽ� ��� - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CMonster::Obj_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CMonster::Obj_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CMonster::Obj_Chase);

    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CMonster::Obj_Attack);
    //m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CMonster::Obj_Jump);
#pragma endregion

#pragma region �ൿ ���¸ӽ� ��� - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
                    // STATE�� ::A �϶� CMonsterŬ������ ::B �Լ��� �����ϴ� ���¸ӽ� 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CMonster::Idle);
    m_tState_Act.Add_Func(STATE_ACT::STAND_OFF, &CMonster::Stand_Off);
    m_tState_Act.Add_Func(STATE_ACT::SUSPICIOUS, &CMonster::Suspicious);
    m_tState_Act.Add_Func(STATE_ACT::DETECT, &CMonster::Detect);
    
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
    m_mapActionKey.Add_Action(ACTION_KEY::LEFT);
    m_mapActionKey.Add_Action(ACTION_KEY::RIGHT);
    m_mapActionKey.Add_Action(ACTION_KEY::UP);
    m_mapActionKey.Add_Action(ACTION_KEY::DOWN);

#pragma endregion

    return S_OK;
}

_int CMonster::Update_GameObject(const _float& fTimeDelta)
{
    m_fFrame += 6.f * fTimeDelta;

    if (14 < m_fFrame)
        m_fFrame = 0.f;

    //���¸ӽ� ���ư��°� 
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// �ൿ
    m_mapActionKey.Update();	// �׼�Ű �ʱ�ȭ

    Height_On_Terrain(); // ����Ÿ�� 

    SUPER::Update_GameObject(fTimeDelta);
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
    m_pPlayerTransformcomp = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // �÷��̾��� ��ġ, �ٶ󺸴� ����
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    // ���� ��ġ, �ٶ󺸴� ����
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    // ���ο� �ִ��� �Ǻ� = ���� �� X (���Ͱ� �÷��̾ ���� ����) -> ����ϰ�� �����ؼ� 180�� �̳� = �þ߰��� 
    _float result = D3DXVec3Dot(&vMonsterLook, &(vMonsterPos - vPlayerPos) );

    if (result >= 0) // �þ߰� ���ο� ������� - cos90 = 0 �̹Ƿ� �����ؾ��� 
        return true;
    else
        return false;
}

float CMonster::m_fDistance()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // �÷��̾��� ��ġ
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    // ���� ��ġ
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    // �ѻ��� �Ÿ� 
    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    // �ڱ��ڽ��� �����ϸ� ũ���� ������ ���� 
    float fDistance = sqrtf(D3DXVec3Dot(&vDistance, &vDistance));
    
    return fDistance;
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
void CMonster::Obj_Idle(float fDeltaTime)
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

void CMonster::Obj_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_iAwareness += fDeltaTime * 1000;
    }

    if (m_tState_Obj.Can_Update())
    {
        // ���� - �÷��̾ �þ߰����� ������ 
        if (0 == m_iAwareness)
        {
            m_tState_Obj.Set_State(STATE_OBJ::IDLE);
        }

        if (m_iMaxAwareness == m_iAwareness)
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }

        //if ( == m_iAwareness)
        //{
        //    m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        //}

    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}
void CMonster::Obj_Chase(float fDeltaTime)
{
}

void CMonster::Obj_Attack(float fDeltaTime)
{
}

//------------------ �ൿ -------------------------

void CMonster::Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered()) // �����ؼ� �ѹ� ����
    {
       //stand �̹��� ��� 
        //Stand_%d , 
    }

    if (m_tState_Obj.Can_Update()) // ����ؼ� ���ư��°�
    {
        if(Monster_Capture()) // ���� �þ߰��� �÷��̾ ���ð��
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
            
        }
    }

    if (m_tState_Obj.IsState_Exit()) // ���� �ʿ��Ҷ��� �־ - ã�ƺ��� 
    {
    }
}

void CMonster::Stand_Off(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
       // �̹��� - suspicious �����̹��� 
        // ui���� ���� �������� ? �߰� 
    }

    if(m_tState_Obj.Can_Update())
    {

    }

    if (m_tState_Act.IsState_Exit())
    {

    }
}

void CMonster::Suspicious(float fDeltaTime)
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

void CMonster::Detect(float fDeltaTime)
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

void CMonster::Walk(float fDeltaTime)
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

void CMonster::Run(float fDeltaTime)
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
