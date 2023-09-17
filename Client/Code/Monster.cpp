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

    // Transform Component - Tool에서 변경 예정
    m_pTransformComp->m_vInfo[INFO_POS] = { 15.f, 10.f, 20.f };

    // INFO
    m_iHP = 40;         // 체력
    m_iAwareness = 15;  // 알아차리는거 게이지
    m_iAttack = 10;     // 공격력

#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CMonster::Obj_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CMonster::Obj_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CMonster::Obj_Chase);

    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CMonster::Obj_Attack);
    //m_tState_Obj.Add_Func(STATE_OBJ::JUMP, &CMonster::Obj_Jump);
#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
                    // STATE가 ::A 일때 CMonster클래스의 ::B 함수를 수행하는 상태머신 
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


#pragma region 액션 키 등록
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

    //상태머신 돌아가는곳 
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    Height_On_Terrain(); // 지형타기 

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

    // 몬스터 텍스처
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
    // 몬스터 시야내 플레이어 있는지 체크하는 함수 

    // 플레이어 위치 가져옴 
    m_pPlayerTransformcomp = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // 플레이어의 위치, 바라보는 방향
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    // 몬스터 위치, 바라보는 방향
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    // 내부에 있는지 판별 = 몬스터 앞 X (몬스터가 플레이어를 보는 벡터) -> 양수일경우 내적해서 180도 이내 = 시야각안 
    _float result = D3DXVec3Dot(&vMonsterLook, &(vMonsterPos - vPlayerPos) );

    if (result >= 0) // 시야각 내부에 있을경우 - cos90 = 0 이므로 포함해야함 
        return true;
    else
        return false;
}

float CMonster::m_fDistance()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransform*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // 플레이어의 위치
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    // 몬스터 위치
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    // 둘사이 거리 
    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    // 자기자신을 내적하면 크기의 제곱과 같다 
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
        // 조건 - 플레이어가 시야각으로 들어오면 
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
        // 조건 - 플레이어가 시야각으로 들어오면 
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

//------------------ 행동 -------------------------

void CMonster::Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered()) // 진입해서 한번 수행
    {
       //stand 이미지 출력 
        //Stand_%d , 
    }

    if (m_tState_Obj.Can_Update()) // 계속해서 돌아가는곳
    {
        if(Monster_Capture()) // 몬스터 시야각에 플레이어가 들어올경우
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
            
        }
    }

    if (m_tState_Obj.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
    }
}

void CMonster::Stand_Off(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {
       // 이미지 - suspicious 단일이미지 
        // ui에서 점점 차오르는 ? 추가 
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
