#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

/// <summary>
/// ActorComponent에서 좌표계를 쓸 수 있도록 기능이 추가된 컴포넌트이다.
/// TransformComponent와는 별개
/// </summary>
class ENGINE_DLL CSceneComponent abstract : public CGameObjComponent
{
	DERIVED_CLASS(CGameObjComponent, CSceneComponent)

protected:
	explicit CSceneComponent();
	explicit CSceneComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSceneComponent(const CSceneComponent& rhs);
	virtual ~CSceneComponent();

public:
	virtual CComponent*		Clone() PURE;
	virtual void			Free();

public:
	virtual HRESULT	Ready_Component() PURE;
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() PURE;
	virtual void	Render_Component() PURE;

public:
#pragma region 트랜스폼 변경 함수들
	// 로컬 행렬을 재조정한다
	void Readjust_Transform()
	{

	}

	// 현재 행렬을 실제 월드 행렬로 변환
	void Set_TransformToWorld(const _matrix& matBaseTransform)
	{
		m_matTransform *= matBaseTransform;
	}
	
#pragma endregion

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX1(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_UP],		Up,		GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_LOOK],	Look,	GET_C_REF)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET_C_REF, SET_C)

	GETSET_EX2(_vec3,	m_vScale,			Scale,			GET_C_REF, SET_C)
	GETSET_EX2(_vec3,	m_vAngle,			Angle,			GET_C_REF, SET_C)
	GETSET_EX2(_matrix, m_matWorld,			WorldMatrix,	GET_C_REF, SET_C)

protected:
	_vec3		m_vInfo[INFO_END];		// 위치, 방향 정보
	_vec3		m_vAngle;				// 오일러 회전축
	_vec3		m_vScale;				// 크기
	_matrix		m_matTransform;			// 월드 행렬
};

END