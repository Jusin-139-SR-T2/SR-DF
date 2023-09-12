#pragma once

#include "ImguiWin.h"

class CImguiWin_Test : public CImguiWin
{
	DERIVED_CLASS(CImguiWin, CImguiWin_Test)

private:
	explicit CImguiWin_Test();
	virtual ~CImguiWin_Test();

public:
	static CImguiWin_Test* Create();

protected:
	virtual void Free();

public:
	virtual HRESULT Ready_ImguiWin() override;
	virtual _int	Update_ImgWin(const _float& fTimeDelta) override;
};

