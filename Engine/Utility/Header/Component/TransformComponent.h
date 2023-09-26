#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

/// <summary>
/// 트랜스폼 컴포넌트는 게임오브젝트에 추가되어 오브젝트의 위치정보를 저장하는 컴포넌트이다.
/// SceneComponent에서도 트랜스폼이 제공되기는 하지만 이는 그 컴포넌트를 위한 좌표값을 가지는 것이며.
/// 이 클래스와는 상관이 없다.
/// </summary>
class ENGINE_DLL CTransformComponent : public CGameObjComponent
{
	DERIVED_CLASS(CGameObjComponent, CTransformComponent)

protected:
	explicit CTransformComponent();
	explicit CTransformComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransformComponent(const CTransformComponent& rhs);
	virtual ~CTransformComponent();

public:
	static CTransformComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void		Free();

public:
	HRESULT			Ready_Transform();
	virtual _int	Update_Component(const _float& fTimeDelta);

	void			Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed);
	const _matrix*	Compute_LootAtTarget(const _vec3* pTargetPos);

public:
	// 좌표 변경함수
	void		Move_Pos(const _vec3* const pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}

	// 회전 변경 함수 (축 1개만 가능)
	void		Rotate(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vRotation) + eType) += fAngle;
	}

	// 벡터 복사
	void			Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matTransform.m[eType][0], sizeof(_vec3));
	}

	void		Set_WorldMatrixS(const _matrix* pWorld) { m_matTransform = *pWorld; }

	// Set_Pos
	void		Set_Pos(const _float& fX, const _float& fY, const _float& fZ)
	{
		m_vInfo[INFO_POS] = { fX, fY, fZ };
	}

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX2(_vec3, m_vInfo[INFO_RIGHT], Right, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_UP], Up, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_LOOK], Look, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS], Pos, GET_C_REF, SET_C)
	void Set_PosX(const _float value) { m_vInfo[INFO_POS].x = value; }
	void Set_PosY(const _float value) { m_vInfo[INFO_POS].y = value; }
	void Set_PosZ(const _float value) { m_vInfo[INFO_POS].z = value; }


	GETSET_EX2(_vec3, m_vRotation, Rotation, GET_C_REF, SET_C)
	void Set_RotationX(const _float value) { m_vRotation.x = value; }
	void Set_RotationY(const _float value) { m_vRotation.y = value; }
	void Set_RotationZ(const _float value) { m_vRotation.z = value; }

	GETSET_EX2(_vec3, m_vScale, Scale, GET_C_REF, SET_C)
	void Set_ScaleX(const _float value) { m_vScale.x = value; }
	void Set_ScaleY(const _float value) { m_vScale.y = value; }
	void Set_ScaleZ(const _float value) { m_vScale.z = value; }

	GETSET_EX2(_matrix, m_matTransform, Transform, GET_C_PTR, SET_C_PTR)

protected:
	_vec3		m_vInfo[INFO_END];		// 위치, 방향 정보
	_vec3		m_vRotation;			// 오일러 회전축
	_vec3		m_vScale;				// 크기
	_matrix		m_matTransform;			// 월드 행렬

};

END