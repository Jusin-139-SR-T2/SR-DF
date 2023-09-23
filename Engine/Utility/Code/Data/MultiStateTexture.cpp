#include "MultiStateTexture.h"

#include <future>
#include <thread>



CMultiStateTexture::CMultiStateTexture(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
	m_eTexComType = ETEXTURE_COMTYPE::MULTI_STATE;
}

CMultiStateTexture::~CMultiStateTexture()
{
}

CMultiStateTexture* CMultiStateTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Texture(pGraphicDev)))
	{
		Safe_Release(pInstance);

		MSG_BOX("Texture Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CMultiStateTexture::Free()
{
	for (auto item : m_mapMultiState)
	{
		for(auto indexItem : item.second)
			Safe_Release(indexItem);
	}
}

HRESULT CMultiStateTexture::Ready_Texture(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

HRESULT CMultiStateTexture::Insert_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	// 스테이트 키에 이미 텍스처 셋이 있으면 처리하지 않는다.
	if (m_mapMultiState.find(pStateKey) != m_mapMultiState.end())
		return E_FAIL;

	// 파일 존재여부 확인, 특히 사이즈에 해당하는 카운트 값이 존재하는지 확인, 잘못된 경로가 있다면 오류 반환
	//{
	//	TCHAR	szFileName[256] = L"";
	//	wsprintf(szFileName, pFilePath, iCntRange.second);

	//	wifstream file(szFileName);
	//	FALSE_CHECK_RETURN(file.good(), E_FAIL);
	//	
	//}
	

	vector<LPDIRECT3DBASETEXTURE9> vecTexture;
	vector<future<HRESULT>> vecAsync;
	vector<wstring> vecPath;

	// 초기세팅, 안전 검사후 예약
	vecPath.resize(iCntRange.second - iCntRange.first + 1U);
	vecTexture.resize(iCntRange.second - iCntRange.first + 1U);
	m_mapMultiState[pStateKey].reserve(iCntRange.second + 1U);
	m_mapMultiState.emplace(pStateKey, vector<LPDIRECT3DBASETEXTURE9>());

	for (_uint i = 0; i <= iCntRange.second; ++i)
	{
		TCHAR	szFileName[256] = L"";
		wsprintf(szFileName, pFilePath, i);
		vecPath[i] = szFileName;
	}

	for (_uint i = 0; i <= iCntRange.second; ++i)
	{
		vecAsync.push_back(async(launch::async, &CMultiStateTexture::Insert_TextureAsync, this, vecPath[i].c_str(), eType, ref(vecTexture), i));
	}

	for (_uint i = 0; i <= iCntRange.second; ++i)
	{
		vecAsync[i].get();
		m_mapMultiState[pStateKey].push_back(vecTexture[i]);
	}

	return S_OK;
}

HRESULT CMultiStateTexture::Insert_TextureAsync(const _tchar* pFilePath, TEXTUREID eType, vector<LPDIRECT3DBASETEXTURE9>& vecTexture, _uint iIndex)
{
	switch (eType)
	{
	case TEX_NORMAL:
	{
		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, pFilePath, (LPDIRECT3DTEXTURE9*)&vecTexture[iIndex]), E_FAIL);
		break;
	}
	case TEX_CUBE:
	{
		FAILED_CHECK_RETURN(D3DXCreateCubeTextureFromFile(m_pGraphicDev, pFilePath, (LPDIRECT3DCUBETEXTURE9*)&vecTexture[iIndex]), E_FAIL);
		break;
	}
	case TEX_VOLUME:
	{
		FAILED_CHECK_RETURN(D3DXCreateVolumeTextureFromFile(m_pGraphicDev, pFilePath, (LPDIRECT3DVOLUMETEXTURE9*)&vecTexture[iIndex]), E_FAIL);
		break;
	}
	}

	return S_OK;
}

void CMultiStateTexture::Transfer_Texture(vector<LPDIRECT3DBASETEXTURE9>* pVecTexture, const _tchar* pStateKey)
{
	if (m_mapMultiState.empty())
		return;

	auto iter = m_mapMultiState.find(pStateKey);

	if (iter == m_mapMultiState.end())
		return;

	if (m_mapMultiState[pStateKey].size() > pVecTexture->size())
	{
		pVecTexture->reserve(m_mapMultiState[pStateKey].size());
	}
		(*pVecTexture) = m_mapMultiState[pStateKey];
}
