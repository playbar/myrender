//
//  MJGamepadThumbStick.m
//  ble
//
//  Created by 秦宏 on 15/5/21.
//
//

#import "MJGamepadThumbStick.h"

@interface MJGamepadThumbStick ()
{
    CGPoint *_buffer;
}

@end

@implementation MJGamepadThumbStick
@synthesize xAxis = _xAxis;
@synthesize yAxis = _yAxis;
@synthesize radian = _radian;
@synthesize offsetToCenter = _offsetToCenter;
@synthesize timestamp = _timestamp;
@synthesize valueChangedHandler = _valueChangedHandler;

- (void)dealloc
{
    free(_buffer);
/*
    //[_xAxis release];
    //[_yAxis release];
    //[_radian release];
    //[_offsetToCenter release];
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
        _buffer = (CGPoint *)malloc(sizeof(CGPoint));
    }
    return self;
}

- (void)setPoint:(CGPoint)point
{
    if (point.x != _buffer->x || point.y != _buffer->y)
    {
        _buffer->x = point.x;
        _buffer->y = point.y;
        _xAxis = point.x;
        _yAxis = point.y;
        
        _offsetToCenter = sqrtf(_xAxis*_xAxis+_yAxis*_yAxis);
        _radian = atan2f(_yAxis, _xAxis) * 180.f * M_1_PI;
        
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, _xAxis, _yAxis);
        }
    
    }
}

- (void)setTouchPoint:(CGPoint)point
{
    //if (point.x != _buffer->x || point.y != _buffer->y)
    {
        _buffer->x = point.x;
        _buffer->y = point.y;
        _xAxis = point.x;
        _yAxis = point.y;
        
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, _xAxis, _yAxis);
        }
        
    }
}

//- (void)setXAxis:(float)xAxis
//{
//    
//    _xAxis = xAxis;
//}
//
//- (void)setYAxis:(float)yAxis
//{
//    _yAxis = yAxis;
//}
//
//- (void)setRadian:(float)radian
//{
//    _radian = radian;
//}
//
//- (void)setOffsetToCenter:(float)offsetToCenter
//{
//    _offsetToCenter = offsetToCenter;
//}

- (void)setTimestamp:(NSTimeInterval)timestamp
{
    _timestamp = timestamp;
}

@end
