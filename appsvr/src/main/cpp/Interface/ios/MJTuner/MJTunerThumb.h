//
//  MJTunerThumb.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MJTunerThumb_h
#define MojingSDK_MJTunerThumb_h

#import <Foundation/Foundation.h>

@class MJTunerThumb;

typedef void (^MJTunerThumbStickValueChangedBlock)(MJTunerThumb *stick, uint8_t value);

@interface MJTunerThumb : NSObject
{
    uint8_t valueTurn;
    NSTimeInterval _timestamp;
    MJTunerThumbStickValueChangedBlock _valueChangedHandler;
}

@property (nonatomic, readonly) uint8_t valueTurn;
@property (nonatomic, readonly) NSTimeInterval timestamp;
@property (nonatomic, copy) MJTunerThumbStickValueChangedBlock valueChangedHandler;

- (void)setTurnValue:(uint8_t)value;
- (void)setTimestamp:(NSTimeInterval)timestamp;

@end

#endif
