//
//  MJGamepad.m..
//  ble
//
//  Created by 秦宏 on 15/5/21.
//
//

#import "MJGamepad.h"
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreGraphics/CoreGraphics.h>
#import <GameController/GameController.h>
#import "CWLSynthesizeSingleton.h"
#import "CBUUID+StringExtraction.h"
#import "MJGamepad_Structures.h"
#import "MJGamepadButton.h"
#import "MJGamepadDirectionPad.h"
#import "MJGamepadThumbStick.h"
#import "MJGamepadDirectionPad.h"

/*
 * 解密
 */
#define L01 1
#define L02 2
#define L03 3
#define L04 4
#define L05 5
#define L06 6
#define L07 7
#define L08 8
#define L09 9
#define L10 10
#define L11 11
#define L12 12
#define L13 13
#define L14 14
#define L15 15

#define B0  0
#define B1  1
#define B2  2
#define B3  3
#define B4  4
#define B5  5
#define B6  6
#define B7  7

const UInt8 jiami_data_tab[96][2] =
{
    {L13,B7},{L12,B6},{L11,B5},{L02,B1},{L02,B0},{L10,B7},{L09,B6},{L08,B5},
    {L13,B6},{L12,B5},{L11,B4},{L03,B1},{L03,B0},{L10,B6},{L09,B5},{L08,B4},
    {L13,B5},{L12,B4},{L11,B3},{L04,B1},{L04,B0},{L10,B5},{L09,B4},{L08,B3},
    {L13,B4},{L12,B3},{L11,B2},{L05,B1},{L05,B0},{L10,B4},{L09,B3},{L08,B2},
    {L13,B3},{L12,B2},{L11,B1},{L06,B1},{L06,B0},{L10,B3},{L09,B2},{L08,B1},
    {L13,B2},{L12,B1},{L11,B0},{L07,B1},{L07,B0},{L10,B2},{L09,B1},{L08,B0},
    {L07,B7},{L05,B6},{L06,B5},{L04,B4},{L02,B3},{L08,B7},{L08,B6},{L03,B5},
    {L07,B6},{L05,B5},{L06,B4},{L04,B3},{L02,B2},{L09,B7},{L09,B0},{L03,B4},
    {L07,B5},{L05,B4},{L06,B3},{L04,B2},{L02,B4},{L10,B1},{L10,B0},{L03,B3},
    {L07,B4},{L05,B3},{L06,B2},{L04,B5},{L02,B5},{L11,B7},{L11,B6},{L03,B2},
    {L07,B3},{L05,B2},{L06,B6},{L04,B6},{L02,B6},{L12,B7},{L12,B0},{L03,B6},
    {L07,B2},{L05,B7},{L06,B7},{L04,B7},{L02,B7},{L13,B1},{L13,B0},{L03,B7},
    
};

void dec(UInt8 *in_buff, UInt8 *out_buff)
{
    UInt8 i, j, m, n;
    for(i = 0; i < 14; i++)
    {
        out_buff[i] = 0;
    }
    for(i = 0; i < 12; i++)
    {
        for(j = 0; j < 8; j++)
        {
            n = jiami_data_tab[i*8+j][1];
            m = in_buff[i+2]&(1<<(7-j));
            if(m!=0)
            {
                out_buff[jiami_data_tab[i*8+j][0]] |= (1<<n);
            }
        }
    }
}

/*
UInt8 num_data_d(UInt8 x, UInt8 y)
{
    UInt8 count, t;
    count = 0;
    t = x; //数据
    for (int i=1; i <= 8; i++)
    {
        //奇数 && 当前位数据等于0或1
        if( ((i%2) != 0) && ((t & 0x01) == y) )
        {
            count++;
        }
        t = t >> 1;
    }
    return count;
}
UInt8 get_check_sum_proc(UInt8 x, UInt8 y)
{
    UInt8 sum = 0;
    UInt8 t;
    sum = num_data_d(x, 0); //x奇数位为0的个数
    sum += num_data_d(((y>>1)&0x7f), 1); //y偶数位为1的个数
    t = x >> 1;
    t = (t^y) & 0x55;
    t = (num_data_d(t, 1) << 1) & 0xfe;
    sum += t;
    if(sum >= 16)
    {
        sum = 0;
    }
    return sum;
}
//*/
//返回n的奇数位为cmp的数量
UInt8 getnum(UInt8 n, UInt8 cmp)
{
    UInt8 t = (n & 0x01)==cmp;
    t += ((n & 0x04)>>2)==cmp;
    t += ((n & 0x10)>>4)==cmp;
    t += ((n & 0x40)>>6)==cmp;
    return t;
}

/*
 Check_sum算法：
 共6bit, 它的值为: X轴奇数位为0的个数 + Y轴偶数位为1的个数 + X轴（8642）与 Y轴(7531)不相同位的个数（即异或为1）再乖以2,
 如果得到的结果大于或等于16则减去16
 当所得的值X轴Y轴同为0x80 时(-128) check_sum为0 此时所上传的数据为0x00 0x00
 
 例如解释出来的数据:  X轴为0x34    Y轴为0xf2
 展开为二进制为X轴为00110100      Y轴为11110010
 X轴奇数位为0110       Y轴偶数位为1101
 X轴8642位为0100      Y轴7531位为 1100     相对应位不相同的为x轴8与y轴的7共1个
 X轴奇数位为0的个数为2            Y轴偶数位为1的个数为3
 
 因此check_sum= 2+3+1*2=7
 */
UInt8 getchecksum(UInt8 x, UInt8 y)
{
    UInt8 t = 0;
    if (x == 0x80 && y == 0x80)
    {
        t = 0;
    }
    else
    {
        t = getnum(x, 0);
        t += getnum(y>>1, 1);
        t += (getnum((x>>1)^y & 0x55, 1)<<1) & 0xfe;
        if (t >= 16)
        {
            t = 0;
        }
    }
    return t;
}

UInt8 getchecksum2(UInt8 x, UInt8 y)
{
    UInt8 t = 0;
//    if (x == 0x80 && y == 0x80)
//    {
//        t = 0;
//    }
//    else
    {
        t = getnum(x, 0)<<1;
        t += getnum(y>>1, 1)<<2;
        t += (getnum((x>>1)^y & 0x55, 1)<<1) & 0xfe;
//        if (t >= 16)
//        {
//            t = 0;
//        }
    }
    return t;
}

MJGamepadButtonType radianToButtonType(CGFloat radian)
{
    MJGamepadButtonType buttonType = MJGamepadButtonUnknown;
    if (radian >= 45.f && radian < 135.f)
    {
        buttonType = MJGamepadButtonUp;
    }
    else if (radian >= -135.f && radian < -45.f)
    {
        buttonType = MJGamepadButtonDown;
    }
    else if ((radian >= 135.f && radian <= 180.f) || (radian > -180.f && radian < -135.f))
    {
        buttonType = MJGamepadButtonLeft;
    }
    else if ((radian >= 0.f && radian < 45.f) || (radian >= -45.f && radian < -0.f))
    {
        buttonType = MJGamepadButtonRight;
    }
    return buttonType;
}

@interface MJGamepad ()<CBCentralManagerDelegate, CBPeripheralDelegate>
{
    CBCentralManager *_centralManager;
    CBPeripheral *_peripheral;
    
    mj_buffer *_buffer;
}


- (BOOL)onReceiveNewKeyCode:(UInt8 *)rawData peripheralName:(NSString *)name;
- (void)processKeyStatus;
- (void)processThumbStick;

@end

@implementation MJGamepad
@synthesize btnCBCMEnabled = _btnCBCMEnabled;
@synthesize btnPadConnect = _btnPadConnect;
@synthesize buttonBack = _buttonBack;
@synthesize buttonStart = _buttonStart;
@synthesize buttonMenu = _buttonMenu;
@synthesize thumbStick = _thumbStick;
@synthesize vdpad = _vdpad;
@synthesize valueChangedHandler = _valueChangedHandler;
@synthesize sdkVersion = _sdkVersion;
@synthesize battery = _battery;

@synthesize centralManager = _centralManager;
@synthesize peripheral = _peripheral;
@synthesize centralManagerState = _centralManagerState;
@synthesize peripheralState = _peripheralState;
@synthesize isAutoScan = _isAutoScan;



CWL_SYNTHESIZE_SINGLETON_FOR_CLASS(MJGamepad)

#pragma mark - init

- (void)dealloc
{
    free(_buffer);
   
/*
    [_btnCBCMEnabled release];
    [_btnPadConnect release];
    [_buttonBack release];
    [_buttonStart release];
    [_buttonMenu release];
    [_thumbStick release];
    [_vdpad release];
    [_valueChangedHandler release];
    [_sdkVersion release];
    //[_battery release];
    
    [_centralManager release];
    [_peripheral release];
    [super dealloc];
*/
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _sdkVersion = @"1.0";
        _centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:nil];
        
        _buffer = (mj_buffer *)malloc(sizeof(mj_buffer));
        _buffer->all_flags = MJGamepadButtonUnknown;
        _buffer->raw_char_size = 0;
        _buffer->timestamp = 0;
        _buffer->left_thumb_point.x = 0.f;
        _buffer->left_thumb_point.y = 0.f;
        _buffer->right_thumb_point.x = 0.f;
        _buffer->right_thumb_point.y = 0.f;
        
        //
        _btnCBCMEnabled = [[MJGamepadButton alloc] init];
        _btnPadConnect = [[MJGamepadButton alloc] init];
        _buttonMenu = [[MJGamepadButton alloc] init];
        _buttonBack = [[MJGamepadButton alloc] init];
        _buttonStart = [[MJGamepadButton alloc] init];
        
        _thumbStick = [[MJGamepadThumbStick alloc] init];
        _vdpad = [[MJGamepadDirectionPad alloc] init];
    }
    
    return self;
}

#pragma mark - 

- (void)processKeyStatus
{
    [_buttonStart setTimestamp:_buffer->timestamp];
    [_buttonStart setPressed:(_buffer->start)==0?NO:YES];
    [_buttonBack setTimestamp:_buffer->timestamp];
    [_buttonBack setPressed:(_buffer->back)==0?NO:YES];
    [_buttonMenu setTimestamp:_buffer->timestamp];
    [_buttonMenu setPressed:(_buffer->menu)==0?NO:YES];
}

- (void)processThumbStick
{
    [_thumbStick setTimestamp:_buffer->timestamp];
    [_thumbStick setPoint:_buffer->left_thumb_point];
    
    [_vdpad setTimestamp:_buffer->timestamp];
    mj_direction vpad_buff;
    vpad_buff.all_flags = 0;
    vpad_buff.up = _buffer->up;
    vpad_buff.down = _buffer->down;
    vpad_buff.left = _buffer->left;
    vpad_buff.right = _buffer->right;
    vpad_buff.center = _buffer->center;
    [_vdpad setBuffer:vpad_buff];
}

- (CGFloat)ConvertMotionData:(UInt8)lsb msb:(UInt8)msb
{
    UInt16 n = msb<<8 | lsb;
    CGFloat f = 0;
    if(n == 0)
    {
        return 0;
    }
    if(n <= 0x8000)
    {
        f = n/32768.f;
    }
    else
    {
        f = (0x8000-n)/32767.f;
    }

//  NSLog(@"lsb : %d ---- msb : %d ---- Uint16 : %d ----- CGFloat : %f", lsb, msb, n, f);
    return f;
}

//收到新数据
- (BOOL)onReceiveNewKeyCode:(UInt8 *)raw_data peripheralName:(NSString *)name
{
    //设备键值
    BOOL is_ok = NO;
    BOOL is_c = NO;
    BOOL is_menu = NO;
    UInt8 x, y = 0;
    CGFloat m,n = 0.f;
    CGFloat radian = 0.f;
    CGFloat offset_to_center = 0.f;
        
    //分别处理2种型号的设备数据，得到键值
    if (_buffer->raw_char_size == 16)
    {
        if ([name isEqualToString:@"Mojing"] || [name isEqualToString:@"3DBB"]) //3DBB
        {
            x = raw_data[2]; // X AXIS
            y = raw_data[1]; // Y AXIS
            UInt8 btn = raw_data[3];
            is_ok = (btn & 0x10) == 0x10;
            is_menu = (btn & 0x40) == 0x40;
            is_c = (btn & 0x20) == 0x20;
            CGFloat fGryo_X = [self ConvertMotionData:raw_data[4] msb:raw_data[5]];
            CGFloat fGryo_Y = [self ConvertMotionData:raw_data[6] msb:raw_data[7]];
            CGFloat fGryo_Z = [self ConvertMotionData:raw_data[8] msb:raw_data[9]];
            CGFloat fGSensor_X = [self ConvertMotionData:raw_data[10] msb:raw_data[11]];
            CGFloat fGSensor_Y = [self ConvertMotionData:raw_data[12] msb:raw_data[13]];
            CGFloat fGSensor_Z = [self ConvertMotionData:raw_data[14] msb:raw_data[15]];
            
            if(fGryo_X != 0 && fGryo_Y !=0)
                NSLog(@"Gryo : ( x:%f y:%f z:%f ) ----- GSensor : ( x:%f y:%f z:%f )", fGryo_X, fGryo_Y, fGryo_Z, fGSensor_X, fGSensor_Y, fGSensor_Z);
        }
        else
        {
            x = raw_data[2]; // X AXIS
            y = raw_data[3]; // Y AXIS
            //UInt8 z = raw_data[4]; // Z Axis
            //UInt8 rz = raw_data[5]; // RZ
            //UInt8 br = raw_data[6]; // BREAK
            //UInt8 gas = raw_data[7]; // GAS
            UInt8 btn1 = raw_data[8]; // btn1
            UInt8 btn2 = raw_data[9]; // btn2
            //UInt8 btn3 = raw_data[10]; // btn3
            //UInt8 rv1 = raw_data[11]; // reverse
            //UInt8 rv2 = raw_data[12]; // reverse
            //UInt8 rv3 = raw_data[13]; // reverse

            /*
             btn1:
             Mojing4版本：0x40 0010 0000 - OK    (Mojing版本: 0x40)
             Mojing4版本：0x10 0001 0000 - Menu  (Mojing版本: 0x01)
             btn2:
             Mojing4版本：0x01 0000 0001 - C     (Mojing版本: 0x01)
         */
            is_ok = (btn1 & 0x40) == 0x40;
            is_menu = (btn1 & 0x10) == 0x10;
            is_c = (btn2 & 0x01) == 0x01;
        }
        
        //convert xy to an unit length
        CGFloat unit_x = x <= 0x80 ? 128.f : 127.f;
        CGFloat unit_y = y <= 0x80 ? 128.f : 127.f;
        CGFloat ox = (x - 0x80)/unit_x;
        CGFloat oy = (0x80 - y)/unit_y;
        //mapping rect to an unit circle (r = 1.0f)
        m = ox * sqrtf(1.f - 0.5f*oy*oy);
        n = oy * sqrtf(1.f - 0.5f*ox*ox);
        if ([name isEqualToString:@"Mojing"] || [name isEqualToString:@"3DBB"]) //3DBB
        {
            m = -m;
            n = -n;
        }
        //radian
        radian = atan2f(n, m) * 180.f * M_1_PI;
        //offsetToCenter
        offset_to_center = sqrtf(m*m + n*n);
//      NSLog(@"ok:%x c:%x menu:%x, m:%f, n:%f, radian:%f, offset: %f, direction: %d", is_ok, is_c, is_menu, m, n, radian, offset_to_center, radianToButtonType(radian));
        
    }
    //*6字节旧版本（TI版本）协议废弃
//    else if (_buffer->raw_char_size == 6)
//    {
//        //旧版
//        /*
//         数据格式: 6 bytes
//         byte0: KEY1
//         byte1: KEY2
//         byte2: X低8位
//         byte3: X高8位
//         byte4: Y低8位
//         byte5: Y高8位
//         
//         按键数据解释：
//         KEY1:
//         0x40 = 0100 0000 - OK键
//         0x80 = 1000 0000 - C键
//         KEY2:
//         01 = 0000 0001 - menu键
//         
//         x轴或y轴数据解释:
//         0000 0000 0000 1111 - 0x000f X或Y中间4位
//         0000 0000 0111 0000 - 0x0070 checksum3位
//         0000 0001 1000 0000 - 0x0180 X或Y高2位
//         0000 0110 0000 0000 - 0x0600 X或Y低2位
//         0000 1000 0000 0000 - 0x0800 翻转位
//         1111 0000 0000 0000 - 0xf000 符号位
//         
//         符号位: 1111为负数, 000为正数
//         翻转位: 每隔一次翻转一次
//         */
//        UInt8 key1 = raw_data[0];
//        UInt8 key2 = raw_data[1];
//        UInt16 low_x = raw_data[2];
//        UInt16 high_x = raw_data[3];
//        UInt16 low_y = raw_data[4];
//        UInt16 high_y = raw_data[5];
//        
//        UInt16 x_data = high_x << 8 | low_x;
//        UInt16 y_data = high_y << 8 | low_y;
//        
//        UInt8 x = ((x_data & 0x0180) >> 1) | ((x_data & 0x000f) << 2) | ((x_data & 0x0600) >> 9);
//        UInt8 y = ((y_data & 0x0180) >> 1) | ((y_data & 0x000f) << 2) | ((y_data & 0x0600) >> 9);
//        //UInt8 symbol_x = (x_data & 0xf000) >> 12;
//        //UInt8 symbol_y = (y_data & 0xf000) >> 12;
//        //UInt8 checksum = ((x_data & 0x0070) >> 1) | ((y_data & 0x0070) >> 4);
//        
//        if (x_data == 0 && y_data == 0)
//        {
//            x = 0x80;
//            y = 0x80;
//            //checksum = 0;
//        }
//        
//        is_ok = ((key1 & 0x40) >> 6) == 1;
//        is_c = ((key1 & 0x80) >> 7) == 1;
//        is_menu = (key2 & 0x01) == 1;
//        
//        //convert xy to an unit length
//        CGFloat unit_x = x <= 0x80 ? 128.f : 127.f;
//        CGFloat unit_y = y <= 0x80 ? 128.f : 127.f;
//        CGFloat ox = (x - 0x80)/unit_x;
//        CGFloat oy = (0x80 - y)/unit_y;
//        //mapping rect to an unit circle (r = 1.0f)
//        m = ox * sqrtf(1.f - 0.5f*oy*oy);
//        n = oy * sqrtf(1.f - 0.5f*ox*ox);
//        //radian
//        radian = atan2f(n, m) * 180.f * M_1_PI;
//        //offsetToCenter
//        offset_to_center = sqrtf(m*m + n*n);
//    }
    //*/
    else
    {
        return NO;
    }
    
    //*处理得到后的键值

    //更新状态字
    MJGamepadButtonType currentKeyStatus = MJGamepadButtonUnknown;
    BOOL leftThumbChanged = (m != _buffer->left_thumb_point.x || n != _buffer->left_thumb_point.y);
    if (is_ok)
    {
        currentKeyStatus |= MJGamepadButtonStart;
    }
    if (is_c)
    {
        currentKeyStatus |= MJGamepadButtonBack;
    }
    if (is_menu)
    {
        currentKeyStatus |= MJGamepadButtonMenu;
    }
    if (leftThumbChanged)
    {
        //摇杆改变
        currentKeyStatus |= MJGamepadButtonLeftThumb;
        
        //虚拟dpad
        // 判断触发半径
        if (offset_to_center >= 0.75f)
        {
            MJGamepadButtonType vdpadButtonType = radianToButtonType(radian);
            if (vdpadButtonType == MJGamepadButtonUp)
            {
                currentKeyStatus |= MJGamepadButtonUp;
            }
            else if (vdpadButtonType == MJGamepadButtonDown)
            {
                currentKeyStatus |= MJGamepadButtonDown;
            }
            else if (vdpadButtonType == MJGamepadButtonLeft)
            {
                currentKeyStatus |= MJGamepadButtonLeft;
            }
            else if (vdpadButtonType == MJGamepadButtonRight && (m != 0.f || n != 0.f))
            {
                //虚拟dpad当摇杆为0时方向也是右
                currentKeyStatus |= MJGamepadButtonRight;
            }
        }
        else
        {
           currentKeyStatus |= MJGamepadButtonCenter;
        }
    }
    
    //状态字改变 或者 摇杆改变
    if (currentKeyStatus != _buffer->all_flags || leftThumbChanged)
    {
        _buffer->all_flags = currentKeyStatus;
        _buffer->left_thumb_point.x = m;
        _buffer->left_thumb_point.y = n;
        _buffer->timestamp = time(NULL);
        
        [self processKeyStatus];
        
        if (leftThumbChanged)
        {
            [self processThumbStick];
        }
        
        if (_valueChangedHandler)
        {
            _valueChangedHandler(self, nil);
        }
    }
    
    //*/
    return YES;
}

#pragma mark - CBCentralManager Delegate
- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    NSString *state = nil;
    
    [self updateStatus];
    
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
            if (self.peripheral != nil)
            {
                 [_btnPadConnect setTimestamp:_buffer->timestamp];
                 [_btnPadConnect setPressed:NO];
            }
            [_btnCBCMEnabled setTimestamp:_buffer->timestamp];
            [_btnCBCMEnabled setPressed:NO];
            break;
        case CBCentralManagerStatePoweredOn:
            state = @"CBCentralManagerStatePoweredOn";
            [_btnCBCMEnabled setTimestamp:_buffer->timestamp];
            [_btnCBCMEnabled setPressed:YES];
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
    
    //NSLog(@"didDiscoverPeripheral: %@", peripheral);
    //6字节旧版本（TI版本）协议名称废弃
    if ([peripheral.name isEqualToString:@"Mojing"]
        || [peripheral.name isEqualToString:@"3DBB"]
        || [peripheral.name isEqualToString:@"Mojing4"])
    {
        NSLog(@"Discover peripheral name : %@", peripheral.name);
        
        NSLog(@"peripheral.state == %ld", (long)peripheral.state);
        
        if (peripheral.state == CBPeripheralStateDisconnected)
        {
            peripheral.delegate = self;
            self.peripheral = peripheral;//[peripheral retain];
            [central connectPeripheral:peripheral options:nil];
        }
    }
    
    [self updateStatus];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSLog(@"didConnectPeripheral");

    [_btnPadConnect setTimestamp:_buffer->timestamp];
    [_btnPadConnect setPressed:YES];
  
    self.peripheral = peripheral;
    [self updateStatus];

    [central stopScan];
    [peripheral discoverServices:nil];
    
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"didDisconnectPeripheral");
    
    self.peripheral = peripheral;
    [self updateStatus];
    
    [_btnPadConnect setTimestamp:_buffer->timestamp];
    [_btnPadConnect setPressed:NO];

    if (_isAutoScan)
    {
        [central scanForPeripheralsWithServices:nil options:nil];
    }
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"didFailToConnectPeripheral");
    [self updateStatus];
}

- (void)centralManager:(CBCentralManager *)central didRetrievePeripherals:(NSArray *)peripherals
{
    NSLog(@"didRetrievePeripherals");
    [self updateStatus];
}

- (void)centralManager:(CBCentralManager *)central didRetrieveConnectedPeripherals:(NSArray *)peripherals
{
    NSLog(@"didRetrieveConnectedPeripherals");
    [self updateStatus];
}

- (void)centralManager:(CBCentralManager *)central willRestoreState:(NSDictionary *)dict
{
    NSLog(@"willRestoreState");
    [self updateStatus];
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
            uuidString = [characteristic.UUID representativeString];
        }
        //NSLog(@"uuid: %@", uuidString);
        //*/

        NSLog(@"peripheral %@ Discovered characteristic %@ for service %@", peripheral, characteristic, service);
        // reading the value of a characteristic
        //[peripheral readValueForCharacteristic:characteristic];
        
        //只订阅FFF4(由于硬件原因订阅其他值可能会出问题)
        /*
         旧版：FFF0, 新版: FFF4
         */
        if ([uuidString isEqualToString:@"FFF4"] || [uuidString isEqualToString:@"FFF0"])
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
        uuidString = [characteristic.UUID representativeString];
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
        if (valueLength == 6)
        {
            //旧版
            _buffer->raw_char_size = 6;
            
            //解析
            UInt8 low_x = raw_data[2];
            UInt8 high_x = raw_data[3];
            UInt8 low_y = raw_data[4];
            UInt8 high_y = raw_data[5];
            
            UInt16 x_data = high_x << 8 | low_x;
            UInt16 y_data = high_y << 8 | low_y;
            
            UInt8 x = ((x_data & 0x0180) >> 1) | ((x_data & 0x000f) << 2) | ((x_data & 0x0600) >> 9);
            UInt8 y = ((y_data & 0x0180) >> 1) | ((y_data & 0x000f) << 2) | ((y_data & 0x0600) >> 9);
            UInt8 checksum = ((x_data & 0x0070) >> 1) | ((y_data & 0x0070) >> 4);
            if (x_data == 0 && y_data == 0)
            {
                x = 0x80;
                y = 0x80;
                checksum = 0;
            }
            
            //校验
            UInt8 sum = getchecksum(x, y);
            if (sum != checksum)
            {
                return;
            }
            
            //处理
            [self onReceiveNewKeyCode:raw_data peripheralName:peripheral.name];
        }
        else if (valueLength == 16)
        {
            _buffer->raw_char_size = 16;
            
            if ([peripheral.name isEqualToString:@"Mojing"] || [peripheral.name isEqualToString:@"3DBB"])
            {
                //3DBB暂未加密
                [self onReceiveNewKeyCode:raw_data peripheralName:peripheral.name];
            }
            else
            {
                //mojing4
                if (raw_data[0] == 0xa1 && raw_data[1] == 0xc4)
                {
                    //头2字节符合
                    UInt8 t[16];
                    //解密
                    dec(raw_data, t);
                    //校验
                    UInt8 cs1 = getchecksum2(t[4], t[5]);
                    UInt8 cs2 = getchecksum2(t[2], t[3]);
                    if (cs1 != raw_data[14] || cs2 != raw_data[15])
                    {
                        NSData *data = [NSData dataWithBytes:t length:16];
                        NSLog(@"Checksum Error: %@, %x, %x", data, cs1, cs2);
                        return;
                    }
                    //处理
                    [self onReceiveNewKeyCode:t peripheralName:peripheral.name];
                }
            }
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

#pragma mark - scan

- (void)updateStatus
{
    self.peripheralState = _peripheral.state;

    if(self.peripheralState != CBPeripheralStateDisconnected)
    {
        NSLog(@"--->updateStatus: %ld", (long)self.peripheralState);
    }
    
    self.centralManagerState = _centralManager.state;
}

- (void)scan
{
    if (_centralManager != nil && [_centralManager isKindOfClass:[CBCentralManager class]])
    {
        [_centralManager scanForPeripheralsWithServices:nil options:nil];
    }
}

- (void)stopScan
{
    if (_centralManager != nil && [_centralManager isKindOfClass:[CBCentralManager class]])
    {
        [_centralManager stopScan];
    }
}

- (void)disconnect
{
    if (_centralManager != nil && [_centralManager isKindOfClass:[CBCentralManager class]]
        && _peripheral != nil && [_peripheral isKindOfClass:[CBPeripheral class]])
    {
        [_centralManager cancelPeripheralConnection:_peripheral];
    }
}


@end
