//
//  MojingUIView.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/17.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_iCadeGamePad_h
#define MojingSDK_iCadeGamePad_h

#import <Foundation/Foundation.h>
#import "MojingKeyCode.h"
#import "iCadeReaderView.h"

@class MJiCadeGamePad;

typedef void(^MJiCadeGamepadValueChangedHandler)(KEY_GAMEPAD keyID, BOOL bPressed);

//@interface MJiCadeGamePad : NSObject<iCadeEventDelegate>
@interface MJiCadeGamePad : NSObject
{
    iCadeReaderView *control;
}

@property (copy) MJiCadeGamepadValueChangedHandler valueChangedHandler;

- (void)registerKey :(UIViewController* )viewCon;

+ (instancetype)sharediCadeGamepad;
@end

#endif
