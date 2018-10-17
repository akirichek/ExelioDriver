
#include "ExellioDriver.h"
#include "include/AddInDefBase.h"
#import <ExellioFiscalDriver/ExellioFiscalDriver.h>
#import <Foundation/Foundation.h>
#import "DTDevices.h"
#import "Utils.h"

namespace EXELLIO_DRIVER
{

};

using namespace EXELLIO_DRIVER;


@interface ExellioDriverHelper: NSObject <EAFiscalProtocol>
{
    IAddInDefBaseEx         *m_iConn;
}

@property (strong, nonatomic) ExellioFiscalDriver *fpu;
@property (strong, nonatomic) NSString *result;
@property (assign, nonatomic) BOOL isConnected;

+ (instancetype)sharedInstanceWithConnect:(IAddInDefBaseEx*)piConnect;

- (void)printXReport;
- (void)printZReport;
- (void)printReceipt;
- (void)dealloc;

@end;

@implementation ExellioDriverHelper

+ (instancetype)sharedInstanceWithConnect:(IAddInDefBaseEx*)piConnect
{
    static ExellioDriverHelper *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[ExellioDriverHelper alloc] init];
    });
    sharedInstance->m_iConn = piConnect;
    return sharedInstance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.fpu = [[ExellioFiscalDriver alloc]initUseAutoConnection: true];
        [self.fpu addDelegate:self];
    }
    return self;
}

-(void)dealloc {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    
 //   [self.fpu removeDelegate:self];
//    [self.fpu dealloc];
    
    //m_iConn = NULL;
    [super dealloc];
}

-(void)EAFiscalConnectedSerial:(NSString *)serial {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSLog(@"ED Connected %@",serial);
    self.isConnected = YES;
}
-(void)EAFiscalAnswerForPacket:(receivePacket *)packet {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSString *initStr = [[NSString alloc]initWithData:[packet data] encoding:NSWindowsCP1251StringEncoding];
    self.result = initStr;
    NSLog(@"ED EAFiscalAnswerForPacket %@, event buffer depth: %ld",[initStr componentsSeparatedByString:@","], m_iConn->GetEventBufferDepth());
    
    NSString *resultEvent = [NSString stringWithFormat:@"%d|%@", packet.cmd, initStr];
    wchar_t *g_paramEvent = [Utils wcharFromNSString:resultEvent];
    WCHAR_T *s_paramEvent = 0;
    convToShortWchar(&s_paramEvent, g_paramEvent);

    if (m_iConn) {
        m_iConn->ExternalEvent(s_classNameExellioDriver, s_exellioDriverResult, s_paramEvent);
    }

    delete[] s_paramEvent;
    delete[] g_paramEvent;

}
-(void)EAFiscalDisconnected {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSLog(@"ED Disconnected");
    self.isConnected = NO;
}
-(void)EAFiscalError:(NSNumber *)errorCode {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSLog(@"ED Error -%@",errorCode);
}

- (void)runCustomCommand:(NSInteger)command withData:(NSString *)data  {
    [self.fpu writeCommand:command withData:data];
}

- (void)printXReport {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    [self.fpu PrintXReport];
}

-(void)printZReport {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    [self.fpu PrintZReport];
}

- (void)printReceipt {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    receiptStage=0;
    [self.fpu OpenFiscalReceiptForDefaultOperatorAndCallMethod:@selector(receiptWorker:) ofObject:self];
}

int receiptStage;
-(void)receiptWorker:(receivePacket*)packet {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    
    switch (receiptStage) {
        case 0:
            [self.fpu saleItemCode:12 Tax:taxGroupA Price:12.27 ArtName:@"Название артикула" Quantity:1 PercentDiscount:0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 1:
            [self.fpu saleItemCode:12 Tax:taxGroupA Price:14.27 ArtName:@"Название артикула" Quantity:1 PercentDiscount:0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 2:
            [self.fpu saleItemCode:12 Tax:taxGroupA Price:10.27 ArtName:@"Название артикула" Quantity:2 PercentDiscount:-5.0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 3:
            [self.fpu CloseFiscalReceiptAndPayAndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;

        default:
            break;
    }
}

@end

//----------------------------------------------------------------------------//

#if !defined(BUILD_DYNAMIC_LIBRARY)

namespace EXELLIO_DRIVER
{

#endif //!BUILD_DYNAMIC_LIBRARY
    
bool CExellioDriver::isConnected()
{
    __block BOOL isConnected = NO;
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        isConnected = exellioDriverHelper.isConnected;
        
    });
    
    NSLog(@"%s isConnected: %d", __PRETTY_FUNCTION__, isConnected);
    
    return isConnected;
}

void CExellioDriver::printXReport()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    dispatch_async(dispatch_get_main_queue(), ^{
        [exellioDriverHelper printXReport];
    });
}

void CExellioDriver::printZReport()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    dispatch_async(dispatch_get_main_queue(), ^{
        [exellioDriverHelper printZReport];
    });
}

void CExellioDriver::printReceipt()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    dispatch_async(dispatch_get_main_queue(), ^{
        [exellioDriverHelper printReceipt];
    });
}
    
void CExellioDriver::runCustomCommand(tVariant cmd, tVariant data)
{
    NSInteger cmdNumber = cmd.intVal;
    NSString *args = [NSString stringWithCharacters:data.pwstrVal length:data.wstrLen];
    
    NSLog(@"%s cmd: %ld, params: %@", __PRETTY_FUNCTION__, (long)cmdNumber, args);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [exellioDriverHelper runCustomCommand:cmdNumber
                                     withData:args];
    });
}
    
const wchar_t* CExellioDriver::getResult()
{
    __block NSString *result = @"";
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        result = exellioDriverHelper.result;
    });

    NSLog(@"%s %@", __PRETTY_FUNCTION__, result);
    
    size_t length = result.length;
    
    NSLog(@"%s %@, length: %lu", __PRETTY_FUNCTION__, result, length);
    
    wchar_t* exellioResult = new wchar_t[length + 3];
    
    for (int i = 0; i < length; i++) {
        if (i < length) {
            exellioResult[i] = [result characterAtIndex:i];
        }
    }
    
    exellioResult[length] = 0x00;
    exellioResult[length + 1] = 0x00;
    exellioResult[length + 2] = 0x00;
    
    return exellioResult;
}
    
#if !defined(BUILD_DYNAMIC_LIBRARY)

}

#endif //!BUILD_DYNAMIC_LIBRARY



@interface LineaProDriverHelper: NSObject <DTDeviceDelegate>
{
    IAddInDefBaseEx         *m_iConn;
}

@property (strong, nonatomic) DTDevices *lib;

+ (instancetype)sharedInstanceWithConnect:(IAddInDefBaseEx*)piConnect;

- (int)connectionState;

- (void)turnDeviceOn;
- (void)turnDeviceOff;
- (void)dealloc;

@end;

@implementation LineaProDriverHelper

+ (instancetype)sharedInstanceWithConnect:(IAddInDefBaseEx*)piConnect
{
    static LineaProDriverHelper *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[LineaProDriverHelper alloc] init];
    });
    sharedInstance->m_iConn = piConnect;
    return sharedInstance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.lib = [DTDevices sharedDevice];
        [self.lib addDelegate:self];
    }
    return self;
}

-(void)dealloc {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    
    m_iConn = NULL;
    [super dealloc];
}

- (int)connectionState {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    return self.lib.connstate;
}

- (void)turnDeviceOn {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    [self.lib connect];
}

- (void)turnDeviceOff {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    [self.lib disconnect];
}

- (NSString *)getBatteryCapacity {
    UIDevice *myDevice = [UIDevice currentDevice];
    [myDevice setBatteryMonitoringEnabled:YES];
    int batteryLevel = [myDevice batteryLevel] * 100;
    NSLog(@"<%@:%@> %d", NSStringFromClass([self class]), NSStringFromSelector(_cmd), batteryLevel);
    return [NSString stringWithFormat:@"%d", batteryLevel];
}

- (NSString *)getLineaProBatteryCapacity {
    int capacity = 0;
    NSError *error = nil;
    [self.lib getBatteryCapacity:&capacity voltage:nil error:&error];
    NSLog(@"<%@:%@> %d", NSStringFromClass([self class]), NSStringFromSelector(_cmd), capacity);
    return [NSString stringWithFormat:@"%d", capacity];
}

- (BOOL)getLineaProCharging {
    BOOL charging = NO;
    NSError *error = nil;
    [self.lib getCharging:&charging error:&error];
    NSLog(@"<%@:%@> %d", NSStringFromClass([self class]), NSStringFromSelector(_cmd), charging);
    return charging;
}

- (void)turnOnLineaProCharging {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSError *error = nil;
    [self.lib setCharging:YES error:&error];
}

- (void)turnOffLineaProCharging {
    NSLog(@"<%@:%@>", NSStringFromClass([self class]), NSStringFromSelector(_cmd));
    NSError *error = nil;
    [self.lib setCharging:NO error:&error];
}

#pragma mark - DTDeviceDelegate

- (void)connectionState:(int)state {
    NSLog(@"<%@:%@> state: %d, event buffer depth: %ld", NSStringFromClass([self class]), NSStringFromSelector(_cmd), state, m_iConn->GetEventBufferDepth());
    
    wchar_t *g_paramEvent = [Utils wcharFromNSString:[NSString stringWithFormat:@"%d", state]];
    WCHAR_T *s_paramEvent = 0;
    convToShortWchar(&s_paramEvent, g_paramEvent);
    
    if (m_iConn) {
        m_iConn->ExternalEvent(s_classNameLineoProDriver, s_connectionStatusChanged, s_paramEvent);
    }
    
    delete[] s_paramEvent;
    delete[] g_paramEvent;
}

- (void)barcodeData:(NSString *)barcode type:(int)type {
    NSLog(@"<%@:%@> barcode: %@, type: %d, event buffer depth: %ld", NSStringFromClass([self class]), NSStringFromSelector(_cmd), barcode, type, m_iConn->GetEventBufferDepth());
    
    NSString *barcodeData = [NSString stringWithFormat:@"%@|%d", barcode, type];
    wchar_t *g_paramEvent = [Utils wcharFromNSString:barcodeData];
    WCHAR_T *s_paramEvent = 0;
    convToShortWchar(&s_paramEvent, g_paramEvent);
    
    if (m_iConn) {
        m_iConn->ExternalEvent(s_classNameLineoProDriver, s_barcodeScanned, s_paramEvent);
    }
    
    delete[] s_paramEvent;
    delete[] g_paramEvent;
}

- (void)magneticCardData:(NSString *)track1 track2:(NSString *)track2 track3:(NSString *)track3 {
    NSLog(@"<%@:%@> track1: %@, track2: %@, track3: %@, event buffer depth: %ld", NSStringFromClass([self class]), NSStringFromSelector(_cmd), track1, track2, track3, m_iConn->GetEventBufferDepth());
    
    NSString *magneticCardData = [NSString stringWithFormat:@"%@|%@|%@", track1, track2, track3];
    wchar_t *g_paramEvent = [Utils wcharFromNSString:magneticCardData];
    WCHAR_T *s_paramEvent = 0;
    convToShortWchar(&s_paramEvent, g_paramEvent);
    
    if (m_iConn) {
        m_iConn->ExternalEvent(s_classNameLineoProDriver, s_magneticCardScanned, s_paramEvent);
    }
    
    delete[] s_paramEvent;
    delete[] g_paramEvent;
}

@end


//----------------------------------------------------------------------------//

#if !defined(BUILD_DYNAMIC_LIBRARY)

namespace EXELLIO_DRIVER
{
    
#endif //!BUILD_DYNAMIC_LIBRARY
    
CExellioDriver::CExellioDriver() : m_iCnt(NULL), exellioDriverHelper(NULL), lineaProDriverHelper(NULL) {
    NSLog(@"%s", __PRETTY_FUNCTION__);
}

CExellioDriver::~CExellioDriver()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    if (exellioDriverHelper && lineaProDriverHelper)
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            exellioDriverHelper = NULL;
            lineaProDriverHelper = NULL;
        });
    }
}

void CExellioDriver::setIConnect(IAddInDefBaseEx* piConnect)
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    m_iCnt = piConnect;
    if (m_iCnt) {
        m_iCnt->SetEventBufferDepth(10);
        dispatch_sync(dispatch_get_main_queue(), ^{
            exellioDriverHelper = [ExellioDriverHelper sharedInstanceWithConnect:m_iCnt];
            lineaProDriverHelper = [LineaProDriverHelper sharedInstanceWithConnect:m_iCnt];
        });
    }
}

int CExellioDriver::getConnectionState()
{
    __block int connectionState = 0;
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        connectionState = [lineaProDriverHelper connectionState];
    });
    
    NSLog(@"%s isConnected: %d", __PRETTY_FUNCTION__, connectionState);
    
    return connectionState;
}

void CExellioDriver::turnDeviceOn()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [lineaProDriverHelper turnDeviceOn];
    });
}

void CExellioDriver::turnDeviceOff()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [lineaProDriverHelper turnDeviceOff];
    });
}
    
const wchar_t* CExellioDriver::getBatteryCapacity()
{
    __block NSString *capacity = @"";
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        capacity = [lineaProDriverHelper getBatteryCapacity];
    });
    
    NSLog(@"%s %@", __PRETTY_FUNCTION__, capacity);
    
    size_t length = capacity.length;
    
    NSLog(@"%s %@, length: %lu", __PRETTY_FUNCTION__, capacity, length);
    
    wchar_t* w_capacity = new wchar_t[length + 3];
    
    for (int i = 0; i < length; i++) {
        if (i < length) {
            w_capacity[i] = [capacity characterAtIndex:i];
        }
    }
    
    w_capacity[length] = 0x00;
    w_capacity[length + 1] = 0x00;
    w_capacity[length + 2] = 0x00;
    
    return w_capacity;
}
const wchar_t* CExellioDriver::getLineaProBatteryCapacity()
{
    __block NSString *capacity = @"";
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        capacity = [lineaProDriverHelper getLineaProBatteryCapacity];
    });
    
    NSLog(@"%s %@", __PRETTY_FUNCTION__, capacity);
    
    size_t length = capacity.length;
    
    NSLog(@"%s %@, length: %lu", __PRETTY_FUNCTION__, capacity, length);
    
    wchar_t* w_capacity = new wchar_t[length + 3];
    
    for (int i = 0; i < length; i++) {
        if (i < length) {
            w_capacity[i] = [capacity characterAtIndex:i];
        }
    }
    
    w_capacity[length] = 0x00;
    w_capacity[length + 1] = 0x00;
    w_capacity[length + 2] = 0x00;
    
    return w_capacity;
}
bool CExellioDriver::getLineaProCharging()
{
    __block bool charging = 0;
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        charging = [lineaProDriverHelper getLineaProCharging];
    });
    
    NSLog(@"%s %d", __PRETTY_FUNCTION__, charging);
    
    return charging;
}
void CExellioDriver::turnOnLineaProCharging()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [lineaProDriverHelper turnOnLineaProCharging];
    });
}
void CExellioDriver::turnOffLineaProCharging()
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [lineaProDriverHelper turnOffLineaProCharging];
    });
}
    
#if !defined(BUILD_DYNAMIC_LIBRARY)
    
}

#endif //!BUILD_DYNAMIC_LIBRARY
