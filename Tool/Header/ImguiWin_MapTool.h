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
	ImGuiStyle				m_pStyle;

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

private:	// 뷰 추가 레이아웃
	vector<string>	m_vecCoord_Item = { u8"월드", u8"로컬", u8"오일러" };
	_uint			m_iIndex_CoordItem = 0;

	vector<string>	m_vecPivot_Item = { u8"오브젝트", u8"커서", u8"중간점" };
	_uint			m_iIndex_PivotItem = 0;

	vector<string>	m_vecSnap_Item = { u8"좌표", u8"점", u8"선", u8"면" };
	_uint			m_iIndex_SnapItem = 0;

	_bool			m_bSnap_Active = false;


private:	// 선택 타입
	enum ESELECTED_TYPE : _ulong
	{
		ESELECTED_TYPE_NONE,
		ESELECTED_TYPE_SCENE,
		ESELECTED_TYPE_LAYER,
		ESELECTED_TYPE_OBJECT,
		ESELECTED_TYPE_LIGHT
	};
	ESELECTED_TYPE				m_eSelectedProperty_Type = ESELECTED_TYPE_NONE;

private:	// 계층 관련 정의부
	enum EPRIORITY_OBJECT : _ulong
	{
		EPRIORITY_OBJECT_UPDATE,
		EPRIORITY_OBJECT_LATE,
		EPRIORITY_OBJECT_RENDER,
		EPRIORITY_OBJECT_END
	};
	struct FObjectData
	{
		string		strName = "";
		string		strObjectID = "";

		_vec3		vPos = { 0.f,0.f,0.f };
		_vec3		vRot = { 0.f,0.f,0.f };
		_vec3		vScale = { 1.f,1.f,1.f };
		_float		fPriority[EPRIORITY_OBJECT_END] = { 0.f, 0.f, 0.f };
		_bool		bUsePriority[EPRIORITY_OBJECT_END] = { true, true, true };
	};
	struct FLayerData
	{
		string						strName = "";
		_float						fPriority = 0.f;
		vector<FObjectData>		vecObject;
	};
	struct FTerrainData
	{
		string			strName = "";
		_vec3			vVertexCnt = { 0.f, 0.f, 0.f };
		_vec3			vScale = { 1.f, 1.f, 1.f };
		_vec3			vOffset = { 0.f, 0.f, 0.f };
	};
	struct FSceneData
	{
		string						strName = "";
		FTerrainData				tTerrain;
		vector<FLayerData>		vecLayer;
	};

private:
	void			Reset_Hierarchi()
	{
		m_iSelected_Layer = -1;
		m_iSelected_Layer_Remain = -1;
		m_iSelected_Object = -1;
		m_iSelected_Object_Remain = -1;
		m_eSelectedProperty_Type = ESELECTED_TYPE_NONE;
	}
	
private:
	// 목록에 있는 씬 모두 저장
	void			Save_SceneAll();
	// 선택 씬만 저장
	void			Save_Scene() {}
	void			Export_Scene(const FSerialize_Scene& tSceneSerial);
	// 전체 씬 로드
	void			Load_SceneAll();
	void			Import_Scene(const string& strName, FSerialize_Scene& tSceneSerial, FSceneData& tSceneData);

	// 오브젝트 파싱 관련
	void			Save_Object();
	void			Export_Object();
	void			Load_Object();
	void			Import_Object();

private:
	_bool						m_bScene_Init = true;

	string						m_strAdd_SceneName;				// 씬 추가하기
	_int						m_iSelected_Scene = -1;			// 선택된 씬
	_int						m_iLoaded_Scene = -1;			// 실제 로드된 씬
	_bool						m_bScene_Loaded = false;

	vector<FSceneData>			m_vecScene;					// 씬, 레이어, 오브젝트
	_int						m_iSelected_Layer = -1;
	_int						m_iSelected_Layer_Remain = -1;
	_int						m_iSelected_Object = -1;
	_int						m_iSelected_Object_Remain = -1;

	char						m_arrAddLayer_Buf[256] = "";
	_bool						m_bFocusedLayer_Edit = false;


private:			// 터레인 관련
	void			Apply_Terrain(const string& strTerrainName);
	void			Save_Terrain(const _int iSelected_Scene);
	void			Export_Terrain(const FSerialize_Terrain& tTerrain);
	void			Load_Terrain(const _int iSelected_Scene, const string& strName);
	void			Import_Terrain(const _int iSelected_Scene, const string& strName, FSerialize_Terrain& tTerrain);


private:	// 속성 관련
	_bool			m_bInput_Warning = false;


	


private:	// 유틸리티
	void			Set_Button_ActiveColor();
	void			Set_Button_NonActiveColor();
	void			Set_Button_ReturnColor();

	void			Clamp_Vec3(_vec3& vVec, _float fValue)
	{
		if (vVec.x < -fValue)
			vVec.x = -fValue;
		else if (vVec.x > fValue)
			vVec.x = fValue;

		if (vVec.y < -fValue)
			vVec.y = -fValue;
		else if (vVec.y > fValue)
			vVec.y = fValue;

		if (vVec.z < -fValue)
			vVec.z = -fValue;
		else if (vVec.z > fValue)
			vVec.z = fValue;
	}

	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventChar != 0 && strlen(data->Buf) >= 10)
			return 1;
		return 0;
	}
};

