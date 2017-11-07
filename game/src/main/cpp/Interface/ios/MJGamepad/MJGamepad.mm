//
//  MJGamepad.m..
//  ble
//
//  Created by 秦宏 on 15/5/21.
//
//

#include "JoystickData_Mojing.h"

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
#import "MJGamepadConst.h"

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
    bool m_bConnecting;
    int m_nReconnectCount;
    long m_LastStartScanTime;
    JoystickData_Mojing *m_pJoystick;
    
    float m_joystickDistance;
    
    NSMutableArray *_deviceInfoServerArray;
}

- (void)startScanning:(CBCentralManager *)central;
- (BOOL)onReceiveNewKeyCode:(UInt8 *)rawData peripheralName:(NSString *)name;
- (BOOL)onReceiveMJMotionKeyCode:(UInt8 *)raw_data peripheralName:(NSString *)name;
- (void)processKeyStatus;
- (void)processThumbStick;
- (void)processTouchPadPos;
- (void)processSensorData;

@end

@implementation MJGamepad
@synthesize btnCBCMEnabled = _btnCBCMEnabled;
@synthesize btnPadConnect = _btnPadConnect;
@synthesize buttonBack = _buttonBack;
@synthesize buttonStart = _buttonStart;
@synthesize buttonHome = _buttonHome;
@synthesize buttonClick = _buttonClick;
@synthesize buttonMenu = _buttonMenu;
@synthesize btnVolumeDown = _btnVolumeDown;
@synthesize btnVolumeUp = _btnVolumeUp;
@synthesize thumbStick = _thumbStick;
@synthesize sensorStick = _sensorStick;
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
    if(m_pJoystick)
    {
        delete m_pJoystick;
    }
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
        _centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil options:@{CBCentralManagerOptionShowPowerAlertKey:[NSNumber numberWithBool:NO]}];
        
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
        _buttonHome = [[MJGamepadButton alloc] init];
        _buttonClick = [[MJGamepadButton alloc] init];
        _btnVolumeDown = [[MJGamepadButton alloc] init];
        _btnVolumeUp = [[MJGamepadButton alloc] init];
        
        _sensorStick = [[MJGamepadSensor alloc] init];
        _thumbStick = [[MJGamepadThumbStick alloc] init];
        _vdpad = [[MJGamepadDirectionPad alloc] init];
        
        _deviceInfoServerArray = [[NSMutableArray alloc]init];
        
        m_pJoystick = new JoystickData_Mojing();
        m_nReconnectCount = 0;
        _deviceInfoCharValueDictionary = [[NSMutableDictionary alloc]init];
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
    [_buttonHome setTimestamp:_buffer->timestamp];
    [_buttonHome setPressed:(_buffer->home)==0?NO:YES];
    [_buttonClick setTimestamp:_buffer->timestamp];
    [_buttonClick setPressed:(_buffer->click)==0?NO:YES];
    [_btnVolumeUp setTimestamp:_buffer->timestamp];
    [_btnVolumeUp setPressed:(_buffer->volume_up)==0?NO:YES];
    [_btnVolumeDown setTimestamp:_buffer->timestamp];
    [_btnVolumeDown setPressed:(_buffer->volume_down)==0?NO:YES];
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

- (void)processTouchPadPos
{
    [_thumbStick setTimestamp:_buffer->timestamp];
    [_thumbStick setTouchPoint:_buffer->touchpad_point];
}

- (void)processSensorData
{
    [_sensorStick setTimestamp:_buffer->timestamp];
    [_sensorStick setHomeKeyStatus:_buffer->timestamp keyStaus:(_buffer->home)==0?NO:YES];
    [_sensorStick setSensorData:_buffer->sensor_data];
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

//解析Mojing Motion数据
- (BOOL)onReceiveMJMotionKeyCode:(UInt8 *)raw_data peripheralName:(NSString *)name
{
    if(m_pJoystick == NULL) return FALSE;
    
    STJoystickData joystickData;
    if(m_pJoystick->BytesToData(raw_data, &joystickData) == false)
    {
        return FALSE;
    }
    
    //更新状态字
    uint32_t currentKeyStatus = MJGamepadButtonUnknown;
    //设备键值
    if (joystickData.keyState & 0x1 || joystickData.keyState & 0x20)
    {
        currentKeyStatus |= MJGamepadButtonClick;
    }
    if (joystickData.keyState & 0x2)
    {
        currentKeyStatus |= MJGamepadButtonHome;
    }
    if (joystickData.keyState & 0x4)
    {
        currentKeyStatus |= MJGamepadButtonBack;
    }
    if (joystickData.keyState & 0x8)
    {
        currentKeyStatus |= MJGamepadButtonVolumeDown;
    }
    if (joystickData.keyState & 0x10)
    {
        currentKeyStatus |= MJGamepadButtonVolumeUp;
    }

    BOOL touchPadChanged = joystickData.bTouched;//(joystickData.fTouchpadX != _buffer->touchpad_point.x || joystickData.fTouchpadY != _buffer->touchpad_point.y);
    if (touchPadChanged)
    {
        //暂未发送touch状态
        currentKeyStatus |= MJGamepadButtonTouchPad;
    }
    
    _buffer->timestamp = joystickData.dTimestamp;
    //按键
    if ( currentKeyStatus != _buffer->all_flags )
    {
        _buffer->all_flags = currentKeyStatus;
        [self processKeyStatus];
    }
    
    //Touch
    static bool bLastSendTouchPoint = false;
    if (touchPadChanged)
    {
        _buffer->touchpad_point.x = joystickData.fTouchpadX;
        _buffer->touchpad_point.y = joystickData.fTouchpadY;
        [self processTouchPadPos];
        bLastSendTouchPoint = true;
    }
    else if(bLastSendTouchPoint)
    {
        _buffer->touchpad_point.x = 0;
        _buffer->touchpad_point.y = 0;
        [self processTouchPadPos];
        bLastSendTouchPoint = false;
    }

    
    //Sensor
    _buffer->sensor_data.fOrientationX = -joystickData.fOrientationX;
    _buffer->sensor_data.fOrientationY = -joystickData.fOrientationY;
    _buffer->sensor_data.fOrientationZ = joystickData.fOrientationZ;
    _buffer->sensor_data.fOrientationW = joystickData.fOrientationW;
    _buffer->sensor_data.fAccelX = joystickData.fAccelX;
    _buffer->sensor_data.fAccelY = joystickData.fAccelY;
    _buffer->sensor_data.fAccelZ = joystickData.fAccelZ;
    _buffer->sensor_data.fGyroX = joystickData.fGyroX;
    _buffer->sensor_data.fGyroY = joystickData.fGyroY;
    _buffer->sensor_data.fGyroZ = joystickData.fGyroZ;
    [self processSensorData];
    
    if (_valueChangedHandler)
    {
        _valueChangedHandler(self, nil);
    }
    
    return YES;
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
    uint32_t currentKeyStatus = MJGamepadButtonUnknown;
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

/*
 计算公式：
 
 d = 10^((abs(RSSI) - A) / (10 * n))
 
 其中：
 
 d - 计算所得距离(单位米)
 
 RSSI - 接收信号强度（负值）
 
 A - 发射端和接收端相隔1米时的信号强度
 
 n - 环境衰减因子
*/

- (float)calcDistByRSSI:(int)rssi
{
    int A = 59;
    float n = 2.0;
    
    int iRssi = abs(rssi);
    float power = (iRssi-A)/(10*n);
    return pow(10, power);
}


#pragma mark - CBCentralManager Delegate
- (void)startScanning:(CBCentralManager *)central
{
    NSLog(@"startScanning retrieveConnectedPeripheralsWithServices...");
    m_bConnecting = false;
    
    //serivces uuid
    NSMutableArray * services = [NSMutableArray array];
    [services addObject:[CBUUID UUIDWithString:@"622f5b2a-60da-4484-929a-69e3a9140258"]];
    [services addObject:[CBUUID UUIDWithString:@"FFF0"]];
    [services addObject:[CBUUID UUIDWithString:@"FFF4"]];
    
    NSMutableArray* retrievePeripherals = [NSMutableArray array];
    retrievePeripherals = [central retrieveConnectedPeripheralsWithServices:services];
    
    for (CBPeripheral* peripheral in retrievePeripherals) {
        if ([peripheral.name isEqualToString:@"Mojing"]
            || [peripheral.name isEqualToString:@"Mojing4"]
            || [peripheral.name isEqualToString:@"mojing-motion"])
        {
            NSLog(@"Get retrieveperipheral name : %@ , state == %ld ", peripheral.name, (long)peripheral.state);
            //peripheral.services.
            //[central cancelPeripheralConnection:peripheral];
            peripheral.delegate = self;
            self.peripheral = peripheral;//[peripheral retain];
            //[central connectPeripheral:peripheral options:nil];
            m_bConnecting = true;
            //NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES],CBConnectPeripheralOptionNotifyOnDisconnectionKey, nil];
            [central connectPeripheral:peripheral options:nil];
            m_nReconnectCount = 0;
            return;
        }
    }
    
    NSLog(@"startScanning scanForPeripheralsWithServices...");
    m_LastStartScanTime = time(NULL);
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],CBCentralManagerScanOptionAllowDuplicatesKey, nil];
    [_centralManager scanForPeripheralsWithServices:nil options:options];
    //[central scanForPeripheralsWithServices:nil options:nil];
}


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
            [self startScanning:central];
            break;
        default:
            break;
    }
    NSLog(@"centralManagerDidUpdateState: %@", state);
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
    if(m_bConnecting)
    {
        return;
    }
    
    //间隔5s重新一次扫描
    if(time(NULL) - m_LastStartScanTime > 5)
    {
        NSLog(@"Start a new scanning...");
        [central stopScan];
        [self startScanning:central];
        return;
    }
    
    if(peripheral == nil )
    {
        return;
    }
    
    self.peripheral = peripheral;
    self.peripheral.delegate = self;
 
    //NSLog(@"didDiscoverPeripheral: %@", peripheral);
    //6字节旧版本（TI版本）协议名称废弃
    if ([peripheral.name isEqualToString:@"Mojing"]
        || [peripheral.name isEqualToString:@"3DBB"]
        || [peripheral.name isEqualToString:@"Mojing4"]
        || [peripheral.name isEqualToString:@"mojing-motion"])
    {
        
        NSLog(@"Discover peripheral name : %@ , state == %ld", self.peripheral.name , (long)self.peripheral.state);
        
        if (self.peripheral.state == CBPeripheralStateDisconnected)
        {
            float distance = [self calcDistByRSSI:RSSI.intValue];
            if (distance < 2)
            {
                m_bConnecting = true;
                [central connectPeripheral:self.peripheral options:nil];
                m_nReconnectCount = 0;
                m_joystickDistance = 0;
            }
            
        }
    }
    
    [self updateStatus];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSLog(@"didConnectPeripheral peripheral name : %@", peripheral.name);

    [_btnPadConnect setTimestamp:_buffer->timestamp];
    [_btnPadConnect setPressed:YES];
  
    self.peripheral = peripheral;
    [self updateStatus];

    //if([central isScanning])
    {
        [central stopScan];
    }
    [peripheral discoverServices:nil];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"didDisconnectPeripheral peripheral name : %@ , error code = %ld", peripheral.name, (long)error.code);
    
    if((NSInteger)[central state] == CBCentralManagerStatePoweredOn)
    {
        if([peripheral.name isEqualToString:@"mojing-motion"] && (error == nil || error.code == 0 ) )
        {
            if(m_nReconnectCount < 5)  //最多重连5次
            {
                NSLog(@"Reconnecting to peripheral name : %@ ", peripheral.name);
                peripheral.delegate = self;
                self.peripheral = peripheral;//[peripheral retain];
                m_bConnecting = true;
                //NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:YES],CBConnectPeripheralOptionNotifyOnConnectionKey, nil];
                [central connectPeripheral:peripheral options:nil];
                m_nReconnectCount++;
                return;
            }
        }
    }
    
    [_deviceInfoServerArray removeAllObjects];
    [_deviceInfoCharValueDictionary removeAllObjects];
    
    self.peripheral = peripheral;
    [self updateStatus];
    
    [_btnPadConnect setTimestamp:_buffer->timestamp];
    [_btnPadConnect setPressed:NO];

    if (_isAutoScan)
    {
        [self startScanning:central];
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
    if ([service.UUID isEqual:DEVICE_INFO_SERVICE_UUID] || [service.UUID isEqual:DEVICE_BETTERY_SERVICE_UUID])
    {
        [_deviceInfoServerArray addObjectsFromArray:service.characteristics];
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
        
        //魔镜手柄，只订阅FFF4(由于硬件原因订阅其他值可能会出问题)
        /*
         旧版：FFF0, 新版: FFF4
         */
        if ([uuidString isEqualToString:@"FFF4"] || [uuidString isEqualToString:@"FFF0"])
        {
            [peripheral setNotifyValue:YES forCharacteristic:characteristic];
        }
        
        //mojing-motion
        if ([uuidString isEqualToString:@"FFB4A651-C224-420B-A134-13D3ABA313FB"])
        {
            [peripheral setNotifyValue:YES forCharacteristic:characteristic];
            for (CBDescriptor *d in characteristic.descriptors) {
                NSString *uuidStr = d.UUID.UUIDString;
                //NSLog(@"CBDescriptor name is :%@", uuidStr);
                if([uuidString isEqualToString:@"2902"])
                {
                    NSData *data = [@"10" dataUsingEncoding:NSUTF8StringEncoding];
                    [peripheral writeValue:data forDescriptor:d];
                }
            }
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
    else if([uuidString isEqualToString:@"FFB4A651-C224-420B-A134-13D3ABA313FB"])
    {
        if (valueLength == 20)
        {
            _buffer->raw_char_size = 20;
            [self onReceiveMJMotionKeyCode:raw_data peripheralName:peripheral.name];
        }
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
        //NSLog(@"--->updateStatus: %ld", (long)self.peripheralState);
    }
    
    self.centralManagerState = (CBCentralManagerState)_centralManager.state;
}

- (void)scan
{
    if (_centralManager != nil && [_centralManager isKindOfClass:[CBCentralManager class]])
    {
        [self startScanning:_centralManager];
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
        for(CBService *service in _peripheral.services)
        {
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
           
                //mojing-motion
                if ([uuidString isEqualToString:@"FFB4A651-C224-420B-A134-13D3ABA313FB"])
                {
                    [_peripheral setNotifyValue:NO forCharacteristic:characteristic];
                }
            }
        }
        
        m_nReconnectCount = 5; //主动断开的不重连
        [_centralManager cancelPeripheralConnection:_peripheral];
    }
}

- (NSDictionary *)getDeviceInfoCharValueDictionary
{
    for (CBCharacteristic *characteristic in _deviceInfoServerArray)
    {
        NSData *valueData = characteristic.value;
        
        if ([characteristic.UUID isEqual:DEVICE_MANUFACTURER_NAME_CHARACTERISTIC_UUID])
        {
            NSString *manufactureName = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (manufactureName != nil && manufactureName.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:manufactureName forKey:MANUFACTURER_NAME];
            }
        }
        /* Read the model number that is assigned by the device vendor from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_MODEL_NUMBER_CHARACTERISTIC_UUID])
        {
            NSString *modelNumberString = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (modelNumberString != nil && modelNumberString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:modelNumberString forKey:MODEL_NUMBER];
            }
        }
        /* Read the serial number for a particular instance of the device from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_SERIAL_NUMBER_CHARACTERISTIC_UUID])
        {
            NSString *serialNumberString = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (serialNumberString != nil && serialNumberString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:serialNumberString forKey:SERIAL_NUMBER];
            }
        }
        /* Read the hardware revision for the hardware within the device from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_HARDWARE_REVISION_CHARACTERISTIC_UUID])
        {
            NSString *hardwareRevisionString = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (hardwareRevisionString != nil && hardwareRevisionString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:hardwareRevisionString forKey:HARDWARE_REVISION];
            }
        }
        else if ([characteristic.UUID isEqual:DEVICE_FIRMWARE_REVISION_CHARACTERISTIC_UUID])
        {
            NSString *firmwareRevisionString = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (firmwareRevisionString != nil && firmwareRevisionString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:firmwareRevisionString forKey:FIRMWARE_REVISION];
            }
        }
        /* Read the software revision for the software within the device from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_SOFTWARE_REVISION_CHARACTERISTIC_UUID])
        {
            
            NSString *softwareRevisionString = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (softwareRevisionString != nil && softwareRevisionString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:softwareRevisionString forKey:SOFTWARE_REVISION];
            }
        }
        /* Read a structure containing an Organizationally Unique Identifier (OUI) followed by a manufacturer-defined identifier and is unique for each individual instance of the product from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_SYSTEMID_CHARACTERISTIC_UUID])
        {
            
            //            NSString *systemID = [NSString stringWithFormat:@"%@",characteristic.value];
            NSString *systemID = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            if (systemID != nil && systemID.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:systemID forKey:SYSTEM_ID];
            }
            
        }
        /* Read the regulatory and certification information for the product in a list defined in IEEE 11073-20601 from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_CERTIFICATION_DATALIST_CHARACTERISTIC_UUID])
        {
            
            //            NSString *certificationDataList = [NSString stringWithFormat:@"%@",valueData];
            NSString *certificationDataList = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (certificationDataList != nil && certificationDataList.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:certificationDataList forKey:REGULATORY_CERTIFICATION_DATA_LIST];
            }
        }
        /* Read a set of values used to create a device ID value that is unique for this device from characteristic
         */
        else if ([characteristic.UUID isEqual:DEVICE_PNPID_CHARACTERISTIC_UUID])
        {
            
            //            NSString *pnpID = [NSString stringWithFormat:@"%@",characteristic.value];
            NSString *pnpID = [[NSString alloc] initWithData:valueData encoding:NSUTF8StringEncoding];
            
            if (pnpID != nil && pnpID.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:pnpID forKey:PNP_ID];
                
            }
        }
        else if ([characteristic.UUID isEqual:DEVICE_BETTERY_CHARACTERISTIC_UUID])
        {
            
            //            NSString *pnpID = [NSString stringWithFormat:@"%@",characteristic.value];
            int betterValue;
            [valueData getBytes: &betterValue length: sizeof(betterValue)];
            
            NSString *betteryString = [NSString stringWithFormat:@"%d",betterValue];
            if (betteryString != nil && betteryString.length > 0)
            {
                [_deviceInfoCharValueDictionary setObject:betteryString forKey:BETTERY_VALUE];
            }
        }
    }
    
    return [NSDictionary dictionaryWithDictionary:_deviceInfoCharValueDictionary];
}
- (NSString *)representativeString:(NSData *)data;
{
    //NSData *data = UUID.data;
    
    NSUInteger bytesToConvert = [data length];
    const unsigned char *uuidBytes = (unsigned char *)[data bytes];
    NSMutableString *outputString = [NSMutableString stringWithCapacity:16];
    
    for (NSUInteger currentByteIndex = 0; currentByteIndex < bytesToConvert; currentByteIndex++)
    {
        switch (currentByteIndex)
        {
            case 3:
            case 5:
            case 7:
            case 9:[outputString appendFormat:@"%02x-", uuidBytes[currentByteIndex]]; break;
            default:[outputString appendFormat:@"%02x", uuidBytes[currentByteIndex]];
        }
        
    }
    
    return outputString;
}
@end
