#pragma once

inline int IndexOfEdict(const edict_t* pEdict)
{
	return (int)(pEdict - gpGlobals->pEdicts);
}
inline edict_t* PEntityOfEntIndex(int iEntIndex)
{
	if (iEntIndex >= 0 && iEntIndex < gpGlobals->maxEntities)
	{
		return (edict_t*)(gpGlobals->pEdicts + iEntIndex);
	}
	return NULL;
}
inline int GetTypeDescOffs(typedescription_t* td)
{
	return td->fieldOffset;
}