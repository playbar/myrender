//
//  MJTuner.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/7.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import <CoreGraphics/CoreGraphics.h>
#import "MJTuner.h"
#import "CWLSynthesizeSingleton.h"
#import "MJTuner_Structures.h"

@interface MJTuner ()<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    CBCentralManager *_centralManager;
    CBPeripheral *_peripheral;
    
    mj_tuner_buffer *_buffer;
}

- (BOOL)onReceiveNewKeyCode:(UInt8 *)rawData;
- (void)processKeyStatus;

@end

@implementation MJTuner
@synthesize btnConnect = _btnConnect;
@synthesize buttonMenu = _buttonMenu;
@synthesize thumbTurn = _thumbTurn;
@synthesize valueChangedHandler = _valueChangedHandler;

@synthesize battery = _battery;
@synthesize state = _state;

@synthesize centralManager = _centralManager;
@synthesize peripheral = _peripheral;

CWL_SYNTHESIZE_SINGLETON_FOR_CLASS(MJTuner)

#pragma mark - init

- (void)dealloc
{
    free(_buffer);
    
   /*
    [_btnConnect release];
    [_buttonMenu release];
    [_thumbTurn release];
    [_valueChangedHandler release];
    [_centralManager release];
    [_peripheral release];
     //[_battery release];
    //[_state release];
    
    [super dealloc];
    */
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:nil];
        
        _buffer = (mj_tuner_buffer *)malloc(sizeof(mj_tuner_buffer));
        _buffer->all_flags = MJTunerButtonUnknown;
        _buffer->raw_char_size = 0;
        _buffer->timestamp = 0;
        
        //
        _btnConnect = [[MJTunerButton alloc] init];
        _buttonMenu = [[MJTunerButton alloc] init];
        _thumbTurn = [[MJTunerThumb alloc] init];
    }
    
    return self;
}

#pragma mark -

- (void)processKeyStatus
{
    [_buttonMenu setTimestamp:_buffer->timestamp];
    [_buttonMenu setPressed:(_buffer->menu)==0?NO:YES];
}

- (void)processThumbTurn
{
    [_thumbTurn setTimestamp:_buffer->timestamp];
    [_thumbTurn setTurnValue:_buffer->value_tuner];
}

//收到新数据
- (BOOL)onReceiveNewKeyCode:(UInt8 *)raw_data
{
    //设备键值
    BOOL is_menu = NO;
    uint8_t value_tuner;
    //分别处理2种型号的设备数据，得到键值
    if (_buffer->raw_char_size == 15)
    {
        UInt8 btn = raw_data[13]; // btn1
        is_menu = btn > 0;
        value_tuner = (uint8_t)raw_data[14];
    }
    else
    {
        return NO;
    }
    
    //*处理得到后的键值
    
    //更新状态字
    MJTunerButtonType currentKeyStatus = MJTunerButtonUnknown;

    if (is_menu)
    {
        currentKeyStatus |= MJTunerButtonMenu;
    }
    
    //状态字改变 或者 摇杆改变
    if (currentKeyStatus != _buffer->all_flags
        || abs(value_tuner - _buffer->value_tuner) > 1)
    {
        _buffer->all_flags = currentKeyStatus;
        _buffer->value_tuner = value_tuner;
        _buffer->timestamp = time(NULL);
        
        [self processThumbTurn];
        
        [self processKeyStatus];
                
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, nil);
        }
    }
    
    //*/
    
    return YES;
}

- (MJTunerState)state
{
    return (MJTunerState)self.peripheral.state;
}

#pragma mark - CBCentralManager Delegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSString *state = nil;
    switch ([central state])
    {
        case CBCentralManagerStateUnknown:
            state = @"CBCentralManagerStateUnknown";
            break;
        case CBCentralManagerStateResetting:
            state = @"CBCentralManagerStateResetting";
            break;
        case CBCentralManagerStateUnsupported:
            state = @"CBCentralManagerStateUnsupported";
            break;
        case CBCentralManagerStateUnauthorized:
            state = @"CBCentralManagerStateUnauthorized";
            break;
        case CBCentralManagerStatePoweredOff:
            state = @"CBCentralManagerStatePoweredOff";
            break;
        case CBCentralManagerStatePoweredOn:
            state = @"CBCentralManagerStatePoweredOn";
            [central scanForPeripheralsWithServices:nil options:nil];
            break;
        default:
            break;
    }
    NSLog(@"centralManagerDidUpdateState: %@", state);
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    if(peripheral == nil)
    {
        return;
    }
    
    NSLog(@"didDiscoverPeripheral: %@", peripheral.name);
    //6字节旧版本（TI版本）协议名称废弃
    if (/*[peripheral.name isEqualToString:@"Mojing"] || [peripheral.name isEqualToString:@"魔镜"] ||*/[peripheral.name isEqualToString:@"Mojing"])
    {
        NSLog(@"发现Mojing Tuner.");
        
        NSLog(@"peripheral.state == %ld", (long)peripheral.state);
        
        //self.state = (MJTunerState)peripheral.state;
        
        if (peripheral.state == CBPeripheralStateDisconnected)
        {
            peripheral.delegate = self;
            self.peripheral = peripheral;//[peripheral retain];
            [central connectPeripheral:peripheral options:nil];
        }
    }
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    [_btnConnect setTimestamp:_buffer->timestamp];
    [_btnConnect setPressed:YES];
    //self.state = (MJTunerState)peripheral.state;
    
    [central stopScan];
    [peripheral discoverServices:nil];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"didDisconnectPeripheral");
    
    [_btnConnect setTimestamp:_buffer->timestamp];
    [_btnConnect setPressed:NO];
    //self.state = (MJTunerState)peripheral.state;
    
    [central scanForPeripheralsWithServices:nil options:nil];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"didFailToConnectPeripheral");
    //self.state = (MJTunerState)peripheral.state;
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals
{
    NSLog(@"didRetrievePeripherals");
}

- (void)centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals
{
    NSLog(@"didRetrieveConnectedPeripherals");
}

- (void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary *)dict
{
    NSLog(@"willRestoreState");
}

#pragma mark - CBPeripheralManager Delegate
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error discovering services: %@", [error localizedDescription]);
        return;
    }
    
    for (CBService *service in peripheral.services)
    {
        NSLog(@"peripheral %@ Discovered service: %@", peripheral, service);
        
        [peripheral discoverCharacteristics:nil forService:service];
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error discovering characteristics: %@", [error localizedDescription]);
        return;
    }
    
    for (CBCharacteristic *characteristic in service.characteristics)
    {
        //*
        NSString *uuidString = nil;
        if ([characteristic.UUID respondsToSelector:@selector(UUIDString)])
        {
            //iOS 7.1+
            uuidString = characteristic.UUID.UUIDString;
        }
        else
        {
            // < iOS 7.1
            //uuidString = [characteristic.UUID representativeString];
        }
        //NSLog(@"uuid: %@", uuidString);
        //*/
        
        NSLog(@"peripheral %@ Discovered characteristic %@ for service %@", peripheral, characteristic, service);
        // reading the value of a characteristic
        //[peripheral readValueForCharacteristic:characteristic];
        
        //只订阅FFF4(由于硬件原因订阅其他值可能会出问题)
        if ([uuidString isEqualToString:@"FFF4"])
        {
            [peripheral setNotifyValue:YES forCharacteristic:characteristic];
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error reading characteristics: %@", [error localizedDescription]);
        return;
    }
    
    //NSLog(@"UUID: %@, value: %@", characteristic.UUID, characteristic.value);
    
    NSData *valueData = characteristic.value; //数据
    NSUInteger valueLength = valueData.length; //数据长度
    UInt8 *raw_data = (UInt8 *)[valueData bytes]; //原始数据
    
    //*
    NSString *uuidString = nil;
    if ([characteristic.UUID respondsToSelector:@selector(UUIDString)])
    {
        //iOS 7.1+
        uuidString = characteristic.UUID.UUIDString;
    }
    else
    {
        // < iOS 7.1
        //uuidString = [characteristic.UUID representativeString];
    }
    //NSLog(@"uuid: %@", uuidString);
    //*/
    if ([uuidString isEqualToString:@"Battery Level"])
    {
        //电量
        if (valueLength != 1) return;
        _battery = raw_data[0]; //raw battery value
    }
    else if ([uuidString isEqualToString:@"FFF4"])
    {
        //键值
        if (valueLength == 15)
        {
            //新版
            _buffer->raw_char_size = 15;
            
            //处理
            [self onReceiveNewKeyCode:raw_data];
        }
        else
        {
            return;
        }
    }
    else if ([uuidString isEqualToString:@"2A31"])
    {
        // 此数据不用关心
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
    if (error)
    {
        NSLog(@"Error changing notification state: %@", [error description]);
    }
    
    // Notification has started
    if (characteristic.isNotifying)
    {
        NSLog(@"Notification began on %@", characteristic);
    }
}

@end
