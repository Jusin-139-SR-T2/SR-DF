#include "CalculatorComponent.h"

CCalculatorComponent::CCalculatorComponent(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CCalculatorComponent::~CCalculatorComponent()
{
}

HRESULT CCalculatorComponent::Ready_Calculator()
{
	return S_OK;
}

CCalculatorComponent* CCalculatorComponent::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Calculator()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Calculator Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CCalculatorComponent::Clone()
{
	return new ThisClass(*this);
}

void CCalculatorComponent::Free()
{
	SUPER::Free();
}

_float CCalculatorComponent::Compute_HeightOnTerrain(const _vec3* pPos, const _vec3* pTerrainVtxPos, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv)
{
	// ������ ���̸� ����ϴ� �Լ�
	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fRatioX = _float(pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fRatioZ = _float(pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE	Plane;

	// ������ ��� �ﰢ��
	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	// ���� �ϴ� �ﰢ��
	else
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + 1],
			&pTerrainVtxPos[dwIndex]);
	}

	// ax + by + cz + d
	// by = -ax - cz - d
	// y = (-ax - cz - d) / b

	// ����� ���������� y�� ���� ��ȯ�Ѵ�.
	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

