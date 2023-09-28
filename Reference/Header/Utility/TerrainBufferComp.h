#pragma once

#include "VIBufferComp.h"

BEGIN(Engine)

class ENGINE_DLL CTerrainBufferComp : public CVIBufferComp
{
	DERIVED_CLASS(CVIBufferComp, CTerrainBufferComp)

protected:
	explicit CTerrainBufferComp();
	explicit CTerrainBufferComp(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTerrainBufferComp(const CTerrainBufferComp& rhs);
	virtual ~CTerrainBufferComp();

#pragma region ����, ����
public:
	// ���丮
	static CTerrainBufferComp* Create(LPDIRECT3DDEVICE9 pGraphicDev,
										const _tchar* pFileName = L"",
										const _ulong& dwCntX = VTXCNTX,
										const _ulong& dwCntZ = VTXCNTZ,
										const _ulong& dwVtxItv = VTXITV);
	// ������
	virtual CComponent* Clone();

private:
	// ����
	virtual void Free();
#pragma endregion

#pragma region �غ�, ����
public:
	virtual HRESULT		Ready_Buffer(const _tchar* pFileName, const _ulong& dwCntX, const _ulong& dwCntZ, const _ulong& dwVtxItv);
	virtual void		Render_Buffer();

public:
	const _vec3* Get_VtxPos() const { return m_pPos; }
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

private:
	_vec3* m_pPos;


	// CVIBufferComp��(��) ���� ��ӵ�
	virtual HRESULT Ready_Component() override;

	virtual void LateUpdate_Component() override;

	virtual void Render_Component() override;

};

END