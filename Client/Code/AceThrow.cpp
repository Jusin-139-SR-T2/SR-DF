#include "stdafx.h"
#include "AceThrow.h"

CAceThrow::CAceThrow(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CAceThrow::CAceThrow(const CAceThrow& rhs)
	: Base(rhs)
{
}

CAceThrow::~CAceThrow()
{
}

CAceThrow* CAceThrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pObjTag, 
    const _float _fx, const _float _fy, const _float _fz, const _float iCnt)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        Safe_Release(pInstance);

        MSG_BOX("ThrowObject Create Failed");
        return nullptr;
    }

    pInstance->m_pTransformComp->Set_Pos({ _fx, _fy, _fz });
    pInstance->ThrowName(pObjTag);
    pInstance->m_fFrame = iCnt;
    return pInstance;
}

CAceThrow* CAceThrow::Create(LPDIRECT3DDEVICE9 pGraphicDev, const FSerialize_GameObject& tObjectSerial)
{
    ThisClass* pInstance = new ThisClass(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject(tObjectSerial)))
    {
        Safe_Release(pInstance);

        MSG_BOX("ThrowObject Create Failed");
        return nullptr;
    }

    return pInstance;
}

HRESULT CAceThrow::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

HRESULT CAceThrow::Ready_GameObject(const FSerialize_GameObject& tObjectSerial)
{
    FAILED_CHECK_RETURN(Ready_GameObject(), E_FAIL);

    m_pTransformComp->Set_Pos(tObjectSerial.vPos);
    m_pTransformComp->Set_Rotation(tObjectSerial.vRotation);
    m_pTransformComp->Set_Scale(tObjectSerial.vScale);

    wstring strConvName(tObjectSerial.tHeader.strName.begin(), tObjectSerial.tHeader.strName.end());
    Set_ObjectName(strConvName);

    m_fPriority[0] = tObjectSerial.fPriority_Update;
    m_fPriority[1] = tObjectSerial.fPriority_LateUpdate;
    m_fPriority[2] = tObjectSerial.fPriority_Render;

    m_bUsePriority[0] = tObjectSerial.bUsePriority_Update;
    m_bUsePriority[1] = tObjectSerial.bUsePriority_LateUpdate;
    m_bUsePriority[2] = tObjectSerial.bUsePriority_Render;


    m_eFactoryClass = OBJECT_CLASS::THROW;

    return S_OK;
}

_int CAceThrow::Update_GameObject(const _float& fTimeDelta)
{
    SUPER::Update_GameObject(fTimeDelta);

    // 지형타기 
    Height_On_Terrain();

    // 빌보드 
    BillBoard(fTimeDelta);

    //-------------------------------------------------------
 
    if (Engine::IsKey_Pressed(DIK_U))
    {
        m_bThrow = true;
        m_fHp = m_fCrackedHp;
    }
    if (Engine::IsKey_Pressed(DIK_I))
    {
        m_bThrow = true;
        m_fHp = m_fBrokenHp;
    }

    //-------------------------------------------------------

    // 변수에 저장된 enum으로 texture 결정 - eaten 변경때문에 
    Change_Texture(m_pCurName);

    // Renderer 등록 
    Engine::Add_RenderGroup(RENDER_ALPHATEST, this);

    return S_OK;
}

void CAceThrow::LateUpdate_GameObject()
{
    SUPER::LateUpdate_GameObject();
}

void CAceThrow::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformComp->Get_Transform());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    m_pTextureComp->Render_Texture(_ulong(m_fFrame));
    m_pBufferComp->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAceThrow::Free()
{
    SUPER::Free();
}

HRESULT CAceThrow::Add_Component()
{
    NULL_CHECK_RETURN(m_pBufferComp = Set_DefaultComponent_FromProto<CRcBufferComp>(ID_STATIC, L"Com_Buffer", L"Proto_RcTexBufferComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTextureComp = Set_DefaultComponent_FromProto<CTextureComponent>(ID_STATIC, L"Com_Texture", L"Proto_MonsterTextureComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pTransformComp = Set_DefaultComponent_FromProto<CTransformComponent>(ID_DYNAMIC, L"Com_Transform", L"Proto_TransformComp"), E_FAIL);
    NULL_CHECK_RETURN(m_pCalculatorComp = Set_DefaultComponent_FromProto<CCalculatorComponent>(ID_STATIC, L"Com_Calculator", L"Proto_CalculatorComp"), E_FAIL);

    return S_OK;
}

#pragma region Basic Setting
HRESULT CAceThrow::BillBoard(const _float& fTimeDelta)
{
    CTransformComponent* m_pPlayerTransformcomp = dynamic_cast<CTransformComponent*>(Engine::Get_Component(ID_DYNAMIC, L"GameLogic", L"Player", L"Com_Transform"));
    NULL_CHECK_RETURN(m_pPlayerTransformcomp, -1);

    _vec3 vDir = m_pPlayerTransformcomp->Get_Pos() - m_pTransformComp->Get_Pos();

    D3DXVec3Normalize(&vDir, &vDir);

    _float rad = atan2f(vDir.x, vDir.z);

    m_pTransformComp->Set_RotationY(rad);

    return S_OK;
}

void CAceThrow::Height_On_Terrain()
{
    _vec3		vPos;
    m_pTransformComp->Get_Info(INFO_POS, &vPos);

    CTerrainBufferComp* pTerrainBufferComp = dynamic_cast<CTerrainBufferComp*>(Engine::Get_Component(ID_STATIC, L"Terrain", L"Terrain", L"Com_Buffer"));
    NULL_CHECK(pTerrainBufferComp);

    _float	fHeight = m_pCalculatorComp->Compute_HeightOnTerrain(&vPos,
        pTerrainBufferComp->Get_VtxPos(),
        pTerrainBufferComp->Get_VertexCountX() + 1U,
        pTerrainBufferComp->Get_VertexCountZ() + 1U,
        pTerrainBufferComp->Get_Scale(),
        pTerrainBufferComp->Get_InvOffset());

    m_pTransformComp->Set_Pos(vPos.x, fHeight + 0.5f, vPos.z);
}

void CAceThrow::ThrowName(const _tchar* pObjTag)
{
    // Paper
    if ((wcscmp(pObjTag, L"PAPERBOX") == 0) || (wcscmp(pObjTag, L"PaperBox") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PAPERBOX;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"PaperBox");
    }
    else if ((wcscmp(pObjTag, L"SCRUNCHEDPAPER_0") == 0) || (wcscmp(pObjTag, L"ScrunchedPaper_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::SCRUNCHEDPAPER;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"ScrunchedPaper_0");
    }
    else if ((wcscmp(pObjTag, L"SCRUNCHEDPAPER_1") == 0) || (wcscmp(pObjTag, L"ScrunchedPaper_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::SCRUNCHEDPAPER;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"ScrunchedPaper_1");
    }
    else if ((wcscmp(pObjTag, L"FEDORA") == 0) || (wcscmp(pObjTag, L"Fedora") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::FEDORA;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Fedora");
    }
    else if ((wcscmp(pObjTag, L"ROPE") == 0) || (wcscmp(pObjTag, L"Rope") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::ROPE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Rope");
    }
    else if ((wcscmp(pObjTag, L"SACK_0") == 0) || (wcscmp(pObjTag, L"Sack_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::SACK;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Sack_0");
    }
    else if ((wcscmp(pObjTag, L"SACK_1") == 0) || (wcscmp(pObjTag, L"Sack_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::SACK;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Sack_1");
    }
    else if ((wcscmp(pObjTag, L"SACK_2") == 0) || (wcscmp(pObjTag, L"Sack_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::SACK;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Sack_2");
    }
    else if ((wcscmp(pObjTag, L"HANDBAG") == 0) || (wcscmp(pObjTag, L"Handbag") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::HANDBAG;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Handbag");
    }    

    // Wood
    else if ((wcscmp(pObjTag, L"BARSTOOL") == 0) || (wcscmp(pObjTag, L"BarStool") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::BARSTOOL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"BarStool");
    }
    else if ((wcscmp(pObjTag, L"CAFESIGN") == 0) || (wcscmp(pObjTag, L"CafeSign") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CAFESIGN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"CafeSign");
    }
    else if ((wcscmp(pObjTag, L"CAFETABLE") == 0) || (wcscmp(pObjTag, L"CafeTable") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CAFETABLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"CafeTable");
    }
    else if ((wcscmp(pObjTag, L"COATHANGER") == 0) || (wcscmp(pObjTag, L"CoatHanger") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::COATHANGER;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"CoatHanger");
    }
    else if ((wcscmp(pObjTag, L"TOOLBOX") == 0) || (wcscmp(pObjTag, L"Toolbox") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TOOLBOX;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Toolbox");
    }
    else if ((wcscmp(pObjTag, L"UMBRELLASTAND") == 0) || (wcscmp(pObjTag, L"UmbrellaStand") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::UMBRELLASTAND;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"UmbrellaStand");
    }
    else if ((wcscmp(pObjTag, L"WOODENBARREL") == 0) || (wcscmp(pObjTag, L"WoodenBarrel") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::WOODENBARREL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"WoodenBarrel");
    }
    else if ((wcscmp(pObjTag, L"WOODENSTOOL") == 0) || (wcscmp(pObjTag, L"WoodenStool") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::WOODENSTOOL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"WoodenStool");
    }
    else if ((wcscmp(pObjTag, L"MATCHES") == 0) || (wcscmp(pObjTag, L"Matches") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::MATCHES;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Matches");
    }
    else if ((wcscmp(pObjTag, L"BOARD") == 0) || (wcscmp(pObjTag, L"Board") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::BOARD;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Board");
    }
    else if ((wcscmp(pObjTag, L"MANNEQUIN") == 0) || (wcscmp(pObjTag, L"Mannequin") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::MANNEQUIN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Mannequin");
    }
    else if ((wcscmp(pObjTag, L"CABLEREEL") == 0) || (wcscmp(pObjTag, L"CableReel") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CABLEREEL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"CableReel");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_0") == 0) || (wcscmp(pObjTag, L"Chair_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_0");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_1") == 0) || (wcscmp(pObjTag, L"Chair_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_1");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_2") == 0) || (wcscmp(pObjTag, L"Chair_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_2");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_3") == 0) || (wcscmp(pObjTag, L"Chair_3") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_3");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_4") == 0) || (wcscmp(pObjTag, L"Chair_4") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_4");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_5") == 0) || (wcscmp(pObjTag, L"Chair_5") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_5");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_6") == 0) || (wcscmp(pObjTag, L"Chair_6") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_6");
    }
    else if ((wcscmp(pObjTag, L"CHAIR_7") == 0) || (wcscmp(pObjTag, L"Chair_7") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAIR;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Chair_7");
    }

    // Glass
    else if ((wcscmp(pObjTag, L"BEERGLASS") == 0) || (wcscmp(pObjTag, L"BeerGlass") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::BEERGLASS;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"BeerGlass");
    }
    else if ((wcscmp(pObjTag, L"BEERMUG") == 0) || (wcscmp(pObjTag, L"BeerMug") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::BEERMUG;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"BeerMug");
    }
    else if ((wcscmp(pObjTag, L"WHISKEYGlASS") == 0) || (wcscmp(pObjTag, L"WhiskeyGlass") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::WHISKEYGlASS;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"WhiskeyGlass");
    }
    else if ((wcscmp(pObjTag, L"POTPLANT") == 0) || (wcscmp(pObjTag, L"Potplant") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::POTPLANT;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Potplant");
    }
    else if ((wcscmp(pObjTag, L"TABLEPLANTLEAF") == 0) || (wcscmp(pObjTag, L"TablePlantLeaf") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TABLEPLANTLEAF;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TablePlantLeaf");
    }
    else if ((wcscmp(pObjTag, L"TABLEPLANTFLOWER") == 0) || (wcscmp(pObjTag, L"TablePlantFlower") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TABLEPLANTFLOWER;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TablePlantFlower");
    }
    else if ((wcscmp(pObjTag, L"MARTINIGLASS") == 0) || (wcscmp(pObjTag, L"MartiniGlass") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::MARTINIGLASS;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"MartiniGlass");
    }
    else if ((wcscmp(pObjTag, L"PERFUME_0") == 0) || (wcscmp(pObjTag, L"Perfume_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PERFUME;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Perfume_0");
    }
    else if ((wcscmp(pObjTag, L"PERFUME_1") == 0) || (wcscmp(pObjTag, L"Perfume_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PERFUME;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Perfume_1");
    }
    else if ((wcscmp(pObjTag, L"PERFUME_2") == 0) || (wcscmp(pObjTag, L"Perfume_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PERFUME;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Perfume_2");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_0") == 0) || (wcscmp(pObjTag, L"GenericBottles_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_0");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_1") == 0) || (wcscmp(pObjTag, L"GenericBottles_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_1");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_2") == 0) || (wcscmp(pObjTag, L"GenericBottles_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_2");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_3") == 0) || (wcscmp(pObjTag, L"GenericBottles_3") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_3");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_4") == 0) || (wcscmp(pObjTag, L"GenericBottles_4") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_4");
    }
    else if ((wcscmp(pObjTag, L"GENERICBOTTLE_5") == 0) || (wcscmp(pObjTag, L"GenericBottles_5") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::GENERICBOTTLE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"GenericBottles_5");
    }
    else if ((wcscmp(pObjTag, L"LAMP_0") == 0) || (wcscmp(pObjTag, L"Lamp_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::LAMP;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Lamp_0");
    }
    else if ((wcscmp(pObjTag, L"LAMP_1") == 0) || (wcscmp(pObjTag, L"Lamp_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::LAMP;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Lamp_1");
    }

    // Steel
    else if ((wcscmp(pObjTag, L"BUCKET") == 0) || (wcscmp(pObjTag, L"Bucket") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::BUCKET;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Bucket");
    }
    else if ((wcscmp(pObjTag, L"TRAFFICCONE") == 0) || (wcscmp(pObjTag, L"TrafficCone") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRAFFICCONE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TrafficCone");
    }
    else if ((wcscmp(pObjTag, L"TRASHBIN_0") == 0) || (wcscmp(pObjTag, L"Trashbin_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHBIN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Trashbin_0");
    }
    else if ((wcscmp(pObjTag, L"TRASHBIN_1") == 0) || (wcscmp(pObjTag, L"Trashbin_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHBIN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Trashbin_1");
    }
    else if ((wcscmp(pObjTag, L"TRASHBIN_2") == 0) || (wcscmp(pObjTag, L"Trashbin_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHBIN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Trashbin_2");
    }
    else if ((wcscmp(pObjTag, L"TRASHCAN_0") == 0) || (wcscmp(pObjTag, L"TrashCan_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TrashCan_0");
    }
    else if ((wcscmp(pObjTag, L"TRASHCAN_1") == 0) || (wcscmp(pObjTag, L"TrashCan_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TrashCan_1");
    }
    else if ((wcscmp(pObjTag, L"TRASHCAN_2") == 0) || (wcscmp(pObjTag, L"TrashCan_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRASHCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TrashCan_2");
    }
    else if ((wcscmp(pObjTag, L"MICROPHONE") == 0) || (wcscmp(pObjTag, L"Microphone") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::MICROPHONE;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"Microphone");
    }
    else if ((wcscmp(pObjTag, L"CHAMPAGNEBUCKET") == 0) || (wcscmp(pObjTag, L"ChampagneBucket") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::CHAMPAGNEBUCKET;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"ChampagneBucket");
    }
    else if ((wcscmp(pObjTag, L"OILBARREL") == 0) || (wcscmp(pObjTag, L"OilBarrel") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::OILBARREL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"OilBarrel");
    }
    else if ((wcscmp(pObjTag, L"TRAFFICBOLLARD") == 0) || (wcscmp(pObjTag, L"TrafficBollard") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::TRAFFICBOLLARD;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"TrafficBollard");
    }
    else if ((wcscmp(pObjTag, L"PAINTCAN_0") == 0) || (wcscmp(pObjTag, L"PaintCan_0") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PAINTCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"PaintCan_0");
    }
    else if ((wcscmp(pObjTag, L"PAINTCAN_1") == 0) || (wcscmp(pObjTag, L"PaintCan_1") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PAINTCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"PaintCan_1");
    }
    else if ((wcscmp(pObjTag, L"PAINTCAN_2") == 0) || (wcscmp(pObjTag, L"PaintCan_2") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PAINTCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"PaintCan_2");
    }
    else if ((wcscmp(pObjTag, L"PAINTCAN_3") == 0) || (wcscmp(pObjTag, L"PaintCan_3") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::PAINTCAN;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"PaintCan_3");
    }
    else if ((wcscmp(pObjTag, L"METALBARREL") == 0) || (wcscmp(pObjTag, L"MetalBarrel") == 0))
    {
        m_pCurName = CAceThrow::THROW_NAME::METALBARREL;
        m_pTextureComp->Receive_Texture(TEX_NORMAL, L"Throw", L"MetalBarrel");
    }
    else
        m_pCurName = CAceThrow::THROW_NAME::THROW_END;
}

void CAceThrow::Change_Texture(THROW_NAME eReceiveName)
{ //바꾸는애들
    if (m_bThrow)
    {
        switch (eReceiveName)
        {
        case CAceThrow::THROW_NAME::LAMP:
            if (m_fMaxHP == m_fHp)
                m_fFrame = 0.f;
            else if (m_fBrokenHp == m_fHp)
                m_fFrame = 1.f;
            break;

        case CAceThrow::THROW_NAME::TRASHBIN:
            if (m_fMaxHP == m_fHp)
                m_fFrame = 0.f;
            else if (m_fCrackedHp == m_fHp)
                m_fFrame = 1.f;
            else if (m_fBrokenHp == m_fHp)
                m_fFrame = 2.f;
            break;

        case CAceThrow::THROW_NAME::TRASHCAN:
            if (m_fMaxHP == m_fHp)
                m_fFrame = 0.f;
            else if (m_fCrackedHp == m_fHp)
                m_fFrame = 1.f;
            else if (m_fBrokenHp == m_fHp)
                m_fFrame = 2.f;
            break;
        }
        m_bThrow = false;
    }
}

void CAceThrow::OnCollision(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceThrow::OnCollisionEntered(CGameObject* pDst, const FContact* const pContact)
{
}

void CAceThrow::OnCollisionExited(CGameObject* pDst)
{
}

#pragma endregion 