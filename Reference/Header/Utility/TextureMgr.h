#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CTexture;

class CTextureMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CTextureMgr)

private:
	explicit CTextureMgr();
	virtual ~CTextureMgr();

public:
	HRESULT			Ready_Texture(const _tchar* pProtoTag);
	HRESULT			Insert_Texture(const _tchar* pFilePath);
	HRESULT			Read_ImgPath(const _tchar* pFilePath);

private:
	_unmap<wstring, CTexture*>		m_mapTexture;

public:
	virtual void	Free();
};

END