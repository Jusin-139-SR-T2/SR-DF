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
		D3DXMatrixIdentity(&m_matTransform);

		// 3x3��ŭ ���� ��ķκ��� vInfo�� ����
		for (_int i = 0; i < INFO_POS; ++i)
			memcpy(&m_vInfo[i], &m_matTransform.m[i][0], sizeof(_vec3));

		// ũ�� ��ȯ
		for (_int i = 0; i < INFO_POS; ++i)
		{
			D3DXVec3Normalize(&m_vInfo[i], &m_vInfo[i]);
			m_vInfo[i] *= *(((_float*)&m_vScale) + i);
		}

		// ȸ�� ��ȯ
		_matrix		matRot[ROT_END];

		D3DXMatrixRotationX(&matRot[ROT_X], m_vRotation.x);
		D3DXMatrixRotationY(&matRot[ROT_Y], m_vRotation.y);
		D3DXMatrixRotationZ(&matRot[ROT_Z], m_vRotation.z);

		for (_int i = 0; i < INFO_POS; ++i)
		{
			for (_int j = 0; j < ROT_END; ++j)
			{
				D3DXVec3TransformNormal(&m_vInfo[i], &m_vInfo[i], &matRot[j]);
			}
		}

		// ���� ��� ����
		for (_int i = 0; i < INFO_END; ++i)
			memcpy(&m_matTransform.m[i][0], &m_vInfo[i], sizeof(_vec3));
	}

	// ���� ����� ���� ���� ��ķ� ��ȯ
	void Set_TransformToWorld(const _matrix& matBaseTransform)
	{
		m_matTransform *= matBaseTransform;
	}

	// ���� ����� �������ϰ� ���� ��ķ� ��ȯ�Ͽ� ��ȯ
	void Get_CalculatedWorldTransform(const _matrix& matBaseTransform)
	{
		Readjust_Transform();
		Set_TransformToWorld(matBaseTransform);
	}
	
#pragma endregion

public:		// Ʈ������ ����, Transform���� �Űܿ� ����
	GETSET_EX1(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_UP],		Up,		GET_C_REF)
	GETSET_EX1(_vec3, m_vInfo[INFO_LOOK],	Look,	GET_C_REF)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET_C_REF, SET_C)


	GETSET_EX2(_vec3,	m_vRotation,		Rotation,		GET_C_REF, SET_C)
	GETSET_EX2(_vec3,	m_vScale,			Scale,			GET_C_REF, SET_C)
	GETSET_EX2(_matrix, m_matTransform,		Transform,		GET_C_REF, SET_C)

protected:
	_vec3		m_vInfo[INFO_END];		// ��ġ, ���� ����
	_vec3		m_vRotation;			// ���Ϸ� ȸ����
	_vec3		m_vScale;				// ũ��
	_matrix		m_matTransform;			// ���� ���
};

END