//
//  MJGamepadDirectionPad.m
//  ble
//
//  Created by 秦宏 on 15/5/21.
//
//

#import "MJGamepadDirectionPad.h"
#import "MJGamepad_Structures.h"

@interface MJGamepadDirectionPad ()
{
    mj_direction *_buffer; //状态字
    NSTimer *_timer; //计时器，用于连续操作
    mj_direction *_lastState; //上一次的状态
}

@end

@implementation MJGamepadDirectionPad
@synthesize up = _up;
@synthesize down = _down;
@synthesize left = _left;
@synthesize right = _right;
@synthesize center = _center;
@synthesize timestamp = _timestamp;
@synthesize valueChangedHandler = _valueChangedHandler;

- (void)dealloc
{
    free(_buffer);
    
/*
    [_up release];
    [_down release];
    [_left release];
    [_right release];
    [_center release];
    //[_timestamp release];
    
    [_valueChangedHandler release];
    
    [super dealloc];
*/
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _up = [[MJGamepadButton alloc] init];
        _down = [[MJGamepadButton alloc] init];
        _left = [[MJGamepadButton alloc] init];
        _right = [[MJGamepadButton alloc] init];
        _center = [[MJGamepadButton alloc] init];
        
        _buffer = (mj_direction *)malloc(sizeof(mj_direction));
        _buffer->all_flags = 0;
        _lastState = (mj_direction *)malloc(sizeof(mj_direction));
        _lastState->all_flags = 0;
    }
    
    return self;
}

- (void)setBuffer:(mj_direction)buffer
{
    //所有状态和之前不一样
    if (buffer.all_flags != _buffer->all_flags)
    {
        _buffer->all_flags = buffer.all_flags;
        
        [_up setTimestamp:_timestamp];
        [_up setPressed:_buffer->up];
        [_down setTimestamp:_timestamp];
        [_down setPressed:_buffer->down];
        [_left setTimestamp:_timestamp];
        [_left setPressed:_buffer->left];
        [_right setTimestamp:_timestamp];
        [_right setPressed:_buffer->right];
        [_center setTimestamp:_timestamp];
        [_center setPressed:_buffer->center];
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self);
            
            if (_timer != nil)
            {
                [_timer invalidate];
                _timer = nil;
            }

            if (_buffer->all_flags != 0 && _buffer->center == 0 )
            {
                _timer = [NSTimer scheduledTimerWithTimeInterval:0.3f target:self selector:@selector(timerSchedule) userInfo:nil repeats:YES];
            }
        }
    }
}

- (mj_direction)getDirection
{
    return *(_buffer);
}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

- (void)timerSchedule
{
    _valueChangedHandler(self);
}

@end
