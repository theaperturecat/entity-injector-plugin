#pragma once

class PdbParser
{
    IDiaDataSource* pDiaDataSource = NULL;
    IDiaSession* pDiaSession = NULL;
    IDiaSymbol* pGlobalScope = NULL;

public:
    PdbParser(const char* pdb);
    ~PdbParser();
    DWORD LookupSymbol(const char* symbolName);
    DWORD LookupVTable(const char * classname);
};