#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGraphicDev;
class CManagement;

END

class CImguiWin;

class CImguiMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CImguiMgr)

private:
	explicit CImguiMgr();
	virtual ~CImguiMgr();

public:
	virtual void	Free();

public:
	HRESULT Ready_Imgui(CGraphicDev** ppGraphicClass, LPDIRECT3DDEVICE9* ppGraphicDev);
	HRESULT Update_Imgui(const _float& fTimeDelta);
	HRESULT Render_Imgui();
	HRESULT Render_AdditionImgui(HRESULT dwDeviceState);

public:
	// 디바이스의 매개변수에 창 사이즈를 바꿔 초기화를 다시해줍니다.
	void ResetDevice(_uint dwResizeWidth = 0, _uint dwResizeHeight = 0);
	bool LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight);

private:		// 접근을 최소화하는 함수, 변수
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:		// IMGUI 관련 변수
	ImGuiIO*	m_pIO;

private:
	bool		m_bShow_DemoWindow =false;
	bool		m_bShow_AnotherWindow =	false;
	bool		m_bShow_MapWindow =	false;								// 소영 새로운창 create용
	ImVec4		m_vClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
	_unmap<const _tchar*, CImguiWin*>		m_mapImguiWin;					// IMGUI 윈도우 클래스 저장 컨테이너
};

