
#ifndef __NATIVEAPIEXELLIODRIVER_H__
#define __NATIVEAPIEXELLIODRIVER_H__

#include "ConversionWchar.h"
#include <time.h>
#include "include/mobile.h"

#if defined(__OBJC__)

@class ExellioDriverHelper;
@class LineaProDriverHelper;
@class BluePad50DriverHelper;

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
    
static const wchar_t g_bluePad50OnConnectionStatusChange[] = L"BluePad50OnConnectionStatusChange";
static const wchar_t g_bluePad50OnTransactionFinish[] = L"BluePad50OnTransactionFinish";
static const wchar_t g_bluePad50OnReverseRequestFinish[] = L"BluePad50OnReverseRequestFinish";
static const wchar_t g_ClassBluePad50Driver[] = L"BluePad50Driver";

static WcharWrapper s_bluePad50OnConnectionStatusChange(g_bluePad50OnConnectionStatusChange);
static WcharWrapper s_bluePad50OnTransactionFinish(g_bluePad50OnTransactionFinish);
static WcharWrapper s_bluePad50OnReverseRequestFinish(g_bluePad50OnReverseRequestFinish);
static WcharWrapper s_classNameBluePad50Driver(g_ClassBluePad50Driver);
    

#if defined(__APPLE__) 

#if !defined(__OBJC__)

    class ExellioDriverHelper;
    class LineaProDriverHelper;
    class BluePad50DriverHelper;
    
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
        
        void BluePad50Connect();
        void BluePad50StartTransaction(tVariant amount, tVariant purpose, tVariant token);
        void BluePad50ReverseRequest(tVariant transactionId, tVariant serialNumber, tVariant token);
        
        void setIConnect(IAddInDefBaseEx* piConnect);
        
    private:
        IAddInDefBaseEx*    m_iCnt;
        ExellioDriverHelper *exellioDriverHelper;
        LineaProDriverHelper *lineaProDriverHelper;
        BluePad50DriverHelper *bluePad50DriverHelper;
    };
    
#if defined(__APPLE__) && !defined(BUILD_DYNAMIC_LIBRARY)

};

#endif //__APPLE__ && !BUILD_DYNAMIC_LIBRARY
    
#endif //__NATIVEAPIEXELLIODRIVER_H__

