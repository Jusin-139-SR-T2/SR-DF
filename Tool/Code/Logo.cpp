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
	// �ΰ�� �ؽ�ó��
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"../Client/Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);

	// ������Ÿ�� �ν��Ͻ��� ����Ѵ�.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// Ʈ������
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// ����
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// �ؽ�ó
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Title")), E_FAIL);	// �ؽ�ó

	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// ����
	// ���̾� �ʱ�ȭ
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create(0.f)), E_FAIL);

	// ������
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", CBackGround::Create(m_pGraphicDev)), E_FAIL);

	// �÷��̾� �ؽ�ó ����
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"MyTexture", CMyTexture::Create(m_pGraphicDev)), E_FAIL);

	return S_OK;
}


