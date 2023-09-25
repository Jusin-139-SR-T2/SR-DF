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

    m_pTransformComp->m_vScale.x = 0.4f;
    m_pTransformComp->m_vInfo[INFO_POS] = { 10.f, 10.f, 25.f };
    m_fFrame = 0;
    m_fFrameEnd = 0;
    m_fFrameSpeed = 10.f;

    // INFO
    m_iHP = 40;         // 체력
    m_iAttack = 10;     // 공격력
    
#pragma region 목표 상태머신 등록 - (AI) Judge
    m_tState_Obj.Set_State(STATE_OBJ::IDLE); // 초기상태 지정 

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

    m_tState_Obj.Add_Func(STATE_OBJ::BLOCK, &CGray::AI_Block);
    m_tState_Obj.Add_Func(STATE_OBJ::CROTCHHIT, &CGray::AI_CrotchHit);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADSHOT, &CGray::AI_HeadShot);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

    m_tState_Obj.Add_Func(STATE_OBJ::DAZED, &CGray::AI_Dazed);
    m_tState_Obj.Add_Func(STATE_OBJ::CHOPPED, &CGray::AI_Chopped);
    m_tState_Obj.Add_Func(STATE_OBJ::HEADLESS, &CGray::AI_Headless);
    m_tState_Obj.Add_Func(STATE_OBJ::DEATH, &CGray::AI_Death);

#pragma endregion

#pragma region 행동 상태머신 등록 - Acting
    m_tState_Act.Set_State(STATE_ACT::IDLE);

    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
//    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
//    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CGray::Moving);
//    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);

#pragma endregion
#pragma region 액션 키 등록
    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
//    m_mapActionKey.Add_Action(ACTION_KEY::WALK); //중간
//    m_mapActionKey.Add_Action(ACTION_KEY::INCHFORWARD); //가까울때
//    m_mapActionKey.Add_Action(ACTION_KEY::STRAFING); // 가까울때
//
//    m_mapActionKey.Add_Action(ACTION_KEY::JUMP); // y축에 차이가 있을때 
//
//    m_mapActionKey.Add_Action(ACTION_KEY::BASIC_ATTACK);
//    m_mapActionKey.Add_Action(ACTION_KEY::HEAVY_ATTACK);

#pragma endregion

    return S_OK;
}

_int CGray::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    Height_On_Terrain(); // 지형타기 

    m_fFrame += m_fFrameSpeed * fTimeDelta;

    // ---------- 테스트 빌드 ----------------------

    if (Engine::IsKey_Pressing(DIK_K))
    {
        m_iHP = 0; //즉사 기믹 확인용 
    }

    if (Engine::IsKey_Pressing(DIK_L))
    {
        m_iHP = 50; // 피격 기믹 확인용 
    }

    // --------------------------------------------

    //상태머신
    m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
    m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
    m_mapActionKey.Update();	// 액션키 초기화

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;

        if (STATE_OBJ::TAUNT == m_tState_Obj.Get_State())
            m_fCheck += 1;
    }

    FaceTurn(fTimeDelta);

    Engine::Add_RenderGroup(RENDER_ALPHA, this);

    return 0;
}

void CGray::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CGray::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_WorldMatrix());
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

    // 몬스터 텍스처
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_GrayTextureComp"), E_FAIL);

    return S_OK;
}

void CGray::Free()
{
    SUPER::Free();
}

void CGray::FaceTurn(const _float& fTimeDelta)
{ // case2. 빌보드 구성하기 
     //빌보드 = 자전의 역 / 자전 역 * 스 * 자 * 이 ->스케일문제 = 나중에 넣으면되잖? 
    _matrix		matWorld, matView, matBill;

    matWorld = *m_pTransformComp->Get_WorldMatrix();

    m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matBill);

    matBill._11 = matView._11;
    matBill._13 = matView._13;
    matBill._31 = matView._31;
    matBill._33 = matView._33;

    D3DXMatrixInverse(&matBill, 0, &matBill);

    m_pTransformComp->Set_WorldMatrixS(&(matBill * matWorld));

    m_pTransformComp->m_vScale.x = 0.4f;
}

_bool CGray::Detect_Player()
{
    _vec3 vPlayerPos, vMonsterPos, vPlayerLook, vMonsterLook;

    m_pPlayerTransformcomp->Get_Info(INFO_POS, &vPlayerPos);
    m_pPlayerTransformcomp->Get_Info(INFO_LOOK, &vPlayerLook);

    m_pTransformComp->Get_Info(INFO_POS, &vMonsterPos);
    m_pTransformComp->Get_Info(INFO_LOOK, &vMonsterLook);

    _vec3 MonToPlayer = vMonsterPos - vPlayerPos;
    float currdistance = D3DXVec3Length(&MonToPlayer); // 현재 플레이어 위치에서 몬스터까지의 길이 = 시야거리 비교 
   
    D3DXVec3Normalize(&MonToPlayer, &MonToPlayer);

    //현재 플레이어 위치가 몬스터 시야거리 외부(밖)에 있다 
    if (currdistance > m_fMonsterSightDistance)
        return false;

    // 내부에 있는지 판별 = 몬스터 앞 X (몬스터가 플레이어를 보는 벡터) -> 양수일경우 내적해서 180도 이내 = 시야각안 
    _float fradian = acos(D3DXVec3Dot(&vMonsterLook, &MonToPlayer)) * 180 / D3DX_PI;

    // 시야각도 내부 && 시야거리 이내 
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

void CGray::AI_Idle(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Single", L"Idle");
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
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Suspicious(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        // idle상태 그대로 이어서 텍스처 유지 
    }

    if (m_tState_Obj.Can_Update())
    {
        if (Detect_Player()) // 시야각 이내에 위치 + 시야거리 이내 위치 
        {
            m_fAwareness += fDeltaTime * 5.f;

            // 2. 인지값이 MAX가 되면 플레이어 추격 시작 
            if (m_fMaxAwareness <= m_fAwareness)
            {
                m_fAwareness = m_fMaxAwareness; // 추후 감소를 위해 최대값으로 고정 

                int iCombo = (rand() % 10) + 1; // 1~10 

                if (6 <= iCombo) // 6~10
                    m_tState_Obj.Set_State(STATE_OBJ::YOUDIE);

                if (6 > iCombo) // 1~5
                    m_tState_Obj.Set_State(STATE_OBJ::TAUNT);

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
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
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
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
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
        // 플레이어 놓치면 계속 주변돌아보면서 체크하는곳 
    }
    if (m_tState_Obj.Can_Update())
    {
        m_fConsider -= fDeltaTime * 1.f;

        if (m_fConsider < 0)
            m_fConsider = 0;

        if (0 == m_fConsider) //인지값이 초기화되면 
        {
            m_fConsider = m_fMaxConsider; // 다시 할수있으니 값 복구 
            m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        }
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Chase(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // --------거리비교 상태머신 -----------
                    // 뛰어서 다가옴 : a > 8
        if (Detect_Player())
        {
            if (m_fRunDistance < Calc_Distance())
            {
                m_tState_Obj.Set_State(STATE_OBJ::RUN);
            }
            // 걸어서 다가옴 : 7 < a <= 8 
            else if (m_fWalkDistance < Calc_Distance() && m_fRunDistance >= Calc_Distance())
            {
                m_tState_Obj.Set_State(STATE_OBJ::WALK);
            }
            // 무빙 : 4 < a <= 7
            else if (m_fInchDistance < Calc_Distance() && m_fWalkDistance >= Calc_Distance())
            {
                int iCombo = (rand() % 10) + 1;

                if (6 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::KEEPEYE);

                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::SIDEWALK);
            }
            else    // 공격함
            {
                int iCombo = (rand() % 15) + 1;

                if (11 <= iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::THROW);

                if (11 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::ATTACK);
                if (6 > iCombo)
                    m_tState_Obj.Set_State(STATE_OBJ::HEAVYATTACK);
            }
        }
    }
    else // 쫒다가도 시야에서 벗어나면 게이지 줄어들어서 SUSPICIOUS로 돌아감 
    {
        m_fAwareness -= fDeltaTime * 4.f;

        if (m_fAwareness < 0)
            m_fAwareness = 0;

        if (0 == m_fAwareness) //인지값이 초기화되면 
        {
            m_tState_Obj.Set_State(STATE_OBJ::RECONNAISSANCE);
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
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
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
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
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
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"YouDie");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
    }

    if (m_tState_Obj.Can_Update())
    {
        // 조건 - 플레이어가 시야각으로 들어오면 
        if (Detect_Player())
        {
            m_tState_Obj.Set_State(STATE_OBJ::CHASE);
        }
    }


    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_SideWalk(float fDeltaTime)
{
}

void CGray::AI_Rest(float fDeltaTime)
{
    if (m_tState_Obj.IsState_Entered())
    {
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Brown_Single", L"Stand_South");
        m_fFrameEnd = _float(m_pTextureComp->Get_VecTexture()->size());
        m_iPreHP = m_iHP; // 상태체크용 hp저장 
    }

    if (m_tState_Obj.Can_Update())
    {

         m_tState_Obj.Set_State(STATE_OBJ::SUSPICIOUS);
        
    }

    if (m_tState_Obj.IsState_Exit())
    {

    }
}

void CGray::AI_Throw(float fDeltaTime)
{
}

void CGray::AI_Attack(float fDeltaTime)
{
}

void CGray::AI_UpRightRun(float fDeltaTime)
{
}

void CGray::AI_Frighten(float fDeltaTime)
{
}

void CGray::AI_HeavyAttack(float fDeltaTime)
{
}

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

void CGray::Idle(float fDeltaTime)
{
}
