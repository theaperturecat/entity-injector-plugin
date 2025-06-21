#pragma once
class variant_t
{
	union
	{
		bool bVal;
		string_t iszVal;
		int iVal;
		float flVal;
		float vecVal[3];
		color32 rgbaVal;
	};
	int eVal; // this can't be in the union because it has a constructor.

	fieldtype_t fieldType;

public:

	// constructor
	variant_t() : iVal(0), fieldType(FIELD_VOID) {}

	//inline bool Bool(void) const { return(fieldType == FIELD_BOOLEAN) ? bVal : false; }
	inline const char* String(void) const { return(fieldType == FIELD_STRING) ? STRING(iszVal) : ToString(); }
	//inline string_t StringID(void) const { return(fieldType == FIELD_STRING) ? iszVal : NULL_STRING; }
	//inline int Int(void) const { return(fieldType == FIELD_INTEGER) ? iVal : 0; }
	//inline float Float(void) const { return(fieldType == FIELD_FLOAT) ? flVal : 0; }
	//inline const CHandle<CBaseEntity>& Entity(void) const;
	//inline color32 Color32(void) const { return rgbaVal; }
	//inline void Vector3D(Vector& vec) const;

	fieldtype_t FieldType(void) { return fieldType; }

	void SetBool(bool b) { bVal = b; fieldType = FIELD_BOOLEAN; }
	void SetString(string_t str) { iszVal = str, fieldType = FIELD_STRING; }
	void SetInt(int val) { iVal = val, fieldType = FIELD_INTEGER; }
	void SetFloat(float val) { flVal = val, fieldType = FIELD_FLOAT; }
	//void SetEntity(CBaseEntity* val);
	void SetVector3D(const Vector& val) { vecVal[0] = val[0]; vecVal[1] = val[1]; vecVal[2] = val[2]; fieldType = FIELD_VECTOR; }
	void SetPositionVector3D(const Vector& val) { vecVal[0] = val[0]; vecVal[1] = val[1]; vecVal[2] = val[2]; fieldType = FIELD_POSITION_VECTOR; }
	void SetColor32(color32 val) { rgbaVal = val; fieldType = FIELD_COLOR32; }
	void SetColor32(int r, int g, int b, int a) { rgbaVal.r = r; rgbaVal.g = g; rgbaVal.b = b; rgbaVal.a = a; fieldType = FIELD_COLOR32; }
	void Set(fieldtype_t ftype, void* data);
	//void SetOther(void* data);
	//bool Convert(fieldtype_t newType);

	static typedescription_t m_SaveBool[];
	static typedescription_t m_SaveInt[];
	static typedescription_t m_SaveFloat[];
	static typedescription_t m_SaveEHandle[];
	static typedescription_t m_SaveString[];
	static typedescription_t m_SaveColor[];
	static typedescription_t m_SaveVector[];
	static typedescription_t m_SavePositionVector[];
	static typedescription_t m_SaveVMatrix[];
	static typedescription_t m_SaveVMatrixWorldspace[];
	static typedescription_t m_SaveMatrix3x4Worldspace[];

protected:

	//
	// Returns a string representation of the value without modifying the variant.
	//
	const char* ToString(void) const;

	friend class CVariantSaveDataOps;
};

const char* variant_t::ToString(void) const
{
	COMPILE_TIME_ASSERT(sizeof(string_t) == sizeof(int));

	static char szBuf[512];

	switch (fieldType)
	{
	case FIELD_STRING:
	{
		return(STRING(iszVal));
	}

	case FIELD_BOOLEAN:
	{
		if (bVal == 0)
		{
			Q_strncpy(szBuf, "false", sizeof(szBuf));
		}
		else
		{
			Q_strncpy(szBuf, "true", sizeof(szBuf));
		}
		return(szBuf);
	}

	case FIELD_INTEGER:
	{
		Q_snprintf(szBuf, sizeof(szBuf), "%i", iVal);
		return(szBuf);
	}

	case FIELD_FLOAT:
	{
		Q_snprintf(szBuf, sizeof(szBuf), "%g", flVal);
		return(szBuf);
	}

	case FIELD_COLOR32:
	{
		Q_snprintf(szBuf, sizeof(szBuf), "%d %d %d %d", (int)rgbaVal.r, (int)rgbaVal.g, (int)rgbaVal.b, (int)rgbaVal.a);
		return(szBuf);
	}

	case FIELD_VECTOR:
	{
		Q_snprintf(szBuf, sizeof(szBuf), "[%g %g %g]", (double)vecVal[0], (double)vecVal[1], (double)vecVal[2]);
		return(szBuf);
	}

	case FIELD_VOID:
	{
		szBuf[0] = '\0';
		return(szBuf);
	}

	case FIELD_EHANDLE:
	{
		//const char* pszName = (Entity()) ? STRING(Entity()->GetEntityName()) : "<<null entity>>";
		//Q_strncpy(szBuf, pszName, 512);
	}
	}

	return("No conversion to string");
}

void variant_t::Set(fieldtype_t ftype, void* data)
{
	fieldType = ftype;

	switch (ftype)
	{
	case FIELD_BOOLEAN:		bVal = *((bool*)data);				break;
	case FIELD_CHARACTER:	iVal = *((char*)data);				break;
	case FIELD_SHORT:		iVal = *((short*)data);			break;
	case FIELD_INTEGER:		iVal = *((int*)data);				break;
	case FIELD_STRING:		iszVal = *((string_t*)data);		break;
	case FIELD_FLOAT:		flVal = *((float*)data);			break;
	case FIELD_COLOR32:		rgbaVal = *((color32*)data);		break;

	case FIELD_VECTOR:
	case FIELD_POSITION_VECTOR:
	{
		vecVal[0] = ((float*)data)[0];
		vecVal[1] = ((float*)data)[1];
		vecVal[2] = ((float*)data)[2];
		break;
	}

	case FIELD_EHANDLE:		eVal = 0;			break;//Bad
	case FIELD_CLASSPTR:	eVal = 0;		break;//Bad
	case FIELD_VOID:
	default:
		iVal = 0; fieldType = FIELD_VOID;
		break;
	}
}

class CBaseEntityOutput
{
public:
	~CBaseEntityOutput();

	//void ParseEventAction(const char* EventData);
	//void AddEventAction(void* pEventAction);
	//void RemoveEventAction(void* pEventAction);

	//int Save(ISave& save);
	//int Restore(IRestore& restore, int elementCount);

	//int NumberOfElements(void);

	//float GetMaxDelay(void);

	//fieldtype_t ValueFieldType() { return m_Value.FieldType(); }

	void FireOutput(variant_t Value, CBaseEntity* pActivator, CBaseEntity* pCaller, float fDelay = 0);

	/// Delete every single action in the action list. 
	//void DeleteAllElements(void);

	//void* GetFirstAction() { return m_ActionList; }

	//const void* GetActionForTarget(string_t iSearchTarget) const;
protected:
	variant_t m_Value;
	void* m_ActionList;
	DECLARE_SIMPLE_DATADESC();

	CBaseEntityOutput() {} // this class cannot be created, only it's children

private:
	CBaseEntityOutput(CBaseEntityOutput&); // protect from accidental copying
};

template< class Type, fieldtype_t fieldType >
class CEntityOutputTemplate : public CBaseEntityOutput
{
public:
	//
	// Sets an initial value without firing the output.
	//
	void Init(Type value)
	{
		m_Value.Set(fieldType, &value);
	}

	//
	// Sets a value and fires the output.
	//
	void Set(Type value, CBaseEntity* pActivator, CBaseEntity* pCaller)
	{
		m_Value.Set(fieldType, &value);
		FireOutput(m_Value, pActivator, pCaller);
	}

	//
	// Returns the current value.
	//
	Type Get(void)
	{
		return *((Type*)&m_Value);
	}
};

typedef CEntityOutputTemplate<variant_t, FIELD_INPUT>		COutputVariant;