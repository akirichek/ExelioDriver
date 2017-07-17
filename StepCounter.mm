
#include "StepCounter.h"
#include "include/AddInDefBase.h"

#import <Foundation/Foundation.h>
#import <ExellioFiscalDriver/ExellioFiscalDriver.h>

namespace COM_1C_STEP_COUNTER
{

const double UPDATE_INTERVAL_DATA = 1.0f / 10.0f;

};

using namespace COM_1C_STEP_COUNTER;

@interface CStepCounterRecorder: NSObject<EAFiscalProtocol>{
    ExellioFiscalDriver *fpu;
    NSOperationQueue        *theQueue;
    IAddInDefBaseEx         *m_iConn;
    
    MotionData              *shrdData;
    bool                     isChangedData;
}

-(id)initWithSharedData:(IAddInDefBaseEx*)m_iCnt : (MotionData*)sharedData;
-(void)dealloc;
-(void)startStepCounterUpdates;
-(void)stopStepCounterUpdates;

@end;

//----------------------------------------------------------------------------//


@implementation CStepCounterRecorder


- (id)initWithSharedData:(IAddInDefBaseEx*)m_iCnt : (MotionData*)sharedData
{
    self = [super init];
    if (self)
    {
        m_iConn = NULL;
        fpu = NULL;
        theQueue = NULL;

        shrdData = sharedData;
        shrdData->isComplete = false;
        shrdData->stepCount = 0;
        shrdData->orientXOZ = 0.0;
        shrdData->orientXOY = 0.0;
        shrdData->orientYOZ = 0.0;
        shrdData->typeMotion = eTypeUndefined;
        shrdData->startDate = time(NULL);
        
        m_iConn = (IAddInDefBaseEx*)m_iCnt;
        if (m_iConn)
            m_iConn->SetEventBufferDepth(10);

        fpu = [[ExellioFiscalDriver alloc]initUseAutoConnection: true protocolString:@"com.datecs.printer.fiscal"];
        [fpu addDelegate:self];
        
        theQueue = [[NSOperationQueue alloc] init];
    }
    return self;
}

-(void)dealloc
{
    if (fpu)
        [fpu removeDelegate:self];
        [fpu release];
    if (theQueue)
        [theQueue release];
    m_iConn = NULL;
    shrdData = NULL;
    [super dealloc];
}

- (void)startStepCounterUpdates
{
    shrdData->isComplete = false;
    shrdData->startDate = time(NULL);
    shrdData->stepCount = 0;
    
    if (!m_iConn)
        return;

    shrdData->isComplete = true;
}

- (void)stopStepCounterUpdates
{
    if (m_iConn)
        m_iConn->CleanEventBuffer();
}

@end


//----------------------------------------------------------------------------//

#if !defined(BUILD_DYNAMIC_LIBRARY)

namespace COM_1C_STEP_COUNTER
{

#endif //!BUILD_DYNAMIC_LIBRARY

CStepCounter::CStepCounter() : isRun(false), m_iCnt(NULL), motionRecorder(NULL) {}

CStepCounter::~CStepCounter()
{
    if (motionRecorder) 
    {
        [motionRecorder stopStepCounterUpdates];
        [motionRecorder release];
        motionRecorder = NULL;
    }
}

void CStepCounter::setIConnect(IAddInDefBaseEx* piConnect)
{
    m_iCnt = piConnect;
    if (m_iCnt) 
    {
        motionRecorder = [[CStepCounterRecorder alloc]initWithSharedData: m_iCnt: &sharedData];
    }
}

void CStepCounter::startStepCounterUpdatesFromDate()
{
    // Set date&time
    sharedData.startDate = time(NULL);
    if (motionRecorder) 
    {
        [motionRecorder startStepCounterUpdates];
        isRun = true;
    }
}

void CStepCounter::stopStepCounterUpdates()
{
    isRun = false;
    if (motionRecorder)
        [motionRecorder stopStepCounterUpdates];
}

long CStepCounter::stepCount()
{
    if (sharedData.isComplete)
        return sharedData.stepCount;
    return 0;
}

TypesMotion CStepCounter::typeMotion()
{
    TypesMotion buf = eTypeUndefined;
    if (sharedData.isComplete) 
    {
        buf = sharedData.typeMotion;
        if ((buf < 0) || (buf >= eTypeLast))
            buf = eTypeUndefined;
    }
    return buf;
}

double CStepCounter::orientation(const Orientations plane)
{
    double res = 0.0;
    if (sharedData.isComplete) 
    {
        switch (plane) 
        {
            case eOrientXOZ:
                res = sharedData.orientXOZ;
                break;
            case eOrientXOY:
                res = sharedData.orientXOY;
                break;
            case eOrientYOZ:
                res = sharedData.orientYOZ;
                break;
            default:
                res = 0.0;
        }
    }
    return res;
}

bool CStepCounter::getRunningStepCounterUpdates()
{
    return isRun;
}
    
#if !defined(BUILD_DYNAMIC_LIBRARY)

}

#endif //!BUILD_DYNAMIC_LIBRARY

