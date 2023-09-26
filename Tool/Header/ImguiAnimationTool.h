#pragma once

#include "ImguiWin.h"
#include "MyTexture.h"

class CImguiAnimationTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiAnimationTool)

private:
	explicit CImguiAnimationTool();
	virtual ~CImguiAnimationTool();

public:
	static CImguiAnimationTool* Create();


public:
	// 이미지를 추가하는 함수
	void AddImage(const std::string& imagePath);
	//std::string	OpenImageFileDialog();
	std::wstring ConvertToWideString(const std::string& ansiString);
	HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT	SaveData(const _tchar* mapTag);
	void	SaveUI(wstring wstrFileName, wstring wstrFilePath);

	CMyTexture* FindUI(const _tchar* szKey);

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)

protected:
	virtual void Free();
	
private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface = nullptr;

	map<const _tchar*, CMyTexture*> m_mapTexture;

};

