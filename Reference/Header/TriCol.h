#pragma once

#include "Engine_Define.h"
#include "VIBuffer.h"

BEGIN(Engine)

/// <summary>
/// 삼각형을 색과 함께 그리는 컴포넌트
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
	/// CTriCol 팩토리 함수
	/// </summary>
	/// <param name="pGraphicDev">다이렉트 장치</param>
	/// <returns>생성 인스턴스</returns>
	static CTriCol*		Create(LPDIRECT3DDEVICE9 pGraphicDev);
	/// <summary>
	/// CTriCol 프로토타입 함수
	/// </summary>
	/// <returns>복사</returns>
	virtual	CComponent* Clone();

private:
	/// <summary>
	/// 상속된 해제
	/// </summary>
	virtual void		Free();
};

END