
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
   {L"ConnectionState"};

static const wchar_t *g_MethodNames[] =
   {L"PrintXReport",
    L"PrintZReport",
    L"PrintReceipt",
    L"RunCustomCommand",
    L"GetResult",
    L"IsConnected",
    L"TurnDeviceOn",
    L"TurnDeviceOff",
    L"GetBatteryCapacity",
    L"GetLineaProBatteryCapacity",
    L"GetLineaProCharging",
    L"TurnOnLineaProCharging",
    L"TurnOffLineaProCharging",
       "BluePad50Connect"
       "BluePad50StartTransaction"
       "BluePad50GetReceipt"
   };

static const wchar_t *g_PropNamesRu[] =
   {L"СостояниеПодключения"};

static const wchar_t *g_MethodNamesRu[] =
    {L"РаспечататьXОтчет",
     L"РаспечататьZОтчет",
     L"РаспечататьКвитанцию",
     L"ЗупуститьКоманду",
     L"ПолучитьРезультат",
     L"УстройствоПодключено",
     L"ВключитьУстройство",
     L"ВыключитьУстройство",
     L"ПолучитьЕмкостьБатареи",
     L"ПолучитьLineaProЕмкостьБатареи",
     L"ПолучитьLineaProЗаряжает",
     L"ВключитьLineaProЗаряжает",
     L"ВыключитьLineaProЗаряжает"
    };

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
    fprintf(stdout, "%s \n", __PRETTY_FUNCTION__);
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
    fprintf(stdout, "%s \n", __PRETTY_FUNCTION__);
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
    fprintf(stdout, "%s \n", __PRETTY_FUNCTION__);
}
    
//---------------------------------------------------------------------------//
ExellioDriver::~ExellioDriver()
{
    fprintf(stdout, "%s \n", __PRETTY_FUNCTION__);
}
    
/////////////////////////////////////////////////////////////////////////////
// IInitDoneBase
//---------------------------------------------------------------------------//
bool ExellioDriver::Init(void* pConnection)
{
    fprintf(stdout, "%s \n", __PRETTY_FUNCTION__);
    
    m_iConnect = (IAddInDefBaseEx*)pConnection;
    if (m_iConnect)
    {
        cExellioDriver.setIConnect(m_iConnect);
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
        case ePropConnectionState:
        {
            TV_VT(pvarPropVal) = VTYPE_I4;
            TV_I4(pvarPropVal) = cExellioDriver.getConnectionState();
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
    return true;
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
    fprintf(stdout, "%s, %ld \n", __PRETTY_FUNCTION__, lMethodNum);
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
    fprintf(stdout, "%s, %ld \n", __PRETTY_FUNCTION__, lMethodNum);
    
    switch (lMethodNum)
    {
        case eMethRunCustomCommand:
            return 2;
    }
    return 0;
}
    
//---------------------------------------------------------------------------//
bool ExellioDriver::GetParamDefValue(const long lMethodNum, const long lParamNum,
                        tVariant *pvarParamDefValue)
{
    fprintf(stdout, "%s, %ld \n", __PRETTY_FUNCTION__, lMethodNum);
    // There are no parameter values by default
    TV_VT(pvarParamDefValue) = VTYPE_EMPTY;
    
    switch (lMethodNum)
    {
        case eMethPrintZReport:
        case eMethPrintXReport:
        case eMethPrintReceipt:
        case eMethRunCustomCommand:
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
    fprintf(stdout, "%s, %ld \n", __PRETTY_FUNCTION__, lMethodNum);
    
    switch (lMethodNum)
    {
        case eMethIsConnected:
        case eMethGetResult:
        case eMethGetBatteryCapacity:
        case eMethGetLineaProBatteryCapacity:
        case eMethGetLineaProCharging:
            return true;
            break;
        default:
            return false;
    }
    return false;
}
    
    
void printVariant(tVariant paParams)
{
    fprintf(stdout, "i8Val: %hhd \n", paParams.i8Val);
    fprintf(stdout, "shortVal: %hd \n", paParams.shortVal);
    fprintf(stdout, "lVal: %d \n", paParams.lVal);
    fprintf(stdout, "intVal: %d \n", paParams.intVal);
    fprintf(stdout, "uintVal: %u \n", paParams.uintVal);
    fprintf(stdout, "llVal: %lld \n", paParams.llVal);
    fprintf(stdout, "ui8Val: %hhu \n", paParams.ui8Val);
    fprintf(stdout, "ushortVal: %hu \n", paParams.ushortVal);
    fprintf(stdout, "ulVal: %u \n", paParams.ulVal);
    fprintf(stdout, "ullVal: %llu \n", paParams.ullVal);
    fprintf(stdout, "errCode: %d \n", paParams.errCode);
    fprintf(stdout, "hRes: %ld \n", paParams.hRes);
    fprintf(stdout, "fltVal: %f \n", paParams.fltVal);
    fprintf(stdout, "dblVal: %f \n", paParams.dblVal);
    fprintf(stdout, "bVal: %d \n", paParams.bVal);
    fprintf(stdout, "chVal: %c \n", paParams.chVal);
    //fprintf(stdout, "%s \n", paParams->wchVal);
    fprintf(stdout, "date: %f \n", paParams.date);
    //fprintf(stdout, "%s \n", paParams->IDVal);
    //fprintf(stdout, "%s \n", paParams->pvarVal);
    //fprintf(stdout, "%s \n", paParams->tmVal);
    //fprintf(stdout, "%s \n", paParams->pInterfaceVal);
    //fprintf(stdout, "%s \n", paParams->InterfaceID);
    
    fprintf(stdout, "strLen: %u \n", paParams.strLen);
    if (paParams.strLen > 0) {
        char *str = paParams.pstrVal;
        
        for (int i = 0; i < paParams.strLen; i++) {
            fprintf(stdout, "pstrVal: %c \n", str[i]);
        }
        
        //fprintf(stdout, "pstrVal: %s \n", str);
    } else {
        fprintf(stdout, "pstrVal: NULL \n");
    }
    
    fprintf(stdout, "wstrLen: %u \n", paParams.wstrLen);
    if (paParams.wstrLen > 0) {
        
        for (int i = 0; i < paParams.wstrLen; i++) {
            //fprintf(stdout, "pwstrVal: %c \n", paParams.pwstrVal[i]);
            fprintf(stdout, "pwstrVal: %d \n", paParams.pwstrVal[i]);
        }
        
        
        //fprintf(stdout, "pwstrVal: %s \n", paParams.pwstrVal);
    } else {
        fprintf(stdout, "pwstrVal: NULL \n");
    }

    fprintf(stdout, "cbElements: %u \n", paParams.cbElements);
    fprintf(stdout, "vt: %hu \n", paParams.vt);
}
    
    //---------------------------------------------------------------------------//
bool ExellioDriver::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{
    fprintf(stdout, "%s, %ld, lSizeArray: %ld \n", __PRETTY_FUNCTION__, lMethodNum, lSizeArray);
    if (paParams) {
        printVariant(paParams[0]);
        printVariant(paParams[1]);
    }
    
    switch(lMethodNum)
    {
        case eMethPrintXReport:
            cExellioDriver.printXReport();
            break;
        case eMethPrintZReport:
            cExellioDriver.printZReport();
            break;
        case eMethPrintReceipt:
            cExellioDriver.printReceipt();
            break;
        case eMethRunCustomCommand: {
            if (!paParams || lSizeArray != 2) {
                return false;
            }
            
            tVariant cmd = paParams[0];
            tVariant data = paParams[1];
            
            cExellioDriver.runCustomCommand(cmd, data);
            break;
        }
        case eMethTurnDeviceOn:
            cExellioDriver.turnDeviceOn();
            break;
        case eMethTurnDeviceOff:
            cExellioDriver.turnDeviceOff();
            break;
        case eMethTurnOnLineaProCharging:
            cExellioDriver.turnOnLineaProCharging();
            break;
        case eMethTurnOffLineaProCharging:
            cExellioDriver.turnOffLineaProCharging();
            break;
        default:
            return false;
    }
    
    return true;
}

//---------------------------------------------------------------------------//
bool ExellioDriver::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
    fprintf(stdout, "%s, %ld \n", __PRETTY_FUNCTION__, lMethodNum);
    
    switch(lMethodNum)
    {
        case eMethIsConnected:
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = cExellioDriver.isConnected();
            return true;
            break;
        case eMethGetResult:
        {
            // No parameters
            if (lSizeArray || paParams)
                return false;
            
            WCHAR_T *result = 0;
            TV_VT(pvarRetValue) = VTYPE_PWSTR;

            const wchar_t* exellioResult = cExellioDriver.getResult();
            
            uint32_t iActualSize = static_cast<uint32_t>(::wcslen(exellioResult) + 1);

            if (m_iMemory)
            {
                if(m_iMemory->AllocMemory((void**)&result, iActualSize * sizeof(WCHAR_T)))
                {
                    convToShortWchar(&result, exellioResult, iActualSize);
                    pvarRetValue->pwstrVal = result;
                    pvarRetValue->wstrLen = static_cast<uint32_t>(::wcslen(exellioResult));
                    
                    printVariant(*pvarRetValue);
                }
            }
            
            delete[] exellioResult;
            
            return true;
            break;
        }
        case eMethGetBatteryCapacity: {
            WCHAR_T *capacity = 0;
            TV_VT(pvarRetValue) = VTYPE_PWSTR;
            
            const wchar_t* w_capacity = cExellioDriver.getBatteryCapacity();
            
            uint32_t iActualSize = static_cast<uint32_t>(::wcslen(w_capacity) + 1);
            
            if (m_iMemory)
            {
                if(m_iMemory->AllocMemory((void**)&capacity, iActualSize * sizeof(WCHAR_T)))
                {
                    convToShortWchar(&capacity, w_capacity, iActualSize);
                    pvarRetValue->pwstrVal = capacity;
                    pvarRetValue->wstrLen = static_cast<uint32_t>(::wcslen(w_capacity));
                }
            }
            
            delete[] w_capacity;
            return true;
            break;
        }
        case eMethGetLineaProBatteryCapacity: {
            WCHAR_T *capacity = 0;
            TV_VT(pvarRetValue) = VTYPE_PWSTR;
            
            const wchar_t* w_capacity = cExellioDriver.getLineaProBatteryCapacity();
            
            uint32_t iActualSize = static_cast<uint32_t>(::wcslen(w_capacity) + 1);
            
            if (m_iMemory)
            {
                if(m_iMemory->AllocMemory((void**)&capacity, iActualSize * sizeof(WCHAR_T)))
                {
                    convToShortWchar(&capacity, w_capacity, iActualSize);
                    pvarRetValue->pwstrVal = capacity;
                    pvarRetValue->wstrLen = static_cast<uint32_t>(::wcslen(w_capacity));
                }
            }
            
            delete[] w_capacity;
            return true;
            break;
        }
        case eMethGetLineaProCharging:
            TV_VT(pvarRetValue) = VTYPE_BOOL;
            TV_BOOL(pvarRetValue) = cExellioDriver.getLineaProCharging();
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
    fprintf(stdout, "%s, textMsg: %ls \n", __PRETTY_FUNCTION__, textMsg);
//    if (m_iConnect)
//    {
//        m_iConnect->ExternalEvent(s_classNameExellioDriver, s_eventChangeName, s_paramEvent);
//        return true;
//    }
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
