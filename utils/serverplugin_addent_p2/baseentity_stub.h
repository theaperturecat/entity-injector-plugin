#pragma once
//Todo add IENTITYINFO


#include <stdio.h>
#include <stdexcept>
#undef GAME_DLL
#ifdef GAME_DLL
#include "cbase.h"
#endif
#include <stdio.h>
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "Color.h"
#include "vstdlib/random.h"
#include "engine/IEngineTrace.h"
#include "tier2/tier2.h"
#include "timedeventmgr.h"
#include "game/server/iplayerinfo.h"
#include "vphysics_interface.h"
#include "entlist_stub.h"
#include "addresses.h"
//#include "game/server/pluginvariant.h"
//#include "game/server/ientityinfo.h"
//#include "game/server/igameinfo.h"

// VPHYSICS object game-specific flags
#define FVPHYSICS_DMG_SLICE				0x0001		// does slice damage, not just blunt damage
#define FVPHYSICS_CONSTRAINT_STATIC		0x0002		// object is constrained to the world, so it should behave like a static
#define FVPHYSICS_PLAYER_HELD			0x0004		// object is held by the player, so have a very inelastic collision response
#define FVPHYSICS_PART_OF_RAGDOLL		0x0008		// object is part of a client or server ragdoll
#define FVPHYSICS_MULTIOBJECT_ENTITY	0x0010		// object is part of a multi-object entity
#define FVPHYSICS_HEAVY_OBJECT			0x0020		// HULK SMASH! (Do large damage even if the mass is small)
#define FVPHYSICS_PENETRATING			0x0040		// This object is currently stuck inside another object
#define FVPHYSICS_NO_PLAYER_PICKUP		0x0080		// Player can't pick this up for some game rule reason
#define	FVPHYSICS_WAS_THROWN			0x0100		// Player threw this object
#define FVPHYSICS_DMG_DISSOLVE			0x0200		// does dissolve damage, not just blunt damage
#define FVPHYSICS_NO_IMPACT_DMG			0x0400		// don't do impact damage to anything
#define FVPHYSICS_NO_NPC_IMPACT_DMG		0x0800		// Don't do impact damage to NPC's. This is temporary for NPC's shooting combine balls (sjb)
#define FVPHYSICS_NO_SELF_COLLISIONS	0x8000		// don't collide with other objects that are part of the same entity

#include "iserverentity.h"
#include "util.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
//#include <ServerNetworkProperty.h>

class CCollisionProperty : public ICollideable {
public:
	void SetSolid(SolidType_t);
	void SetSolidFlags(int);
};

inline IHandleEntity* CBaseHandle::Get() const
{
	if (m_Index != -1) {
		if (g_pEntityList->m_EntPtrArray[m_Index & 0xffff].m_SerialNumber == (int)(m_Index >> 0x10)) {
			return g_pEntityList->m_EntPtrArray[m_Index & 0xffff].m_pEntity;
		}
	}
	return NULL;
}

typedef void (CBaseEntity::* BASEPTR)(void);

class CBaseEntity : public IServerEntity {
public:
	void* operator new(size_t stAllocateBlock);
	void* operator new(size_t stAllocateBlock,void *);
	void operator delete(void* pMem);

	void Construct();

	unsigned char BENT_data[0x358];
	static int baseClass_vt_addr;
	static int baseClass_vt_size;
	inline static void* GetBaseVfunc(int index) {
		return ((void**)((DWORD_PTR)serverDLL + (DWORD)baseClass_vt_addr))[index];
	}
	int PrecacheModel(const char *);
	void * GetVariableFromOffset(int offset);

	static bool callBaseDestructor;

//Stub vfunc
	virtual ~CBaseEntity() override { if (callBaseDestructor) { ((void(__thiscall*)(CBaseEntity*))(GetBaseVfunc(0)))(this); } Msg("False destructor called"); };
	void SetRefEHandle(const CBaseHandle&) override { Msg("Oh fiddlesticks what now"); };
	CBaseHandle& GetRefEHandle() const override { Msg("Oh fiddlesticks what now"); throw std::runtime_error("No valid handle available"); };
	virtual ICollideable* GetCollideable() override { Msg("Oh fiddlesticks what now"); return NULL; };
	virtual IServerNetworkable* GetNetworkable() override { Msg("Oh fiddlesticks what now"); return NULL; };
	virtual CBaseEntity* GetBaseEntity() override { Msg("Oh fiddlesticks what now"); return NULL; };
	virtual int GetModelIndex(void) const override { Msg("Oh fiddlesticks what now"); return NULL; };
	virtual string_t GetModelName(void) const override { Msg("Oh fiddlesticks what now");  throw std::runtime_error("No valid handle available"); };
	virtual void SetModelIndex(int index) override { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc9() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc10() { Msg("Oh fiddlesticks what now"); };
	virtual datamap_t* GetDataDescMap() { Msg("Oh fiddlesticks what now"); return NULL; };
	virtual void vfunc12() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc13() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc14() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc15() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc16() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc17() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc18() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc19() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc20() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc21() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc22() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc23() { Msg("Oh fiddlesticks what now"); };
	virtual void Spawn() { Msg("Oh fiddlesticks what now"); };
	virtual void Precache() { Msg("Oh fiddlesticks what now"); };
	virtual void SetModel(const char* szModelName) { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc27() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc28() { Msg("Oh fiddlesticks what now"); };
	virtual void PostConstructor(const char*) { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc30() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc31() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc32() { Msg("Oh fiddlesticks what now"); };
	virtual bool KeyValue(const char* szKeyName, int nValue) { Msg("Oh fiddlesticks what now"); return false; };
	virtual void vfunc34() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc35() { Msg("Oh fiddlesticks what now"); };
	virtual bool GetKeyValue(const char* szKeyName, char* szValue, int iMaxLen) { Msg("Oh fiddlesticks what now"); return false; };
	virtual void vfunc37() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc38() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc39() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc40() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc41() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc42() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc43() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc44() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc45() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc46() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc47() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc48() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc49() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc50() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc51() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc52() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc53() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc54() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc55() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc56() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc57() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc58() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc59() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc60() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc61() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc62() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc63() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc64() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc65() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc66() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc67() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc68() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc69() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc70() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc71() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc72() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc73() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc74() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc75() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc76() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc77() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc78() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc79() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc80() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc81() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc82() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc83() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc84() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc85() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc86() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc87() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc88() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc89() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc90() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc91() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc92() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc93() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc94() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc95() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc96() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc97() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc98() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc99() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc100() { Msg("Oh fiddlesticks what now"); };
	virtual void Use() { Msg("Oh fiddlesticks what now"); };
	virtual void StartTouch(CBaseEntity* pOther) { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc103() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc104() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc105() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc106() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc107() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc108() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc109() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc110() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc111() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc112() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc113() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc114() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc115() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc116() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc117() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc118() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc119() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc120() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc121() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc122() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc123() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc124() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc125() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc126() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc127() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc128() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc129() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc130() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc131() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc132() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc133() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc134() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc135() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc136() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc137() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc138() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc139() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc140() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc141() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc142() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc143() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc144() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc145() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc146() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc147() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc148() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc149() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc150() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc151() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc152() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc153() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc154() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc155() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc156() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc157() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc158() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc159() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc160() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc161() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc162() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc163() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc164() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc165() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc166() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc167() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc168() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc169() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc170() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc171() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc172() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc173() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc174() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc175() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc176() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc177() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc178() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc179() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc180() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc181() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc182() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc183() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc184() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc185() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc186() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc187() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc188() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc189() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc190() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc191() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc192() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc193() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc194() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc195() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc196() { Msg("Oh fiddlesticks what now"); };
	IServerNetworkable* NetworkProp() { return GetNetworkable(); };
	CCollisionProperty* CollisionProp() { return (CCollisionProperty*)GetCollideable(); };
	inline IPhysicsObject* VPhysicsGetObject() { return ((IPhysicsObject*)(*((void**)GetVariableFromOffset(0x14c)))); };
	inline CBaseEntity* GetParent() { 
		return (CBaseEntity*)(((CBaseHandle*)GetVariableFromOffset(0xd4))->Get());
	};
	inline void SetSolid(SolidType_t);
	inline void SetSolidFlags(int);
	void AddSpawnFlags(int flags);
	void SetNextThink(float, const char *);
	BASEPTR ThinkSet(BASEPTR, float, const char*);
};

inline void CBaseEntity::SetSolid(SolidType_t val)
{
	CollisionProp()->SetSolid(val);
}

inline void CBaseEntity::SetSolidFlags(int flags)
{
	CollisionProp()->SetSolidFlags(flags);
}