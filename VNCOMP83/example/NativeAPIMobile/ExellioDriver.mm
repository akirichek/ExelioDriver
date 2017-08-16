
#include "ExellioDriver.h"
#include "include/AddInDefBase.h"
#import <ExellioFiscalDriver/ExellioFiscalDriver.h>
#import <Foundation/Foundation.h>

namespace EXELLIO_DRIVER
{

};

using namespace EXELLIO_DRIVER;


@interface ExellioDriverHelper: NSObject <EAFiscalProtocol>

@property (strong, nonatomic) ExellioFiscalDriver *fpu;
@property (strong, nonatomic) NSString *result;
@property (assign, nonatomic) BOOL isConnected;

+ (instancetype)sharedInstance;

- (void)printXReport;
- (void)printZReport;
- (void)printReceipt;
- (void)dealloc;

@end;

@implementation ExellioDriverHelper

+ (instancetype)sharedInstance
{
    static ExellioDriverHelper *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[ExellioDriverHelper alloc] init];
    });
    return sharedInstance;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        self.fpu = [[ExellioFiscalDriver alloc]initUseAutoConnection: true protocolString:@"com.datecs.printer.fiscal"];
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
    NSLog(@"ED EAFiscalAnswerForPacket %@",[initStr componentsSeparatedByString:@","]);

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
    
CExellioDriver::CExellioDriver() : m_iCnt(NULL), exellioDriverHelper(NULL) {
    NSLog(@"%s", __PRETTY_FUNCTION__);
}

CExellioDriver::~CExellioDriver()
{
    NSLog(@"%s, exellioDriverHelper: %@", __PRETTY_FUNCTION__, exellioDriverHelper);
    if (exellioDriverHelper)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            exellioDriverHelper = NULL;
        });
    }
}

void CExellioDriver::setIConnect(IAddInDefBaseEx* piConnect)
{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    m_iCnt = piConnect;
    if (m_iCnt) {
        dispatch_async(dispatch_get_main_queue(), ^{
            exellioDriverHelper = [ExellioDriverHelper sharedInstance];
        });
    }
}
    
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

