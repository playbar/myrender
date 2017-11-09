//
//  MICCKeyObject.m
//
//  Created by Vrtrid on 15/10/10.
//  Copyright © 2015年 Vrtrid. All rights reserved.
//

#import "MGMICCKeyObject.h"
#import "sys/utsname.h"

#define EVERY_READ_LENGTH 4096

@implementation MGMICCKeyObject
@synthesize     aqc;
@synthesize     audioDataLength;

static void AQInputCallback (void                   * inUserData,
                             AudioQueueRef          inAudioQueue,
                             AudioQueueBufferRef    inBuffer,
                             const AudioTimeStamp   * inStartTime,
                             UInt32                 inNumPackets,
                             const AudioStreamPacketDescription * inPacketDesc){
    
    MGMICCKeyObject * engine = (__bridge MGMICCKeyObject *) inUserData;
    if (inNumPackets > 0){
        [engine processAudioBuffer:inBuffer withQueue:inAudioQueue];
    }
    
    if (engine.aqc.run){
        AudioQueueEnqueueBuffer(engine.aqc.queue, inBuffer, 0, NULL);
    }
}

- (void)getMachineType
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
//    NSLog(@"Device %@", deviceString);
    if ([deviceString isEqualToString:@"iPhone8,2"]){        // iPhone 6 Plus, iPhone 6s Plus
        DEVV = 17000;
    } else if ([deviceString isEqualToString:@"iPhone7,2"]){ // iPhone 6
        DEVV = 15500;
    } else if([deviceString isEqualToString:@"iPhone8,1"]){  // iPhone 6s
        DEVV = 15500;
    } else {
        DEVV = 16000;
    }
}

- (void)setAudioFormat
{
    aqc.mDataFormat.mSampleRate       = 44100.0;
    aqc.mDataFormat.mFormatID         = kAudioFormatLinearPCM;
    aqc.mDataFormat.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    aqc.mDataFormat.mFramesPerPacket  = 1;
    aqc.mDataFormat.mChannelsPerFrame = 2;
    aqc.mDataFormat.mBitsPerChannel   = 16;
    aqc.mDataFormat.mBytesPerPacket   = 1;
    aqc.mDataFormat.mBytesPerFrame    = aqc.mDataFormat.mChannelsPerFrame * sizeof (SInt16);
    aqc.mDataFormat.mBytesPerPacket   = aqc.mDataFormat.mBytesPerFrame;
}

- (void) createAudioQueue
{
    if (![self checkAudioQueueSuccess:AudioQueueNewInput(&aqc.mDataFormat, AQInputCallback, (__bridge void *)(self), NULL, kCFRunLoopCommonModes, 0, &aqc.queue)]) {
        return;
    }
    ;
    for (int i=0; i<kNumberBuffers; i++) {
        if (![self checkAudioQueueSuccess:AudioQueueAllocateBuffer(aqc.queue, EVERY_READ_LENGTH, &aqc.mBuffers[i])]) {
            break;
        }
        if (![self checkAudioQueueSuccess:AudioQueueEnqueueBuffer(aqc.queue, aqc.mBuffers[i], 0, NULL)]) {
            break;
        }
    }
    aqc.recPtr = 0;
    aqc.run    = 1;
}

#pragma mark - error
- (void)errorForOSStatus:(OSStatus)status error:(NSError *__autoreleasing *)outError
{
    if (status != noErr && outError != NULL)
    {
        *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:status userInfo:nil];
    }
}

- (BOOL)checkAudioQueueSuccess:(OSStatus)status
{
    if (status != noErr)
    {
        if (aqc.queue)
        {
            AudioQueueDispose(aqc.queue, YES);
            aqc.queue = NULL;
        }
        
        NSError *error = nil;
        [self errorForOSStatus:status error:&error];
        return NO;
    }
    return YES;
}

- (id) init{
    self = [super init];
    if (self){
        // 获取设备硬件类型
        [self getMachineType];
        // 设置音频格式
        [self setAudioFormat];
        // 创建音频队列
        [self createAudioQueue];
    }
    audioDataIndex = 0;
    mpos           = 0;
    
    mData[0] = 0;
    mData[1] = 0;
    mData[2] = 0;
    mData[3] = 0;
    mData[4] = 0;
    mData[5] = 0;
    
    [self restReceiveParameters];
    return self;
}

- (void) restReceiveParameters
{
    mLastUp    = FALSE;
    mLastDown  = FALSE;
    mFirstDown = TRUE;
    mLastValue = 0;
    mLastX     = 0;
    x          = 0;
    
    mBitCount  = 0;
    mError     = FALSE;
    mTempChar  = 0x00;
    
    HEAD[0]    = 0x00;
    HEAD[1]    = 0x00;
    HEAD[2]    = 0x00;
    HEAD[3]    = 0x00;
    mByteNum   = 0;
}

- (void) dealloc {
    aqc.run = 0;
    AudioQueueStop(aqc.queue, TRUE);
    AudioQueueDispose(aqc.queue, TRUE);
    [super dealloc];
}

- (void) start {
    NSLog(@"Audio Listening Start");
    AudioQueueStart(aqc.queue, NULL);
}

- (void) stop {
    NSLog(@"Audio Listening Stop");
    AudioQueueStop(aqc.queue, TRUE);
}

- (void) pause {
    NSLog(@"Audio Listening Pause");
    AudioQueuePause(aqc.queue);
}

- (void)addSample:(short) sample{
    bool upline     = FALSE;
    bool downline   = FALSE;
    
    if(mpos > 3){
        if((mData[1] - mData[0]) > 0){
            if((mData[1] - mData[0])>DEVV)
                upline = TRUE;
            else if((mData[2] - mData[0])>DEVV)
                upline = TRUE;
            else if((mData[3] - mData[0])>DEVV)
                upline = TRUE;
            else if((mData[4] - mData[0])>DEVV)
                upline = TRUE;
            else if((sample - mData[0])>DEVV)
                upline = TRUE;
            
            if(upline){
                if(mLastUp == FALSE){
                    int dx = x - mLastX;
                    if(dx > 60){
                        
                    }else if(dx>18 && dx<30){
                        if(mLastValue == 1){
                            [self addbit:1];
                        }
                    }else if(dx>40 && dx<50){
                        if(mLastValue == 0){
                            [self addbit:1];
                        }
                    }
                    
                    if(dx>10){
                        x = 0;
                        mLastX = x;
                    }else{
                        x = x+1;
                    }
                }else{
                    x = x+1;
                }
                mLastUp = TRUE;
                mLastDown = FALSE;
            }else{
                if(x<100)
                    x = x+1;
                else{
                    [self restReceiveParameters];
                    x = 100;
                }
                mLastDown   = FALSE;
                mLastUp     = FALSE;
            }
            
        }else if((mData[0] - mData[1]) > 0){
            if((mData[0] - mData[1]) > DEVV)
                downline = TRUE;
            else if((mData[0] - mData[2]) > DEVV)
                downline = TRUE;
            else if((mData[0] - mData[3]) > DEVV)
                downline = TRUE;
            else if((mData[0] - mData[4]) > DEVV)
                downline = TRUE;
            else if((mData[0] - sample) > DEVV)
                downline = TRUE;
            
            if(downline){
                if(mLastDown == FALSE){
                    
                    int dx = x - mLastX;
                    
                    if(dx>60){
                        
                    }else if(dx>18 && dx<30){
                        if(mFirstDown){
                            [self addbit:0];
                        }else if(mLastValue == 0){
                            [self addbit:0];
                        }
                        
                    }else if(dx>40 && dx<50){
                        if(mLastValue == 1){
                            [self addbit:0];
                        }
                    }
                    
                    if(dx>10){
                        x = 0;
                        mLastX = x;
                    }else{
                        x = x+1;
                    }
                }else{
                    x=x+1;
                }
                
                mLastUp = FALSE;
                mLastDown = TRUE;
                mFirstDown = FALSE;
            }else{
                if(x < 100)
                    x=x+1;
                else{
                    [self restReceiveParameters];
                    x=100;
                }
                mLastDown = FALSE;
                mLastUp = FALSE;
            }
        }else{
            if(x<100)
                x = x+1;
            else {
                [self restReceiveParameters];
                x=100;
            }
            mLastDown   = FALSE;
            mLastUp     = FALSE;
        }
    }
    
    if(mpos < 10000)
        mpos ++;
    
    mData[0] = mData[1];
    mData[1] = mData[2];
    mData[2] = mData[3];
    mData[3] = mData[4];
    mData[4] = sample;
    
}

- (void) addbit:(int) v {
    
    mLastValue = v;
    if(mBitCount < 3 || mBitCount == 11){
        if(v > 0){
            mError = TRUE;
        }
    }else{
        mTempChar = mTempChar<<1 | (0x01 & v);
    }
    if(mBitCount == 11){
        if(mByteNum < 4)
            HEAD[mByteNum] = mTempChar;
        else if(HEAD[0]==0x00 && HEAD[1]==0x00 && HEAD[2]==0x48 && HEAD[3]==0x54 && mTempChar!=0xFF){
            NSString* keyValue = [[NSString alloc] initWithString:[NSString stringWithFormat:@"%d", mTempChar & 0x7f]];
            NSString* keyPress  = [[NSString alloc] initWithString:[NSString stringWithFormat:@"%d",(mTempChar & 0x80)>>7]];
            if (_mMGOutputCallBack)
                dispatch_async(dispatch_get_main_queue(), ^{
                    _mMGOutputCallBack(keyValue, keyPress);
                });
        }
        mByteNum ++;
    }
    
    if(mBitCount < 11)
        mBitCount ++;
    else
        mBitCount = 0;
}

- (void) getAudioKey:(void *) addr alen:(int) len{
    short* data = (short*) addr;
    int i = 0;
    for(i = 0; i < len; i += 4){
        data ++;{
            [self addSample:(*data)];
        }
        data ++;
    }
}

- (void)processAudioBuffer:(AudioQueueBufferRef) buffer withQueue:(AudioQueueRef) queue{
    
    [self getAudioKey:buffer->mAudioData alen:buffer->mAudioDataByteSize];
    audioDataIndex +=buffer->mAudioDataByteSize;
    audioDataLength = audioDataIndex;
}

@end