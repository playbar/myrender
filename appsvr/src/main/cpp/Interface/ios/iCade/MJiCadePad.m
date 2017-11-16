//
//  MojingUIView.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/17.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import "MJiCadePad.h"
#import "CWLSynthesizeSingleton.h"

@implementation MJiCadeGamePad
@synthesize valueChangedHandler = _valueChangedHandler;

CWL_SYNTHESIZE_SINGLETON_FOR_CLASS(MJiCadeGamePad)

#pragma mark - init

+ (instancetype)sharediCadeGamepad{
    static MJiCadeGamePad *share=nil;
    
    if (!share) {
        share=[[MJiCadeGamePad alloc] init];
    }
    return share;
}

/*
- (void)dealloc
{
    [_valueChangedHandler release];
    [control release];
    
    [super dealloc];
}
*/

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        control = [[iCadeReaderView alloc] initWithFrame:CGRectZero];
    }
    return self;
    
}

- (void)registerKey :(UIViewController* )viewCon
{
//    if (control != nil)
//    {
//        [viewCon.view addSubview:control];
//        control.active = YES;
//        control.delegate = self;
//        control.valueChangedHandler = ^void(iCadeState button, BOOL bPressed){
//            if(_valueChangedHandler)
//            {
//                switch (button) {
//                    case iCadeButtonA:
//                        _valueChangedHandler(BUTTON_Y, bPressed);
//                        break;
//                    case iCadeButtonB:
//                        _valueChangedHandler(BUTTON_B, bPressed);
//                        break;
//                    case iCadeButtonC:
//                        _valueChangedHandler(BUTTON_A, bPressed);
//                        break;
//                    case iCadeButtonD:
//                        _valueChangedHandler(BUTTON_X, bPressed);
//                        break;
//                    case iCadeButtonE:
//                        _valueChangedHandler(BUTTON_L1, bPressed);
//                        break;
//                    case iCadeButtonF:
//                        _valueChangedHandler(BUTTON_R1, bPressed);
//                        break;
//                    case iCadeButtonG:
//                        _valueChangedHandler(BUTTON_SELECT, bPressed);
//                        break;
//                    case iCadeButtonH:
//                        _valueChangedHandler(BUTTON_START, bPressed);
//                        break;
//                    case iCadeJoystickUp:
//                        _valueChangedHandler(KEY_UP, bPressed);
//                        break;
//                    case iCadeJoystickRight:
//                        _valueChangedHandler(KEY_RIGHT, bPressed);
//                        break;
//                    case iCadeJoystickDown:
//                        _valueChangedHandler(KEY_DOWN, bPressed);
//                        break;
//                    case iCadeJoystickLeft:
//                        _valueChangedHandler(KEY_LEFT, bPressed);
//                        break;
//                    default:
//                        break;
//                }
//            }
//        };
//    }
}

@end

