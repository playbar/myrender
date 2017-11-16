//
//  MICCKeyObject.h
//
//  Created by Vrtrid on 15/10/10.
//  Copyright © 2015年 Vrtrid. All rights reserved.
//

#ifndef MICCKeyObject_h
#define MICCKeyObject_h

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudioTypes.h>
#import <AVFoundation/AVFoundation.h>

#define kNumberBuffers   3
#define bri             0.2

typedef struct AQCallbackStruct
{
    AudioStreamBasicDescription mDataFormat;
    AudioQueueRef               queue;
    AudioQueueBufferRef         mBuffers[kNumberBuffers];
    AudioFileID                 outputFile;
    
    UInt32                      frameSize;
    long long                   recPtr;
    int                         run;
    
} AQCallbackStruct;

typedef void(^MGMICCKeyObjectOutputCallBack)(NSString *keyValue, NSString *keyPress);

@interface MGMICCKeyObject : NSObject
{
    AQCallbackStruct aqc;
    AudioFileTypeID  fileFormat;
    long             audioDataLength;
    long             audioDataIndex;
    unsigned         char mTempChar;
    unsigned         char HEAD[4];
    int              mByteNum;
    bool             mLastUp;
    bool             mLastDown;
    bool             mFirstDown;
    bool             mError;
    int              mLastValue;
    int              mLastX;
    int              DEVV;
    int              x;
    short            mData[8];
    int              mBitCount;
    int              mpos;
}
- (id)   init;
- (void) start;
- (void) stop;
- (void) pause;
- (void) processAudioBuffer:(AudioQueueBufferRef) buffer withQueue:(AudioQueueRef) queue;
- (void) getAudioKey:(void *) addr alen:(int) len;

@property (nonatomic, assign) AQCallbackStruct aqc;
@property (nonatomic, assign) long audioDataLength;
@property (copy) MGMICCKeyObjectOutputCallBack mMGOutputCallBack;

@end

#endif
