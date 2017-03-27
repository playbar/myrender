//
//  GameController.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/9/15.
//  Copyright (c) 2015年 baofeng. All rights reserved.

#import <GameController/GameController.h>
#import "MJGameController.h"

@interface MJGameController()
{
    GCController *gameController;
    MFIButtonValueChangedHandler _buttonValueChangedHandler;
    MFIAxisValueChangedHandler _axisValueChangedHandler;
}
@end

@implementation MJGameController
@synthesize buttonValueChangedHandler = _buttonValueChangedHandler;
@synthesize axisValueChangedHandler = _axisValueChangedHandler;

+ (instancetype)sharedMFIGamepad{
    static MJGameController *share=nil;
    
    if (!share) {
        share=[[MJGameController alloc] init];
    }
    return share;
}

/*
- (void)dealloc
{
    [_buttonValueChangedHandler release];
    [_axisValueChangedHandler release];
    
    [super dealloc];
}
*/
/*
- (instancetype)init
{
    self = [super init];
    if (self)
    {
        control = [[MJGameController alloc] init];
    }
    return self;   
}
*/

- (void)registerKey
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidConnect:) name:GCControllerDidConnectNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidDisconnect:) name:GCControllerDidDisconnectNotification object:nil];
    
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^{

        NSLog(@"Game controller start WirelessController Discovery.");
    // we don't use any code here since when new controllers are found we will get notifications
        
    }];
    
    [self configureConnectedGameControllers];
}

- (void)gameControllerDidConnect:(NSNotification *)notification {
    NSLog(@"Game controller connected. Do you want to use it?");
    [self configureConnectedGameControllers];
}

- (void)gameControllerDidDisconnect:(NSNotification *)notification {
    NSLog(@"Game controller has disconnected.");
    gameController = nil;
    if(_buttonValueChangedHandler)
    {
        _buttonValueChangedHandler(AXIS_NONE, KEY_CONNECT, NO);
    }
}

- (void)configureConnectedGameControllers {
    if (gameController == nil)
    {
        if ([[GCController controllers] count] == 0)
        {
             return;
        }

        // no controller has index 0 so connect the first one and make it player 1
        gameController = [[GCController controllers] objectAtIndex:0];
        gameController.playerIndex = 0;
        
    	[self setupController:gameController];
        
        if(_buttonValueChangedHandler)
        {
            _buttonValueChangedHandler(AXIS_NONE, KEY_CONNECT, YES);
        }
    }
}

- (void)setupController:(GCController *)controller
{
    if(controller == NULL)
    {
        return;
    }
    
    /** 对Pause键位进行注册
     **/
    controller.controllerPausedHandler = ^(GCController *controller){
        if (_buttonValueChangedHandler)
        {
            _buttonValueChangedHandler(AXIS_NONE, BUTTON_PAUSE, YES);
        }
    };
    
    if (controller.gamepad && controller.extendedGamepad == NULL)
    {
        if(controller.gamepad.dpad)
        {
            controller.gamepad.dpad.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue) {
                if (_axisValueChangedHandler)
                {
                    _axisValueChangedHandler(AXIS_DPAD, xValue, yValue);
                }
            };
            
            controller.gamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_UP, pressed);
                }
            };
            controller.gamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_DOWN, pressed);
                }
            };
            controller.gamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_LEFT, pressed);
                }
            };
            controller.gamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_RIGHT, pressed);
                }
            };
        }
        
        controller.gamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_A, pressed);
            }
        };
        controller.gamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_B, pressed);
            }
        };
        controller.gamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_X, pressed);
            }
        };
        controller.gamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_Y, pressed);
            }
        };
        
        controller.gamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_L1, pressed);
            }
        };
        controller.gamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_R1, pressed);
            }
        };
    }
    else if (controller.extendedGamepad)
    {
        if(controller.extendedGamepad.dpad)
        {
            controller.extendedGamepad.dpad.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue) {
                if (_axisValueChangedHandler)
                {
                    _axisValueChangedHandler(AXIS_DPAD, xValue, yValue);
                }
            };
            
            controller.extendedGamepad.dpad.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_UP, pressed);
                }
            };
            controller.extendedGamepad.dpad.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_DOWN, pressed);
                }
            };
            controller.extendedGamepad.dpad.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_LEFT, pressed);
                }
            };
            controller.extendedGamepad.dpad.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_RIGHT, pressed);
                }
            };
        }
        
        if(controller.extendedGamepad.leftThumbstick)
        {
            controller.extendedGamepad.leftThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue) {
                if (_axisValueChangedHandler)
                {
                    _axisValueChangedHandler(AXIS_LEFT, xValue, yValue);
                }
            };
            
            controller.extendedGamepad.leftThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_UP, pressed);
                }
            };
            controller.extendedGamepad.leftThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_DOWN, pressed);
                }
            };
            controller.extendedGamepad.leftThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_LEFT, pressed);
                }
            };
            controller.extendedGamepad.leftThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_RIGHT, pressed);
                }
            };
        }
        
        if(controller.extendedGamepad.rightThumbstick)
        {
            controller.extendedGamepad.rightThumbstick.valueChangedHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue) {
                if (_axisValueChangedHandler)
                {
                    _axisValueChangedHandler(AXIS_RIGHT, xValue, yValue);
                }
            };
            
            controller.extendedGamepad.rightThumbstick.up.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_UP, pressed);
                }
            };
            controller.extendedGamepad.rightThumbstick.down.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_DOWN, pressed);
                }
            };
            controller.extendedGamepad.rightThumbstick.left.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_LEFT, pressed);
                }
            };
            controller.extendedGamepad.rightThumbstick.right.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
                if(_buttonValueChangedHandler)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_RIGHT, pressed);
                }
            };
        }
        
        controller.extendedGamepad.buttonA.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_A, pressed);
            }
        };
        controller.extendedGamepad.buttonB.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_B, pressed);
            }
        };
        controller.extendedGamepad.buttonX.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_X, pressed);
            }
        };
        controller.extendedGamepad.buttonY.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_Y, pressed);
            }
        };
    
        controller.extendedGamepad.leftShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_L1, pressed);
            }
        };
        controller.extendedGamepad.rightShoulder.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_R1, pressed);
            }
        };
        controller.extendedGamepad.leftTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_L2, pressed);
            }
        };
        controller.extendedGamepad.rightTrigger.valueChangedHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed) {
            if (_buttonValueChangedHandler)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_R2, pressed);
            }
        };
    }
    
    
#if 0
    /** 对X,Y,A,B键位进行注册
     **/
    GCControllerButtonValueChangedHandler XYABButtonHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
    {
     	NSLog(@"Button : %@, Value = %f", pressed? @"按下":@"松开", value);
        
        if(_buttonValueChangedHandler == nil) return;
        
        if (controller.gamepad && controller.extendedGamepad != NULL) {
            if(button == controller.gamepad.buttonA )
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_A, pressed);
            }
            if(button == controller.gamepad.buttonB )
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_B, pressed);
            }
            if(button == controller.gamepad.buttonX )
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_X, pressed);
            }
            if(button == controller.gamepad.buttonY )
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_Y, pressed);
            }
        }
        else if (controller.extendedGamepad) {
            if(button == controller.extendedGamepad.buttonA)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_A, pressed);
            }
            if(button == controller.extendedGamepad.buttonB)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_B, pressed);
            }
            if(button == controller.extendedGamepad.buttonX)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_X, pressed);
            }
            if(button == controller.extendedGamepad.buttonY)
            {
                _buttonValueChangedHandler(AXIS_NONE, BUTTON_Y, pressed);
            }
        }
    };
    if (controller.gamepad){
        controller.gamepad.buttonA.valueChangedHandler = XYABButtonHandler;
        controller.gamepad.buttonB.valueChangedHandler = XYABButtonHandler;
        controller.gamepad.buttonX.valueChangedHandler = XYABButtonHandler;
        controller.gamepad.buttonY.valueChangedHandler = XYABButtonHandler;
    }
    if (controller.extendedGamepad) {
        controller.extendedGamepad.buttonA.valueChangedHandler = XYABButtonHandler;
        controller.extendedGamepad.buttonB.valueChangedHandler = XYABButtonHandler;
        controller.extendedGamepad.buttonX.valueChangedHandler = XYABButtonHandler;
        controller.extendedGamepad.buttonY.valueChangedHandler = XYABButtonHandler;
    }
    
    /** 对肩部的键位进行注册
     **/
    GCControllerButtonValueChangedHandler LRShoulderHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
    {
     	NSLog(@"Button : %@, Value = %f", pressed? @"按下":@"松开", value);
        
        if(_buttonValueChangedHandler == nil) return;
        
        if (controller.gamepad && controller.extendedGamepad != NULL) {
        	if(button == controller.gamepad.leftShoulder )
        	{
            	_buttonValueChangedHandler(AXIS_NONE, BUTTON_L1, pressed); //BUTTON_SELCT
        	}
        	if(button == controller.gamepad.rightShoulder )
        	{
            	_buttonValueChangedHandler(AXIS_NONE, BUTTON_R1, pressed); //BUTTON_START
        	}
        }
        else if (controller.extendedGamepad) {
        	if(button == controller.extendedGamepad.leftShoulder )
        	{
            	_buttonValueChangedHandler(AXIS_NONE, BUTTON_L2, pressed);
        	}
        	if(button == controller.extendedGamepad.rightShoulder )
        	{
            	_buttonValueChangedHandler(AXIS_NONE, BUTTON_R2, pressed);
        	}
        }
    };
    if (controller.gamepad) {
        controller.gamepad.leftShoulder.valueChangedHandler = LRShoulderHandler;
        controller.gamepad.rightShoulder.valueChangedHandler = LRShoulderHandler;
    }
    if (controller.extendedGamepad) {
        controller.extendedGamepad.leftShoulder.valueChangedHandler = LRShoulderHandler;
        controller.extendedGamepad.rightShoulder.valueChangedHandler = LRShoulderHandler;
    }
    
    /** 对方向键位进行注册
     **/
    GCControllerButtonValueChangedHandler ArrowButtonHandler = ^(GCControllerButtonInput *button, float value, BOOL pressed)
    {
     	NSLog(@"Button : %@, Value = %f", pressed? @"按下":@"松开", value);
        
        if(_buttonValueChangedHandler == nil) return;
        
        if (controller.gamepad && controller.extendedGamepad != NULL) {
            if (controller.gamepad.dpad) {
                if(button == controller.gamepad.dpad.up)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_UP, pressed);
                }
                if(button == controller.gamepad.dpad.down)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_DOWN, pressed);
                }
                if(button == controller.gamepad.dpad.left)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_LEFT, pressed);
                }
                if(button == controller.gamepad.dpad.right)
                {
                    _buttonValueChangedHandler(AXIS_DPAD, KEY_RIGHT, pressed);
                }
            }
        }
        else if (controller.extendedGamepad) {
            if (controller.extendedGamepad.leftThumbstick) {
                if(button == controller.extendedGamepad.leftThumbstick.up)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_UP, pressed);
                }
                if(button == controller.extendedGamepad.leftThumbstick.down)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_DOWN, pressed);
                }
                if(button == controller.extendedGamepad.leftThumbstick.left)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_LEFT, pressed);
                }
                if(button == controller.extendedGamepad.leftThumbstick.right)
                {
                    _buttonValueChangedHandler(AXIS_LEFT, KEY_RIGHT, pressed);
                }
            }
            if (controller.extendedGamepad.rightThumbstick) {
                if(button == controller.extendedGamepad.rightThumbstick.up)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_UP, pressed);
                }
                if(button == controller.extendedGamepad.rightThumbstick.down)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_DOWN, pressed);
                }
                if(button == controller.extendedGamepad.rightThumbstick.left)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_LEFT, pressed);
                }
                if(button == controller.extendedGamepad.rightThumbstick.right)
                {
                    _buttonValueChangedHandler(AXIS_RIGHT, KEY_RIGHT, pressed);
                }
            }
        }
    };
    if (controller.gamepad){
        if (controller.gamepad.dpad) {
            controller.gamepad.dpad.up.valueChangedHandler = ArrowButtonHandler;
            controller.gamepad.dpad.down.valueChangedHandler = ArrowButtonHandler;
            controller.gamepad.dpad.left.valueChangedHandler = ArrowButtonHandler;
            controller.gamepad.dpad.right.valueChangedHandler = ArrowButtonHandler;
        }
    }
    if (controller.extendedGamepad) {
        if (controller.extendedGamepad.leftThumbstick) {
            controller.extendedGamepad.leftThumbstick.up.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.leftThumbstick.down.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.leftThumbstick.left.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.leftThumbstick.right.valueChangedHandler = ArrowButtonHandler;
        }
        if (controller.extendedGamepad.rightThumbstick) {
            controller.extendedGamepad.rightThumbstick.up.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.rightThumbstick.down.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.rightThumbstick.left.valueChangedHandler = ArrowButtonHandler;
            controller.extendedGamepad.rightThumbstick.right.valueChangedHandler = ArrowButtonHandler;
        }
    }
    
    /**对轴进行注册
     **/
    GCControllerDirectionPadValueChangedHandler LRThumbHandler = ^(GCControllerDirectionPad *dpad, float xValue, float yValue)
    {
        NSLog(@"摇杆坐标: (%f, %f)", xValue, yValue);
        
        if(_axisValueChangedHandler == nil) return;
        
        if (controller.extendedGamepad) {
            if(dpad == controller.extendedGamepad.leftThumbstick)
            {
                _axisValueChangedHandler(AXIS_LEFT, xValue, yValue);
            }
            if(dpad == controller.extendedGamepad.rightThumbstick)
            {
                _axisValueChangedHandler(AXIS_RIGHT, xValue, yValue);
            }
        }
        if (controller.gamepad.dpad) {
            if(dpad == controller.gamepad.dpad)
            {
                _axisValueChangedHandler(AXIS_DPAD, xValue, yValue);
            }
        }
        
    };
    if (controller.extendedGamepad) {
        controller.extendedGamepad.leftThumbstick.valueChangedHandler = LRThumbHandler;
        controller.extendedGamepad.rightThumbstick.valueChangedHandler = LRThumbHandler;
    }
    if (controller.gamepad.dpad) {
        controller.gamepad.dpad.valueChangedHandler = LRThumbHandler;
    }
#endif
}

@end

