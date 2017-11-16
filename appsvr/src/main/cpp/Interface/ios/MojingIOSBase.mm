//
//  MojingIOSBase.m
//  MojingSDK
//
//  Created by 汤申亮 on 15/7/21.
//  Copyright (c) 2015年 baofeng. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import "SFHFKeychainUtils.h"
//#import <AdSupport/AdSupport.h>

#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <sys/types.h>
#include <mach/machine.h>

#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
#include "../../Render/MojingRenderBase.h"
#include "MojingOpenUDID.h"
#include "MojingIOSBase.h"

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

NSArray *modelArray = @[
                        @"i386",
                        @"x86_64",
                        @"iPhone1,1",
                        @"iPhone1,2",
                        @"iPhone2,1",
                        @"iPhone3,1",
                        @"iPhone3,2",
                        @"iPhone3,3",
                        @"iPhone4,1",
                        @"iPhone5,1",
                        @"iPhone5,2",
                        @"iPhone5,3",
                        @"iPhone5,4",
                        @"iPhone6,1",
                        @"iPhone6,2",
                        @"iPhone7,1",
                        @"iPhone7,2",
                        @"iPhone8,1",
                        @"iPhone8,2",
                        @"iPhone8,3",
                        @"iPhone8,4",
                        @"iPhone9,1",
                        @"iPhone9,2",
                        @"iPhone9,3",
                        @"iPhone9,4",
                        
                        @"iPod1,1",
                        @"iPod2,1",
                        @"iPod3,1",
                        @"iPod4,1",
                        @"iPod5,1",
                        
                        @"iPad1,1",
                        
                        @"iPad2,1",
                        @"iPad2,2",
                        @"iPad2,3",
                        @"iPad2,4",
                        @"iPad2,5",
                        @"iPad2,6",
                        @"iPad2,7",
                        
                        @"iPad3,1",
                        @"iPad3,2",
                        @"iPad3,3",
                        @"iPad3,4",
                        @"iPad3,5",
                        @"iPad3,6",
                        /*
                        @"iPad4,1",
                        @"iPad4,2",
                        @"iPad4,3",
                        @"iPad4,4",
                        @"iPad4,5",
                        @"iPad4,6",
                         */
                        ];

NSArray *modelNameArray = @[
                        @"iPhone Simulator",
                        @"iPhone Simulator",
                        @"iPhone",
                        @"iPhone 3G",
                        @"iPhone 3GS",
                        @"iPhone 4(GSM)",
                        @"iPhone 4(GSM Rev A)",
                        @"iPhone 4(CDMA)",
                        @"iPhone 4S",
                        @"iPhone 5(GSM)",
                        @"iPhone 5(GSM+CDMA)",
                        @"iPhone 5c(GSM)",
                        @"iPhone 5c(Global)",
                        @"iPhone 5s(GSM)",
                        @"iPhone 5s(Global)",
                        @"iPhone 6 Plus",
                        @"iPhone 6",
                        @"iPhone 6s",
                        @"iPhone 6s Plus",
                        @"iPhone SE(GSM+CDMA)",
                        @"iPhone SE(GSM)",
                        @"iPhone 7",
                        @"iPhone 7 Plus",
                        @"iPhone 7",
                        @"iPhone 7 Plus",
                                                                      
                        @"iPod Touch 1G",
                        @"iPod Touch 2G",
                        @"iPod Touch 3G",
                        @"iPod Touch 4G",
                        @"iPod Touch 5G",
                        
                        @"iPad",
                        
                        @"iPad 2(WiFi)",
                        @"iPad 2(GSM)",
                        @"iPad 2(CMDA)",
                        @"iPad 2(WiFi+New Chip)",
                        @"iPad mini(WiFi)",
                        @"iPad mini(GSM)",
                        @"iPad mini(GSM+CDMA)",
                        
                        @"iPad 3(WiFi)",
                        @"iPad 3(GSM+CDMA)",
                        @"iPad 3(GSM)",
                        @"iPad 4(WiFi)",
                        @"iPad 4(GSM)",
                        @"iPad 4(GSM+CDMA)",
                        /*
                        @"iPad Air()",
                        @"iPad Air()",
                        @"iPad Air()",
                        @"iPad mini 2(WiFi)",
                        @"iPad mini 2()",
                        @"iPad mini 2(+TD-LTE)",
                         */
                        ];

NSArray *modelInnerNameArray = @[
                            @"iPhone Simulator",
                            @"iPhone Simulator",
                            @"iPhone (M68AP)",
                            @"iPhone 3G (N82AP)",
                            @"iPhone 3GS (N88AP)",
                            @"iPhone 4 (N90AP)",
                            @"iPhone 4 (N90BAP)",
                            @"iPhone 4 (N92AP)",
                            @"iPhone 4S (N94AP)",
                            @"iPhone 5 (N41AP)",
                            @"iPhone 5 (N42AP)",
                            @"iPhone 5c (N48AP)",
                            @"iPhone 5c (N49AP)",
                            @"iPhone 5s (N51AP)",
                            @"iPhone 5s (N53AP)",
                            @"iPhone 6 Plus (N56AP)",
                            @"iPhone 6 (N61AP)",
                            @"iPhone 6s (N71AP/N71mAP)",
                            @"iPhone 6s Plus (N66AP/N66mAP)",
                            @"iPhone SE (N69AP)",
                            @"iPhone SE (N69uAP)",
                            @"iPhone 7 (D10AP)",
                            @"iPhone 7 Plus (D11AP)", 
                            @"iPhone 7 (D101AP)",
                            @"iPhone 7 Plus (D111AP)",
                            
                            @"iPod Touch 1G (A1213)",
                            @"iPod Touch 2G (A1288)",
                            @"iPod Touch 3G (A1318)",
                            @"iPod Touch 4G (A1367)",
                            @"iPod Touch 5G (A1421/A1509)",
                            
                            @"iPad 1G (A1219/A1337)",
                            
                            @"iPad 2 (A1395)",
                            @"iPad 2 (A1396)",
                            @"iPad 2 (A1397)",
                            @"iPad 2 (A1395+New Chip)",
                            @"iPad mini 1G (A1432)",
                            @"iPad mini 1G (A1454)",
                            @"iPad mini 1G (A1455)",
                            
                            @"iPad 3 (A1416)",
                            @"iPad 3 (A1403)",
                            @"iPad 3 (A1430)",
                            @"iPad 4 (A1458)",
                            @"iPad 4 (A1459)",
                            @"iPad 4 (A1460)",
                            /*
                             @"iPad Air (A1474)",
                             @"iPad Air (A1475)",
                             @"iPad Air (A1476)",
                             @"iPad mini 2G (A1489)",
                             @"iPad mini 2G (A1490)",
                             @"iPad mini 2G (A1491)",
                             */
                            ];

#define SUPPORTS_IOKIT_EXTENSIONS   0
#if SUPPORTS_IOKIT_EXTENSIONS

#include <sys/types.h>

#import <mach/mach_host.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#pragma mark IOKit miniheaders

#define kIODeviceTreePlane        "IODeviceTree"

enum {
    kIORegistryIterateRecursively    = 0x00000001,
    kIORegistryIterateParents        = 0x00000002
};

typedef mach_port_t    io_object_t;
typedef io_object_t    io_registry_entry_t;
typedef char        io_name_t[128];
typedef UInt32        IOOptionBits;

CFTypeRef
IORegistryEntrySearchCFProperty(
                                io_registry_entry_t    entry,
                                const io_name_t        plane,
                                CFStringRef        key,
                                CFAllocatorRef        allocator,
                                IOOptionBits        options );

kern_return_t
IOMasterPort( mach_port_t    bootstrapPort,
             mach_port_t *    masterPort );

io_registry_entry_t
IORegistryGetRootEntry(
                       mach_port_t    masterPort );

CFTypeRef
IORegistryEntrySearchCFProperty(
                                io_registry_entry_t    entry,
                                const io_name_t        plane,
                                CFStringRef        key,
                                CFAllocatorRef        allocator,
                                IOOptionBits        options );

kern_return_t
mach_port_deallocate(
                     ipc_space_t                       task,
                     mach_port_name_t                          name);



NSArray *getValue(NSString *iosearch)
{
    mach_port_t          masterPort;
    CFTypeID             propID = (CFTypeID) NULL;
    unsigned int         bufSize;
    
    kern_return_t kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (kr != noErr) return nil;
    
    io_registry_entry_t entry = IORegistryGetRootEntry(masterPort);
    if (entry == MACH_PORT_NULL) return nil;
    
    CFTypeRef prop = IORegistryEntrySearchCFProperty(entry, kIODeviceTreePlane, (CFStringRef) iosearch, nil, kIORegistryIterateRecursively);
    if (!prop) return nil;
    
    propID = CFGetTypeID(prop);
    if (!(propID == CFDataGetTypeID()))
    {
        mach_port_deallocate(mach_task_self(), masterPort);
        return nil;
    }
    
    CFDataRef propData = (CFDataRef) prop;
    if (!propData) return nil;
    
    bufSize = CFDataGetLength(propData);
    if (!bufSize) return nil;
    
    //NSString *p1 = [[[NSString alloc] initWithBytes:CFDataGetBytePtr(propData) length:bufSize encoding:1] autorelease];
    NSString *p1 = [[[NSString alloc] initWithBytes:CFDataGetBytePtr(propData) length:bufSize encoding:NSUTF8StringEncoding] autorelease];
    mach_port_deallocate(mach_task_self(), masterPort);
    return [p1 componentsSeparatedByString:@"/0"];
}

const char* GetMobileIMEI()
{
     NSArray *results = getValue(@"device-imei");
     if (results)
     {
         NSString *string_content = [results objectAtIndex:0];
         const char *char_content = [string_content UTF8String];
         NSString* strIMEI = [[NSString alloc] initWithCString:(const char*)char_content encoding:NSUTF8StringEncoding];
         return [strIMEI UTF8String];
     }

    return "";
}
#endif

#pragma mark MAC
// Return the local MAC addy
// Courtesy of FreeBSD hackers email list
// Accidentally munged during previous update. Fixed thanks to mlamb.
const char* GetMobileMAC()
{
    int mib[6];
    size_t len;
    char *buf;
    unsigned char *ptr;
    struct if_msghdr *ifm;
    struct sockaddr_dl *sdl;
    
    
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0;
    mib[3] = AF_LINK;
    mib[4] = NET_RT_IFLIST;
    
    if ((mib[5] = if_nametoindex("en0")) == 0) {
        MOJING_WARN(g_APIlogger,"Error: if_nametoindex error");
        return "";
    }
    
    if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0) {
        MOJING_WARN(g_APIlogger, "Error: sysctl, take 1");
        return "";
    }
    
    if ((buf = (char* )malloc(len)) == NULL) {
        MOJING_WARN(g_APIlogger, "Could not allocate memory. error!");
        return "";
    }
    
    if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
        MOJING_WARN(g_APIlogger, "Error: sysctl, take 2");
        return "";
    }

    ifm = (struct if_msghdr *)buf;
    sdl = (struct sockaddr_dl *)(ifm + 1);
    ptr = (unsigned char *)LLADDR(sdl);
    
    //After ios7, it always return 02:00:00:00:00:00
    NSString *outstring = [NSString stringWithFormat:@"%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
    free(buf);
    
    NSString *macString = [outstring uppercaseString];
    
    return [macString UTF8String];
}

#if 0
const char* GetAdvertiseIdentifier()
{
    NSString *identifierForAdvertising = [[ASIdentifierManager sharedManager].advertisingIdentifier UUIDString];
    
    return [identifierForAdvertising UTF8String];
}
#endif

const char* GetMobileSystemVersion()
{
    NSString* systemName = [[UIDevice currentDevice] systemName];
    NSString* version = [[UIDevice currentDevice] systemVersion];
    NSString* systemVersion = [systemName stringByAppendingString:version];
    return [systemVersion UTF8String];
}

const char* GetMobileSimpModelName()
{
    NSString* phoneModel = [[UIDevice currentDevice] model];
    
    return [phoneModel UTF8String];
}

const char* GetMobileModelName()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    NSInteger modelIndex = - 1;
    NSString *modelNameString = nil;
    modelIndex = [modelArray indexOfObject:deviceString];
    if (modelIndex >= 0 && modelIndex < [modelNameArray count]) {
        modelNameString = [modelNameArray objectAtIndex:modelIndex];
    }
    
    if(modelNameString)
        return [modelNameString UTF8String];
    else
    {
        MOJING_WARN(g_APIlogger, "GetMobileModelName failed");
        return [deviceString UTF8String];
        //return "UNKNOWN";
    }
}

const char* GetMobileInnerModelName()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    NSInteger modelIndex = - 1;
    NSString *modelNameString = nil;
    modelIndex = [modelArray indexOfObject:deviceString];
    if (modelIndex >= 0 && modelIndex < [modelInnerNameArray count]) {
        modelNameString = [modelInnerNameArray objectAtIndex:modelIndex];
    }
    
    if(modelNameString)
        return [modelNameString UTF8String];
    else
    {
        MOJING_WARN(g_APIlogger, "GetMobileInnerModelName failed");
        return [deviceString UTF8String];
        //return "UNKNOWN";
    }
    
}


const char* GetMobileBrand()
{
    return "Apple";
}

bool GetMobileScreenSize(int &nWidth, int &nHeight, float &xDpi, float &yDpi , float& scale , bool bIsUnity /*= false*/)
{
    UIScreen *MainScreen = [UIScreen mainScreen];
    CGSize Size = [MainScreen bounds].size;
    scale = [MainScreen scale];
    CGFloat screenWidth = Size.width ;
    CGFloat screenHeight = Size.height;
    
    nWidth = (int) fmaxf(screenWidth, screenHeight);
    nHeight = (int) fminf(screenWidth, screenHeight);
    nWidth *= scale;
    nHeight*= scale;
    
    if (nHeight == 750 ) // iPhone6 Standard
    {
        xDpi = 327 / 1;//(float) fmaxf(Size.width, Size.height);
        yDpi = 327 / 1;//(float) fminf(Size.width, Size.height);
    }
    else if(nHeight == 640) //iPhone6 Zoomed
    {
        xDpi = yDpi = 279.04;
    }
    else if(nHeight == 1125) //iPhone6 Plus Zoomed
    {
        if (bIsUnity)
        {//iPhone6+  1920 X 1080
            nWidth = (int) 2048;
            nHeight = (int) 1024;
            xDpi = yDpi = 416.3152;
        }
        else
        {
        	xDpi = yDpi = 416.3152;
        }
    }
    else if(nHeight == 1242) // iPhone6 Plus Standard
    {
        if (bIsUnity)
        {//iPhone6+  1920 X 1080
            nWidth = (int) 2048;
            nHeight = (int) 1024;
            xDpi = yDpi = 416.3152;
        }
        else
        {//iPhone6+ 2208X1242
            xDpi = yDpi = 460.6;
        }
    }
    else
    {
        MOJING_ERROR(g_APIlogger, "GetMobileScreenSize failed! The device is not supported.");
        return false;
    }
       
    return true;
}


const char* GetCurAppName()
{   
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    NSString *appCurName = [infoDictionary objectForKey:@"CFBundleDisplayName"];
    
    if(appCurName == nil)
    {
        appCurName = [infoDictionary objectForKey:@"CFBundleName"];
        if(appCurName == nil)
        {
            NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
            appCurName = [[NSFileManager defaultManager] displayNameAtPath: bundlePath];
            if(appCurName == nil)
            {
                MOJING_WARN(g_APIlogger, "GetCurAppName failed");
                return "UNKNOWN";
            }
        }
    }
    
    return [appCurName UTF8String];
}

const char* GetCurAppVersion()
{
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    
    NSString *appCurName = [infoDictionary objectForKey:@"CFBundleDisplayName"];
    if(appCurName == nil)
    {
        appCurName = [infoDictionary objectForKey:@"CFBundleName"];
        if(appCurName == nil)
        {
            NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
            appCurName = [[NSFileManager defaultManager] displayNameAtPath: bundlePath];
            if(appCurName == nil)
            {
                MOJING_WARN(g_APIlogger, "GetCurAppName failed");
                return "UNKNOWN";
            }
        }
    }
    
    NSString *appCurVersion = [infoDictionary objectForKey:@"CFBundleVersion"];
    
    NSString *appVersion = [appCurName stringByAppendingString:@" V"];
    appVersion = [appVersion stringByAppendingString:appCurVersion];

    return [appVersion UTF8String];
}

const char* GetCurPackageName()
{
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    NSString *packageName = [infoDictionary objectForKey:@"CFBundleIdentifier"];
    
    return [packageName UTF8String];
}

const char* GetSFHFKeychainUserID()
{
    NSError *err;
    NSString *KeyID = [SFHFKeychainUtils getPasswordForUsername: @"UserID" andServiceName: @"com.baofeng.MoijingSDK" error: &err];
    if(KeyID == nil)
    {
        CFUUIDRef uuid = CFUUIDCreate(nil);
        KeyID = (NSString *)CFBridgingRelease(CFUUIDCreateString(nil, uuid));
        CFRelease(uuid);
  
        BOOL bRet = [SFHFKeychainUtils storeUsername:@"UserID" andPassword:KeyID forServiceName:@"com.baofeng.MoijingSDK" updateExisting:true error:&err];
        if(!bRet)
        {
            MOJING_WARN(g_APIlogger, "Set UserID to keychain failed");
        }
    }

    return [KeyID UTF8String];
}

const char* GetSFHFKeychainSerialID()
{
    NSError *err;
    NSString *KeyID = [SFHFKeychainUtils getPasswordForUsername: @"SerialID" andServiceName: @"com.baofeng.MoijingSDK" error: &err];
    if(KeyID == nil)
    {
        CFUUIDRef uuid = CFUUIDCreate(nil);
        KeyID = (NSString *)CFBridgingRelease(CFUUIDCreateString(nil, uuid));
        CFRelease(uuid);
        
        NSString *SeriralID = [KeyID stringByReplacingOccurrencesOfString : @"-" withString : @"" ];
        
        BOOL bRet = [SFHFKeychainUtils storeUsername:@"SerialID" andPassword:SeriralID forServiceName:@"com.baofeng.MoijingSDK" updateExisting:true error:&err];
        if(!bRet)
        {
            MOJING_WARN(g_APIlogger, "Set SerialID to keychain failed");
        }
        
        return [SeriralID UTF8String];
    }
    
    return [KeyID UTF8String];
}


const char* GetUUID()
{
    CFUUIDRef uuid = CFUUIDCreate(nil);
    NSString *sid = (NSString *)CFBridgingRelease(CFUUIDCreateString(nil, uuid));
    CFRelease(uuid);
    
    return [sid UTF8String];
}

const char* GetVerdorUUID()
{
    NSString * vid =  [UIDevice currentDevice].identifierForVendor.UUIDString;
    
    return [vid UTF8String];
}

const char* GetOpenUDID()
{
    NSString *openUDID = [MJOpenUDID value];
    
    return [openUDID UTF8String];
}

const char* GetUserID()
{
    return GetSFHFKeychainUserID();
}

const char* GetMobileSerialNumber()
{
    /*
     return GetMobileIMEI();
     return GetMobileMAC();
     return  GetAdvertiseIdentifier();
     return GetOpenUDID();
     return "UNKOWN";
     */
    
    return GetSFHFKeychainSerialID();
}


//获取Documents目录
NSString* GetDirDocuments()
{
    //[NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    //NSLog(@"app_home_doc: %@",documentsDirectory);
    return documentsDirectory;
}

//获取Library目录
NSString* GetDirLibrary()
{
    //[NSHomeDirectory() stringByAppendingPathComponent:@"Library"];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    NSString *libraryDirectory = [paths objectAtIndex:0];
    //NSLog(@"app_home_lib: %@",libraryDirectory);
    return libraryDirectory;
}

//获取Cache目录
NSString* GetDirCaches()
{
    NSArray *cacPath = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachePath = [cacPath objectAtIndex:0];
    //NSLog(@"app_home_lib_cache: %@",cachePath);
    return  cachePath;
}


//获取Tmp目录
NSString* GetDirTmp()
{
    //[NSHomeDirectory() stringByAppendingPathComponent:@"tmp"];
    NSString *tmpDirectory = NSTemporaryDirectory();
    //NSLog(@"app_home_tmp: %@",tmpDirectory);
    return tmpDirectory;
}

NSString* GetDirAPPData()
{
    NSString* dirHome = NSHomeDirectory();
    
    NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
    NSString* appCurName = [[NSFileManager defaultManager] displayNameAtPath: bundlePath];
    if(appCurName == nil)
    {
        MOJING_WARN(g_APIlogger, "GetDirAPPData failed");
        return dirHome;
    }
    else
    {
        return [dirHome stringByAppendingPathComponent:appCurName];
    }
}


const char* GetIOSAppPrivateDir(int nType)
{
    NSString *path = nil;
    switch(nType)
    {
        case APP_DIR_HOME:
            path = NSHomeDirectory();
            break;
        case APP_DIR_APP:
            path = GetDirAPPData();
            break;
        case APP_DIR_LIBRARY:
            path = GetDirLibrary();
            break;
        case APP_DIR_CACHES:
            path = GetDirCaches();
            break;
        case APP_DIR_TMP:
            path = GetDirTmp();
            break;
        default:
            path = GetDirDocuments();
            break;
    }
    
    return  [path UTF8String];
}

//创建文件夹
void CreateIOSDir(const char* path)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *testDirectory = [NSString stringWithUTF8String:path];

    BOOL res=[fileManager createDirectoryAtPath:testDirectory withIntermediateDirectories:YES attributes:nil error:nil];
    if (res)
    {
        //MOJING_TRACE(g_APIlogger,"Create dir succeed! path = " << path);
        //NSLog(@"文件夹创建成功");
    }else{
        //MOJING_WARN(g_APIlogger,"Create dir failed! path = " << path);
        //NSLog(@"文件夹创建失败");
    }
}

const char* GetSDKAssetsDirPath()
{
    NSString *bundlePath = [[NSBundle mainBundle] pathForResource:@"MojingSDK" ofType:@"bundle"];  
    if(bundlePath == nil)
    {
        MOJING_WARN(g_APIlogger, "MojingSDK.bundle not found.");
        return NULL;
    }
 
    return [bundlePath UTF8String];
}

NSBundle * GetResourceBundle(NSString *bundleName)
{
    //显然这里你也可以通过其他的方式取得，总之找到bundle的完整路径即可。
    NSString *bundlePath = [[NSBundle mainBundle] pathForResource:bundleName ofType:@"bundle"];
    NSBundle *bundle = [NSBundle bundleWithPath:bundlePath];
    
    return bundle;
}

const char* GetSDKAssetsFilePath(const char* filename)
{
    char* p = strrchr(filename, '.');
    if(p == NULL)
    {
        return NULL;
    }
    char name[260];
    memset(name, 0, 260);
    memcpy(name, filename, p-filename);
    
    //我这里是有一个assets.bundle的bundle文件
    NSBundle *bundle = GetResourceBundle(@"MojingSDK");
    NSString* filePath = [bundle pathForResource:[NSString stringWithUTF8String:name] ofType:[NSString stringWithUTF8String:p+1]];
    
    return [filePath UTF8String];
}

//Get application's preload data path
const char* GetAppAssetsFilePath(const char* filename)
{
    char* p = strrchr(filename, '.');
    if(p == NULL)
    {
        return NULL;
    }
    char name[260];
    memset(name, 0, 260);
    memcpy(name, filename, p-filename);
    
    NSString* filePath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:name] ofType:[NSString stringWithUTF8String:p+1]];
   
    return [filePath UTF8String];
}


float GetBatteryLevel()
{
    [[UIDevice currentDevice] setBatteryMonitoringEnabled:YES];
    return [[UIDevice currentDevice] batteryState];
}

NSString* getCPUType()
{
    NSMutableString *cpu = [[NSMutableString alloc] init];
   	size_t size;
    cpu_type_t type;
    cpu_subtype_t subtype;
    size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &size, NULL, 0);

    size = sizeof(subtype);
    sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
    
    // values for cputype and cpusubtype defined in mach/machine.h
    if (type == CPU_TYPE_X86)
    {
    	[cpu appendString:@"x86 "];
        // check for subtype ...
    }
    else if (type == CPU_TYPE_ARM)
    {
        [cpu appendString:@"ARM"];
        [cpu appendFormat:@",Type:%d",subtype];
    }
    else
    {
        [cpu appendString:@"UNKNOWN"];
    }
	return cpu;
}

const char* GetCPUArchName()
{
    NSString *cpuArch = getCPUType();
    
    return  [cpuArch UTF8String];
}


const char* GetGLVersion()
{
    return (const char *)glGetString(GL_VERSION);
}

const char* GetGLESVersion()
{
    return (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
}

