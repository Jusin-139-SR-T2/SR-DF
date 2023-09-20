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
	Engine::Update_InputDev();		// 다이렉트 인풋 업데이트를 키 매니저에서 해주어 일관성을 유지한다.
}

void CKeyMgr::LateUpdate_Key()
{
	//for (int i = 0; i < m_vInputKeys.size(); ++i)
	//{
	//	if (nullptr == m_vInputKeys[i])
	//		continue;

	//	for (int j = 0; j < m_vInputKeys[i]->vKeys.size(); ++j)
	//	{
	//		// 입력을 받았을 때
	//		if (GetAsyncKeyState(get<0>(m_vInputKeys[i]->vKeys[j])) & 0x8000)
	//		{
	//			if (EINPUT_KEY_STATE::NUETRAL >= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::PRESSING;
	//		}
	//		// 입력이 없을 때
	//		else
	//		{
	//			if (EINPUT_KEY_STATE::PRESSING <= get<1>(m_vInputKeys[i]->vKeys[j]))
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::RELEASED;
	//			else
	//				get<1>(m_vInputKeys[i]->vKeys[j]) = EINPUT_KEY_STATE::NUETRAL;
	//		}
	//	}
	//}

	// m_bKeyState에 대해 키 상태를 업데이트 합니다.
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
	// 이전에는 눌린 적이 없고 현재 눌렸을 경우
	if (!m_bKeyState[iKey] && (Engine::Get_DIKeyState(iKey) & 0x80))
		return true;

	return false;
}

bool CKeyMgr::Key_Up(const int& iKey)
{
	// 이전에는 눌린 적이 있고 현재 눌리지 않았을 경우
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
