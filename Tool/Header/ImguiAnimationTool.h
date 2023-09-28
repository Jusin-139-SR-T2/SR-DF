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
	wchar_t* ConverCtoWC2(char* str);

	//wchar_t를 char로 변경
	char* ConverWCtoC(wchar_t* str);

	//유니코드 변환
	string WstringToUTF8(const wstring& wstr);

	std::wstring ConvertToWideString(const std::string& ansiString);
#pragma endregion
	// 이미지 로드 함수
	void LoadImg(const _tchar* folderPath);

	// 재귀 함수 (재귀하면서 폴더 하나하나 들어가서 찾아줌)
	void Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace);

	//void RenderSelectedFrameTexture(const int selectedItemIndex);

public:
	// 오브젝트 설정 및 관리 함수
	void ObjectSetting();

	// // 데이터 로드 함수
	void LoadObjectInforamtionData();

	// 데이터 저장 함수
	void SaveObjectInformationData();

	// 파일명 제거 함수
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

	// 문자열 키값
	vector<wstring>	uniqueObj;
	vector<wstring>	uniqueState;

	// 애니메이션 정보를 담을 벡터
	vector<MYANIMATIONINFO>	m_vecAnimationInfo;

	// 파일을 담을 맵
	map<wstring, vector<wstring>> m_MapFile;

	// 텍스처를 담을 맵
	map<pair<wstring, wstring>, LPDIRECT3DTEXTURE9> m_MapTexture;


	//OBJ_TYPE m_eObjectType;
	//OBJ_NAME m_eObjName;

	OBJECTTYPE m_eObjName[RIGHT_OBJECT];

private: // 멤버 변수
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	const char* items[100]; // ListBox에 추가할 아이템 배열
	int itemIndex = 0;

	int	m_iObjectCreatIndex = -1;
	int selectedItemIndex = -1; // 선택된 항목 인덱스, 기본값 -1은 아무 항목도 선택되지 않음을 나타냅니다.

	// @@@@@@엔진의 멤버가 아니라그래서 주석걸어뒀음@@@@@@@
	//std::map<OBJECTKEY, Engine::CGameObject*> m_mapObject; // 객체를 저장하는 맵
	// @@@@@@엔진의 멤버가 아니라그래서 주석걸어뒀음@@@@@@@
};
