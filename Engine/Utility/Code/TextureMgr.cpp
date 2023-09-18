#include "TextureMgr.h"

#include "Export_System.h"

#include "SingleTexture.h"
#include "SingleStateTexture.h"
#include "MultiTexture.h"
#include "MultiStateTexture.h"

IMPLEMENT_SINGLETON(CTextureMgr)

CTextureMgr::CTextureMgr()
{
}

CTextureMgr::~CTextureMgr()
{
	Free();
}

void CTextureMgr::Free()
{
	for (auto item : m_mapTexture)
	{
		item.second->Free();
	}
}

HRESULT CTextureMgr::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

HRESULT CTextureMgr::Load_Texture(TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	

	return S_OK;
}

HRESULT CTextureMgr::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	CTexture* pTexture;
	HRESULT hr = S_OK;
	ETEXTURE_COMTYPE eComType;

	// ���� ���翩�� Ȯ��, �߸��� ��ΰ� �ִٸ� ���� ��ȯ
	wifstream file(pFilePath);
	FALSE_CHECK_RETURN(!file.good(), E_FAIL);

	// ����Ű�� �ؽ�ó ������ ���� �з�
	if (0U >= iCntRange.second - iCntRange.first)
	{
		if (pStateKey != L"")
			eComType = ETEXTURE_COMTYPE::SINGLE_STATE;
		else
			eComType = ETEXTURE_COMTYPE::SINGLE;
	}
	else
	{
		if (pStateKey != L"")
			eComType = ETEXTURE_COMTYPE::MULTI_STATE;
		else
			eComType = ETEXTURE_COMTYPE::MULTI;
	}
	
	// �ؽ�óŰ�� ���� �ؽ�ó�� �ִ��� ã�ƺ� �� ���ٸ� ���� �����.
	// �ƴϸ� ������ �ؽ�ó ��ü�� ������Ʈ Ű�� �߰��Ѵ�.
	auto iter = m_mapTexture.find(pTextureKey);
	
	// �ؽ�ó�� ������ ���θ����.
	if (iter == m_mapTexture.end())
	{
		switch (eComType)
		{
		case ETEXTURE_COMTYPE::SINGLE:
			pTexture = CSingleTexture::Create(m_pGraphicDev);
			break;
		case ETEXTURE_COMTYPE::SINGLE_STATE:
			pTexture = CSingleStateTexture::Create(m_pGraphicDev);
			break;
		case ETEXTURE_COMTYPE::MULTI:
			pTexture = CMultiTexture::Create(m_pGraphicDev);
			break;
		case ETEXTURE_COMTYPE::MULTI_STATE:
			pTexture = CMultiStateTexture::Create(m_pGraphicDev);
			break;
		}
		m_mapTexture.emplace(pTextureKey, pTexture);
	}
	// �ؽ�ó�� ������ ������ �Ϳ� �߰��Ѵ�.
	else
	{
		// ������Ʈ Ű�� ���� ������ �ߺ��Ǹ� ó������ �ʴ´�.
		// ���� �ε�� �ؽ�ó �¸� �����Ѵ�.
		if (eComType == ETEXTURE_COMTYPE::SINGLE || eComType == ETEXTURE_COMTYPE::MULTI)
			return E_FAIL;

		pTexture = iter->second;
	}

	FAILED_CHECK_RETURN(hr = pTexture->Insert_Texture(pFilePath, eType, pStateKey, iCntRange), E_FAIL);

	return hr;
}
