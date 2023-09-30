#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

class CRcBufferComp;
class CTransformComponent;

/// <summary>
/// GameObject�� ���� �� �ִ� ������Ʈ
/// �ٷ� �Ʒ� ������ ���� ���̸� ��� �� �� �ִ� ������Ʈ
/// </summary>
class ENGINE_DLL CCalculatorComponent : public CGameObjComponent
{
	DERIVED_CLASS(CGameObjComponent, CCalculatorComponent)
private:
	explicit CCalculatorComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCalculatorComponent();

public:
	static CCalculatorComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();
private:
	virtual void Free();

public:
	HRESULT		Ready_Calculator();

	_float		Compute_HeightOnTerrain(const _vec3* pPos, const _vec3* pTerrainVtxPos,
										const _ulong& dwCntX = VTXCNTX, const _ulong& dwCntZ = VTXCNTZ,
										const _ulong& dwVtxItv = 1);

	_vec3		Picking_On_Object(HWND hWnd,
		CRcBufferComp* pRcBufferComp,
		CTransformComponent* pTransformComp);
};

END