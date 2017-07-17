//
//  ExellioFiscalDriver.h
//  ExellioFiscalDriver
//
//  Created by Victor Havinskiy on 06.06.17.
//  Copyright © 2017 Victor Havinskiy. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <ExternalAccessory/ExternalAccessory.h>

//! Project version number for ExellioFiscalDriver.
FOUNDATION_EXPORT double ExellioFiscalDriverVersionNumber;

//! Project version string for ExellioFiscalDriver.
FOUNDATION_EXPORT const unsigned char ExellioFiscalDriverVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <ExellioFiscalDriver/PublicHeader.h>

@interface receivePacket : NSObject
{
    
}
-(void)fillWithData:(NSData*)packet andcommandId:(unsigned long)aCommandId;

@property (nonatomic, readonly) Byte cmd;
@property (nonatomic, readonly) NSData *data;
@property (nonatomic,getter=_getParams) NSArray * params;

@property (nonatomic, readonly) Byte seq;
@property (nonatomic, readonly) Byte len;


@property (nonatomic, readonly) NSData *status;
@property (nonatomic, readonly) NSData *CRC;

@property (nonatomic, readonly) NSArray *statusErrors;
@property (nonatomic, readonly) NSArray *statusWarnings;
@property (nonatomic, readonly) NSArray *statusInformation;
@property (nonatomic, readonly) unsigned long commandId;

@end


typedef NS_ENUM(int,FiscalErrorCodes){
    noError = 0,
    statusError,//see error in last status bytes
    cantUseThisArticle,//Артикул не может быть перепрограммирован, так как по нему уже были продажи в данной открытой смене
    timeoutError,//Превышено время ожидания пакета, повторная отправка пакета не принесла результатов
};

@protocol EAFiscalProtocol <NSObject>

-(void)EAFiscalAnswerForPacket:(receivePacket*)packet;
-(void)EAFiscalError:(NSNumber*)errorCode;
-(void)EAFiscalConnectedSerial:(NSString*)serial;
-(void)EAFiscalDisconnected;

@end

@interface delegateController : NSObject
//asd
-(void)setDelegate:(id <EAFiscalProtocol>) newDelegate;
@property (nonatomic,setter=setDelegate:) id<EAFiscalProtocol> delegate;
@property (nonatomic,getter=respondsToEAFiscalAnswerForPacket) bool EAFiscalAnswerForPacket;
@property (nonatomic,getter=respondsToEAFiscalError) bool EAFiscalError;
@property (nonatomic,getter=respondsToEAFiscalConnectedSerial) bool EAFiscalConnectedSerial;
@property (nonatomic,getter=respondsToEAFiscalDisconnected) bool EAFiscalDisconnected;

@end

typedef NS_ENUM(int,TaxGroups){
    taxGroupA=1,
    taxGroupB,
    taxGroupC,
    taxGroupD,
    taxGroupX,
    taxGroupM,
    taxGroupH,
};

typedef NS_ENUM(int,PaymentTypes){
    PaymentCash=1,
    PaymentCredit,
    PaymentCheck,
    PaymentCard,
    PaymentProgrammable1,
    PaymentProgrammable2,
    PaymentProgrammable3,
    PaymentProgrammable4,
};


@interface ExellioFiscalDriver : NSObject

-(void)addDelegate:(id <EAFiscalProtocol>)newDelegate;
-(void)removeDelegate:(id <EAFiscalProtocol>)remDelegate;
-(void)callDelegatesEAFiscalAnswerForPacket:(id)object;
-(void)callDelegatesError:(FiscalErrorCodes)code;


-(void)setupControllerForFiscal:(EAAccessory*)accessory withProtocolString:(NSString *)protocolString;
-(id)initUseAutoConnection:(BOOL)autoConnect;
-(id)initUseAutoConnection:(BOOL)autoConnect protocolString:(NSString*)protocol;
-(id) initUseIp:(NSString*)url Port:(int)port;
- (unsigned long) writeCommand:(Byte) cmd withData:(NSString*)data;
- (unsigned long) writeCommand:(Byte) cmd withData:(NSString*)data andCallMethod:(SEL)method ofObject:(id)receiver;



-(void)OpenNonFiscalReceiptAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)OpenNonFiscalReceipt;
-(void)CloseNonFiscalReceiptAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)CloseNonFiscalReceipt;
-(void)OpenFiscalReceiptForOperator:(Byte)OpCode password:(NSString*)OpPassword AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)OpenFiscalReceiptForOperator:(Byte)OpCode password:(NSString*)OpPassword;
-(void)OpenFiscalReceiptForDefaultOperator;
-(void)OpenFiscalReceiptForDefaultOperatorAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)CloseFiscalReceiptAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)CloseFiscalReceipt;
-(void)OpenReturnReceiptForOperator:(Byte)OpCode password:(NSString*)OpPassword AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)OpenReturnReceiptForOperator:(Byte)OpCode password:(NSString*)OpPassword;
-(void)OpenReturnReceiptForDefaultOperator;
-(void)OpenReturnReceiptForDefaultOperatorAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintNonFiscalText:(NSString*)text AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintNonFiscalText:(NSString*)text;
-(void)PrintFiscalText:(NSString*)text AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintFiscalText:(NSString*)text;
-(void)PrintZReportUsingPassword:(NSString*)password;
-(void)PrintZReport;
-(void)PrintZReportUsingPassword:(NSString*)password AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintZReportAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintXReportUsingPassword:(NSString*)password;
-(void)PrintXReport;
-(void)PrintXReportUsingPassword:(NSString*)password AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)PrintXReportAndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)setArticleCode:(int)code Tax:(TaxGroups)tax ArtGroup:(int)group Price:(double)price ArtName:(NSString*)artName AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt PercentDiscount:(double)PercDisc;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt SumDiscount:(double)SumDisc;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt PercentDiscount:(double)PercDisc AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt SumDiscount:(double)SumDisc AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt Price:(double)price PercentDiscount:(double)PercDisc;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt Price:(double)price SumDiscount:(double)SumDisc;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt Price:(double)price PercentDiscount:(double)PercDisc AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)registerArticleCode:(int)artNum Quantity:(double)qnt Price:(double)price SumDiscount:(double)SumDisc AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)cashInOut:(double)cash;
-(void)cashInOut:(double)cash AndCallMethod:(SEL)method ofObject:(id)receiver;//TODO:implementation

-(void)saleItemCode:(int)code Tax:(TaxGroups)tax Price:(double)price ArtName:(NSString*)artName Quantity:(double)qnt PercentDiscount:(double)percDisc;

-(void)changePrice:(double)price ForArticleCode:(int)code;
-(void)changePrice:(double)price ForArticleCode:(int)code AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)changePrice:(double)price ForArticleCode:(int)code UsingPassword:(NSString*)password;
-(void)changePrice:(double)price ForArticleCode:(int)code UsingPassword:(NSString*)password AndCallMethod:(SEL)method ofObject:(id)receiver;


-(void)saleItemCode:(int)code Tax:(TaxGroups)tax Price:(double)price ArtName:(NSString *)artName Quantity:(double)qnt AbsoluteDiscount:(double)absDisc AndCallMethod:(SEL)method ofObject:(id)receiver;
-(void)saleItemCode:(int)code Tax:(TaxGroups)tax Price:(double)price ArtName:(NSString *)artName Quantity:(double)qnt AbsoluteDiscount:(double)absDisc;
-(void)saleItemCode:(int)code Tax:(TaxGroups)tax Price:(double)price ArtName:(NSString *)artName Quantity:(double)qnt PercentDiscount:(double)percDisc AndCallMethod:(SEL)method ofObject:(id)receiver;

-(void)totalType:(PaymentTypes)paymentType Sum:(double)sum Comment:(NSString*)comment;
-(void)totalType:(PaymentTypes)paymentType Sum:(double)sum Comment:(NSString*)comment AndCallMethod:(SEL)method ofObject:(id)receiver;

-(void)CloseFiscalReceiptAndPay;
-(void)CloseFiscalReceiptAndPayAndCallMethod:(SEL)method ofObject:(id)receiver;



@property (nonatomic,readonly, getter=_getLastStatus) NSData *lastStatus;
@property (nonatomic,readonly) bool Error;
@property (nonatomic) NSString * protocolString;



@end

