#pragma once


#include <Windows.h>

extern HMODULE serverDLL;

void LoadLib();

class IEntityFactory;

#define DECLARE_CLASS( className, baseClassName, vtLen ) \
		typedef baseClassName BaseClass; \
		typedef baseClassName ServBaseClass; \
		typedef className ThisClass; \
		static int * __vtable_new[ vtLen + 1 ];

//Use this if a class inherits a class in this plugin, and that one inherits a class in server.dll
#define DECLARE_CLASS_EXT( className, baseClassName, serverBaseClassName, vtLen ) \
		typedef baseClassName BaseClass; \
		typedef serverBaseClassName ServBaseClass; \
		typedef className ThisClass; \
		static int * __vtable_new[ vtLen + 1 ];

// entity creation
// creates an entity that has not been linked to a classname
template< class T >
T* _CreateEntityTemplate(T* newEnt, const char* className)
{
	MEM_ALLOC_CREDIT_("Entities");
	//newEnt = new T; // this is the only place 'new' should be used! 
	newEnt = (T*)calloc(1, sizeof(T));//Dont call constructor
	newEnt->Construct();
	newEnt = new  (newEnt) T();//This is so classes that extend can have their constructor called.
	*((int***)newEnt) = T::__vtable_new + 1;//Now we can do this

	newEnt->PostConstructor(className);
	return newEnt;
}

#include "tier0/memdbgoff.h"



// This is the glue that hooks .MAP entity class names to our CPP classes
class IEntityFactoryDictionary
{
public:
	virtual void InstallFactory(IEntityFactory * pFactory, const char* pClassName) = 0;
	virtual IServerNetworkable* Create(const char* pClassName) = 0;
	virtual void Destroy(const char* pClassName, IServerNetworkable* pNetworkable) = 0;
	virtual IEntityFactory* FindFactory(const char* pClassName) = 0;
	virtual const char* GetCannonicalName(const char* pClassName) = 0;
};

IEntityFactoryDictionary* EntityFactoryDictionary();

inline bool CanCreateEntityClass(const char* pszClassname)
{
	return (EntityFactoryDictionary() != NULL && EntityFactoryDictionary()->FindFactory(pszClassname) != NULL);
}

class IEntityFactory
{
public:
	virtual IServerNetworkable * Create(const char* pClassName) = 0;
	virtual void Destroy(IServerNetworkable* pNetworkable) = 0;
	virtual size_t GetEntitySize() = 0;
};

template <class T>
class CEntityFactory : public IEntityFactory
{
public:
	CEntityFactory(const char* pClassName)
	{
		LoadLib();
		// Warning: There is multiple problems with this code
		// 1. Its memory unsafe (but that is this entire project)
		// 2. It does not properly handle multiple inheritance
		// 3. typeid gets confused and dynamic_cast doesnt cast to a derived class in this plugin (both of these are because I do not copy RTTI entirely)
 


 
		//Magic sorcery that took me a while to figure out

		//Best way to make this work: change declare_class macro to define these as well ()
		DWORD rva = T::baseClass_vt_addr - 4;//Get RTTI ptr as well
		DWORD_PTR vtAddr = (DWORD_PTR)serverDLL + rva;
		
		int** vt_baseentity = (int**)vtAddr;
		T* get_vt = new T;

		typename T::ServBaseClass* get_vt2 = new typename T::ServBaseClass();//Get baseclass //remember: DONT call delete, will cause memory corruption and crash

		memcpy(T::__vtable_new, (*((int***)get_vt)) - 1, sizeof(T::__vtable_new));//Copy with rtti
		//delete get_vt;
		T::__vtable_new[0] = vt_baseentity[0];//Copy rtti
		//T::__vtable_new[1] = vt_baseentity[1];//Workaround because of destructor
		for (int i = 1; i < T::baseClass_vt_size; i++) {
			Msg("vtable entry num %i selected\n", i);
			Msg("plugin vtable entry: %i\n", T::__vtable_new[i + 1]);
			Msg("plugin base vtable entry: %i\n", (*((int***)get_vt2))[i]);
			if (T::__vtable_new[i + 1] == (*((int***)get_vt2))[i]) {
				T::__vtable_new[i + 1] = vt_baseentity[i + 1];
				Msg("Replacing vtable entry number %i\n\n", i);
			}
		}
		//This is the best way to do this i can think of.
		CBaseEntity::callBaseDestructor = false;
		delete get_vt;

		delete get_vt2;
		CBaseEntity::callBaseDestructor = true;
		EntityFactoryDictionary()->InstallFactory(this, pClassName);
	}

	IServerNetworkable* Create(const char* pClassName)
	{
		T* pEnt = _CreateEntityTemplate((T*)NULL, pClassName);

		return pEnt->NetworkProp();
	}

	void Destroy(IServerNetworkable* pNetworkable)
	{
		if (pNetworkable)
		{
			pNetworkable->Release();
		}
	}

	virtual size_t GetEntitySize()
	{
		return sizeof(T);
	}
};

#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) int* DLLClassName::__vtable_new[] = {};\
	static CEntityFactory<DLLClassName> mapClassName( #mapClassName );


//
// Conversion among the three types of "entity", including identity-conversions.
//
extern CGlobalVars* gpGlobals;
extern bool g_bIsLogging;

inline int ENTINDEX(edict_t* pEdict)
{
	if (!pEdict)
		return 0;
	int edictIndex = pEdict - gpGlobals->pEdicts;
	Assert(edictIndex < MAX_EDICTS && edictIndex >= 0);
	return edictIndex;
}

int	  ENTINDEX(CBaseEntity* pEnt);

inline edict_t* INDEXENT(int iEdictNum)
{
	Assert(iEdictNum >= 0 && iEdictNum < MAX_EDICTS);
	if (gpGlobals->pEdicts)
	{
		edict_t* pEdict = gpGlobals->pEdicts + iEdictNum;
		if (pEdict->IsFree())
			return NULL;
		return pEdict;
	}
	return NULL;
}

// Testing the three types of "entity" for nullity
inline bool FNullEnt(const edict_t* pent)
{
	return pent == NULL || ENTINDEX((edict_t*)pent) == 0;
}

// Dot products for view cone checking
#define VIEW_FIELD_FULL		(float)-1.0 // +-180 degrees
#define	VIEW_FIELD_WIDE		(float)-0.7 // +-135 degrees 0.1 // +-85 degrees, used for full FOV checks 
#define	VIEW_FIELD_NARROW	(float)0.7 // +-45 degrees, more narrow check used to set up ranged attacks
#define	VIEW_FIELD_ULTRA_NARROW	(float)0.9 // +-25 degrees, more narrow check used to set up ranged attacks

class CBaseEntity;
class CBasePlayer;

// Misc useful
inline bool FStrEq(const char* sz1, const char* sz2)
{
	return (sz1 == sz2 || stricmp(sz1, sz2) == 0);
}
