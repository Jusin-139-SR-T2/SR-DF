#include "MyTexture.h"

CMyTexture::CMyTexture()
{
}

CMyTexture::~CMyTexture()
{
}

CMyTexture* CMyTexture::Create()
{
	return nullptr;
}

HRESULT CMyTexture::Ready_ImguiWin()
{
	return E_NOTIMPL;
}

_int CMyTexture::Update_ImguiWin(const _float& fTimeDelta)
{
	return _int();
}

void CMyTexture::Free()
{
}
