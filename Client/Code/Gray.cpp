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
    m_tState_Obj.Add_Func(STATE_OBJ::REST, &CGray::AI_Rest);
    m_tState_Obj.Add_Func(STATE_OBJ::YOUDIE, &CGray::AI_YouDie);
    m_tState_Obj.Add_Func(STATE_OBJ::TAUNT, &CGray::AI_Taunt);

    m_tState_Obj.Add_Func(STATE_OBJ::CHASE, &CGray::AI_Chase); // 최종결정하는곳 

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
//    m_tState_Act.Set_State(STATE_ACT::IDLE);
//    // STATE가 ::A 일때 CGray클래스의 ::B 함수를 수행하는 상태머신 
//    m_tState_Act.Add_Func(STATE_ACT::IDLE, &CGray::Idle);
//    m_tState_Act.Add_Func(STATE_ACT::APPROACH, &CGray::Approach);
//    m_tState_Act.Add_Func(STATE_ACT::MOVING, &CGray::Moving);
//    m_tState_Act.Add_Func(STATE_ACT::ATTACK, &CGray::Attack);

#pragma endregion
#pragma region 액션 키 등록
//    m_mapActionKey.Add_Action(ACTION_KEY::RUN); // 멀때 
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
    
    if (Engine::IsKey_Pressed(DIK_N))
    {
        m_fFrameEnd = 8;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Gray_Multi", L"FacePunch");
    }

    m_fFrame += m_fFrameSpeed * fTimeDelta;

    if (m_fFrame > m_fFrameEnd)
    {
        m_fFrame = 0.f;
    }

    //상태머신
   // m_tState_Obj.Get_StateFunc()(this, fTimeDelta);	// AI
   // m_tState_Act.Get_StateFunc()(this, fTimeDelta);	// 행동
   // m_mapActionKey.Update();	// 액션키 초기화

    Height_On_Terrain(); // 지형타기 

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

void CGray::AI_Idle(float fDeltaTime)
{
}

void CGray::AI_Rest(float fDeltaTime)
{
}

void CGray::AI_Taunt(float fDeltaTime)
{
}

void CGray::AI_YouDie(float fDeltaTime)
{
}

void CGray::AI_Run(float fDeltaTime)
{
}

void CGray::AI_Walk(float fDeltaTime)
{
}

void CGray::AI_KeepEye(float fDeltaTime)
{
}

void CGray::AI_SideWalk(float fDeltaTime)
{
}

void CGray::AI_Chase(float fDeltaTime)
{
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
