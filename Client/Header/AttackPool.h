//#pragma once
//
//#include "GameObject.h"
//
//#include "Export_System.h"
//#include "Export_Utility.h"
//
//#include "Attack.h"
//
//template <typename T,
//	typename = std::enable_if_t<std::is_base_of<CAttack, T>::value>>
//class CAttackPool : public CBase
//{
//private: 
//    CAttackPool(const CAttackPool<T>&);
//    CAttackPool& operator = (const CAttackPool<T>&);
//private: 
//    static CAttackPool<T>* m_pInstance; 
//public: 
//    static CAttackPool<T>* GetInstance(void)
//    {
//        if (0 == m_pInstance) {
//            m_pInstance = new  CAttackPool<T>();
//        } 
//        return m_pInstance;
//    }
//    static void DestroyInstance(void)
//    {
//        if (0 != m_pInstance)
//        {
//            delete m_pInstance; m_pInstance = 0;
//        }
//    }
//
//public:
//
//};
//
//
//template <>
//CAttackPool<CAttack>* CAttackPool<CAttack>::m_pInstance = nullptr;