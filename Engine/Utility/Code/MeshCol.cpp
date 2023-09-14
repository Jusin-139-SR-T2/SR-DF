#include "MeshCol.h"

CMeshCol::CMeshCol()
{
}

CMeshCol::CMeshCol(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMeshCol::CMeshCol(const CMeshCol& rhs)
	: Base(rhs)
{
}

CMeshCol::~CMeshCol()
{
}

void CMeshCol::BoxMesh_Ready(LPDIRECT3DDEVICE9 pDevice, FLOAT _Width, FLOAT _Height, FLOAT Depth, LPD3DXMESH* ppMesh)
{
	D3DXCreateBox(pDevice, Width, Height, Depth, ppMesh, NULL);
	m_Mesh = ppMesh;
}

void CMeshCol::Sphere_Ready(LPDIRECT3DDEVICE9  pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
	D3DXCreateSphere(pDevice, Radius, Slices, Stacks, ppMesh, ppAdjacency);
	m_Mesh = ppMesh; 
}

void CMeshCol::BoxMesh_Col()
{
}

void CMeshCol::SphereMesh_Col()
{
}

CMeshCol* CMeshCol::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMeshCol* pInstance = new CMeshCol(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MeshCol Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CMeshCol::Clone()
{
	// 복사 생성
	return new CMeshCol(*this);
}

void CMeshCol::Free()
{
	SUPER::Free();
}
