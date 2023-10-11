#pragma once

#include "ImguiWin.h"

#include "imgui.h"

/// <summary>
/// ������Ʈ�� ������ Ÿ���� �����ϴ� ��
/// </summary>
class CImguiWin_ProtoTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_ProtoTool)

private:
	explicit CImguiWin_ProtoTool();
	virtual ~CImguiWin_ProtoTool();

public:
	static CImguiWin_ProtoTool* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

private:
	void Layout_ObjectBrowser(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Component(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Property(const ImGuiWindowFlags& iMain_Flags);
	void Layout_Viewer(const ImGuiWindowFlags& iMain_Flags);

private:
	struct FComponentData
	{
		
	};

	struct FProtoData
	{
		string							strName = "";
		map<string, FComponentData>		mapComponent;
	};

private:	// ������
	string						m_strAdd_Proto = "";			// �� �߰��ϱ�
	map<string, FProtoData>		m_mapProtoData;					// ������ Ÿ�� ������
	string						m_strCur_Proto = "";			// ���� ���õ� ������ Ű
	string						m_strCur_Component = "";		// ���� ���õ� ������Ʈ Ű
};

