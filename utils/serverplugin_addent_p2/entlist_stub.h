#pragma once
#include "ihandleentity.h"
#include "const.h"
class CEntInfo
{
public:
	IHandleEntity* m_pEntity;
	int				m_SerialNumber;
	CEntInfo* m_pPrev;
	CEntInfo* m_pNext;
	string_t		m_iName;
	string_t		m_iClassName;

	void			ClearLinks();
};


class CBaseEntityList
{
public:

	// Overridables.

	// These are notifications to the derived class. It can cache info here if it wants.
	virtual void OnAddEntity(IHandleEntity* pEnt, CBaseHandle handle);

	// It is safe to delete the entity here. We won't be accessing the pointer after
	// calling OnRemoveEntity.
	virtual void OnRemoveEntity(IHandleEntity* pEnt, CBaseHandle handle);

	class CEntInfoList
	{
	public:
		CEntInfoList();

		const CEntInfo* Head() const { return m_pHead; }
		const CEntInfo* Tail() const { return m_pTail; }
		CEntInfo* Head() { return m_pHead; }
		CEntInfo* Tail() { return m_pTail; }
		void			AddToHead(CEntInfo* pElement) { LinkAfter(NULL, pElement); }
		void			AddToTail(CEntInfo* pElement) { LinkBefore(NULL, pElement); }

		void LinkBefore(CEntInfo* pBefore, CEntInfo* pElement);
		void LinkAfter(CEntInfo* pBefore, CEntInfo* pElement);
		void Unlink(CEntInfo* pElement);
		bool IsInList(CEntInfo* pElement);

	private:
		CEntInfo* m_pHead;
		CEntInfo* m_pTail;
	};


	// The first MAX_EDICTS entities are networkable. The rest are client-only or server-only.
	CEntInfo m_EntPtrArray[NUM_ENT_ENTRIES];
	CEntInfoList	m_activeList;
	CEntInfoList	m_freeNonNetworkableList;
};


extern CBaseEntityList* g_pEntityList;
