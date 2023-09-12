#pragma once

#include "Base.h"
#include "Engine_Define.h"

/// <summary>
/// IMGUI를 기반으로 설계된 윈도우 단위의 클래스
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
	// IMGUI는 자동으로 렌더를 돌리기 때문에 따로 함수가 필요없다.
};

