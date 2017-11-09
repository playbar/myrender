//
//  MJTunerButton.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import "MJTunerButton.h"
#import "MJTuner_Structures.h"

@interface MJTunerButton ()
{
    mj_tuner_button_buffer *_buffer;
}

@end

@implementation MJTunerButton
@synthesize analog = _analog;
@synthesize pressed = _pressed;
@synthesize value = _value;
@synthesize timestamp = _timestamp;
@synthesize valueChangedHandler = _valueChangedHandler;

- (void)dealloc
{
    free(_buffer);
    /*
    [_valueChangedHandler release];
    
    [super dealloc];
     */
}

- (instancetype)init
{
    _analog = NO;
    _pressed = NO;
    _value = 0.f;
    
    _buffer = (mj_tuner_button_buffer *)malloc(sizeof(mj_tuner_button_buffer));
    _buffer->analog = 0;
    _buffer->pressed = 0;
    _buffer->value = 0;
    
    return self;
}

- (void)setAnalog:(BOOL)analog
{
    _analog = analog;
}

- (void)setPressed:(BOOL)pressed
{
    if (pressed != _buffer->pressed)
    {
        _pressed = pressed;
        _buffer->pressed = pressed;
        
        //触发改变事件
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, _pressed, _value);
        }
    }
}

- (void)setValue:(float)value
{
    if (value != _buffer->value && value > 0.f)
    {
        _value = value;
        _buffer->value = value;
        
        _pressed = YES;
        _buffer->pressed = _pressed;
        
        //触发改变事件
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, _pressed, _value);
        }
    }
}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

- (NSDictionary *)toDictionary
{
    NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:
                          [NSNumber numberWithBool:_pressed], @"pressed",
                          [NSNumber numberWithBool:_analog], @"analog",
                          [NSNumber numberWithFloat:_value], @"value",
                          [NSNumber numberWithDouble:_timestamp], @"timestamp",
                          nil];
    return dict;
}

@end
