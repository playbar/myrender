//
//  MJAudioSession.m
//  MJMRemoteTest
//
//  Created by HUO on 16/8/5.
//  Copyright © 2016年 baofeng. All rights reserved.
//

#import "MJAudioSession.h"
#import "MGMICCKeyObject.h"
#import "MGConstant.h"

@interface MJAudioSession ()
{
    MGMICCKeyObject *_mMGMICCKeyObject;
    BOOL _isSesstionActive;
}

@end

@implementation MJAudioSession

+ (instancetype)sharedManager {
    static MJAudioSession *shareManager = nil;
    static dispatch_once_t predicate;
    dispatch_once(&predicate, ^{
        shareManager = [[MJAudioSession alloc] init];
    });
    return shareManager;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        [self initMJAudioSession];
        
    }
    return self;
}

- (void)initMJAudioSession {
    if (_isSesstionActive) return;
    AudioSessionInitialize(NULL, NULL, NULL, NULL);
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    NSError *categoryError;
    if (![audioSession setCategory:AVAudioSessionCategoryRecord error:&categoryError]) {
        //NSLog(@"Session category failed!, %@", categoryError);
    }
    NSError *activeError;
    if ([audioSession setActive: YES error:&activeError]) {
        _isSesstionActive = YES;
    } else {
        //NSLog(@"Session active failed!, %@", activeError);
    }
    [self addAudioRouteChangeListener];
}

- (void)addAudioRouteChangeListener {
    OSStatus status = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange,
                                                      audioSessionRouteChangeListenerCallback,
                                                      (__bridge void *)(self));
    // status
    if (status != noErr) {
        NSLog(@"Listerner faild!, %d", status);
    }
}

- (void)updateAudioSessionProperty:(MJAudioSession *)a {
    AudioSessionInitialize(NULL, NULL, NULL, NULL);
    UInt32 propertySize = sizeof(CFStringRef);
    CFStringRef type;
    AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &propertySize, &type);
    
    if ([(__bridge NSString *)type isEqualToString:(NSString *)kAudioSessionInputRoute_HeadsetMic]) {
        if (!_mMGMICCKeyObject)
            _mMGMICCKeyObject = [[MGMICCKeyObject alloc] init];

        [_mMGMICCKeyObject start];
        
        _mMGMICCKeyObject.mMGOutputCallBack = ^void(NSString *keyValue, NSString *keyPress) {
            BOOL pressed = [keyPress isEqualToString:@MGKEY_DOWN] ? YES : NO;
            if (!_audioSessionHandler) return;
            if([keyValue isEqualToString:@MGKEY_TOP]) {
                _audioSessionHandler(KEY_UP, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_RIGHT]) {
                _audioSessionHandler(KEY_RIGHT, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_BOTTOM]) {
                _audioSessionHandler(KEY_DOWN, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_LEFT]) {
                _audioSessionHandler(KEY_LEFT, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_MENU]) {
                _audioSessionHandler(KEY_MENU, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_OK]) {
                _audioSessionHandler(KEY_OK, pressed);
            }
            if([keyValue isEqualToString:@MGKEY_BACK]) {
                _audioSessionHandler(KEY_BACK, pressed);
            }
        };
    } else {
        if (_mMGMICCKeyObject)
            [_mMGMICCKeyObject pause];
    }
}

static void audioSessionRouteChangeListenerCallback (void                      *inUserData,
                                                     AudioSessionPropertyID    inPropertyID,
                                                     UInt32                    inPropertyValueS,
                                                     const void                *inPropertyValue) {
    MJAudioSession *engine = (__bridge MJAudioSession *)inUserData;
    [engine updateAudioSessionProperty:engine];
}

@end
