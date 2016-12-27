//
//  MJTuner.h
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#ifndef MojingSDK_MJTuner_h
#define MojingSDK_MJTuner_h

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "MJTunerButton.h"
#import "MJTunerThumb.h"

@class MJTuner, MJTunerButton, MJTunerThumb;

typedef void(^MJTunerValueChangedHandler)(MJTuner *tuner, NSObject *element);

typedef NS_ENUM(NSInteger, MJTunerState) {
    MJTunerStateDisconnected = 0,
    MJTunerStateConnecting,
    MJTunerStateConnected,
};

@interface MJTuner : NSObject

@property (nonatomic, retain) MJTunerButton *btnConnect; //Check Tuner connect or not
@property (nonatomic, retain) MJTunerButton *buttonMenu;
@property (nonatomic, retain) MJTunerThumb *thumbTurn;
@property (copy) MJTunerValueChangedHandler valueChangedHandler;

@property (nonatomic, assign, readonly) int battery;
@property (nonatomic, readonly, getter=getState) MJTunerState state;
@property (nonatomic, retain) CBCentralManager *centralManager;
@property (nonatomic, retain) CBPeripheral *peripheral;

+ (instancetype)sharedMJTuner;

@end

#endif
