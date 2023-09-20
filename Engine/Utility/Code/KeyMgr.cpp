#include "KeyMgr.h"

#include "Export_System.h"

IMPLEMENT_SINGLETON(CKeyMgr)

CKeyMgr::CKeyMgr()
{
	ZeroMemory(m_bKeyState, sizeof(m_bKeyState));
}

CKeyMgr::~CKeyMgr()
{
}

void CKeyMgr::Free()
{
	if (m_mapInputKeys.empty())
		return;

	/*for_each(m_mapInputKeys.begin(), m_mapInputKeys.end(), [](pair<const _tchar*, vector<INPUT_KEY_INFO*>>& vecInputKeyInfo) {
		for_each(vecInputKeyInfo.second.begin(), vecInputKeyInfo.second.end(), ()[INPUT_KEY_INFO*& pInputKey] {
			Safe_Delete(pInputKey);
			})
		});*/
}

HRESULT CKeyMgr::Ready_Key()
{
	Add_InputKey(new INPUT_KEY_INFO(L"Right"));
	//m_vInputKeys.back()->Add_Keys(VK_RIGHT, 'D');
	Add_InputKey(new INPUT_KEY_INFO(L"Left"));
	//m_vInputKeys.back()->Add_Keys(VK_LEFT, 'A');
	Add_InputKey(new INPUT_KEY_INFO(L"Up"));
	//m_vInputKeys.back()->Add_Key(VK_UP);
	Add_InputKey(new INPUT_KEY_INFO(L"Down"));
	//m_vInputKeys.back()->Add_Key(VK_DOWN);

	Add_InputKey(new INPUT_KEY_INFO(L"UI_Accept"));
	//m_vInputKeys.back()->Add_Key(VK_ACCEPT);
	Add_InputKey(new INPUT_KEY_INFO(L"UI_Cancel"));
	//m_vInputKeys.back()->Add_Key(VK_ESCAPE);
	Add_InputKey(new INPUT_KEY_INFO(L"UI_Menu"));
	//m_vInputKeys.back()->Add_Key(VK_ACCEPT);

	return S_OK;
}

void CKeyMgr::Update_Key()
{
	Engine::Update_InputDev();		// ���̷�Ʈ ��ǲ ������Ʈ�� Ű �Ŵ������� ���־� �ϰ����� �����Ѵ�.
}

void CKeyMgr::LateUpdate_Key()
{
	//for (int i = 0; i < m_vInputKeys.size(); ++i)
	//{
	//	if (nullptr == m_vInputKeys[i])
	//		continue;

	//	for (int j = 0; j < m_vInputKeys[i]->vKeys.size(); ++j)
	//	{
	//		// �Է��� �޾��� ��
	//		if (GetAsyncKeyState(get<0>(m_vInputKeys[i]->vKeys[j])) & 0x8000)
	//		{
	//			if (EINPUT_KEY_STATE::NUETRAL >= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSING;
	//		}
	//		// �Է��� ���� ��
	//		else
	//		{
	//			if (EINPUT_KEY_STATE::PRESSING <= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::RELEASED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::NUETRAL;
	//		}
	//	}
	//}

	// m_bKeyState�� ���� Ű ���¸� ������Ʈ �մϴ�.
	for (int i = 0; i < MAX_DINPUT_KEY; ++i)
	{
		if (m_bKeyState[i] && !(Engine::Get_DIKeyState(i) & 0x80))
			m_bKeyState[i] = !m_bKeyState[i];
		else if (!m_bKeyState[i] && (Engine::Get_DIKeyState(i) & 0x80))
			m_bKeyState[i] = !m_bKeyState[i];
	}
}

bool CKeyMgr::Key_Pressing(const int& iKey)
{
	if (Engine::Get_DIKeyState(iKey) & 0x80)
		return true;

	return false;
}

bool CKeyMgr::Key_Down(const int& iKey)
{
	// �������� ���� ���� ���� ���� ������ ���
	if (!m_bKeyState[iKey] && (Engine::Get_DIKeyState(iKey) & 0x80))
		return true;

	return false;
}

bool CKeyMgr::Key_Up(const int& iKey)
{
	// �������� ���� ���� �ְ� ���� ������ �ʾ��� ���
	if (m_bKeyState[iKey] && !(Engine::Get_DIKeyState(iKey) & 0x80))
	{
		return true;
	}

	return false;
}

void CKeyMgr::Add_InputKey(INPUT_KEY_INFO* pInputKey)
{
}

void CKeyMgr::Remove_InputKey(INPUT_KEY_INFO* pInputKey)
{
}
