#include "baseentity_stub.h"
#include "Windows.h"
#include <atlcomcli.h>
#include "E:\vs2022ide\DIA SDK\include/dia2.h"
#include "DbgHelp.h"
//#include "util.h"

#include "pdbparser.h"

void Error()
{
    Warning("Error occured with pdb parser");
}

const wchar_t* GetWinChar(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

//CoInitialize(NULL); // Initialize COM
//CoUninitialize();

PdbParser::PdbParser(const char* pdb)
{
    const wchar_t* wPdb = GetWinChar(pdb);

    // Create an instance of the DIA data source

    HRESULT hr = CoCreateInstance(CLSID_DiaSource, NULL, CLSCTX_INPROC_SERVER, IID_IDiaDataSource, (void**)&pDiaDataSource);

    if (FAILED(hr))
    {
        Error();
        Warning("could not create dia src\n");
    }

    // Load the PDB file
    hr = pDiaDataSource->loadDataFromPdb(wPdb);

    delete wPdb;

    if (FAILED(hr))
    {
        Error();
        Warning("no pdb\n");
        pDiaDataSource->Release();
        CoUninitialize();
    }

    // Open a session to query symbols

    hr = pDiaDataSource->openSession(&pDiaSession);
    if (FAILED(hr))
    {
        Error();
        Warning("no session\n");
        pDiaDataSource->Release();
        CoUninitialize();
    }


    hr = pDiaSession->get_globalScope(&pGlobalScope);
    if (FAILED(hr))
    {
        Error();
        Warning("no globscope\n");
        pDiaSession->Release();
        pDiaDataSource->Release();
        CoUninitialize();
    }


}

PdbParser::~PdbParser()
{
    pGlobalScope->Release();
    pDiaSession->Release();
    pDiaDataSource->Release();
}


//pass symbol name, returns RVA
DWORD PdbParser::LookupSymbol(const char* symbolName)
{
    const wchar_t* wsymbolName = GetWinChar(symbolName);
    //BSTR symbolName = SysAllocString(L"ImpRendererFactory::s_pInstance");
    IDiaEnumSymbols* pEnumSymbols = NULL;
    HRESULT hr = pGlobalScope->findChildren(SymTagNull, wsymbolName, nsNone, &pEnumSymbols);
    //SysFreeString(symbolName);
    delete wsymbolName;

    if (FAILED(hr))
    {
        return 0;
    }

    // Enumerate over the found symbols
    IDiaSymbol* pFunctionSymbol = NULL;
    ULONG celt = 0;

    while (pEnumSymbols->Next(1, &pFunctionSymbol, &celt) == S_OK && celt == 1)
    {
        DWORD tag;
        pFunctionSymbol->get_symTag(&tag);
        if (true)
        {
            BSTR functionName = NULL;
            pFunctionSymbol->get_name(&functionName);
            char buff[100];

            wcstombs(buff, functionName, 90);

            SysFreeString(functionName);


            DWORD_PTR functionAddress = 0;

            pFunctionSymbol->get_relativeVirtualAddress(&functionAddress);

            pFunctionSymbol->Release();
            pEnumSymbols->Release();

            return functionAddress;
        }
        
    }

    // Cleanup
    if(pFunctionSymbol)
    pFunctionSymbol->Release();
    pEnumSymbols->Release();
    
    Error();

    return 0;
}

DWORD PdbParser::LookupVTable(const char* className)
{
    /*const wchar_t* wsymbolName = GetWinChar(className);
    //BSTR symbolName = SysAllocString(L"ImpRendererFactory::s_pInstance");
    IDiaEnumSymbols* pEnumSymbols = NULL;
    HRESULT hr = pGlobalScope->findChildren(SymTagUDT, NULL, nsNone, &pEnumSymbols);
    //SysFreeString(symbolName);
    delete wsymbolName;

    if (FAILED(hr))
    {
        return 0;
    }




    // Iterate through all UDT symbols (classes, structs, enums, and unions)
    IDiaSymbol* pUDTSymbol = NULL;
    ULONG celt = 0;
    while (pEnumSymbols->Next(1, &pUDTSymbol, &celt) == S_OK && celt == 1) {
        // Check if this symbol is a class (i.e., user-defined type)
        DWORD dwSymTag = 0;
        pUDTSymbol->get_symTag(&dwSymTag);
        if (dwSymTag == SymTagUDT) {
            BSTR className = NULL;
            pUDTSymbol->get_name(&className);
            wprintf(L"Found class: %s\n", className);

            // Cleanup
            SysFreeString(className);

            IDiaEnumSymbols* pEnumSymbols2 = NULL;
            HRESULT hr = pUDTSymbol->findChildren(SymTagNull, NULL, nsNone, &pEnumSymbols2);


            if (FAILED(hr))
            {
                return 0;
            }


            // Enumerate over the found symbols
            IDiaSymbol* pFunctionSymbol = NULL;
            ULONG celt2 = 0;

            while (pEnumSymbols2->Next(1, &pFunctionSymbol, &celt2) == S_OK && celt2 == 1)
            {
                DWORD tag;
                pFunctionSymbol->get_symTag(&tag);
                if (true)
                {
                    BSTR functionName = NULL;
                    pFunctionSymbol->get_name(&functionName);
                    char buff[100];

                    wcstombs(buff, functionName, 90);

                    Msg("VTable name: %s\n", buff);

                    SysFreeString(functionName);


                    DWORD_PTR functionAddress = 0;

                    pFunctionSymbol->get_relativeVirtualAddress(&functionAddress);

                    pFunctionSymbol->Release();
                    pEnumSymbols2->Release();

                    //return functionAddress;
                }

            }

            pEnumSymbols2->Release();



        }

        pUDTSymbol->Release();


    }



    pEnumSymbols->Release();




    Error();*/

    return 0;
}