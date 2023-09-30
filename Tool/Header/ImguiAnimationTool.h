#pragma once

#include "ImguiWin.h"
#include "MyTexture.h"

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

// Ű������ ����ü
struct Keyframe 
{
	char name[64];			// Ű������ �̸� (ǥ�ÿ�)

	float time;				// Ű�������� �ð� (0.0f ~ 1.0f ����)
	float value;			// �ִϸ��̼� �� (ũ��, ȸ��, �̵� ��)
	float color[3];			// Ű������ ���� (R, G, B)
	float size;				// ũ�� �ִϸ��̼ǿ� �ʿ��� ���
	float rotation;			// ȸ�� �ִϸ��̼ǿ� �ʿ��� ���
	float translation[2];	// �̵� �ִϸ��̼ǿ� �ʿ��� ���

	bool isEaseIn;			// Ease In ���� (True �Ǵ� False)
	bool isEaseOut;			// Ease Out ���� (True �Ǵ� False)

	int type;				// �ִϸ��̼� Ÿ�� (0: ũ��, 1: ȸ��, 2: �̵�)
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
	wchar_t* ConverCtoWC2(char* str);

	//wchar_t�� char�� ����
	char* ConverWCtoC(wchar_t* str);

	//�����ڵ� ��ȯ
	string WstringToUTF8(const wstring& wstr);

	std::wstring ConvertToWideString(const std::string& ansiString);
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

public:
	// ������Ʈ ���� �� ���� �Լ�
	void ObjectSetting();

	// // ������ �ε� �Լ�
	void LoadObjectInforamtionData();

	// ������ ���� �Լ�
	void SaveObjectInformationData();

	// ���ϸ� ���� �Լ�
	void PathRemoveFileSpec(TCHAR* path);


	//test
	//ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t);
	
	// ��� ��ư
	void ToggleButton(const char* str_id, bool* v);

	// �ִϸ��̼� ����
	void SaveAnimationToFile(const char* fileName);

	// �ִϸ��̼� �ҷ�����
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImguiWin(const _float& fTimeDelta) override;

public:
	GETSET_EX1(LPDIRECT3DTEXTURE9, m_pTexture, Texture, GET_REF)

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

	bool	m_bTest;

	// �ִϸ��̼� Ÿ�� ����
	std::vector<Keyframe> timeline;

	// �ִϸ��̼� Ÿ�Ӷ���
	float currentTime = 0.0f;
	bool isDraggingTimeline = false;
	bool isPlaying = false;
	float playbackSpeed = 1.0f;

	// Ű������ �巡��
	bool	isDraggingKeyframe = false;
	int		draggingKeyframeIndex = 0;

	// �ִϸ��̼� ���
	int animationFrame = 0;
	float playbackTime = 0.f;

	// @@@@@@������ ����� �ƴ϶�׷��� �ּ��ɾ����@@@@@@@
	//std::map<OBJECTKEY, Engine::CGameObject*> m_mapObject; // ��ü�� �����ϴ� ��
	// @@@@@@������ ����� �ƴ϶�׷��� �ּ��ɾ����@@@@@@@
};
