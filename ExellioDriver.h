
#ifndef __NATIVEAPIEXELLIODRIVER_H__
#define __NATIVEAPIEXELLIODRIVER_H__

#include "ConversionWchar.h"
#include <time.h>
#include "include/mobile.h"

#if defined(__OBJC__)

@class ExellioDriverHelper;

#endif //__OBJC__

class IAddInDefBaseEx;

class IMemoryManager;

#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

namespace EXELLIO_DRIVER
{
    
#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY

static const wchar_t g_kClassNames[] = L"ExellioDriver";
static const wchar_t g_eventChangeName[] = L"OnChange";
static const wchar_t g_paramEvent[] = L"_";
static const wchar_t g_ClassExellioDriver[] = L"ExellioDriver";

static WcharWrapper s_kClassNames(g_kClassNames);
static WcharWrapper s_eventChangeName(g_eventChangeName);
static WcharWrapper s_paramEvent(g_paramEvent);
static WcharWrapper s_classNameExellioDriver(g_ClassExellioDriver);

#if defined(__APPLE__) 

#if !defined(__OBJC__)

    class ExellioDriverHelper;
    
#endif //!__OBJC__
    
#elif defined(MOBILE_PLATFORM_WINRT) //__APPLE__

    ref class ExellioDriverHelper;
    
#else //MOBILE_PLATFORM_WINRT

    class ExellioDriverHelper;
    
#endif //!__APPLE__ && !MOBILE_PLATFORM_WINRT

    class CExellioDriver
    {
 
    public:
        CExellioDriver();
        ~CExellioDriver();
    
        void printXReport();
        void printZReport();
        void printReceipt();
        void setIConnect(IAddInDefBaseEx* piConnect);
    private:
        IAddInDefBaseEx*    m_iCnt;
        ExellioDriverHelper *exellioDriverHelper;
    };
    
#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

};

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY
    
#endif //__NATIVEAPIEXELLIODRIVER_H__

