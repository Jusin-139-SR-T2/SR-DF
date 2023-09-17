#pragma once

#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class FPHY_Shape;

/// <summary>
/// 충돌체 처리 클래스
/// </summary>
class ENGINE_DLL CPhysicsMgr : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CPhysicsMgr)

private:
	explicit CPhysicsMgr();
	virtual ~CPhysicsMgr();

private:
	virtual		void	Free();

public:
	HRESULT				Ready_Physics();
	_int				Update_Physics();

public:
	void				Add_Shape(FPHY_Shape* pShape);

private:
	list<FPHY_Shape*>			m_ShapeList;		// 충돌체 리스트
};

END