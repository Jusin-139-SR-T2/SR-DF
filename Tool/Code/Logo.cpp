#include "Logo.h"

#include "BackGround.h"
#include "MyTexture.h"

#include "Export_System.h"
#include "Export_Utility.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLogo::Free()
{
	SUPER::Free();
}

HRESULT CLogo::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	SUPER::Render_Scene();
}

HRESULT CLogo::Ready_Prototype()
{
	// 로고용 텍스처들
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RightHand1.png", TEX_NORMAL, L"UI", L"RightHand"), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Player/RightHand%d.png", TEX_NORMAL, L"UI", L"RightHand", _range<_uint>(0U, 1U)), E_FAIL);

	// 플레이어 오른손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"RightHand")), E_FAIL);
	
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);

#pragma region Brown Monster - 작업대기상태 
	// Monster Texture - Brown - Single : 3EA
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Stand/Stand_South.png", TEX_NORMAL, L"Brown_Single", L"Stand_South"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Single/CrotchHit.png", TEX_NORMAL, L"Brown_Single", L"CrotchHit"), E_FAIL);

	// Monster Texture - Brown - Multi : 18EA
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"BasicAttack", _range<_uint>(0U, 4U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 11U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Pooping/Pooping_%d.png", TEX_NORMAL, L"Brown_Multi", L"Pooping", _range<_uint>(0U, 2U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Rest/IdleReady_%d.png", TEX_NORMAL, L"Brown_Multi", L"Rest", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/GetUp/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Jump/Jump_%d.png", TEX_NORMAL, L"Brown_Multi", L"Jump", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U)), E_FAIL);
#pragma endregion

	// 프로토타입 인스턴스를 등록한다.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// 트랜스폼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// 버퍼
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// 텍스처
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"RightHand")), E_FAIL);	// 텍스처

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Brown_Single", L"Suspicious")), E_FAIL);	// Brown몬스터 

	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// 정상
	// 레이어 초기화
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create(0.f)), E_FAIL);

	// 배경생성
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", CBackGround::Create(m_pGraphicDev)), E_FAIL);

	// 플레이어 텍스처 생성
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MyTexture", CMyTexture::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}


