#pragma once

#include "ImguiWin.h"
#include "MyTexture.h"

// �ִϸ��̼� Info ����ü
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
	// �̹����� �߰��ϴ� �Լ�
	void AddImage(const std::string& imagePath);
	//std::string	OpenImageFileDialog();
	std::wstring ConvertToWideString(const std::string& ansiString);
	//HRESULT OpenImageFileDialog(const _tchar* folderPath, LPDIRECT3DDEVICE9 pGraphicDev);
	//HRESULT	SaveData(const _tchar* mapTag);
	//void	SaveUI(wstring wstrFileName, wstring wstrFilePath);

	//CMyTexture* FindUI(const _tchar* szKey);

	//char�� wchar_t�� ����
	//void ConverCtoWC(char* str, wchar_t*& _cstr);

	//wstring�� char�� ����
	char* ConverWStringtoC(const wstring& wstr);

	//char�� wchar_t�� ����
	wchar_t* ConverCtoWC2(char* str);

	//wchar_t�� char�� ����
	char* ConverWCtoC(wchar_t* str);

	//�����ڵ� ��ȯ
	string WstringToUTF8(const wstring& wstr);

	// �̹��� �ε� �Լ�
	void LoadImg(const _tchar* folderPath);

	// ��� �Լ�
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

	// ���ڿ� Ű��
	vector<wstring>	uniqueObj;
	vector<wstring>	uniqueState;

	// �ִϸ��̼� ������ ���� ����
	vector<MYANIMATIONINFO>	m_vecAnimationInfo;

	// ������ ���� ��
	map<wstring, vector<wstring>> m_MapFile;

	// �ؽ�ó�� ���� ��
	map<pair<wstring, wstring>, LPDIRECT3DTEXTURE9> m_MapTexture;

private: // ��� ����
	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fZ = 0.0f;

	const char* items[100]; // ListBox�� �߰��� ������ �迭
	int itemIndex = 0;

	int selectedItemIndex = -1; // ���õ� �׸� �ε���, �⺻�� -1�� �ƹ� �׸� ���õ��� ������ ��Ÿ���ϴ�.

};
