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

public:
	// ������Ʈ ���� �� ���� �Լ�
	void ObjectSetting();

	// // ������ �ε� �Լ�
	void LoadObjectInforamtionData();

	// ������ ���� �Լ�
	void SaveObjectInformationData();

	// ���ϸ� ���� �Լ�
	void PathRemoveFileSpec(TCHAR* path);

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


	//OBJ_TYPE m_eObjectType;
	//OBJ_NAME m_eObjName;

	OBJECTTYPE m_eObjName[RIGHT_OBJECT];

private: // ��� ����
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	const char* items[100]; // ListBox�� �߰��� ������ �迭
	int itemIndex = 0;

	int	m_iObjectCreatIndex = -1;
	int selectedItemIndex = -1; // ���õ� �׸� �ε���, �⺻�� -1�� �ƹ� �׸� ���õ��� ������ ��Ÿ���ϴ�.

	// @@@@@@������ ����� �ƴ϶�׷��� �ּ��ɾ����@@@@@@@
	//std::map<OBJECTKEY, Engine::CGameObject*> m_mapObject; // ��ü�� �����ϴ� ��
	// @@@@@@������ ����� �ƴ϶�׷��� �ּ��ɾ����@@@@@@@
};
