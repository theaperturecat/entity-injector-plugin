#pragma once
#include "baseanimating_stub.h"
class CPhysicsProp : public CBaseAnimating {
public:
	static int baseClass_vt_addr;
	static int baseClass_vt_size;
	void Construct();
	inline static void * GetBaseVfunc(int index){
		return ((void**)((DWORD_PTR)serverDLL + (DWORD)baseClass_vt_addr))[index];
	}
	virtual ~CPhysicsProp() override { Msg("False destructor called"); };

	virtual void vfunc233() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc234() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc235() { Msg("Oh fiddlesticks what now"); };
	virtual void vfunc236() { Msg("Oh fiddlesticks what now"); };
	unsigned char PPROP_data[612];
};