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
	// 지형의 높이를 계산하는 함수
	_ulong	dwIndex = _ulong(pPos->z / dwVtxItv) * dwCntX + _ulong(pPos->x / dwVtxItv);

	_float	fRatioX = _float(pPos->x - pTerrainVtxPos[dwIndex + dwCntX].x) / dwVtxItv;
	_float	fRatioZ = _float(pTerrainVtxPos[dwIndex + dwCntX].z - pPos->z) / dwVtxItv;

	D3DXPLANE	Plane;

	// 오른쪽 상단 삼각형
	if (fRatioX > fRatioZ)
	{
		D3DXPlaneFromPoints(&Plane,
			&pTerrainVtxPos[dwIndex + dwCntX],
			&pTerrainVtxPos[dwIndex + dwCntX + 1],
			&pTerrainVtxPos[dwIndex + 1]);
	}
	// 왼쪽 하단 삼각형
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

	// 평면의 방정식으로 y의 값을 반환한다.
	return (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
}

