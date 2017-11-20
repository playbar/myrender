//
//  MJTunerThumb.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import "MJTunerThumb.h"

@interface MJTunerThumb ()
{
    uint8_t _valueTurn;
}

@end

@implementation MJTunerThumb
@synthesize valueTurn = _valueTurn;
@synthesize timestamp = _timestamp;
@synthesize valueChangedHandler = _valueChangedHandler;

/*
- (void)dealloc
{
    [_valueChangedHandler release];
    
    [super dealloc];
}
*/

- (instancetype)init
{
    self = [super init];
    _valueTurn = 0;
    
    return self;
}

- (void)setTurnValue:(uint8_t)value
{
    if (value != _valueTurn)
    {
        _valueTurn = value;
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, _valueTurn);
        }
        
    }
}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

@end

