//
//  MJiCadeTransfor.m
//  MojingSDK
//
//  Created by 王先生 on 16/8/24.
//  Copyright © 2016年 baofeng. All rights reserved.
//

/*
 UP ON,OFF  = w,e
 RT ON,OFF  = d,c
 DN ON,OFF  = x,z
 LT ON,OFF  = a,q
 A  ON,OFF  = y,t
 B  ON,OFF  = h,r
 C  ON,OFF  = u,f
 D  ON,OFF  = j,n
 E  ON,OFF  = i,m
 F  ON,OFF  = k,p
 G  ON,OFF  = o,g
 H  ON,OFF  = l,v
 */

#define  ORAGINALKEYS @"wexzaqdcufhrjnytoglvimogogkplvlvwexzaqdcytufjnhr"

#import "MJiCadeTransfor.h"
@interface MJiCadeTransfor ()

@property (nonatomic, strong) NSArray *keyCodeArray;

@end


static MJiCadeTransfor *shareInstance = nil;

@implementation MJiCadeTransfor

+ (instancetype)shareIcadeTransfor
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        shareInstance = [[MJiCadeTransfor alloc]init];
    });
    return shareInstance;
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        control = [[iCadeReaderView alloc] initWithFrame:CGRectZero];
        _keysDictionary = [[NSDictionary alloc] init];
        _keyCodeArray = @[
                          @{@"keycode":[NSNumber numberWithInteger:KEY_UP],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_UP],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_DOWN],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_DOWN],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_LEFT],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_LEFT],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_RIGHT],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:KEY_RIGHT],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_A],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_A],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_B],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_B],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_X],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_X],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_Y],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_Y],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_SELECT],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_SELECT],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_START],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_START],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_L1],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_L1],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_L2],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_L2],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_THUMBL],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_THUMBL],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_R1],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_R1],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_R2],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_R2],@"press":[NSNumber numberWithBool:NO]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_THUMBR],@"press":[NSNumber numberWithBool:YES]},
                          @{@"keycode":[NSNumber numberWithInteger:BUTTON_THUMBR],@"press":[NSNumber numberWithBool:NO]},
                          
                          ];
        
        [self dicFromString:ORAGINALKEYS];
    }
    return self;
}

- (BOOL)dicFromString:(NSString *)keysString
{
    NSMutableDictionary *tempdic = [NSMutableDictionary dictionary];
    int mulCouct = 0;
    for (int i = 0; i < keysString.length; i++)
    {
        NSRange keyRange = NSMakeRange(i, 1);
        NSString *key = [keysString substringWithRange:keyRange];
        NSString *preString = [keysString substringToIndex:i];
        if ([preString containsString:key] || [key isEqualToString:@"-"])
        {
            mulCouct += 1;
            continue;
        }
        if (i > [_keyCodeArray count])
        {
            break;
        }
        [tempdic setObject:[_keyCodeArray objectAtIndex:i] forKey:key];
    }
    if ([[tempdic allKeys] count] > 0 && ([[tempdic allKeys] count] % 2) == 0)
    {
        _keysDictionary = [NSDictionary dictionaryWithDictionary:tempdic];
        NSLog(@"keysDic ======== %@",_keysDictionary);
        return YES;
    }
    return NO;
}

- (NSDictionary *)keysDictionary
{
    return  _keysDictionary;
}

- (void)registerKey :(UIViewController* )viewCon
{
    [self clearControllView];
    if (control != nil)
    {
        [viewCon.view addSubview:control];
        control.active = YES;
        //        control.delegate = self;
        __block MJiCadeTransfor *blockMJiCadeTransfor = [MJiCadeTransfor shareIcadeTransfor];
        control.valueChangedHandler = ^void(KEY_GAMEPAD keyID, BOOL pressed){
            if(blockMJiCadeTransfor.valueChangedHandler)
            {
                blockMJiCadeTransfor.valueChangedHandler(keyID,pressed);
            }
        };
    }
}

- (void)clearControllView
{
    control.active = NO;
    [control removeFromSuperview];
}
@end
