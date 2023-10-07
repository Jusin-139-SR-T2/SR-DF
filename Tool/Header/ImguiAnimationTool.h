#pragma once

#include "ImguiWin.h"


enum class OBJ_TYPE // ������Ʈ�� Ÿ��
{ 
	NONE,
	TWO_HAND,
	TWO_OBJECT,
	RIGHT_OBJECT
};

enum class OBJ_NAME // ������Ʈ�� �̸�
{	
	NONE,
	GUN,
	THOMPSON,
	STEELPIPE,
	BEERBOTLE,
	FRYINGPAN
};

// ���� ���� ������Ʈ�� ���� (Enum)
enum OBJECTTYPE 
{
	NONE,
	TWO_HAND,
	TWO_OBJECT,
	RIGHT_OBJECT
};

// ��ü ���� Ű�� (Enum)
enum OBJECTKEY 
{
	KEY_NONE,
	KEY_GUN,
	KEY_THOMPSON,
	KEY_STEELPIPE,
	KEY_BEERBOTLE,
	KEY_FRYINGPAN
};

// ������ �Ӽ� �� ���
enum EDITMODE 
{
	EDITMODE_NONE,
	EDITMODE_SCALE,
	EDITMODE_ROTATION,
	EDITMODE_TRANSLATION,
	EDITMODE_TIME_VALUE,
	EDITMODE_TEXTURE
};

EDITMODE eEditMode = EDITMODE_NONE; // ������ ��� ����

// Ű������ ����ü
struct Keyframe 
{
	char name[64];			// Ű������ �̸� (ǥ�ÿ�)
	
	bool isEaseIn;			// Ease In ���� (True �Ǵ� False)
	bool isEaseOut;			// Ease Out ���� (True �Ǵ� False)

	float time;				// Ű�������� �ð� (0.0f ~ MaxTime ����)
	float value;			// �ִϸ��̼� �� (ũ��, ȸ��, �̵� ��)
	float color[3];			// Ű������ ���� (R, G, B)
	int texureframe;		// �ؽ�ó ���� ��

	OBJ_TYPE m_eObjectType; // Ÿ���� �ο��� �׸� (ex : �Ѽ�, ���)
	OBJ_NAME m_eObjectName; // �̸��� �ο��� �׸� (ex : ����, ��������)

	int type;				// �ִϸ��̼� Ÿ�� (0: ũ��, 1: ȸ��, 2: �̵�)
	
	_vec3	vScale = { 0.f, 0.f, 0.f };			// ũ�⸦ ���� �׸�
	_vec3	vRot = { 0.f, 0.f, 0.f };			// ȸ���� ���� �׸�
	_vec3	vPos = { 0.f, 0.f, 0.f };			// ��ġ�� ���� �׸�

	_vec2	vKeyFramePos = { 0.00000000f, 0.00000000f };		// �������� �ش� Ű������ ��ġ
};

// �ڵ� �ִϸ��̼� ������ �ּ� �� �ִ� ���� �Է� �޴� ����ü
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

// �ִϸ��̼� Info ����ü
typedef struct tagMyInfoAnimation
{
	_vec3	m_vScale;	// ũ�⸦ ���� �׸�
	_vec3	m_vRot;		// ȸ���� ���� �׸�
	_vec3	m_vPos;		// ��ġ�� ���� �׸�

	OBJ_TYPE m_eObjectType; // Ÿ���� �ο��� �׸� (ex : �Ѽ�, ���)
	OBJ_NAME m_eObjectName; // �̸��� �ο��� �׸� (ex : ����, ��������)

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
	// �̹����� �߰��ϴ� �Լ�
	void AddImage(const std::string& imagePath);

	// �ִϸ��̼� ���� ���� �Ѱ��ֱ�
	std::vector<Keyframe>* Get_Animation() { return &timeline; }

	//std::string	OpenImageFileDialog();
	//HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	//HRESULT	SaveData(const _tchar* mapTag);
	//void	SaveUI(wstring wstrFileName, wstring wstrFilePath);
	//CMyTexture* FindUI(const _tchar* szKey);
	//char�� wchar_t�� ����
	//void ConverCtoWC(char* str, wchar_t*& _cstr);

#pragma region ���ڿ� ��ȯ �Լ�
	//wstring�� char�� ����
	char* ConverWStringtoC(const wstring& wstr);

	//char�� wchar_t�� ����
	wchar_t* ConverCtoWC(char* str);

	//wchar_t�� char�� ����
	char* ConverWCtoC(wchar_t* str);

	//�����ڵ� ��ȯ
	string WstringToUTF8(const wstring& wstr);

	std::wstring ConvertToWideString(const std::string& ansiString);
	
	std::vector<const char*> ConvertStringVectorToCharArray(const std::vector<std::string>& stringVector);
#pragma endregion

	// �̹��� �ε� �Լ�
	void LoadImg(const _tchar* folderPath);

	// ��� �Լ� (����ϸ鼭 ���� �ϳ��ϳ� ���� ã����)
	void Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace);

	//void RenderSelectedFrameTexture(const int selectedItemIndex);
	
	void RenderTimeline(); // �ִϸ��̼� Ÿ�Ӷ���

	float EvaluateAnimationAtTime(float time); // �ִϸ��̼� �ð�

	// �ִϸ��̼��� ���۰� ���� �ε巴�� ǥ���ϱ� ���� �Լ�
	float ImEaseInQuad(float start, float end, float t);
	float ImEaseOutQuad(float start, float end, float t);

	// �̸����� �ִϸ��̼� ������ �Լ�
	void RenderPreviewAnimation(float value);

	// ���� ���� �Լ�
	float Lerp(float a, float b, float t);

	//
	void DrawSelectedKeyframeEditor(Keyframe& selectedKeyframe);

	_vec3 Lerp(const _vec3& a, const _vec3& b, float t);

	_float Lerp2(const _float& a, const _float& b, float t);

public: // �ִϸ��̼� �Լ�
	// ������Ʈ ���� �� ���� �Լ�
	void ObjectSetting();

	// // ������ �ε� �Լ�
	void LoadObjectInforamtionData();

	// ������ ���� �Լ�
	void SaveObjectInformationData();

	// ���ϸ� ���� �Լ�
	void PathRemoveFileSpec(TCHAR* path);

	// �ð��� ���� �ִϸ��̼� �ڵ� ���� �Լ�
	void EditKeyframesForAnimation(int numFrames);

	//test
	//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t);
	
	// ��� ��ư
	void ToggleButton(const char* str_id, bool* v);

	// �ִϸ��̼� ����
	void SaveAnimationToFile(const char* fileName);

	// �ִϸ��̼� �ҷ�����
	void LoadAnimationFromFile(const char* fileName);

	// Ű������ �ڵ� ���� �Լ�
	void CreateKeyframesWithLinearInterpolation(
		std::vector<Keyframe>& timeline, float minTime, float maxTime,
		_float minValue, _float maxValue,
		_vec3 minscaleValue, _vec3 maxscaleValue,
		_vec3 minrotationValue, _vec3 maxrotationValue,
		_vec3 mintranslationValue, _vec3 maxtranslationValue, int numKeyframes);

	// Ű�������� �����Ͽ� �ش� Ű������ ����
	void HandleKeyframeClick();

	// ����Ű ���� (Ű�Է�)
	void KeyInput();

	// ����Ű�� Ű�������� �Ӽ� �� ����
	void SelectKeyframeValueChange();

	// Ÿ�Ӷ����� Ű�������� ���콺�� �����Ͽ� �����ϴ� �Լ�
	void SelectKeyframeMouseL();

	// Ű������ ���
	void KeyframeList();

	// Ű������ ���� â
	void KeyframeChangeWindow();

	// Ű������ �̸����� ���� (����)
	void KeyframeAniV();

	// Ű������ ������ �� ���� (���� UI�� ���� ������)
	void KeyframeRender_ValueChange();

	// Ű������ �ڵ� ���� �Լ�
	void KeyframeAutomaticGeneration();

	// ��Ŭ������ Ű������ �����ϴ� �Լ�
	void KeyframeDeleteMouseR();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)
	GETSET_EX2(_bool, isPlaying, FramePlaying, GET, SET)
	GETSET_EX2(_float, currentTime, currentTime, GET, SET)
	GETSET_EX2(_float, bRepetition, Repetition, GET, SET)
		
protected:
	virtual void Free();
	
private:
	LPDIRECT3DTEXTURE9 m_pTexture = nullptr;
	LPDIRECT3DSURFACE9 m_pRenderTargetSurface = nullptr;

	//map<const _tchar*, CMyTexture*> m_mapTexture;

	MYANIMATIONINFO m_AniInfo;

	// ���ڿ� Ű��
	vector<wstring>	uniqueObj;
	vector<wstring>	uniqueState;

	// �ִϸ��̼� ������ ���� ����
	vector<MYANIMATIONINFO>	m_vecAnimationInfo;

	// �ִϸ��̼� ������ ���� ����
	vector<Keyframe>	m_vecAnimationKeyframe;

	// ������ ���� ��
	map<wstring, vector<wstring>> m_MapFile;

	// �ؽ�ó�� ���� ��
	map<pair<wstring, wstring>, LPDIRECT3DTEXTURE9> m_MapTexture;

	//map<OBJECTKEY, Engine::CGameObject*> m_mapObject;

	//OBJ_TYPE m_eObjectType;
	//OBJ_NAME m_eObjName;

	OBJECTTYPE m_eObjName[RIGHT_OBJECT];

private: // ��� ����
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	bool my_tool_active;
	float my_color[4];

	const char* items[100]; // ListBox�� �߰��� ������ �迭
	int itemIndex = 0;

	float g_animationTime = 0.f;
	int g_currentFrame = 0;

	int	m_iObjectCreatIndex = -1;
	int selectedItemIndex = -1; // ���õ� �׸� �ε���, �⺻�� -1�� �ƹ� �׸� ���õ��� ������ ��Ÿ���ϴ�.

	bool isMouseDragging = false; // ���콺 �巡�� ���� ����
	_vec2 mouseClickPos; // Ŭ���� ��ǥ ����

	bool	m_bTest;

private: // �ִϸ��̼� �� ����

	// �ִϸ��̼� Ÿ�� ����
	std::vector<Keyframe> timeline;

	// �ִϸ��̼� Ÿ�Ӷ���
	float currentTime = 0.0f; // ���� �ð� ��
	float MaxTime = 20.f;
	float fDisplayTime = 1.f;	// ���ݸ����� �ð� �� ǥ��
	_bool isDraggingTimeline = false;
	_bool isPlaying = false;		// ��� ����
	float playbackSpeed = 1.0f; // ��� �ӵ�

#pragma region �ּ�, �ִ� ��
	// ũ��
	float	fMin_Scale = -1200.1f;	// �ּ�
	float	fMax_Scale = 1200.f;	// �ִ�

	// ȸ��
	float	fMin_Rot = -180.f;		// �ּ�
	float	fMax_Rot = 180.f;	// �ִ�

	// �̵�
	float	fMin_Pos = -1000.f;		// �ּ�
	float	fMax_Pos = 1000.f;	// �ִ�

	// �ð�
	float	fMin_Time = 0.f;	// �ּ�
	float	fMax_Time = MaxTime;// �ִ�

	// ����
	float	fMin_Value = 0.f;	// �ּ�
	float	fMax_Value = 1.f;	// �ִ�
#pragma endregion

#pragma region �ּ�, �ִ� �ڵ� ���� ��

	_vec2 _v2Time = { 0.f, 0.f };
	_vec2 _v2Value = { 0.f, 0.f };
	_vec3 minScale = { 0.f, 0.f, 0.f };
	_vec3 maxScale = { 0.f, 0.f, 0.f };
	_vec3 minRot = { 0.f, 0.f, 0.f };
	_vec3 maxRot = { 0.f, 0.f, 0.f };
	_vec3 minPos = { 0.f, 0.f, 0.f };
	_vec3 maxPos = { 0.f, 0.f, 0.f };

	_float numKeyframes = 0.f;
#pragma endregion

#pragma region �ִϸ��̼� Ÿ�� ����

	// Ű������ ���� ���� �Լ�
	float oldKeyframeIndex = 0.f;

#pragma endregion

	float oldTime = 0.f;	// ���� �ð� �� ����
	float newTime = 0.f;	// ���ο� �ð� �� (���ϴ� ������ ����)
	float originalIndex;

	float vValueSize = 1.f;	// ������ �� ���� ũ��
	float fValueCurve = 0.1f; // ������ �� ���� �����ϴ� ����


	int closestKeyframeIndex = -1; // # ���� ���콺�� ������ Ű������ �ε���

	// ������ Ű������ �ε����� �����ϴ� ���� (������ Ű������ �ε����� �ʱ�ȭ�մϴ�.)
	int selectedKeyframeIndex = -1;

	// Ű������ �巡��
	bool	isDraggingKeyframe = false;
	int		draggingKeyframeIndex = 0;

	// �ִϸ��̼� ���
	int animationFrame = 0;
	float playbackTime = 0.f;

	float initialKeyframeX = 0.f; // ���콺 �巡�� ���� ��ġ

	// �ִϸ��̼� �ݺ� ����
	int bRepetition = 1;

	// �ִϸ��̼� ���� ���ϸ�
	char cFileSaveName[128] = "Animation";

	// �ִϸ��̼� �ҷ����� ���ϸ�
	char cFileLoadName[128] = "Animation";

	// ������ �׷��� ������ ���� �����ϱ� ���� ����
	_vec2 lastRedCirclePos;

	// ==============���� ���==============
	// �̹��� ��� ����� �����ϴ� ����
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
	int selectedPathIndex = 0; // ���õ� �̹��� ��� �ε���

	float fScaleSpeed = 0.1f; // ũ�� ���� �ӵ� ����
};

#pragma region ����Ű

// "A" Ű : ���� ���콺 ��ġ�� Ű�������� ����
// "R" Ű : ���� �ð�(�ð���)�� 0���� �ʱ�ȭ [ó������ �̵�]
// "T" Ű : ���� �ð�(�ð� ��)�� ���콺 ��ġ�� ����
// "Space" Ű : �ִϸ��̼� ���/����
// "0" Ű : ��� �ӵ� �ʱ�ȭ
// "-" Ű : ��� �ӵ� -
// "+" Ű : ��� �ӵ� +

#pragma endrigion