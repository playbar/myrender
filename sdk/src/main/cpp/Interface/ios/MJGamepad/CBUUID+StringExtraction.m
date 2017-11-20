//
//  CBUUID+StringExtraction.m
//  MJGamepad
//
//  Created by 秦宏 on 15/5/29.
//  Copyright (c) 2015年 秦宏. All rights reserved.
//

#import "CBUUID+StringExtraction.h"

@implementation CBUUID (StringExtraction)

- (NSString *)representativeString
{
    NSData *data = [self data];
    
    NSUInteger bytesToConvert = [data length];
    const unsigned char *uuidBytes = [data bytes];
    NSMutableString *outputString = [NSMutableString stringWithCapacity:16];
    
    for (NSUInteger currentByteIndex = 0; currentByteIndex < bytesToConvert; currentByteIndex++)
    {
        switch (currentByteIndex)
        {
            case 3:
            case 5:
            case 7:
            case 9:[outputString appendFormat:@"%02X-", uuidBytes[currentByteIndex]]; break;
            default:[outputString appendFormat:@"%02X", uuidBytes[currentByteIndex]];
        }
        
    }
    
    return outputString;
}

@end
