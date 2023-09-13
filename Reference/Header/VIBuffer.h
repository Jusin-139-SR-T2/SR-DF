#pragma once

#include "SceneComponent.h"

BEGIN(Engine)

/// <summary>
/// ���� ���� ���۸� �����ϴ� Ŭ����
/// </summary>
class ENGINE_DLL CVIBuffer : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CVIBuffer)
protected:
	explicit CVIBuffer();
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	const _ulong& Get_VertexCount() { return m_dwVtxCnt; }

protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;

	_ulong			m_dwVtxCnt;			// ������ ����
	_ulong			m_dwVtxSize;		// ������ ũ��
	_ulong			m_dwTriCnt;			// �ﰢ���� ����
	_ulong			m_dwFVF;			// ������ �Ӽ�

	_ulong			m_dwIdxSize;		// �ε��� ������
	D3DFORMAT		m_IdxFmt;			// �ε��� ����

public:
	virtual void Free();
};

END