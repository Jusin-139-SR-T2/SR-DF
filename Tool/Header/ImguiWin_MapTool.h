#pragma once

#include "ImguiWin.h"

#include "imgui.h"
#include "Serialize_Core.h"
#include "Serialize_BaseClass.h"

const string g_strSceneExt = ".ascene";
const string g_strTerrainExt = ".aterrain";
const string g_strObjectExt = ".aobject";
const string g_strLightExt = ".alight";
const string g_strCameraExt = ".acamera";

const string g_strDataPath = "../Client/Resource/Data/";
const string g_strScenePath = g_strDataPath + "Scene/";
const string g_strObjectPath = g_strDataPath + "Object/";
const string g_strTerrainPath = g_strDataPath + "Terrain/";
const string g_strLightPath = g_strDataPath + "Light/";

class CImguiWin_MapTool : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_MapTool)

private:
	explicit CImguiWin_MapTool();
	virtual ~CImguiWin_MapTool();

public:
	static CImguiWin_MapTool* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;


private:
	ImGuiStyle		m_pStyle;

	_bool					m_bOpt_FullScreen;
	_bool					m_bOpt_Padding;

	ImGuiDockNodeFlags		m_DockSpace_Flags;
	ImGuiWindowFlags		m_Window_Flags;


private:
	void			Layout_Browser(const ImGuiWindowFlags& iMain_Flags);
	void			Layout_Browser_Scene();
	void			Layout_Browser_Terrain();
	void			Layout_Browser_Object();
	void			Layout_Browser_Light();
	void			Layout_Browser_Camera();

private:
	void			Layout_Hierarchy(const ImGuiWindowFlags& iMain_Flags);

private:
	void			Layout_Property(const ImGuiWindowFlags& iMain_Flags);
	void			Layout_Property_Scene();
	void			Layout_Property_Layer();
	void			Layout_Property_Object();
	void			Layout_Property_Light();

private:
	void			Layout_Viewer(const ImGuiWindowFlags& iMain_Flags);

private:
	enum ESELECTED_TYPE : _ulong
	{
		ESELECTED_TYPE_NONE,
		ESELECTED_TYPE_SCENE,
		ESELECTED_TYPE_LAYER,
		ESELECTED_TYPE_OBJECT,
		ESELECTED_TYPE_LIGHT
	};
	ESELECTED_TYPE				m_ePropertySelected_Type = ESELECTED_TYPE_NONE;


private:


private:	// 씬 추가
	string						m_strAdd_SceneName;			// 씬 추가하기
	vector<string>				m_vecSceneName;				// 씬 이름 표시하기용
	_int						m_iSelected_Scene;			// Selectable에서 선택된 씬

private:	// 레이어 정보
	//vector<string>


private:	// 계층 관련
	struct FObjectProperty
	{
		string		strName;

		_vec3		vPos;
		_vec3		vRot;
		_vec3		vScale;
		_float		fPriority_Update;
		_bool		fUsePriority_Update;
		_float		fPriority_LateUpdate;
		_bool		fUsePriority_LateUpdate;
		_float		fPriority_Render;
		_bool		fUsePriority_Render;
	};
	struct FLayerProperty
	{
		string						strName;
		_float						fPriority;
		vector<FObjectProperty>		vecObject;
	};
	
	string						m_strSceneName = u8"씬";			// 씬 이름
	_bool						m_bScene_Loaded = false;

	vector<FLayerProperty>		m_vecHierarchi;					// 계층 이름, 레이어, 오브젝트
	_int						m_iSelectedHierarchi_Layer = -1;
	_int						m_iSelectedHierarchi_Layer_Remain = -1;
	_int						m_iSelectedHierarchi_Object = -1;
	_int						m_iSelectedHierarchi_Object_Remain = -1;
	char						m_arrAddLayer_Buf[256] = "";
	_bool						m_bFocusedLayer_Edit = false;

private:	// 뷰 추가 레이아웃
	vector<string>	m_vecCoord_Item = { u8"월드", u8"로컬", u8"오일러" };
	_uint			m_iIndex_CoordItem = 0;

	vector<string>	m_vecPivot_Item = { u8"오브젝트", u8"커서", u8"중간점" };
	_uint			m_iIndex_PivotItem = 0;

	vector<string>	m_vecSnap_Item = { u8"좌표", u8"점", u8"선", u8"면" };
	_uint			m_iIndex_SnapItem = 0;

	_bool			m_bSnap_Active = false;


private:	// 터레인 관련
	void			Apply_Terrain(const string& strTerrainName);
	void			Serialize_Terrain(const string& strTerrainName);
	void			Export_ParsedTerrain(const FSerialize_Terrain& tTerrain);
	void			Import_ParsedTerrain(const FSerialize_Terrain& tTerrain);


private:
	enum EINPUT_TERRAIN
	{
		EINPUT_TERRAIN_HORIZON,
		EINPUT_TERRAIN_VERTICLE,
		EINPUT_TERRAIN_HEIGHT,
		EINPUT_TERRAIN_OFFSET_X,
		EINPUT_TERRAIN_OFFSET_Y,
		EINPUT_TERRAIN_OFFSET_Z,
		EINPUT_TERRAIN_SCALE_X,
		EINPUT_TERRAIN_SCALE_Y,
		EINPUT_TERRAIN_SCALE_Z,
		EINPUT_TERRAIN_END
	};
	vector<string>	m_vecInput_Terrain;


private:	// 유틸리티
	void			Set_Button_ActiveColor();
	void			Set_Button_NonActiveColor();
	void			Set_Button_ReturnColor();

	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventChar != 0 && strlen(data->Buf) >= 10)
			return 1;
		return 0;
	}
};

