// Copyright (c) 2014-present, Facebook, Inc. All rights reserved.
//
// You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
// copy, modify, and distribute this software in source code or binary form for use
// in connection with the web services and APIs provided by Facebook.
//
// As with any software that integrates with the Facebook platform, your use of
// this software is subject to the Facebook Developer Principles and Policies
// [http://developers.facebook.com/policy/]. This copyright notice shall be
// included in all copies or substantial portions of the software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#import "FBSDKApplicationDelegate.h"
#import "FBSDKApplicationDelegate+Internal.h"

#import <objc/runtime.h>

#import "FBSDKAppEvents+Internal.h"
#import "FBSDKConstants.h"
#import "FBSDKDynamicFrameworkLoader.h"
#import "FBSDKError.h"
#import "FBSDKEventDeactivationManager.h"
#import "FBSDKFeatureManager.h"
#import "FBSDKGateKeeperManager.h"
#import "FBSDKInstrumentManager.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKLogger.h"
#import "FBSDKServerConfiguration.h"
#import "FBSDKServerConfigurationManager.h"
#import "FBSDKSettings+Internal.h"
#import "FBSDKTimeSpentData.h"

#if !TARGET_OS_TV
<<<<<<< HEAD
#import "FBSDKMeasurementEventListener.h"
#import "FBSDKContainerViewController.h"
#import "FBSDKProfile+Internal.h"
=======
 #import "FBSDKContainerViewController.h"
 #import "FBSDKMeasurementEventListener.h"
 #import "FBSDKProfile+Internal.h"
>>>>>>> origin/develop12
#endif

#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0

NSNotificationName const FBSDKApplicationDidBecomeActiveNotification = @"com.facebook.sdk.FBSDKApplicationDidBecomeActiveNotification";

#else

NSString *const FBSDKApplicationDidBecomeActiveNotification = @"com.facebook.sdk.FBSDKApplicationDidBecomeActiveNotification";

#endif

static NSString *const FBSDKAppLinkInboundEvent = @"fb_al_inbound";
<<<<<<< HEAD
static NSString *const FBSDKKitsBitmaskKey  = @"com.facebook.sdk.kits.bitmask";
=======
static NSString *const FBSDKKitsBitmaskKey = @"com.facebook.sdk.kits.bitmask";
static NSString *const FBSDKAutoInitLaunchArgument = @"_calledFromAutoInitSDK";
>>>>>>> origin/develop12
static BOOL g_isSDKInitialized = NO;
static UIApplicationState _applicationState;

@implementation FBSDKApplicationDelegate
{
  NSHashTable<id<FBSDKApplicationObserving>> *_applicationObservers;
  BOOL _isAppLaunched;
}

#pragma mark - Class Methods

+ (void)load
{
<<<<<<< HEAD
  if ([FBSDKSettings isAutoInitEnabled]) {
=======
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
  if ([FBSDKSettings isAutoInitEnabled]) {
    #pragma clang diagnostic pop
>>>>>>> origin/develop12
    // when the app becomes active by any means,  kick off the initialization.
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(initializeWithLaunchData:)
                                                 name:UIApplicationDidFinishLaunchingNotification
                                               object:nil];
  }
}

// Initialize SDK listeners
// Don't call this function in any place else. It should only be called when the class is loaded.
+ (void)initializeWithLaunchData:(NSNotification *)note
{
  [self initializeSDK:note.userInfo];
  // Remove the observer
  [[NSNotificationCenter defaultCenter] removeObserver:self
                                                  name:UIApplicationDidFinishLaunchingNotification
                                                object:nil];
}

<<<<<<< HEAD
+ (void)initializeSDK:(NSDictionary<UIApplicationLaunchOptionsKey,id> *)launchOptions
{
  if (g_isSDKInitialized) {
    //  Do nothing if initialized already
=======
+ (void)initializeSDK:(NSDictionary<UIApplicationLaunchOptionsKey, id> *)launchOptions
{
  if (g_isSDKInitialized) {
    // Do nothing if initialized already
>>>>>>> origin/develop12
    return;
  }

  g_isSDKInitialized = YES;

  FBSDKApplicationDelegate *delegate = [self sharedInstance];
<<<<<<< HEAD
=======
  [FBSDKSettings recordInstall];
>>>>>>> origin/develop12

  NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
  [defaultCenter addObserver:delegate selector:@selector(applicationDidEnterBackground:) name:UIApplicationDidEnterBackgroundNotification object:nil];
  [defaultCenter addObserver:delegate selector:@selector(applicationDidBecomeActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
<<<<<<< HEAD

  [[FBSDKAppEvents singleton] registerNotifications];

  [delegate application:[UIApplication sharedApplication] didFinishLaunchingWithOptions:launchOptions];
=======
  [defaultCenter addObserver:delegate selector:@selector(applicationWillResignActive:) name:UIApplicationWillResignActiveNotification object:nil];

  [[FBSDKAppEvents singleton] registerNotifications];

  NSMutableDictionary *modifiedLaunchOptions = [NSMutableDictionary dictionaryWithDictionary:launchOptions];
  [FBSDKTypeUtility dictionary:modifiedLaunchOptions setObject:@YES forKey:FBSDKAutoInitLaunchArgument];
  [delegate application:[UIApplication sharedApplication] didFinishLaunchingWithOptions:modifiedLaunchOptions];

  // In case of sdk autoInit enabled sdk expects one appDidBecomeActive notification after app launch and has some logic to ignore it.
  // if sdk autoInit disabled app won't receive appDidBecomeActive on app launch and will ignore the first one it gets instead of handling it.
  // Send first applicationDidBecomeActive notification manually
  if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive) {
    [delegate applicationDidBecomeActive:nil];
  }
>>>>>>> origin/develop12

  [FBSDKFeatureManager checkFeature:FBSDKFeatureInstrument completionBlock:^(BOOL enabled) {
    if (enabled) {
      [FBSDKInstrumentManager enable];
    }
  }];

<<<<<<< HEAD
  [FBSDKFeatureManager checkFeature:FBSDKFeatureRestrictiveDataFiltering completionBlock:^(BOOL enabled) {
    if (enabled) {
      [FBSDKRestrictiveDataFilterManager enable];
    }
  }];

  [FBSDKFeatureManager checkFeature:FBSDKFeatureEventDeactivation completionBlock:^(BOOL enabled) {
    if (enabled) {
      [FBSDKEventDeactivationManager enable];
    }
  }];

  [FBSDKFeatureManager checkFeature:FBSDKFeatureMonitoring completionBlock:^(BOOL enabled) {
    if (enabled && FBSDKSettings.isAutoLogAppEventsEnabled) {
#ifndef DEBUG
      [FBSDKMonitor enable];
#endif
=======
  [FBSDKFeatureManager checkFeature:FBSDKFeatureMonitoring completionBlock:^(BOOL enabled) {
    if (enabled && FBSDKSettings.isAutoLogAppEventsEnabled) {
    #ifndef DEBUG
      [FBSDKMonitor enable];
    #endif
>>>>>>> origin/develop12
    }
  }];

#if !TARGET_OS_TV
  // Register Listener for App Link measurement events
  [FBSDKMeasurementEventListener defaultListener];
  [delegate _logIfAutoAppLinkEnabled];
#endif
  // Set the SourceApplication for time spent data. This is not going to update the value if the app has already launched.
  [FBSDKTimeSpentData setSourceApplication:launchOptions[UIApplicationLaunchOptionsSourceApplicationKey]
                                   openURL:launchOptions[UIApplicationLaunchOptionsURLKey]];
  // Register on UIApplicationDidEnterBackgroundNotification events to reset source application data when app backgrounds.
  [FBSDKTimeSpentData registerAutoResetSourceApplication];

  [FBSDKInternalUtility validateFacebookReservedURLSchemes];
}

+ (FBSDKApplicationDelegate *)sharedInstance
{
  static FBSDKApplicationDelegate *_sharedInstance;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    _sharedInstance = [[self alloc] init];
  });
  return _sharedInstance;
}

#pragma mark - Object Lifecycle

- (instancetype)init
{
  if ((self = [super init]) != nil) {
    _applicationObservers = [[NSHashTable alloc] init];
  }
  return self;
}

- (void)dealloc
{
<<<<<<< HEAD
    [[NSNotificationCenter defaultCenter] removeObserver:self];
=======
  [[NSNotificationCenter defaultCenter] removeObserver:self];
>>>>>>> origin/develop12
}

#pragma mark - UIApplicationDelegate

<<<<<<< HEAD
#if __IPHONE_OS_VERSION_MAX_ALLOWED > __IPHONE_9_0
- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey,id> *)options
{
    if (@available(iOS 9.0, *)) {
        return [self application:application
                         openURL:url
               sourceApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey]
                      annotation:options[UIApplicationOpenURLOptionsAnnotationKey]];
    }

    return NO;
}
=======
#if __IPHONE_OS_VERSION_MAX_ALLOWED> __IPHONE_9_0
- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey, id> *)options
{
  if (@available(iOS 9.0, *)) {
    return [self application:application
                      openURL:url
            sourceApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey]
                   annotation:options[UIApplicationOpenURLOptionsAnnotationKey]];
  }

  return NO;
}

>>>>>>> origin/develop12
#endif

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication
         annotation:(id)annotation
{
  if (sourceApplication != nil && ![sourceApplication isKindOfClass:[NSString class]]) {
    @throw [NSException exceptionWithName:NSInvalidArgumentException
                                   reason:@"Expected 'sourceApplication' to be NSString. Please verify you are passing in 'sourceApplication' from your app delegate (not the UIApplication* parameter). If your app delegate implements iOS 9's application:openURL:options:, you should pass in options[UIApplicationOpenURLOptionsSourceApplicationKey]. "
                                 userInfo:nil];
  }
  [FBSDKTimeSpentData setSourceApplication:sourceApplication openURL:url];

  BOOL handled = NO;
  NSArray<id<FBSDKApplicationObserving>> *observers = [_applicationObservers allObjects];
  for (id<FBSDKApplicationObserving> observer in observers) {
    if ([observer respondsToSelector:@selector(application:openURL:sourceApplication:annotation:)]) {
      if ([observer application:application
                        openURL:url
              sourceApplication:sourceApplication
                     annotation:annotation]) {
        handled = YES;
      }
    }
  }

  if (handled) {
    return YES;
  }

  [self _logIfAppLinkEvent:url];

  return NO;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
<<<<<<< HEAD
    if ([self isAppLaunched]) {
        return NO;
    }

    _isAppLaunched = YES;
    FBSDKAccessToken *cachedToken = [FBSDKSettings accessTokenCache].accessToken;
    [FBSDKAccessToken setCurrentAccessToken:cachedToken];
    // fetch app settings
    [FBSDKServerConfigurationManager loadServerConfigurationWithCompletionBlock:NULL];

    if (FBSDKSettings.isAutoLogAppEventsEnabled) {
      [self _logSDKInitialize];
    }
#if !TARGET_OS_TV
    FBSDKProfile *cachedProfile = [FBSDKProfile fetchCachedProfile];
    [FBSDKProfile setCurrentProfile:cachedProfile];
=======
  if ([[FBSDKTypeUtility dictionary:launchOptions objectForKey:FBSDKAutoInitLaunchArgument ofType:NSNumber.class] boolValue]) {
    // Clean the flag out of launch options
    NSMutableDictionary *originalLaunchOptions = [NSMutableDictionary dictionaryWithDictionary:launchOptions];
    [originalLaunchOptions removeObjectForKey:FBSDKAutoInitLaunchArgument];
    launchOptions = [originalLaunchOptions copy];
  } else {
    // Log because it was not called by autoInit
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    [FBSDKAppEvents logInternalEvent:FBSDKAppEventNameImplementsApplicationDidFinishLaunching
                          parameters:@{}
                  isImplicitlyLogged:YES];
    #pragma clang diagnostic pop
  }

  if (_isAppLaunched) {
    return NO;
  }

  _isAppLaunched = YES;
  FBSDKAccessToken *cachedToken = [FBSDKSettings accessTokenCache].accessToken;
  [FBSDKAccessToken setCurrentAccessToken:cachedToken];
  // fetch app settings
  [FBSDKServerConfigurationManager loadServerConfigurationWithCompletionBlock:NULL];

  if (FBSDKSettings.isAutoLogAppEventsEnabled) {
    [self _logSDKInitialize];
  }
#if !TARGET_OS_TV
  FBSDKProfile *cachedProfile = [FBSDKProfile fetchCachedProfile];
  [FBSDKProfile setCurrentProfile:cachedProfile];
>>>>>>> origin/develop12
#endif
  NSArray<id<FBSDKApplicationObserving>> *observers = [_applicationObservers allObjects];
  BOOL handled = NO;
  for (id<FBSDKApplicationObserving> observer in observers) {
    if ([observer respondsToSelector:@selector(application:didFinishLaunchingWithOptions:)]) {
      if ([observer application:application didFinishLaunchingWithOptions:launchOptions]) {
        handled = YES;
      }
    }
  }

  return handled;
}

- (void)applicationDidEnterBackground:(NSNotification *)notification
{
  _applicationState = UIApplicationStateBackground;
  NSArray<id<FBSDKApplicationObserving>> *observers = [_applicationObservers allObjects];
  for (id<FBSDKApplicationObserving> observer in observers) {
    if ([observer respondsToSelector:@selector(applicationDidEnterBackground:)]) {
      [observer applicationDidEnterBackground:notification.object];
    }
  }
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
  _applicationState = UIApplicationStateActive;
  // Auto log basic events in case autoLogAppEventsEnabled is set
  if (FBSDKSettings.isAutoLogAppEventsEnabled) {
    [FBSDKAppEvents activateApp];
  }
<<<<<<< HEAD
=======
#if !TARGET_OS_TV
  [FBSDKSKAdNetworkReporter checkAndRevokeTimer];
#endif
>>>>>>> origin/develop12

  NSArray<id<FBSDKApplicationObserving>> *observers = [_applicationObservers copy];
  for (id<FBSDKApplicationObserving> observer in observers) {
    if ([observer respondsToSelector:@selector(applicationDidBecomeActive:)]) {
      [observer applicationDidBecomeActive:notification.object];
    }
  }
}

<<<<<<< HEAD
=======
- (void)applicationWillResignActive:(NSNotification *)notification
{
  _applicationState = UIApplicationStateInactive;
  NSArray<id<FBSDKApplicationObserving>> *const observers = [_applicationObservers copy];
  for (id<FBSDKApplicationObserving> observer in observers) {
    if ([observer respondsToSelector:@selector(applicationWillResignActive:)]) {
      [observer applicationWillResignActive:notification.object];
    }
  }
}

>>>>>>> origin/develop12
#pragma mark - Internal Methods

#pragma mark - FBSDKApplicationObserving

- (void)addObserver:(id<FBSDKApplicationObserving>)observer
{
  if (![_applicationObservers containsObject:observer]) {
    [_applicationObservers addObject:observer];
  }
}

- (void)removeObserver:(id<FBSDKApplicationObserving>)observer
{
  if ([_applicationObservers containsObject:observer]) {
    [_applicationObservers removeObject:observer];
  }
}

+ (UIApplicationState)applicationState
{
  return _applicationState;
}

#pragma mark - Helper Methods

- (void)_logIfAppLinkEvent:(NSURL *)url
{
  if (!url) {
    return;
  }
  NSDictionary<NSString *, NSString *> *params = [FBSDKBasicUtility dictionaryWithQueryString:url.query];
  NSString *applinkDataString = params[@"al_applink_data"];
  if (!applinkDataString) {
    return;
  }

<<<<<<< HEAD
  NSDictionary<id, id> *applinkData = [FBSDKBasicUtility objectForJSONString:applinkDataString error:NULL];
=======
  NSDictionary<id, id> *applinkData = [FBSDKTypeUtility dictionaryValue:[FBSDKBasicUtility objectForJSONString:applinkDataString error:NULL]];
>>>>>>> origin/develop12
  if (!applinkData) {
    return;
  }

  NSString *targetURLString = applinkData[@"target_url"];
  NSURL *targetURL = [targetURLString isKindOfClass:[NSString class]] ? [NSURL URLWithString:targetURLString] : nil;

  NSMutableDictionary *logData = [[NSMutableDictionary alloc] init];
  [FBSDKTypeUtility dictionary:logData setObject:targetURL.absoluteString forKey:@"targetURL"];
  [FBSDKTypeUtility dictionary:logData setObject:targetURL.host forKey:@"targetURLHost"];

  NSDictionary *refererData = applinkData[@"referer_data"];
  if (refererData) {
    [FBSDKTypeUtility dictionary:logData setObject:refererData[@"target_url"] forKey:@"referralTargetURL"];
    [FBSDKTypeUtility dictionary:logData setObject:refererData[@"url"] forKey:@"referralURL"];
    [FBSDKTypeUtility dictionary:logData setObject:refererData[@"app_name"] forKey:@"referralAppName"];
  }
  [FBSDKTypeUtility dictionary:logData setObject:url.absoluteString forKey:@"inputURL"];
  [FBSDKTypeUtility dictionary:logData setObject:url.scheme forKey:@"inputURLScheme"];

  [FBSDKAppEvents logInternalEvent:FBSDKAppLinkInboundEvent
                        parameters:logData
                isImplicitlyLogged:YES];
}

- (void)_logSDKInitialize
{
  NSDictionary *metaInfo = [NSDictionary dictionaryWithObjects:@[@"login_lib_included",
                                                                 @"marketing_lib_included",
                                                                 @"messenger_lib_included",
                                                                 @"places_lib_included",
                                                                 @"share_lib_included",
                                                                 @"tv_lib_included"]
                                                       forKeys:@[@"FBSDKLoginManager",
                                                                 @"FBSDKAutoLog",
                                                                 @"FBSDKMessengerButton",
                                                                 @"FBSDKPlacesManager",
                                                                 @"FBSDKShareDialog",
                                                                 @"FBSDKTVInterfaceFactory"]];

  NSInteger bitmask = 0;
  NSInteger bit = 0;
  NSMutableDictionary<NSString *, NSNumber *> *params = NSMutableDictionary.new;
  [FBSDKTypeUtility dictionary:params setObject:@1 forKey:@"core_lib_included"];
  for (NSString *className in metaInfo.allKeys) {
    NSString *keyName = [FBSDKTypeUtility dictionary:metaInfo objectForKey:className ofType:NSObject.class];
    if (objc_lookUpClass([className UTF8String])) {
      [FBSDKTypeUtility dictionary:params setObject:@1 forKey:keyName];
<<<<<<< HEAD
      bitmask |=  1 << bit;
=======
      bitmask |= 1 << bit;
>>>>>>> origin/develop12
    }
    bit++;
  }

  [self _logSwiftRuntimeAvailability];

  NSInteger existingBitmask = [[NSUserDefaults standardUserDefaults] integerForKey:FBSDKKitsBitmaskKey];
  if (existingBitmask != bitmask) {
    [[NSUserDefaults standardUserDefaults] setInteger:bitmask forKey:FBSDKKitsBitmaskKey];
    [FBSDKAppEvents logInternalEvent:@"fb_sdk_initialize"
                          parameters:params
                  isImplicitlyLogged:NO];
  }
}

- (void)_logIfAutoAppLinkEnabled
{
#if !TARGET_OS_TV
  NSNumber *enabled = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"FBSDKAutoAppLinkEnabled"];
  if (enabled.boolValue) {
    NSMutableDictionary<NSString *, NSString *> *params = [[NSMutableDictionary alloc] init];
    if (![FBSDKAppLinkUtility isMatchURLScheme:[NSString stringWithFormat:@"fb%@", [FBSDKSettings appID]]]) {
      NSString *warning = @"You haven't set the Auto App Link URL scheme: fb<YOUR APP ID>";
      [FBSDKTypeUtility dictionary:params setObject:warning forKey:@"SchemeWarning"];
      NSLog(@"%@", warning);
    }
    [FBSDKAppEvents logInternalEvent:@"fb_auto_applink" parameters:params isImplicitlyLogged:YES];
  }
#endif
}

- (void)_logSwiftRuntimeAvailability
{
  NSString *swiftUsageKey = @"is_using_swift";
  NSString *eventName = @"fb_sdk_swift_runtime_check";
  NSMutableDictionary<NSString *, NSNumber *> *params = NSMutableDictionary.new;

  // Tracking if the consuming Application is using Swift
  id delegate = [UIApplication sharedApplication].delegate;
  NSString const *className = NSStringFromClass([delegate class]);
  if ([className componentsSeparatedByString:@"."].count > 1) {
    params[swiftUsageKey] = @YES;
  }

  // Additional check to see if the consuming application perhaps was
  // originally an objc project but is now using Swift
  if (!params[swiftUsageKey].boolValue) {
    double delayInSeconds = 1.0;
    dispatch_time_t delay = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
<<<<<<< HEAD
    dispatch_after(delay, dispatch_get_main_queue(), ^{
      UIViewController *topMostViewController = [FBSDKInternalUtility topMostViewController];
      NSString const *vcClassName = NSStringFromClass([topMostViewController class]);
      if ([vcClassName componentsSeparatedByString:@"."].count > 1) {
        params[swiftUsageKey] = @YES;
        [FBSDKAppEvents logInternalEvent:eventName
                              parameters:params
                      isImplicitlyLogged:NO];
      }
    });
  };
=======
    dispatch_after(delay,
      dispatch_get_main_queue(), ^{
        UIViewController *topMostViewController = [FBSDKInternalUtility topMostViewController];
        NSString const *vcClassName = NSStringFromClass([topMostViewController class]);
        if ([vcClassName componentsSeparatedByString:@"."].count > 1) {
          params[swiftUsageKey] = @YES;
          [FBSDKAppEvents logInternalEvent:eventName
                                parameters:params
                        isImplicitlyLogged:NO];
        }
      });
  }
  ;
>>>>>>> origin/develop12
}

+ (BOOL)isSDKInitialized
{
<<<<<<< HEAD
  return [FBSDKSettings isAutoInitEnabled] || g_isSDKInitialized;
}

// Wrapping this makes it mockable and enables testability
- (BOOL)isAppLaunched {
  return _isAppLaunched;
}

=======
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
  return [FBSDKSettings isAutoInitEnabled] || g_isSDKInitialized;
  #pragma clange diagnostic pop
}

// MARK: - Testability

#if DEBUG

- (BOOL)isAppLaunched
{
  return _isAppLaunched;
}

- (void)setIsAppLaunched:(BOOL)isLaunched
{
  _isAppLaunched = isLaunched;
}

- (NSHashTable<id<FBSDKApplicationObserving>> *)applicationObservers
{
  return _applicationObservers;
}

- (void)resetApplicationObserverCache
{
  _applicationObservers = [NSHashTable new];
}

#endif

>>>>>>> origin/develop12
@end
