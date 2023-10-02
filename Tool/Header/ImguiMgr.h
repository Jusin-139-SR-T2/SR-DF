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
	HRESULT Render_AdditionImgui();
	HRESULT Render_LossHandle(HRESULT dwDeviceState);

public:
	// ����̽��� �Ű������� â ����� �ٲ� �ʱ�ȭ�� �ٽ����ݴϴ�.
	void ResetDevice(_uint dwResizeWidth = 0, _uint dwResizeHeight = 0);
	bool LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight);

public:
	GETSET_EX1(LPDIRECT3DDEVICE9, m_pGraphicDev, GraphicDev, GET)

private:		// ������ �ּ�ȭ�ϴ� �Լ�, ����
	Engine::CGraphicDev*		m_pDeviceClass;
	Engine::CManagement*		m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

private:		// IMGUI ���� ����
	ImGuiIO*	m_pIO;

private:
	bool		m_bShow_DemoWindow =false;
	bool		m_bShow_AnotherWindow =	false;
	bool		m_bShow_MapWindow =	false;								// �ҿ� ���ο�â create��
	ImVec4		m_vClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
	_unmap<const _tchar*, CImguiWin*>			m_mapImguiWin;			// IMGUI ������ Ŭ���� ���� �����̳�
	vector<pair< const _tchar*, CImguiWin*>>	m_vecSortedImguiWin;	// IMGUI value ��� ���� �����̳�

public:
	GETSET_EX2(LPDIRECT3DTEXTURE9, m_pEditorTexture, EditorTexture, SET_PTR, GET_PTR)

private:
	LPDIRECT3DTEXTURE9 m_pEditorTexture = nullptr;

public:
	void HelpMarker(const char* desc) { HelpMarkerEx("(?)", desc); }
	void HelpMarkerEx(const char* marker, const char* desc);
	

public:
	void	Open_Layout(const _tchar* pWin);
	void	Close_Layout(const _tchar* pWin);
};

