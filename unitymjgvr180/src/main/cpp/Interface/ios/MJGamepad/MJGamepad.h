//
//  MJGamepad.h
//  MJGamepadSDK
//
//  Created by 秦宏 on 15/5/21.
//
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "MJGamepadElement.h"
#import "MJGamepad_Structures.h"
#import "MJGamepadButton.h"
#import "MJGamepadThumbStick.h"
#import "MJGamepadDirectionPad.h"
#import "MJGamepadSensor.h"

MJGamepadButtonType radianToButtonType(CGFloat radian);

@class MJGamepadButton, MJGamepadDirectionPad, MJGamepadThumbStick, MJGamepadSensor, MJGamepad, MJGamepadElement;

typedef void(^MJGamepadValueChangedHandler)(MJGamepad *gamepad, MJGamepadElement *element);

typedef NS_ENUM(NSInteger, MJGamepadState) {
    MJGamepadStateDisconnected = 0,
    MJGamepadStateConnecting,
    MJGamepadStateConnected,
};

@interface MJGamepad : NSObject

@property (nonatomic, retain) MJGamepadButton *btnCBCMEnabled;   //Check BluetoothAdapter is enabled or not
@property (nonatomic, retain) MJGamepadButton *btnPadConnect;   //Check GamePad connect or not
@property (nonatomic, retain) MJGamepadButton *buttonBack;
@property (nonatomic, retain) MJGamepadButton *buttonStart;
@property (nonatomic, retain) MJGamepadButton *buttonMenu;
@property (nonatomic, retain) MJGamepadButton *buttonHome;
@property (nonatomic, retain) MJGamepadButton *buttonClick;
@property (nonatomic, retain) MJGamepadButton *btnVolumeDown;
@property (nonatomic, retain) MJGamepadButton *btnVolumeUp;
@property (nonatomic, retain) MJGamepadThumbStick *thumbStick;
@property (nonatomic, retain) MJGamepadSensor *sensorStick;
@property (nonatomic, retain) MJGamepadDirectionPad *vdpad;
@property (copy) MJGamepadValueChangedHandler valueChangedHandler;

@property (nonatomic, retain, readonly) NSString *sdkVersion;
@property (nonatomic, assign, readonly) int battery;
@property (nonatomic, retain) CBCentralManager *centralManager;
@property (nonatomic, retain) CBPeripheral *peripheral;
@property (nonatomic) CBCentralManagerState centralManagerState;
@property (nonatomic) CBPeripheralState peripheralState;
@property (nonatomic) BOOL isAutoScan;
@property (nonatomic, strong) NSMutableDictionary *deviceInfoCharValueDictionary;


- (void)scan; 		//扫描蓝牙设备
- (void)stopScan; 	//停止扫描蓝牙设备
- (void)disconnect; //主动断开

+ (instancetype)sharedMJGamepad;

- (NSDictionary *)getDeviceInfoCharValueDictionary;

@end
