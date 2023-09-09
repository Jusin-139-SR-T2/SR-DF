#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainTexComponent : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CTerrainTexComponent)

protected:
	explicit CTerrainTexComponent();
	explicit CTerrainTexComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainTexComponent(const CTerrainTexComponent& rhs);
	virtual ~CTerrainTexComponent();

#pragma region 생성, 해제
public:
	// 팩토리
	static CTerrainTexComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev,
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
	GETSET_EX2(_ulong, m_dwCountX, VertexCountX, SET, GET_C_REF)
	GETSET_EX2(_ulong, m_dwCountZ, VertexCountZ, SET, GET_C_REF)
	
protected:
	_ulong			m_dwCountX;
	_ulong			m_dwCountZ;
	_float			m_fInterval = 1.f;

public:
	HRESULT			Load_HeightMap(const _tchar* pFileName = L"");

private:
	_bool			m_bHeightMap_Loaded;
	vector<_int>	m_vHeightMap;
};

END