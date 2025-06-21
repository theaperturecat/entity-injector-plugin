#pragma once
#include "baseentity_stub.h"
#include "triggers_shared.h"
class CBaseToggle : public CBaseEntity {
public:

	static int baseClass_vt_addr;
	static int baseClass_vt_size;
	inline static void* GetBaseVfunc(int index) {
		return ((void**)((DWORD_PTR)serverDLL + (DWORD)baseClass_vt_addr))[index];
	}
	virtual ~CBaseToggle() override { Msg("False destructor called"); };
	void Construct();
	char BTGLE_data[120];

	virtual void vfunc197() { Msg("Oh fiddlesticks what now"); };
};
class CBaseTrigger : public CBaseToggle {
public:
	char BTRIG_data[180];
	static int baseClass_vt_addr;
	static int baseClass_vt_size;
	inline static void* GetBaseVfunc(int index) {
		return ((void**)((DWORD_PTR)serverDLL + (DWORD)baseClass_vt_addr))[index];
	}

	virtual ~CBaseTrigger() override { Msg("False destructor called"); };

	void Construct();

	virtual void vfunc198() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc199() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc200() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc201() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc202() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc203() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc204() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc205() { Msg("Oh fiddlesticks what now"); };
	virtual bool PassesTriggerFilters(CBaseEntity* pOther) { Msg("Oh fiddlesticks what now"); return false; };
	virtual void vfunc207() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc208() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc209() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc210() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc211() { Msg("Oh fiddlesticks what now"); };

};