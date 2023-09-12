#pragma once

#include "Base.h"
#include "Engine_Define.h"

/// <summary>
/// IMGUI�� ������� ����� ������ ������ Ŭ����
/// </summary>
class CImguiWin : public CBase
{
	DERIVED_CLASS(CBase, CImguiWin)

public:
	explicit CImguiWin();
	virtual ~CImguiWin();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin();
	virtual _int	Update_ImgWin(const _float& fTimeDelta);
	// IMGUI�� �ڵ����� ������ ������ ������ ���� �Լ��� �ʿ����.
};

