#pragma once

#include "GameObject.h"
#include "ImguiAnimationTool.h"

BEGIN(Engine)

class CRcBufferComp;
class CTextureComponent;
class CTransformComponent;

END

//// Ű������ ����ü
//struct Keyframe
//{
//	char name[64];			// Ű������ �̸� (ǥ�ÿ�)
//
//	bool isEaseIn;			// Ease In ���� (True �Ǵ� False)
//	bool isEaseOut;			// Ease Out ���� (True �Ǵ� False)
//
//	float time;				// Ű�������� �ð� (0.0f ~ MaxTime ����)
//	float value;			// �ִϸ��̼� �� (ũ��, ȸ��, �̵� ��)
//	float color[3];			// Ű������ ���� (R, G, B)
//
//	OBJ_TYPE m_eObjectType; // Ÿ���� �ο��� �׸� (ex : �Ѽ�, ���)
//	OBJ_NAME m_eObjectName; // �̸��� �ο��� �׸� (ex : ����, ��������)
//
//	int type;				// �ִϸ��̼� Ÿ�� (0: ũ��, 1: ȸ��, 2: �̵�)
//
//	_vec3	vScale = { 0.f, 0.f, 0.f };			// ũ�⸦ ���� �׸�
//	_vec3	vRot = { 0.f, 0.f, 0.f };			// ȸ���� ���� �׸�
//	_vec3	vPos = { 0.f, 0.f, 0.f };			// ��ġ�� ���� �׸�
//};

class CBackGround : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CBackGround)

private:
	explicit CBackGround(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBackGround(const CBackGround& rhs);
	virtual ~CBackGround();

public:
	static CBackGround* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	HRESULT				Add_Component();

private:
	// �ִϸ��̼� �ҷ�����
	void LoadAnimationFromFile(const char* fileName);

public:
	virtual HRESULT		Ready_GameObject() override;
	virtual _int		Update_GameObject(const _float& fDelta) override;
	virtual void		LateUpdate_GameObject() override;
	virtual void		Render_GameObject() override;

public:
	GETSET_EX2(CTransformComponent*,	m_pTransformComp,	TransformComponent,	GET, SET)
	GETSET_EX2(CRcBufferComp*,			m_pBufferComp,		BufferComponent,	GET, SET)
	GETSET_EX2(CTextureComponent*,		m_pTextureComp,		TextureComponent,	GET, SET)

private:
	CTransformComponent*			m_pTransformComp = nullptr;
	CRcBufferComp*					m_pBufferComp = nullptr;
	CTextureComponent*				m_pTextureComp = nullptr;
	CTextureComponent*				m_pBackTextureComp = nullptr;

	_matrix		m_ViewMatrix, m_ProjMatrix;

private: //����
	// dt ��
	_float fFrameTimeDelta, fCurFrameTimeDelta;

	// ũ��
	_float fSizeX_Delta, fSizeY_Delta;

	// ȸ��
	_float fRotX_Delta, fRotY_Delta, fRotZ_Delta;

	// �̵�
	_float fPosX_Delta, fPosY_Delta;

	// �ִϸ��̼� ��
	CImguiAnimationTool* m_pAnimationTool;

	// �ִϸ��̼� ��
	std::vector<Keyframe>*  m_vecAnimationInfo;

	//_bool bFrameOn = false;
};

