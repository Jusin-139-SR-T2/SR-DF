#pragma once
#include "GameObject.h"

// �ý��� ���� ��� ��ƼŬ�� ������ �ִ� ���� Ư������ ����
// ��ƼŬ�� ���Ű� ���÷��� �Ҹ�, ���� ���� �����ϴ� ����

BEGIN(Engine)

class CPsystem : public Engine::CGameObject
{
	DERIVED_CLASS(CGameObject, CPsystem)

protected:
	explicit CPsystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPsystem(const CPsystem& rhs);
	virtual ~CPsystem();

protected:
	HRESULT		Ready_Particle(const _tchar* texFileName);	// ����Ʈ ��������Ʈ�� �����ϱ����� ���ؽ� ���۸� ����� �ؽ�ó�� ����µ� �ʱ�ȭ�۾� ó���ϴ°� - ��å�� bool init�� �ش� 
	HRESULT		Update_Particle(const _float& fTimeDelta);
	void		LateUpdate_Particle();
	void		Render_Particle();

protected:
	virtual void ResetParticle(Attribute* attribute) PURE; // ��ƼŬ���� ���µǴ� ����� �ٸ���.
	virtual void addParticle(); // �ý��ۿ� ��ƼŬ�� �߰� 

private:
	virtual void Free();
	virtual void ResetList(); //�ý��� ���� ��� ��ƼŬ �Ӽ��� ����
	virtual void preRender();	// �������� �ռ� �����ؾ� �� �ʱ� ���� ���¸� ����. -> �ý��ۿ� ���� �޶����Ƿ� �����Լ� ���� 
	virtual void postRender();  // Ư�� ��ƼŬ �ý����� �������� �� �ִ� ���� ���¸� �����ϴ� �� �̿�. -> ���� �޼���� ����.
	virtual void RemoveDeadParticles(); // �Ӽ� ����Ʈ _particle�� �˻��Ͽ� ���� ��ƼŬ�� ����Ʈ���� ����.

public:
	bool isEmpty() { return m_ParticleList.empty();	}   // ���� �ý��ۿ� ��ƼŬ�� ���� ��� true ����.
	bool isDead(); // �ý��� ���� ��ƼŬ�� ��� ���� ��� true ����.

	// ���� ================================================
protected:
	DWORD m_vbSize;       // ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��- ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
	DWORD m_vbOffset;     // ��ƼŬ �ý����� �������� �̿�.
	DWORD m_vbBatchSize;  // ��ƼŬ �ý����� �������� �̿�.

	LPDIRECT3DVERTEXBUFFER9			m_pVB;
	LPDIRECT3DTEXTURE9              m_pTexture;
	BoundingBox						m_BoundingBox;    // ��ƼŬ�� �̵��� �� �ִ� ���Ǹ� �����ϴ� �� �̿�. -> ���ϴ� ������ �����ڷ� �����ϸ� �� ������ ��� ��ƼŬ���� ��ٷ� �Ҹ�.
	
	list<Attribute>					m_ParticleList;   // �ý��� �� ��ƼŬ �Ӽ��� ����Ʈ.
	_vec3							m_vOrigin;		  // �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
	_float							m_femitRate;	  // �ý��ۿ� ���ο� ��ƼŬ�� �߰��Ǵ� ����. ������ �ʴ� ��ƼŬ ���� ���.
	_float							m_fsize;		  // �ý��� �� ��� ��ƼŬ�� ũ��
	_int							m_imaxParticles;  // �־��� �ð� ���� �ý����� ���� �� �ִ� �ִ� ��ƼŬ�� ��.

};

END

// �츮�� ��ƼŬ�� ����� �����ϰ� �����ϴ� �� list<Attribute> ����Ʈ�� �̿�.
// ��ƼŬ�� �׸� �غ� �Ϸ�Ǹ� ����Ʈ ����� �Ϻθ� ���ؽ� ���۷� �����ϰ� ��ƼŬ ����� ������ ��ģ��.
// �̾� ���� �ܰ��� �����͸� �����ϰ� �ٽ� ��ƼŬ�� �׸���, ��� ��ƼŬ�� �׸� ������ �� ������ �ݺ�