//
//  MJAudioSession.h
//  MJMRemoteTest
//
//  Created by HUO on 16/8/5.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MojingKeyCode.h"

typedef void(^MJAudioSessionHandler)(KEY_GAMEPAD keyEvent, BOOL pressed);

@interface MJAudioSession : NSObject

+ (instancetype)sharedManager;

@property (copy) MJAudioSessionHandler audioSessionHandler;

@end
