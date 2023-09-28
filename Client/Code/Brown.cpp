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
    m_iHP = 100;         // 체력

#pragma region 목표 상태머신 등록 - (AI) Judge
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

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);
    // STATE가 ::A 일때 CBrown클래스의 ::B 함수를 수행하는 상태머신 
    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CBrown::Idle);
    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CBrown::Approach);
    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CBrown::Moving);
    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CBrown::Attack);
    
#pragma endregion

#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //가까울때
    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // 가까울때

    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y축에 차이가 있을때 
  
    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

#pragma endregion

    return S_OK;
}

_int CBrown::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);
    
    // 위치값 가져오기 
    Get_PlayerPos(fTimeDelta);
    
    // 지형타기 
    Height_On_Terrain(); 

    // 상태머신-------------------------------------

    m_fFrame += m_fFrameSpeed * fTimeDelta;


    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;
   
        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    // 빌보드 --------------------------------------

    //FaceTurn(fTimeDelta);
    
    // --------------------------------------------

    
    // ---------- 테스트 빌드 ----------------------

    Monster_Jump(fTimeDelta);
   
    if (Engine::IsKey_Pressing(DIK_H))
    {
        m_iHP = 0; //즉사 기믹 확인용 
    }

    if (Engine::IsKey_Pressing(DIK_J))
    {
        m_iHP = 50; // 피격 기믹 확인용 
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

_bool CBrown::Detect_Player() // 몬스터 시야내 플레이어 있는지 체크하는 함수 
{
    // 플레이어 위치 가져옴 
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
    //case1. 회전행렬 만들기 
    _matrix		matWorld, matView, matBill, matScale, matChangeScale;

   matWorld = *m_pTransformComp->Get_Transform();

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    _vec3 Pos = m_pTransformComp->Get_Pos();

    _vec3 vDir = vPlayerPos - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    // 회전행렬 생성
    _matrix rotationMatrix;
    D3DXMatrixRotationY(&rotationMatrix, rad);

    m_pTransformComp->Set_WorldMatrixS(&(rotationMatrix * matWorld));

    // case2. 빌보드 구성하기 
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
        m_iPreHP = m_iHP; // 상태체크용 hp저장 
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
        
        // 한번에 뒤에서 백어탭 기습했을경우 
        if (0 == m_iHP) 
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHOPPED);
        }
        
        if( m_iHP != m_iMaxHP )
        {
          //CPlayer::STATE_RIGHTHAND::NONE ;
          m_tState_Obj.Set_State(STATE_OBJ::HIT);
        }

        // 플레이어가 총으로 쏘았을 때
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

        if(Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            m_fAwareness += fDeltaTime * 3.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness; // 추후 감소를 위해 최대값으로 고정 
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT); // 추격으로 시작 
            }
        }
        else // 범위밖은 감소
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
        if (2 == m_fCheck) // 도발 두번 하고 따라가기 
        {
            m_fCheck = 0; //다른데도 쓰니까 0으로 되돌리기 
            m_iPreHP = m_iHP; // 피격없으면 pre로 이전hp 저장 
            m_tState_Obj.Set_State(STATE_OBJ::CHASE); // AI = 추격모드
        }

    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

void CBrown::AI_Chase(float fDeltaTime) // 달리다가 걷다가 잽날리려고함 
{
    if (m_tState_Obj.IsState_Entered())
    {
       // m_pTransformComp->m_vScale.x = 0.5f;
        m_fFrameSpeed = 10.f; //원상복귀 
    }
    if (m_tState_Obj.Can_Update())
    {
        _float CurDistance = m_fDistance();

        if (Detect_Player()) // 사거리에 포착되는경우 
        {
            // 시야거리에 의한 상태머신보다 피격/죽음이 더 빠름  

            // --------피격상태 상태머신 -----------
            if (m_iPreHP != m_iHP)
                m_tState_Obj.Set_State(STATE_OBJ::HIT);

            if (0 == m_iHP)
                m_tState_Obj.Set_State(STATE_OBJ::DEATH);

            // --------거리비교 상태머신 -----------
            // 뛰어서 다가옴 : a > 8
            if (m_fRunDistance < CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7 < a <= 8 
            else if (m_fWalkDistance < CurDistance && m_fRunDistance >= CurDistance)
            {
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 무빙 : 4 < a <= 7
            else if (m_fInchDistance < CurDistance && m_fWalkDistance >= CurDistance)
            {
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::INCHFORWARD);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::STRAFING);
            }
            // 공격함
            else
            {
                int iCombo = (rand() % 10) + 1; 

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::BASICATTACK);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
        else // 쫒다가도 시야에서 벗어나면 게이지 줄어들어서 SUSPICIOUS로 돌아감 
        {
            m_fAwareness -= fDeltaTime * 4.f;

            if (0 >= m_fAwareness) //인지값이 초기화되면 
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
        //행동이 IDLE일때 RUN 가상키 누르기 
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
        //행동이 IDLE일때 WALK 가상키 누르기 - 다가오기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::WALK].Act();

        // 행동 추가 
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
        //행동이 IDLE일때 가상키 누르기 
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
        //행동이 IDLE일때 가상키 누르기 
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
        //행동이 IDLE일때 가상키 누르기 
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
        //행동이 IDLE일때 가상키 누르기 
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
        // 공격상태         
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
        // 플레이어 앉은상태 + 공격상태 
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
        //충돌체 만들어서 HP깎기 (공격하면) 
        
        // 히트 있을때 
        if (0 == m_iHP)
        {
            if (m_fFrame > m_fFrameEnd)
            {
                m_tState_Obj.Set_State(STATE_OBJ::DEATH);
            }
        }

        //아무런 히트가 없음 
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
        if(!Dead) //수정필요 
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
        if (!Dead) //수정필요 
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
            m_fAwareness += fDeltaTime * 4.f; // 이전보다 더 빠르게 증가할것 
            if (m_fAwareness >= m_fMaxAwareness)
            {
                m_fAwareness = m_fMaxAwareness;
                m_tState_Obj.Set_State(STATE_OBJ::TAUNT);
            }
        }

        if (0 >= m_fConsider)
        {
            m_fConsider = 10.f; // 다시 초기 셋팅으로 
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
        //행동이 IDLE일때 WALK 가상키 누르기 
        if (m_tState_Act.IsOnState(STATE_ACT::IDLE))
            m_mapActionKey[ACTION_KEY::GOHOME].Act();

        // 조건 - 처음 위치로 돌아갈때까지 
        m_bGoHome = true;
        m_tState_Obj.Set_State(STATE_OBJ::WALK);
    }

    if (m_tState_Obj.IsState_Exit())
    {
    }
}

//------------------ 행동 -------------------------

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

    if (m_tState_Act.IsState_Exit()) // 가끔 필요할때가 있어서 - 찾아보기 
    {
       
    }
}

void CBrown::Approach(float fDeltaTime) // RUN 액션키 들어가면 수행하는곳
{
    if (m_tState_Act.IsState_Entered())
    {
    }

    // 실행
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
    // 조건
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

    // 실행
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

    // 조건
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

    // 실행
    {
        if (STATE_OBJ::BASICATTACK == m_tState_Obj.Get_State())
        {
            // 충돌체 만들어서 기본공격 수행

        }

        if (STATE_OBJ::HEAVYATTACK == m_tState_Obj.Get_State())
        {
            // 충돌체 만들어서 공격 수행
        }

        m_tState_Act.Set_State(STATE_ACT::IDLE);
    }

    // 조건
    {

    }

    if (m_tState_Act.IsState_Exit())
    {
    }
}

void CBrown::GoHome(float fDeltaTime)
{
}
