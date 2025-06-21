#include "physprop_stub.h"
#include <iostream>
#include <algorithm> // for std::sort
#include <vector>
#include "entoutput.h"


class CInfoDatadescAccessor : public CBaseEntity 
{
public:
	DECLARE_CLASS(CInfoDatadescAccessor, CBaseEntity, 197);
	virtual void Spawn();
	virtual void Precache();
	void ReadData(inputdata_t &);
	//COutputVariant m_DataVal;
	//bool OnCapture();
	DECLARE_DATADESC();
	string_t m_pszTargetEntity;
};

LINK_ENTITY_TO_CLASS(info_datadesc_accessor, CInfoDatadescAccessor);

BEGIN_DATADESC(CInfoDatadescAccessor)
DEFINE_INPUTFUNC(FIELD_STRING,"ReadData",ReadData),
//DEFINE_OUTPUT(m_DataVal,"DataVal"),
DEFINE_KEYFIELD(m_pszTargetEntity,FIELD_STRING,"target_entity"),
END_DATADESC()

void CInfoDatadescAccessor::Spawn() 
{
}



void CInfoDatadescAccessor::Precache() 
{
}

void CInfoDatadescAccessor::ReadData(inputdata_t & name)
{
}

CBaseEntity* UTIL_GetLocalPlayer(void);
CBaseEntity* ENTLIST_FindEntityByName(CBaseEntity* pStartEntity, const char* szName, CBaseEntity* pSearchingEntity = 0, CBaseEntity* pActivator = 0, CBaseEntity* pCaller = 0, void* pFilter = 0);

CON_COMMAND(read_datadesc_val, "Read a single datadesc entry from an entity")
{
	if (args.ArgC() < 3)
	{
		Warning("less than 2 args specified\n");
		return;
	}
	const char* entityname = args.Arg(1);
	const char* fieldname = args.Arg(2);
	CBaseEntity * ent = ENTLIST_FindEntityByName(NULL,entityname);
	if (ent == NULL)
	{
		Warning("No entity with that name found\n");
		return;
	}

	datamap_t * datamap = ent->GetDataDescMap();
	for (int i = 0; i < datamap->dataNumFields; i++)
	{
		if (strcmp(datamap->dataDesc[i].fieldName, fieldname) == 0)
		{
			//We have a match!
			Msg("Field type %i\n",datamap->dataDesc[i].fieldType);
			variant_t variant;
			variant.Set(datamap->dataDesc[i].fieldType,(void *)(((char*)ent)+datamap->dataDesc[i].fieldOffset));
			Msg("Field val %s\n", variant.String());
			return;
		}
	}
	Warning("No field with that name found");
}

bool CompareDesc(const typedescription_t* a, const typedescription_t* b) {
	return a->fieldOffset > b->fieldOffset;
}

const char* fieldTypeStrings[] = {
	"FIELD_VOID",               // 0
	"FIELD_FLOAT",              // 1
	"FIELD_STRING",             // 2
	"FIELD_VECTOR",             // 3
	"FIELD_QUATERNION",         // 4
	"FIELD_INTEGER",            // 5
	"FIELD_BOOLEAN",            // 6
	"FIELD_SHORT",              // 7
	"FIELD_CHARACTER",          // 8
	"FIELD_COLOR32",            // 9
	"FIELD_EMBEDDED",           // 10
	"FIELD_CUSTOM",             // 11
	"FIELD_CLASSPTR",           // 12
	"FIELD_EHANDLE",            // 13
	"FIELD_EDICT",              // 14
	"FIELD_POSITION_VECTOR",     // 15
	"FIELD_TIME",               // 16
	"FIELD_TICK",               // 17
	"FIELD_MODELNAME",          // 18
	"FIELD_SOUNDNAME",          // 19
	"FIELD_INPUT",              // 20
	"FIELD_FUNCTION",           // 21
	"FIELD_VMATRIX",            // 22
	"FIELD_VMATRIX_WORLDSPACE", // 23
	"FIELD_MATRIX3X4_WORLDSPACE", // 24
	"FIELD_INTERVAL",           // 25
	"FIELD_MODELINDEX",         // 26
	"FIELD_MATERIALINDEX",      // 27
	"FIELD_VECTOR2D"           // 28
};

const typedescription_t fakedesc = {FIELD_VOID,"UNALLOCATED",0,0};

void DUMP_dmap_to_console(datamap_t * datamap)
{
	Msg("******CLASS: %s********\n", datamap->dataClassName);

	std::vector<const typedescription_t*> sortedPtrs(datamap->dataNumFields);

	int j = 0;

	for (int i = 0; i < datamap->dataNumFields; ++i) {
		if (datamap->dataDesc[i].inputFunc == NULL)
		{
			sortedPtrs[j] = &datamap->dataDesc[i];
			j++;
		}
	}
	sortedPtrs.resize(j);

	// sort
	std::sort(sortedPtrs.begin(), sortedPtrs.end(), CompareDesc);

	int len = datamap->dataNumFields;

	for (int i = 0; i < len; i++)
	{
		if (len == i+1)
			break;
		if ((sortedPtrs[i]->fieldOffset + sortedPtrs[i]->fieldSize) < sortedPtrs[i + 1]->fieldOffset)
			{
				sortedPtrs.insert((sortedPtrs.begin() + i + 1),&fakedesc);
				len++;
				i++;
			}
	}

	Msg("%-10s %-10s %-30s %-40s %-20s\n", "Offset", "Size", "Field type", "C Name", "Hammer name");

	for (const typedescription_t* typede : sortedPtrs)
	{
		Msg("%-#10x %-#10x %-30s %-40s %-20s\n", typede->fieldOffset, typede->fieldSize, fieldTypeStrings[typede->fieldType], typede->fieldName, typede->externalName);
	}

	Msg("**********************\n");
	if (datamap->baseMap)
	{
		DUMP_dmap_to_console(datamap->baseMap);
	}
}



CON_COMMAND(dump_full_datadesc, "Dumps the full list of datadesc entries for an entity")
{
	if (args.ArgC() < 2)
	{
		Warning("less than 1 args specified\n");
		return;
	}
	const char* entityname = args.Arg(1);
	//const char* fieldname = args.Arg(2);
	CBaseEntity* ent = ENTLIST_FindEntityByName(NULL, entityname);
	if (ent == NULL)
	{
		Warning("No entity with that name found\n");
		return;
	}

	datamap_t* datamap = ent->GetDataDescMap();
	DUMP_dmap_to_console(datamap);
	//Warning("No field with that name found");
}
