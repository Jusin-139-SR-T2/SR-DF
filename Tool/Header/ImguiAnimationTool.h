#pragma once

#include "ImguiWin.h"
#include "MyTexture.h"

enum class OBJ_TYPE // 오브젝트의 타입
{ 
	NONE,
	TWO_HAND,
	TWO_OBJECT,
	RIGHT_OBJECT
};

enum class OBJ_NAME // 오브젝트의 이름
{	
	NONE,
	GUN,
	THOMPSON,
	STEELPIPE,
	BEERBOTLE,
	FRYINGPAN
};

// 게임 로직 오브젝트의 유형 (Enum)
enum OBJECTTYPE 
{
	NONE,
	TWO_HAND,
	TWO_OBJECT,
	RIGHT_OBJECT
};

// 객체 유형 키값 (Enum)
enum OBJECTKEY 
{
	KEY_NONE,
	KEY_GUN,
	KEY_THOMPSON,
	KEY_STEELPIPE,
	KEY_BEERBOTLE,
	KEY_FRYINGPAN
};

// 키프레임 구조체
struct Keyframe 
{
	char name[64];			// 키프레임 이름 (표시용)
	
	bool isEaseIn;			// Ease In 설정 (True 또는 False)
	bool isEaseOut;			// Ease Out 설정 (True 또는 False)

	float time;				// 키프레임의 시간 (0.0f ~ MaxTime 범위)
	float value;			// 애니메이션 값 (크기, 회전, 이동 등)
	float color[3];			// 키프레임 색상 (R, G, B)

	OBJ_TYPE m_eObjectType; // 타입을 부여할 그릇 (ex : 한손, 양손)
	OBJ_NAME m_eObjectName; // 이름을 부여할 그릇 (ex : 권총, 쇠파이프)

	int type;				// 애니메이션 타입 (0: 크기, 1: 회전, 2: 이동)
	
	_vec3	vScale = { 0.f, 0.f, 0.f };			// 크기를 담을 그릇
	_vec3	vRot = { 0.f, 0.f, 0.f };			// 회전을 담을 그릇
	_vec3	vPos = { 0.f, 0.f, 0.f };			// 위치를 담을 그릇
};

// 자동 애니메이션 생성시 최소 및 최대 값을 입력 받는 구조체
struct AnimationProperties {
	float minTime = 0.0f;
	float maxTime = 20.f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	_vec3 minScale = { 0.1f, 0.1f, 0.f };
	_vec3 maxScale = { 5.f, 5.f, 5.f };
	_vec3 minRotation = { 0.f, 0.f, 0.f };
	_vec3 maxRotation = { 360.f, 360.f, 360.f };
	_vec3 minPosition = { 0.f, 0.f, 0.f };
	_vec3 maxPosition = { 300.f, 300.f, 300.f };
};

// 애니메이션 Info 구조체
typedef struct tagMyInfoAnimation
{
	_vec3	m_vScale;	// 크기를 담을 그릇
	_vec3	m_vRot;		// 회전을 담을 그릇
	_vec3	m_vPos;		// 위치를 담을 그릇

	OBJ_TYPE m_eObjectType; // 타입을 부여할 그릇 (ex : 한손, 양손)
	OBJ_NAME m_eObjectName; // 이름을 부여할 그릇 (ex : 권총, 쇠파이프)

}MYANIMATIONINFO;

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

	// 애니메이션 정보 벡터 넘겨주기
	std::vector<Keyframe>* Get_Animation() { return &timeline; }

	//std::string	OpenImageFileDialog();
	//HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	//HRESULT	SaveData(const _tchar* mapTag);
	//void	SaveUI(wstring wstrFileName, wstring wstrFilePath);
	//CMyTexture* FindUI(const _tchar* szKey);
	//char를 wchar_t로 변경
	//void ConverCtoWC(char* str, wchar_t*& _cstr);

#pragma region 문자열 변환 함수
	//wstring을 char로 변경
	char* ConverWStringtoC(const wstring& wstr);

	//char를 wchar_t로 변경
	wchar_t* ConverCtoWC(char* str);

	//wchar_t를 char로 변경
	char* ConverWCtoC(wchar_t* str);

	//유니코드 변환
	string WstringToUTF8(const wstring& wstr);

	std::wstring ConvertToWideString(const std::string& ansiString);
	
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
#pragma endregion

	// 이미지 로드 함수
	void LoadImg(const _tchar* folderPath);

	// 재귀 함수 (재귀하면서 폴더 하나하나 들어가서 찾아줌)
	void Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace);

	//void RenderSelectedFrameTexture(const int selectedItemIndex);
	
	void RenderTimeline(); // 애니메이션 타임라인

	float EvaluateAnimationAtTime(float time); // 애니메이션 시간

	// 애니메이션의 시작과 끝을 부드럽게 표현하기 위한 함수
	float ImEaseInQuad(float start, float end, float t);
	float ImEaseOutQuad(float start, float end, float t);

	// 미리보기 애니메이션 렌더링 함수
	void RenderPreviewAnimation(float value);

	// 선형 보간 함수
	float Lerp(float a, float b, float t);

	//
	void DrawSelectedKeyframeEditor(Keyframe& selectedKeyframe);

public: // 애니메이션 함수
	// 오브젝트 설정 및 관리 함수
	void ObjectSetting();

	// // 데이터 로드 함수
	void LoadObjectInforamtionData();

	// 데이터 저장 함수
	void SaveObjectInformationData();

	// 파일명 제거 함수
	void PathRemoveFileSpec(TCHAR* path);

	// 시간에 따른 애니메이션 자동 생성 함수
	void EditKeyframesForAnimation(int numFrames);

	//test
	//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t);
	
	// 토글 버튼
	void ToggleButton(const char* str_id, bool* v);

	// 애니메이션 저장
	void SaveAnimationToFile(const char* fileName);

	// 애니메이션 불러오기
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)
	GETSET_EX2(_bool, isPlaying, FramePlaying, GET, SET)
	GETSET_EX2(_float, currentTime, currentTime, GET, SET)
		
protected:
	virtual void Free();
	
private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface = nullptr;

	//map<const _tchar*, CMyTexture*> m_mapTexture;

	MYANIMATIONINFO m_AniInfo;

	// 문자열 키값
	vector<wstring>	uniqueObj;
	vector<wstring>	uniqueState;

	// 애니메이션 정보를 담을 벡터
	vector<MYANIMATIONINFO>	m_vecAnimationInfo;

	// 애니메이션 정보를 담을 벡터
	vector<Keyframe>	m_vecAnimationKeyframe;

	// 파일을 담을 맵
	map<wstring, vector<wstring>> m_MapFile;

	// 텍스처를 담을 맵
	map<pair<wstring, wstring>, LPDIRECT3DTEXTURE9> m_MapTexture;

	//map<OBJECTKEY, Engine::CGameObject*> m_mapObject;

	//OBJ_TYPE m_eObjectType;
	//OBJ_NAME m_eObjName;

	OBJECTTYPE m_eObjName[RIGHT_OBJECT];

private: // 멤버 변수
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	bool my_tool_active;
	float my_color[4];

	const char* items[100]; // ListBox에 추가할 아이템 배열
	int itemIndex = 0;

	float g_animationTime = 0.f;
	int g_currentFrame = 0;

	int	m_iObjectCreatIndex = -1;
	int selectedItemIndex = -1; // 선택된 항목 인덱스, 기본값 -1은 아무 항목도 선택되지 않음을 나타냅니다.

	bool isMouseDragging = false; // 마우스 드래그 상태 저장
	_vec2 mouseClickPos; // 클릭된 좌표 저장

	bool	m_bTest;

private: // 애니메이션 툴 변수

	// 애니메이션 타임 라인
	std::vector<Keyframe> timeline;

	// 애니메이션 타임라인
	float currentTime = 0.0f; // 현재 시간 값
	float MaxTime = 20.f;
	float fDisplayTime = 1.f;	// 눈금마다의 시간 값 표시
	_bool isDraggingTimeline = false;
	_bool isPlaying = false;		// 재생 여부
	float playbackSpeed = 1.0f; // 재생 속도

#pragma region 최소, 최대 값
	// 크기
	float	fMin_Scale = 0.1f;	// 최소
	float	fMax_Scale = 800.f;	// 최대

	// 회전
	float	fMin_Rot = 0.f;		// 최소
	float	fMax_Rot = 360.f;	// 최대

	// 이동
	float	fMin_Pos = 0.f;		// 최소
	float	fMax_Pos = 300.f;	// 최대

	// 시간
	float	fMin_Time = 0.f;	// 최소
	float	fMax_Time = MaxTime;// 최대

	// 벨류
	float	fMin_Value = 0.f;	// 최소
	float	fMax_Value = 1.f;	// 최대
#pragma endregion
	// 선택한 키프레임 인덱스를 저장하는 변수 (선택한 키프렘의 인덱스를 초기화합니다.)
	int selectedKeyframeIndex = -1;

	// 키프레임 드래그
	bool	isDraggingKeyframe = false;
	int		draggingKeyframeIndex = 0;

	// 애니메이션 재생
	int animationFrame = 0;
	float playbackTime = 0.f;

	float initialKeyframeX = 0.f; // 마우스 드래그 시작 위치

	// 이전에 그려진 빨간색 원을 저장하기 위한 변수
	_vec2 lastRedCirclePos;

	// ==============폴더 경로==============
	// 이미지 경로 목록을 저장하는 벡터
	std::vector<std::string> imagePaths = 
	{
		"../Client/Resource/Texture/Player",
		"../Client/Resource/Texture/Monster",
		"../Client/Resource/Texture/Item",
		"../Client/Resource/Texture/BUTTON",
		"../Client/Resource/Texture/crosshair",
		"../Client/Resource/Texture/cursor",
		"../Client/Resource/Texture/effect",
		"../Client/Resource/Texture/Explosion",
		"../Client/Resource/Texture/Font",
		"../Client/Resource/Texture/idk",
		"../Client/Resource/Texture/LIGHT",
		"../Client/Resource/Texture/Lights",
		"../Client/Resource/Texture/MAP",
		"../Client/Resource/Texture/number",
		"../Client/Resource/Texture/Scene",
		"../Client/Resource/Texture/SkyBox",
		"../Client/Resource/Texture/Terrain",
		"../Client/Resource/Texture/Tile",
		"../Client/Resource/Texture/UI",
		"../Client/Resource/Texture/Weapon",
		"../Client/Resource/Texture/Woman"
	};

	std::string m_cPath[5] = {
		"Apple",
		"Banana",
		"Cherry",
		"Date"
	};
	int selectedPathIndex = 0; // 선택된 이미지 경로 인덱스

	// @@@@@@엔진의 멤버가 아니라그래서 주석걸어뒀음@@@@@@@
	//std::map<OBJECTKEY, Engine::CGameObject*> m_mapObject; // 객체를 저장하는 맵
	// @@@@@@엔진의 멤버가 아니라그래서 주석걸어뒀음@@@@@@@
};
