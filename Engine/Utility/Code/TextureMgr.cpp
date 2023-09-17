#include "TextureMgr.h"

IMPLEMENT_SINGLETON(CTextureMgr)

CTextureMgr::CTextureMgr()
{
}

CTextureMgr::~CTextureMgr()
{
}

HRESULT CTextureMgr::Ready_Texture(const _tchar* pProtoTag)
{
	return E_NOTIMPL;
}

HRESULT CTextureMgr::Insert_Texture(const _tchar* pFilePath)
{
	return E_NOTIMPL;
}

HRESULT CTextureMgr::Read_ImgPath(const _tchar* pFilePath)
{
	return E_NOTIMPL;
}

void CTextureMgr::Free()
{
}
