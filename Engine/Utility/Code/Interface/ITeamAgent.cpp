#include "ITeamAgent.h"

ULONGLONG ITeamAgent::ID_COUNT;								// 고유 ID 제공용
map<BYTE, map<BYTE, ERELATION_STATE>> ITeamAgent::m_mapTeamRelation;