#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainComponent : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CTerrainComponent)

protected:
	explicit CTerrainComponent();
	explicit CTerrainComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainComponent(const CTerrainComponent& rhs);
	virtual ~CTerrainComponent();

#pragma region 생성, 해제
public:
	// 팩토리
	static CTerrainComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _tchar* pFileName = L"",
										const _ulong& dwCntX = VTXCNTX,
										const _ulong& dwCntZ = VTXCNTZ,
										const _ulong& dwVtxItv = VTXITV);
	// 프로토
	virtual CComponent* Clone();

private:
	// 해제
	virtual void Free();
#pragma endregion

#pragma region 준비, 로직
public:
	virtual HRESULT		Ready_Buffer(const _tchar* pFileName, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual void		Render_Buffer();
#pragma endregion

public:
	void Set_VertexCountX(_ulong value) { m_dwCountX = value; }
	const _ulong& Get_VertexCountX() { return m_dwCountX; }

	void Set_VertexCountZ(_ulong value) { m_dwCountZ = value; }
	const _ulong& Get_VertexCountZ() { return m_dwCountZ; }
	
protected:
	_ulong m_dwCountX;
	_ulong m_dwCountZ;
	_float m_fInterval = 1.f;

public:
	HRESULT			Load_HeightMap(const _tchar* pFileName = L"");

private:
	_bool			m_bHeightMap_Loaded;
	vector<_int>	m_vHeightMap;
};

END