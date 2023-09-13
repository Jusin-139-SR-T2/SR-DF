#pragma once

#include "Component.h"
//#include "Export_Utility.h" //뭐쓰는거 있을까 싶어서 
#include "Export_System.h" // dikeystate용

BEGIN(Engine)

class ENGINE_DLL CCollisionMgr : public CComponent
{
	DERIVED_CLASS(CComponent, CCollisionMgr)


private:
	explicit CCollisionMgr(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _pos, _float _rad, LPD3DXMESH _mesh);
	explicit CCollisionMgr(const CComponent& rhs);
	virtual ~CCollisionMgr();

public:
	virtual HRESULT		Ready_CollisionMgr(CGameObject* _Owner);
	virtual void		Render_Buffer();

	static CCollisionMgr* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _pos, _float _rad, LPD3DXMESH _mesh, CGameObject* _Owner);
	virtual CComponent* Clone();

protected:
	void CheckSphere();		// 구와 충돌하고있는지 계산하는 함수 
	void CheckBox();		// 박스와 충돌하고있는지 계산하는 함수 
	void CollisionSphere(); // 구와 충돌했다면 어떻게 해야할지 결정하는 함수 

public:
	LPD3DXMESH m_pMesh = NULL;
	_float m_frad;
	_vec3 m_vpos;

public:
	virtual void Free();

	enum MeshType { BOX, SPHERE, TORUS, MESHTYPE_END };
};

END