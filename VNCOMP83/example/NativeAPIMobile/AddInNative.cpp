
#include "stdafx.h"
#include "AddInNative.h"
#include "ConversionWchar.h"
#include <stdio.h>

#if defined(__ANDROID__) || defined(__APPLE__)

#include <wchar.h>

#endif //__ANDROID__ || __APPLE__

#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

namespace EXELLIO_DRIVER
{

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY
    
static const wchar_t *g_PropNames[] =
   {L"IsConnected"};

static const wchar_t *g_MethodNames[] =
   {L"PrintXReport",
    L"PrintZReport",
    L"PrintReceipt"};

static const wchar_t *g_PropNamesRu[] =
   {L"Подключен"};

static const wchar_t *g_MethodNamesRu[] =
    {L"ПоказатьУгловоеПоложениеУстройства"
     L"РаспечататьXОтчет",
     L"РаспечататьZОтчет",
     L"РаспечататьКвитанцию"};

static const wchar_t g_ComponentNameType[] = L"ExellioDriverExtension";
    
#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

static const char g_ComponentName[] = "ExellioDriver";

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY

// This component supports 2.1 version
const long g_VersionAddIn = 2100;

static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;

//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface = new ExellioDriver;
        return (long)*pInterface;
    }
    return 0;
}
    
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
    
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pInterface)
{
    if(!*pInterface)
        return -1;

    delete *pInterface;
    *pInterface = 0;
    return 0;
}
    
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    return s_kClassNames;
}
    
//---------------------------------------------------------------------------//
ExellioDriver::ExellioDriver() : m_iConnect(0), m_iMemory(0), isConnected(false)
{
}
    
//---------------------------------------------------------------------------//
ExellioDriver::~ExellioDriver()
{
}
    
/////////////////////////////////////////////////////////////////////////////
// IInitDoneBase
//---------------------------------------------------------------------------//
bool ExellioDriver::Init(void* pConnection)
{
    m_iConnect = (IAddInDefBaseEx*)pConnection;
    if (m_iConnect)
    {
        exellioDriver.setIConnect(m_iConnect);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------//
bool ExellioDriver::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}
    
//---------------------------------------------------------------------------//
long ExellioDriver::GetInfo()
{ 
    // Component should put supported component technology version
    return g_VersionAddIn;
}
    
//---------------------------------------------------------------------------//
void ExellioDriver::Done()
{
    m_iConnect = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool ExellioDriver::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    const wchar_t *wsExtension = g_ComponentNameType;
    uint32_t iActualSize = static_cast<uint32_t>(::wcslen(wsExtension) + 1);

    if (m_iMemory)
    {
        if(m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
        {
            convToShortWchar(wsExtensionName, wsExtension, iActualSize);
            return true;
        }
    }

    return false; 
}
    
//---------------------------------------------------------------------------//
long ExellioDriver::GetNProps()
{ 
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}
    
//---------------------------------------------------------------------------//
long ExellioDriver::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = 0;
    convFromShortWchar(&propName, wsPropName);
    
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}
    
//---------------------------------------------------------------------------//
const WCHAR_T* ExellioDriver::GetPropName(long lPropNum, long lPropAlias)
{ 
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsPropName = NULL;

    switch(lPropAlias)
    {
    case 0: // First language (english)
        wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
        break;
    case 1: // Second language (local)
        wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
        break;
    default:
        return 0;
    }
    
    uint32_t iActualSize = static_cast<uint32_t>(wcslen(wsCurrentName) + 1);

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
            convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
    }

    return wsPropName;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    switch(lPropNum)
    {
        case ePropIsConnected:
        {
            TV_VT(pvarPropVal) = VTYPE_BOOL;
            TV_BOOL(pvarPropVal) = isConnected;
            break;
        }
        default:
            return false;
    }
    return true;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
    return true;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::IsPropReadable(const long lPropNum)
{ 
    switch(lPropNum)
    { 
        case ePropIsConnected:
            return true;
    }
    return false;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::IsPropWritable(const long lPropNum)
{
    return false;
}

//---------------------------------------------------------------------------//
long ExellioDriver::GetNMethods()
{ 
    return eMethLast;
}
    
//---------------------------------------------------------------------------//
long ExellioDriver::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;
    convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}
    
//---------------------------------------------------------------------------//
const WCHAR_T* ExellioDriver::GetMethodName(const long lMethodNum, const long lMethodAlias)
{ 
    if (lMethodNum >= eMethLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsMethodName = NULL;

    switch(lMethodAlias)
    {
    case 0: // First language (english)
        wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
        break;
    case 1: // Second language (local)
        wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
        break;
    default: 
        return 0;
    }

    uint32_t iActualSize = static_cast<uint32_t>(wcslen(wsCurrentName) + 1);

    if (m_iMemory && wsCurrentName)
    {
        if(m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
            convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
    
//---------------------------------------------------------------------------//
long ExellioDriver::GetNParams(const long lMethodNum)
{
    return 0;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::GetParamDefValue(const long lMethodNum, const long lParamNum,
                        tVariant *pvarParamDefValue)
{ 
    // There are no parameter values by default
    TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
    
    switch (lMethodNum)
    {
        case eMethPrintZReport:
        case eMethPrintXReport:
        case eMethPrintReceipt:
        {
            // No parameters
            return false;
        }
    }
    return false;
}
    
    //---------------------------------------------------------------------------//
bool ExellioDriver::HasRetVal(const long lMethodNum)
{
    return false;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{
    return true;
}

//---------------------------------------------------------------------------//
bool ExellioDriver::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    switch(lMethodNum)
    {
        case eMethPrintXReport:
            exellioDriver.printXReport();
            return true;
            break;
        case eMethPrintZReport:
            exellioDriver.printZReport();
            return true;
            break;
        case eMethPrintReceipt:
            exellioDriver.printReceipt();
            return true;
            break;
        default:
            return false;
    }
    return false;
}
    
    /////////////////////////////////////////////////////////////////////////////
// ILocaleBase
//---------------------------------------------------------------------------//
void ExellioDriver::SetLocale(const WCHAR_T* loc)
{
}

/////////////////////////////////////////////////////////////////////////////
// Other

//---------------------------------------------------------------------------//
void ExellioDriver::addError(uint32_t wcode, const wchar_t* source,
                        const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T *err = 0;
        WCHAR_T *descr = 0;
        
        convToShortWchar(&err, source);
        convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);

        delete[] descr;
        delete[] err;
    }
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::sendMessageFromEvent(const wchar_t* textMsg)
{
    if (m_iConnect)
    {
        m_iConnect->ExternalEvent(s_classNameExellioDriver, s_eventChangeName, s_paramEvent);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------//
long ExellioDriver::findName(const wchar_t* names[], const wchar_t* name,
                        const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}

//---------------------------------------------------------------------------//
#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

};

namespace EXELLIO_DRIVER
{

    static LPCVOID addin_exports[] =
    {
        "GetClassObject", (LPCVOID)GetClassObject,
        "DestroyObject", (LPCVOID)DestroyObject,
        "GetClassNames", (LPCVOID)GetClassNames,
        "SetPlatformCapabilities", (LPCVOID)SetPlatformCapabilities,
        NULL
    };

    DECLARE_DLL((const char*)g_ComponentName, addin_exports);
}

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY
