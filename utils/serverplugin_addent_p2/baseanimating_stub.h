#pragma once
#include "baseentity_stub.h"

class CBaseAnimating : public CBaseEntity {
public:
	static int baseClass_vt_addr;
	static int baseClass_vt_size;
	inline static void* GetBaseVfunc(int index) {
		return ((void**)((DWORD_PTR)serverDLL + (DWORD)baseClass_vt_addr))[index];
	}
	void Construct();
	virtual ~CBaseAnimating() override { Msg("False destructor called"); };
	//vfunc stubs
	virtual void vfunc197() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc198() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc199() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc200() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc201() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc202() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc203() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc204() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc205() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc206() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc207() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc208() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc209() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc210() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc211() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc212() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc213() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc214() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc215() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc216() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc217() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc218() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc219() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc220() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc221() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc222() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc223() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc224() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc225() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc226() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc227() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc228() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc229() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc230() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc231() { Msg("Oh fiddlesticks what now"); };
	virtual void OnFizzled() { Msg("Oh fiddlesticks what now"); };
	unsigned char BANIM_data[340];
};