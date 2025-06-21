#include "windows.h"
#include "dbg.h"
#include "physprop_stub.h"
#include "triggers_stub.h"
#include "E:\vs2022ide\DIA SDK\include/dia2.h"
#include "pdbparser.h"


int CBaseAnimating::baseClass_vt_addr = 0;
int CBaseAnimating::baseClass_vt_size = 233;
int CBaseEntity::baseClass_vt_addr = 0;
int CBaseEntity::baseClass_vt_size = 197;
int CPhysicsProp::baseClass_vt_addr = 0;
int CPhysicsProp::baseClass_vt_size = 237;
int CBaseToggle::baseClass_vt_addr = 0;
int CBaseToggle::baseClass_vt_size = 198;
int CBaseTrigger::baseClass_vt_addr = 0;
int CBaseTrigger::baseClass_vt_size = 212;

int EntityFactoryDictionary_RVA = 0;
int BASEENTITY_CONST = 0;
int BASEANIMATING_CONST = 0;
int BASETRIGGER_CONST = 0;
int BASETOGGLE_CONST = 0;
int PHYSPROP_CONST = 0;
int BASEENTITY_PRECACHEMODEL_RVA = 0;
int COLLISIONPROP_SETSOLID_RVA = 0;
int COLLISIONPROP_SETSOLIDFLAGS_RVA = 0;
int ENTLIST_RVA = 0;
int GETLOCALPLAYER = 0;

int BASEENTITY_SETNEXTTHINK = 0;
int BASEENTITY_THINKSET = 0;

int GLOBALENTLIST_FINDENTITIESBYNAME = 0;


bool CBaseEntity::callBaseDestructor = true;

void FillRVAs()
{
    PdbParser parser("E:\\p2014\\game\\portal2\\bin\\server.pdb");
    EntityFactoryDictionary_RVA = parser.LookupSymbol("EntityFactoryDictionary");
    BASEENTITY_CONST = parser.LookupSymbol("CBaseEntity::CBaseEntity");
    BASEANIMATING_CONST = parser.LookupSymbol("CBaseAnimating::CBaseAnimating");
    BASETRIGGER_CONST = parser.LookupSymbol("CBaseTrigger::CBaseTrigger");
    PHYSPROP_CONST = parser.LookupSymbol("CPhysicsProp::CPhysicsProp");
    BASETOGGLE_CONST = parser.LookupSymbol("CBaseToggle::CBaseToggle");
    BASEENTITY_PRECACHEMODEL_RVA = parser.LookupSymbol("CBaseEntity::PrecacheModel");
    COLLISIONPROP_SETSOLID_RVA = parser.LookupSymbol("CCollisionProperty::SetSolid");
    COLLISIONPROP_SETSOLIDFLAGS_RVA = parser.LookupSymbol("CCollisionProperty::SetSolidFlags");
    ENTLIST_RVA = parser.LookupSymbol("g_pEntityList");
    GETLOCALPLAYER = parser.LookupSymbol("UTIL_GetLocalPlayer");
    BASEENTITY_SETNEXTTHINK = parser.LookupSymbol("CBaseEntity::SetNextThink");

    BASEENTITY_THINKSET = parser.LookupSymbol("CBaseEntity::ThinkSet");
    
    GLOBALENTLIST_FINDENTITIESBYNAME = parser.LookupSymbol("CGlobalEntityList::FindEntityByName");

    CBaseAnimating::baseClass_vt_addr = BASEANIMATING_VT_OFF;//parser.LookupVTable("CBaseAnimating");
    CBaseEntity::baseClass_vt_addr = BASEENTITY_VT_OFF;
    CPhysicsProp::baseClass_vt_addr = PHYSPROP_VT_OFF;
    CBaseTrigger::baseClass_vt_addr = BASETRIGGER_VT_OFF;
    CBaseToggle::baseClass_vt_addr = BASETOGGLE_VT_OFF;

    //DebugBreak();
}

IEntityFactoryDictionary * EntityFactoryDictionary() 
{
    return ((IEntityFactoryDictionary*(*)())((DWORD_PTR)serverDLL + EntityFactoryDictionary_RVA))();
}

void* CBaseEntity::operator new(size_t stAllocateBlock)
{
    // call into engine to get memory
    Assert(stAllocateBlock != 0);
    return calloc(1, stAllocateBlock);
};

void* CBaseEntity::operator new(size_t size, void* ptr) {//Needed for placement new
    return ptr;  //return the pointer
}

void CBaseEntity::operator delete(void* pMem)
{
    // get the engine to free the memory
    free(pMem);
}
void CBaseEntity::Construct() 
{//Stop c++ constructors from being bad.
    return ((void(__thiscall *)(CBaseEntity*, bool))((DWORD_PTR)serverDLL + BASEENTITY_CONST))(this,false);
}
void CBaseAnimating::Construct() 
{//Stop c++ constructors from being bad.
    return ((void(__thiscall*)(CBaseEntity*))((DWORD_PTR)serverDLL + BASEANIMATING_CONST))(this);
}

void CBaseTrigger::Construct() 
{
    return ((void(__thiscall*)(CBaseEntity*))((DWORD_PTR)serverDLL + BASETRIGGER_CONST))(this);
}

void CBaseToggle::Construct() 
{
    return ((void(__thiscall*)(CBaseEntity*))((DWORD_PTR)serverDLL + BASETOGGLE_CONST))(this);
}

void CPhysicsProp::Construct() 
{
    return ((void(__thiscall*)(CBaseEntity*))((DWORD_PTR)serverDLL + PHYSPROP_CONST))(this);
}

int CBaseEntity::PrecacheModel(const char * b) 
{
    return ((int(__cdecl*)(const char *))((DWORD_PTR)serverDLL + BASEENTITY_PRECACHEMODEL_RVA))(b);
}

void* CBaseEntity::GetVariableFromOffset(int offset) 
{//Dereference return to get value
    return (((char*)this) + offset);
}

void CBaseEntity::AddSpawnFlags(int flags) 
{//HACK avoid network stuff
    char buff[20];
    GetKeyValue("spawnflags", buff, 10);
    KeyValue("spawnflags", atoi(buff) | flags);
}

void CCollisionProperty::SetSolid(SolidType_t t) 
{
    return ((void(__thiscall*)(CCollisionProperty*,SolidType_t))((DWORD_PTR)serverDLL + COLLISIONPROP_SETSOLID_RVA))(this,t);
}
void CCollisionProperty::SetSolidFlags(int t) 
{
    return ((void(__thiscall*)(CCollisionProperty*,int))((DWORD_PTR)serverDLL + COLLISIONPROP_SETSOLIDFLAGS_RVA))(this,t);
}

CBaseEntity * UTIL_GetLocalPlayer()
{
    return ((CBaseEntity *(__thiscall*)())((DWORD_PTR)serverDLL + GETLOCALPLAYER))();
};

CBaseEntity* ENTLIST_FindEntityByName(CBaseEntity* pStartEntity, const char* szName, CBaseEntity* pSearchingEntity, CBaseEntity* pActivator, CBaseEntity* pCaller, void* pFilter)
{
    return ((CBaseEntity * (__thiscall*)(void*,CBaseEntity * pStartEntity, const char* szName, CBaseEntity * pSearchingEntity, CBaseEntity * pActivator, CBaseEntity * pCaller, void* pFilter))((DWORD_PTR)serverDLL + GLOBALENTLIST_FINDENTITIESBYNAME))(g_pEntityList,pStartEntity, szName, pSearchingEntity, pActivator, pCaller, pFilter);
}

void CBaseEntity::SetNextThink(float a, const char* b)
{
    return ((void (__thiscall*)(CBaseEntity*,float,const char *))((DWORD_PTR)serverDLL + BASEENTITY_SETNEXTTHINK))(this,a,b);
}

typedef void (CBaseEntity::* BASEPTR)(void);

BASEPTR CBaseEntity::ThinkSet(BASEPTR a, float b, const char* c)
{
    return ((BASEPTR(__thiscall*)(CBaseEntity*,BASEPTR, float, const char*))((DWORD_PTR)serverDLL + BASEENTITY_THINKSET))(this,a, b, c);
}