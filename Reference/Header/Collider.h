#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPEWEAPON_END };

public:
	CCollider(LPDIRECT3DDEVICE9* pDevice);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;


public:
	_bool Collsion_AABB(CCollider* pTargetCollider);
	_bool Collsion_OBB(CCollider* pTargetCollider);
	_vec3 Intersect_Ray();

public:
	void Set_isCollison(_bool flag) { m_isCollision = flag; }

private:
	TYPE						m_eType = TYPEWEAPON_END;
	_bool						m_isCollision = false;

private: // 계산함수 
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	_vec3 Compute_Min();
	_vec3 Compute_Max();
	OBBDESC Compute_OBB();

public:
	// CComponent을(를) 통해 상속됨
	static CCollider* Create(LPDIRECT3DDEVICE9* pDevice, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	CComponent* Clone() override;
};

END