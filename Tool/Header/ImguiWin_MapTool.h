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
	void			Warning();

private:
	void			Layout_Browser(const ImGuiWindowFlags& iMain_Flags);
	void			Layout_Browser_Scene();
	void			Layout_Browser_Terrain();
	void			Layout_Browser_Object();
	void			Layout_Browser_Light();
	void			Layout_Browser_Camera();

private:
	void			Layout_Hierarchi(const ImGuiWindowFlags& iMain_Flags);

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




private:	// 씬 추가
	string						m_strAdd_SceneName;			// 씬 추가하기
	vector<string>				m_vecSceneName;				// 씬 이름 표시하기용
	_int						m_iSelected_Scene;			// Selectable에서 선택된 씬

private:	// 레이어 정보
	//vector<string>


private:
	

private:	// 계층 관련
	enum EPRIORITY_OBJECT : _ulong
	{
		EPRIORITY_OBJECT_UPDATE,
		EPRIORITY_OBJECT_LATE,
		EPRIORITY_OBJECT_RENDER,
		EPRIORITY_OBJECT_END
	};
	struct FObjectProperty
	{
		string		strName = "";
		string		strObjectID = "";

		_vec3		vPos = { 0.f,0.f,0.f };
		_vec3		vRot = { 0.f,0.f,0.f };
		_vec3		vScale = { 0.f,0.f,0.f };
		_float		fPriority[EPRIORITY_OBJECT_END] = { 0.f, 0.f, 0.f };
		_bool		bUsePriority[EPRIORITY_OBJECT_END] = { true, true, true };
	};
	struct FLayerProperty
	{
		string						strName = "";
		_float						fPriority = 0.f;
		vector<FObjectProperty>		vecObject;
	};

private:
	void			Reset_Hierarchi()
	{
		m_iSelectedHierarchi_Layer = -1;
		m_iSelectedHierarchi_Layer_Remain = -1;
		m_iSelectedHierarchi_Object = -1;
		m_iSelectedHierarchi_Object_Remain = -1;
		m_ePropertySelected_Type = ESELECTED_TYPE_NONE;
	}
	
private:
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


private:	// 속성 관련
	_bool			m_bInput_Warning = false;

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

	void			Clamp_Vec3Translate(_vec3& vTranslate, _float fValue)
	{
		if (vTranslate.x < -fValue)
			vTranslate.x = -fValue;
		else if (vTranslate.x > fValue)
			vTranslate.x = fValue;

		if (vTranslate.y < -fValue)
			vTranslate.y = -fValue;
		else if (vTranslate.y > fValue)
			vTranslate.y = fValue;

		if (vTranslate.z < -fValue)
			vTranslate.z = -fValue;
		else if (vTranslate.z > fValue)
			vTranslate.z = fValue;
	}
	void			Clamp_Vec3Rot(_vec3& vRot, _float fValue)
	{
		if (vRot.x < -fValue)
			vRot.x = -fValue;
		else if (vRot.x > fValue)
			vRot.x = fValue;

		if (vRot.y < -fValue)
			vRot.y = -fValue;
		else if (vRot.y > fValue)
			vRot.y = fValue;

		if (vRot.z < -fValue)
			vRot.z = -fValue;
		else if (vRot.z > fValue)
			vRot.z = fValue;
	}
	void			Clamp_Vec3Scale(_vec3& vScale, _float fValue)
	{
		if (vScale.x < -fValue)
			vScale.x = -fValue;
		else if (vScale.x > fValue)
			vScale.x = fValue;

		if (vScale.y < -fValue)
			vScale.y = -fValue;
		else if (vScale.y > fValue)
			vScale.y = fValue;

		if (vScale.z < -fValue)
			vScale.z = -fValue;
		else if (vScale.z > fValue)
			vScale.z = fValue;
	}

	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventChar != 0 && strlen(data->Buf) >= 10)
			return 1;
		return 0;
	}
};

