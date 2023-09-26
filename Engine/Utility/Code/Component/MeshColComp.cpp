#include "MeshColComp.h"

CMeshColComp::CMeshColComp()
{
}

CMeshColComp::CMeshColComp(LPDIRECT3DDEVICE9 pGraphicDev)
	: Base(pGraphicDev)
{
}

CMeshColComp::CMeshColComp(const CMeshColComp& rhs)
	: Base(rhs)
{
}

CMeshColComp::~CMeshColComp()
{
}

void CMeshColComp::BoxMesh_Ready(LPDIRECT3DDEVICE9 pDevice, FLOAT _Width, FLOAT _Height, FLOAT Depth, LPD3DXMESH* ppMesh)
{
	D3DXCreateBox(pDevice, Width, Height, Depth, ppMesh, NULL);
	m_Mesh = *ppMesh;
}

void CMeshColComp::Sphere_Ready(LPDIRECT3DDEVICE9  pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency)
{
	D3DXCreateSphere(pDevice, Radius, Slices, Stacks, ppMesh, ppAdjacency);
	m_Mesh = *ppMesh; 
}

void CMeshColComp::BoxMesh_Col()
{
	m_Mesh->DrawSubset(0);
}

void CMeshColComp::SphereMesh_Col()
{
	m_Mesh->DrawSubset(0);
}

CMeshColComp* CMeshColComp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMeshColComp* pInstance = new CMeshColComp(pGraphicDev);

	if (FAILED(pInstance->Ready_Buffer()))
	{
		Safe_Release(pInstance);
		MSG_BOX("MeshCol Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CMeshColComp::Clone()
{
	// ���� ����
	return new CMeshColComp(*this);
}

void CMeshColComp::Free()
{
	SUPER::Free();
}

HRESULT CMeshColComp::Ready_Component()
{
	return E_NOTIMPL;
}

void CMeshColComp::LateUpdate_Component()
{
}

void CMeshColComp::Render_Component()
{
}
