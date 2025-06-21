#include "physprop_stub.h"


class CPropTest : public CPhysicsProp {
public:
	DECLARE_CLASS(CPropTest, CPhysicsProp, 237);//Increase this number if you add another virtual function
	virtual void Spawn();
	virtual void Precache();
};
LINK_ENTITY_TO_CLASS(prop_test, CPropTest);

void CPropTest::Spawn() {
	Msg("Test prop spawned\n");
	Precache();
	SetModel("models/props/metal_box.mdl");
	((void(__thiscall*)(CBaseEntity*))(GetBaseVfunc(VO_BASESPAWN)))(this);//Call base spawn
}

void CPropTest::Precache() {
	PrecacheModel("models/props/metal_box.mdl");
}