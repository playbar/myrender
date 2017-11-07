//
//  MJiCadeTransfor.h
//  MojingSDK
//
//  Created by 王先生 on 16/8/24.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MojingKeyCode.h"
#import "iCadeReaderView.h"

@interface MJiCadeTransfor : NSObject
{
    iCadeReaderView *control;
}

@property (copy) MJiCadeGamepadValueChangedHandler valueChangedHandler;

@property (nonatomic, strong) NSDictionary *keysDictionary;

+ (instancetype)shareIcadeTransfor;

- (void)registerKey :(UIViewController* )viewCon;

- (BOOL)dicFromString:(NSString *)keysString;


@end
