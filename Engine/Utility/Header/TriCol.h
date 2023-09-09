#pragma once

#include "Engine_Define.h"
#include "VIBuffer.h"

BEGIN(Engine)

/// <summary>
/// �ﰢ���� ���� �Բ� �׸��� ������Ʈ
/// </summary>
class ENGINE_DLL CTriCol : public CVIBuffer
{
	DERIVED_CLASS(CVIBuffer, CTriCol)
private:
	explicit CTriCol();
	explicit CTriCol(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTriCol(const CTriCol& rhs);
	virtual ~CTriCol();

public:
	virtual HRESULT		Ready_Buffer();
	virtual void		Render_Buffer();

public:
	/// <summary>
	/// CTriCol ���丮 �Լ�
	/// </summary>
	/// <param name="pGraphicDev">���̷�Ʈ ��ġ</param>
	/// <returns>���� �ν��Ͻ�</returns>
	static CTriCol*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	/// <summary>
	/// CTriCol ������Ÿ�� �Լ�
	/// </summary>
	/// <returns>����</returns>
	virtual	CComponent* Clone();

private:
	/// <summary>
	/// ��ӵ� ����
	/// </summary>
	virtual void		Free();
};

END