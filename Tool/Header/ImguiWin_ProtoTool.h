#pragma once

#include "ImguiWin.h"

#include "imgui.h"

/// <summary>
/// 오브젝트의 프로토 타입을 제조하는 툴
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

private:	// 프로토
	string						m_strAdd_Proto = "";			// 씬 추가하기
	map<string, FProtoData>		m_mapProtoData;					// 프로토 타입 데이터
	string						m_strCur_Proto = "";			// 현재 선택된 프로토 키
	string						m_strCur_Component = "";		// 현재 선택된 컴포넌트 키
};

