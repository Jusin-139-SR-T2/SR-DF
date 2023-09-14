#pragma once

#include "Component.h"
//#include "Export_Utility.h" //�����°� ������ �; 
#include "Export_System.h" // dikeystate��

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
	void CheckSphere();		// ���� �浹�ϰ��ִ��� ����ϴ� �Լ� 
	void CheckBox();		// �ڽ��� �浹�ϰ��ִ��� ����ϴ� �Լ� 
	void CollisionSphere(); // ���� �浹�ߴٸ� ��� �ؾ����� �����ϴ� �Լ� 

public:
	LPD3DXMESH m_pMesh = NULL;
	_float m_frad;
	_vec3 m_vpos;

public:
	virtual void Free();

	enum MeshType { BOX, SPHERE, TORUS, MESHTYPE_END };
};

END