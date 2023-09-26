#pragma once

#include "ImguiWin.h"
#include "MyTexture.h"

// 애니메이션 Info 구조체
typedef struct tagMyInfoAnimation
{
	_vec3	m_vInfo[INFO_END];

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
	std::wstring ConvertToWideString(const std::string& ansiString);
	//HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	//HRESULT	SaveData(const _tchar* mapTag);
	//void	SaveUI(wstring wstrFileName, wstring wstrFilePath);

	//CMyTexture* FindUI(const _tchar* szKey);

	//char를 wchar_t로 변경
	//void ConverCtoWC(char* str, wchar_t*& _cstr);

	//wstring을 char로 변경
	char* ConverWStringtoC(const wstring& wstr);

	//char를 wchar_t로 변경
	wchar_t* ConverCtoWC2(char* str);

	//wchar_t를 char로 변경
	char* ConverWCtoC(wchar_t* str);

	//유니코드 변환
	string WstringToUTF8(const wstring& wstr);

	// 이미지 로드 함수
	void LoadImg(const _tchar* folderPath);

	// 재귀 함수
	void Replace(wstring& strCurrentDirectory, wstring strSearch, wstring strReplace);

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

private: // 멤버 변수
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	const char* items[100]; // ListBox에 추가할 아이템 배열
	int itemIndex = 0;

	int selectedItemIndex = -1; // 선택된 항목 인덱스, 기본값 -1은 아무 항목도 선택되지 않음을 나타냅니다.

};
