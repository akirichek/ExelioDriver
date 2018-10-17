
#ifndef __NATIVEAPIEXELLIODRIVER_H__
#define __NATIVEAPIEXELLIODRIVER_H__

#include "ConversionWchar.h"
#include <time.h>
#include "include/mobile.h"

#if defined(__OBJC__)

@class ExellioDriverHelper;
@class LineaProDriverHelper;

#endif //__OBJC__

class IAddInDefBaseEx;

class IMemoryManager;

#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

namespace EXELLIO_DRIVER 
{
    
#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY

static const wchar_t g_kClassNames[] = L"ExellioDriver";
static const wchar_t g_exellioDriverResult[] = L"OnExellioDriverResult";
static const wchar_t g_ClassExellioDriver[] = L"ExellioDriver";

static WcharWrapper s_kClassNames(g_kClassNames);
static WcharWrapper s_exellioDriverResult(g_exellioDriverResult);
static WcharWrapper s_classNameExellioDriver(g_ClassExellioDriver);
    
//BluePad50OnConnectionStatusChange
//BluePad50OnTransactionFinish
    //       "BluePad50Connect"
    "BluePad50StartTransaction"
static const wchar_t g_connectionStatusChanged[] = L"OnConnectionStatusChange";
static const wchar_t g_barcodeScanned[] = L"OnBarcodeScan";
static const wchar_t g_magneticCardScanned[] = L"OnMagneticCardScan";
static const wchar_t g_magneticCardEncryptedScanned[] = L"OnMagneticCardEncryptedScan";
static const wchar_t g_ClassLineaProDriver[] = L"LineaProDriver";

static WcharWrapper s_connectionStatusChanged(g_connectionStatusChanged);
static WcharWrapper s_barcodeScanned(g_barcodeScanned);
static WcharWrapper s_magneticCardScanned(g_magneticCardScanned);
static WcharWrapper s_magneticCardEncryptedScanned(g_magneticCardEncryptedScanned);
static WcharWrapper s_classNameLineoProDriver(g_ClassLineaProDriver);
    
    
    

#if defined(__APPLE__) 

#if !defined(__OBJC__)

    class ExellioDriverHelper;
    class LineaProDriverHelper;
    
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
        void runCustomCommand(tVariant cmd, tVariant data);
        const wchar_t* getResult();
        bool isConnected();
        
        int getConnectionState();
        void turnDeviceOn();
        void turnDeviceOff();
        const wchar_t* getBatteryCapacity();
        const wchar_t* getLineaProBatteryCapacity();
        bool getLineaProCharging();
        void turnOnLineaProCharging();
        void turnOffLineaProCharging();
        
        void setIConnect(IAddInDefBaseEx* piConnect);
        
    private:
        IAddInDefBaseEx*    m_iCnt;
        ExellioDriverHelper *exellioDriverHelper;
        LineaProDriverHelper *lineaProDriverHelper;
    };
    
#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

};

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY
    
#endif //__NATIVEAPIEXELLIODRIVER_H__

