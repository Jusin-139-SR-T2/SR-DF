#pragma once

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphicDev : public CBase
{
	DECLARE_SINGLETON(CGraphicDev)
public:
	explicit CGraphicDev();
	virtual ~CGraphicDev();

public:
	HRESULT		Ready_GraphicDev(CGraphicDev** ppGraphicClass, HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY);

	void		Render_Begin(D3DXCOLOR Color);
	void		Render_End();

public:
	LPDIRECT3D9				Get_GraphicSDK() { return m_pSDK; }
	LPDIRECT3DDEVICE9		Get_GraphicDev() { return m_pGraphicDev; }
	D3DPRESENT_PARAMETERS	Get_D3DPP() { return m_d3dpp; }
	LPD3DXSPRITE			Get_Sprite() { return m_pSprite; }
	LPD3DXFONT				Get_Font() { return m_pFont; }

private:
	LPDIRECT3D9				m_pSDK;
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	D3DPRESENT_PARAMETERS	m_d3dpp;
	LPD3DXSPRITE			m_pSprite;
	LPD3DXFONT				m_pFont;

public:
	virtual void	Free();
};

END