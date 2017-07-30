
#include "ExellioDriver.h"
#include "include/AddInDefBase.h"
#import <ExellioFiscalDriver/ExellioFiscalDriver.h>
#import <Foundation/Foundation.h>

namespace EXELLIO_DRIVER
{

};

using namespace EXELLIO_DRIVER;


@interface ExellioDriverHelper: NSObject <EAFiscalProtocol>
{
    IAddInDefBaseEx         *m_iConn;
    ExellioFiscalDriver *fpu;
}

- (id)initWithAddInDefBase:(IAddInDefBaseEx*)m_iCnt;
- (void)printXReport;
- (void)printZReport;
- (void)printReceipt;
- (void)dealloc;

@end;

@implementation ExellioDriverHelper


- (id)initWithAddInDefBase:(IAddInDefBaseEx*)m_iCnt
{
    self = [super init];
    if (self)
    {
        m_iConn = NULL;
        fpu = NULL;
        
        m_iConn = (IAddInDefBaseEx*)m_iCnt;
        if (m_iConn)
            m_iConn->SetEventBufferDepth(10);
        
        fpu = [[ExellioFiscalDriver alloc]initUseAutoConnection: true protocolString:@"com.datecs.printer.fiscal"];
        [fpu addDelegate:self];
    }
    return self;
}

-(void)dealloc
{
    if (fpu)
        [fpu release];
    m_iConn = NULL;
    [super dealloc];
}

-(void)EAFiscalConnectedSerial:(NSString *)serial{
    NSLog(@"Connected %@",serial);
}
-(void)EAFiscalAnswerForPacket:(receivePacket *)packet{
    NSString *initStr = [[NSString alloc]initWithData:[packet data] encoding:NSWindowsCP1251StringEncoding];
    NSLog(@"EAFiscalAnswerForPacket %@",[initStr componentsSeparatedByString:@","]);

}
-(void)EAFiscalDisconnected{
    NSLog(@"Disconnected");
}
-(void)EAFiscalError:(NSNumber *)errorCode{
    NSLog(@"Error -%@",errorCode);
}

- (void)printXReport {
    [fpu PrintXReport];
}

-(void)printZReport {
    [fpu PrintZReport];
}

- (void)printReceipt {
    receiptStage=0;
    [fpu OpenFiscalReceiptForDefaultOperatorAndCallMethod:@selector(receiptWorker:) ofObject:self];
}

int receiptStage;
-(void)receiptWorker:(receivePacket*)packet{
    switch (receiptStage) {
        case 0:

            [fpu saleItemCode:12 Tax:taxGroupA Price:12.27 ArtName:@"Название артикула" Quantity:1 PercentDiscount:0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 1:
            [fpu saleItemCode:12 Tax:taxGroupA Price:14.27 ArtName:@"Название артикула" Quantity:1 PercentDiscount:0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 2:
            [fpu saleItemCode:12 Tax:taxGroupA Price:10.27 ArtName:@"Название артикула" Quantity:2 PercentDiscount:-5.0 AndCallMethod:@selector(receiptWorker:) ofObject:self];
            receiptStage++;
            break;
        case 3:
            [fpu CloseFiscalReceiptAndPayAndCallMethod:@selector(receiptWorker:) ofObject:self];
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
    
CExellioDriver::CExellioDriver() : m_iCnt(NULL), exellioDriverHelper(NULL) {}

CExellioDriver::~CExellioDriver()
{
    if (exellioDriverHelper)
    {
        [exellioDriverHelper release];
        exellioDriverHelper = NULL;
    }
}

void CExellioDriver::setIConnect(IAddInDefBaseEx* piConnect)
{
    m_iCnt = piConnect;
    if (m_iCnt)
    {
        exellioDriverHelper = [[ExellioDriverHelper alloc] initWithAddInDefBase:piConnect];
    }
}

void CExellioDriver::printXReport()
{
    [exellioDriverHelper printXReport];
}

void CExellioDriver::printZReport()
{
    [exellioDriverHelper printZReport];
}

void CExellioDriver::printReceipt()
{
    [exellioDriverHelper printReceipt];
}
    
#if !defined(BUILD_DYNAMIC_LIBRARY)

}

#endif //!BUILD_DYNAMIC_LIBRARY

