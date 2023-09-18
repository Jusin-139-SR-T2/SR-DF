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
    m_iHP = 40;         // 체력
    m_iAttack = 10;     // 공격력

#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE);

    m_tState_Obj.Add_Func(STATE_OBJ::IDLE, &CMonster::AI_Idle);
    m_tState_Obj.Add_Func(STATE_OBJ::SUSPICIOUS, &CMonster::AI_Suspicious);
    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CMonster::AI_Chase);

    m_tState_Obj.Add_Func(STATE_OBJ::ATTACK, &CMonster::AI_Attack);
#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
                    // STATE가 ::A 일때 CMonster클래스의 ::B 함수를 수행하는 상태머신 
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


#pragma region 액션 키 등록
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

    //상태머신 돌아가는곳 
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    Height_On_Terrain(); // 지형타기 

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
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // 플레이어의 위치, 바라보는 방향
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    // 몬스터 위치, 바라보는 방향
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    // 몬스터와 플레이어 위치 벡터 계산 + 정규화 
    _vec3 MonToPlayer = vMonsterPos - vPlayerPos;
   float currdistance = D3DXVec3Length(&MonToPlayer); // 현재 플레이어 위치에서 몬스터까지의 길이 = 시야거리 비교 
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);
   
   //현재 플레이어 위치가 몬스터 시야거리 외부(밖)에 있다 
   if (currdistance > m_fMonsterSightDistance) 
       return false;

    // 내부에 있는지 판별 = 몬스터 앞 X (몬스터가 플레이어를 보는 벡터) -> 양수일경우 내적해서 180도 이내 = 시야각안 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI; 

    // 시야각도 내부 && 시야거리 이내 
    if (fradian < m_fMonsterFov * 2 && currdistance <  m_fMonsterSightDistance ) 
        return true;
    else
        return false;
}

float CMonster::m_fDistance()
{
    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    // 플레이어의 위치
    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);

    // 몬스터 위치
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    // 둘사이 벡터 
    _vec3    vDistance = (vPlayerPos - vMonsterPos);

    // 둘사이 길이 
    float fDistance = D3DXVec3Length(&vDistance);
    
    return fDistance;
}

void CMonster::Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed)
{
    _vec3   vMonsterPos;
    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);

    _vec3	vDir = *pTargetPos - vMonsterPos; // 몬스터가 보는 대상 벡터 

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

void CMonster::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        //Taunt이미지 출력 - 싱글텍
       // m_iFrameEnd = 1; 
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Monster_Capture()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            // 인지 변수 지속증가 
            m_fAwareness += fDeltaTime * 10.f;

            // 1. 게이지가 일정수준 이상이면 forward벡터를 회전시켜서 플레이어쪽으로 보게만듬 
            if (8 <= m_fAwareness && m_fMaxAwareness > m_fAwareness && SpinCheck)
            {
                // 플레이어 위치
                m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
                _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

                m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
                m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

                // 몬스터 위치, 바라보는 방향
                m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
                m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

                // 몬스터가 플레이어 보는 벡터
                _vec3 vMonLookPlayer = vPlayerPos - vMonsterPos;

                // 몬스터가 플레이어를 바라보기 위한 목표 각도 (벡터라 성립 - 출발점 달라도됨)
                _float targetAngle = atan2(vMonLookPlayer.z, vMonLookPlayer.x);

                _float ftheta = acos(D3DXVec3Dot(&vPlayerLook, &vMonsterLook));

                if (ftheta == targetAngle)
                {
                    SpinCheck = false;
                }
                else
                    m_pTransformComp->Rotation(ROT_Y, targetAngle * fDeltaTime);

            }

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fAwareness || 15.f <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
                // 돌고 들어오는거 체크 완료 
                m_tState_Obj.Set_State(STATE_OBJ::CHASE); //◀ 여기 문제점 
            }
        }
        else // 범위밖은 감소
        {
            m_fAwareness -= fDeltaTime * 30.f;

            if (m_fAwareness < 0)
                m_fAwareness = 0;
         
            //플레이어가 시야각을 벗어나 인지값이 초기화됨
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
        // 추격할때 이미지 end - 8방향 * 여러상태 
       // m_iFrameEnd = 
    }

    if (m_tState_Obj.Can_Update()) // 시야범위 7 
    {
        // 거리가 너무멀다 - 달리기 
        //if (7.f < m_fDistance())
        //{
        //    m_mapActionKey[ACTION_KEY::RUN].Act();
        //}

        // 둘사이 거리 3~7  - 걷기 
        if (3.f < m_fDistance() && 7.f > m_fDistance())
        {
            m_mapActionKey[ACTION_KEY::WALK].Act();
        }

        // 거리가 일정 거리일때 - 점프 // ▶ 조건을 알 수 없음. 찾아내야함
        //if ( )
        //{
        //    m_mapActionKey[ACTION_KEY::JUMP].Act();
        //}

        // 거리가 일정 거리일때 공격모션으로 바뀐다. 
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
        //간이홀짝을 통해 강공격 약공격 누르기 
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

//------------------ 행동 -------------------------

void CMonster::Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered()) 
    {
    }

    if (m_tState_Obj.Can_Update()) 
    {
        // 액션키를 누른상태분기 
        if (m_mapActionKey[ACTION_KEY::RUN].IsOnAct()) // 이 액션키가 눌리면        
            m_tState_Act.Set_State(STATE_ACT::RUN); // 행동상태머신에서 이 클래스와 연결된 함수를 수행해라 -> 52번 라인보면 walk함수 수행 = 441라인    

        if (m_mapActionKey[ACTION_KEY::WALK].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::WALK);

        if (m_mapActionKey[ACTION_KEY::JUMP].IsOnAct())
            m_tState_Act.Set_State(STATE_ACT::JUMP);
    }

    if (m_tState_Obj.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
    }
}


void CMonster::Walk(float fDeltaTime)
{
    if (m_tState_Act.IsState_Entered())
    {

    }

    // 실행
    {
        _vec3 vPlayerPos, vMonsterPos;
        m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));

        // 위치
        m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
        m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
        
        _vec3	vDir = vPlayerPos - vMonsterPos; // 몬스터가 보는 목표물 벡터 
        D3DXVec3Normalize(&vDir, &vDir);
        
        m_pTransformComp->Move_Pos(&vDir, fDeltaTime, m_fWalkSpeed);
       
        // vMonsterPos += vDir * fDeltaTime * m_fWalkSpeed;

    }

    // 조건
    {
      
    }

    if (m_tState_Act.IsState_Exit())
    {

        //m_tState_Act.Set_State(STATE_ACT::IDLE);
    }
}

void CMonster::Run(float fDeltaTime) // RUN 액션키 들어가면 수행하는곳
{
    //좌우로 오고가면서 플레이어와의 거리 좁힘  
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
