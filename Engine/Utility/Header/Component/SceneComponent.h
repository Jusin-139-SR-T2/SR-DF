#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

/// <summary>
/// ActorComponent���� ��ǥ�踦 �� �� �ֵ��� ����� �߰��� ������Ʈ�̴�.
/// TransformComponent�ʹ� ����
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
#pragma region Ʈ������ ���� �Լ���
	// ���� ����� �������Ѵ�
	void Readjust_Transform()
	{

	}

	// ���� ����� ���� ���� ��ķ� ��ȯ
	void Set_TransformToWorld(const _matrix& matBaseTransform)
	{
		m_matTransform *= matBaseTransform;
	}
	
#pragma endregion

public:		// Ʈ������ ����, Transform���� �Űܿ� ����
	GETSET_EX1(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_UP],		Up,		GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_LOOK],	Look,	GET_C_REF)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET_C_REF, SET_C)

	GETSET_EX2(_vec3,	m_vScale,			Scale,			GET_C_REF, SET_C)
	GETSET_EX2(_vec3,	m_vAngle,			Angle,			GET_C_REF, SET_C)
	GETSET_EX2(_matrix, m_matWorld,			WorldMatrix,	GET_C_REF, SET_C)

protected:
	_vec3		m_vInfo[INFO_END];		// ��ġ, ���� ����
	_vec3		m_vAngle;				// ���Ϸ� ȸ����
	_vec3		m_vScale;				// ũ��
	_matrix		m_matTransform;			// ���� ���
};

END