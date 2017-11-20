//
//  GameController.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/15.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_GameController_h
#define MojingSDK_GameController_h

#import <Foundation/Foundation.h>
#import "MojingKeyCode.h"

@class MJGameController;

typedef void(^MFIButtonValueChangedHandler)(AXIS_GAMEPAD axisID, KEY_GAMEPAD button, BOOL bPressed);
typedef void(^MFIAxisValueChangedHandler)(AXIS_GAMEPAD axisID, float xValue, float yValue);

@interface MJGameController : NSObject

@property (copy) MFIButtonValueChangedHandler buttonValueChangedHandler;
@property (copy) MFIAxisValueChangedHandler axisValueChangedHandler;

- (void)registerKey;

+ (instancetype)sharedMFIGamepad;

@end

#endif
