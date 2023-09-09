#pragma once

#include "Base.h"
#include "Engine_Define.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "imgui.h"

class Engine::CGraphicDev;
class Engine::CManagement;
class ImGuiIO;

class CMainApp : public CBase
{
	DERIVED_CLASS(CBase, CMainApp)
private:		// ������, �Ҹ���
	explicit CMainApp();
	virtual ~CMainApp();

public:			// �Ϲ����� �Լ�, ����
	HRESULT		Ready_MainApp();
	int			Update_MainApp(const _float& fTimeDelta);
	void		LateUpdate_MainApp();
	void		Render_MainApp();

public:		// ���� �Լ�
	static		CMainApp* Create();
	
private:	// �Ҹ� �Լ�
	virtual		void			Free();

protected:		// ��Ӱ��� �Լ�, ����
private:
	HRESULT		Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDev, Engine::CManagement** ppManagement);
	HRESULT		SetUp_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDev);

private:		// ������ �ּ�ȭ�ϴ� �Լ�, ����
	Engine::CGraphicDev* m_pDeviceClass;
	Engine::CManagement* m_pManagementClass;
	LPDIRECT3D9					m_pSDK;
	LPDIRECT3DDEVICE9			m_pGraphicDev;

	ImGuiIO* m_io;
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	LPDIRECT3DSURFACE9 m_pOriginalRenderTarget;

	void CleanupDeviceD3D();
	void ResetDevice();
	bool LoadTextureFromFile(const _tchar* pFileName, LPDIRECT3DTEXTURE9 pOutTex, _int* pOutWidth, _int* pOutHeight);
};
