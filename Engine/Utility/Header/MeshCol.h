#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CMeshCol : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CMeshCol)

protected:
	explicit CMeshCol();
	explicit CMeshCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMeshCol(const CMeshCol& rhs);
	virtual ~CMeshCol();

public:
	void	BoxMesh_Ready(LPDIRECT3DDEVICE9 pDevice, FLOAT _Width, FLOAT _Height, FLOAT Depth, LPD3DXMESH* ppMesh);
	void	Sphere_Ready(LPDIRECT3DDEVICE9  pDevice, FLOAT Radius, UINT Slices, UINT Stacks, LPD3DXMESH* ppMesh, LPD3DXBUFFER* ppAdjacency);
	void	BoxMesh_Col();
	void	SphereMesh_Col();

public:
	static CMeshCol* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	LPD3DXMESH*	m_Mesh;
	FLOAT  Width = 0.f;
	FLOAT  Height = 0.f;
	FLOAT  Depth = 0.f;

private:
	virtual void Free();
};

END