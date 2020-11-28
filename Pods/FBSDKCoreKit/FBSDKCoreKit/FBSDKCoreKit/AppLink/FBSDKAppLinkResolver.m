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

#import "TargetConditionals.h"

#if !TARGET_OS_TV

<<<<<<< HEAD
#import "FBSDKAppLinkResolver.h"

#import <UIKit/UIKit.h>

#import "FBSDKAccessToken.h"
#import "FBSDKAppLink.h"
#import "FBSDKAppLinkTarget.h"
#import "FBSDKGraphRequest+Internal.h"
#import "FBSDKGraphRequestConnection.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKLogger.h"
#import "FBSDKSettings+Internal.h"
#import "FBSDKUtility.h"
=======
 #import "FBSDKAppLinkResolver.h"

 #import <UIKit/UIKit.h>

 #import "FBSDKAccessToken.h"
 #import "FBSDKAppLink.h"
 #import "FBSDKAppLinkTarget.h"
 #import "FBSDKGraphRequest+Internal.h"
 #import "FBSDKGraphRequestConnection.h"
 #import "FBSDKInternalUtility.h"
 #import "FBSDKLogger.h"
 #import "FBSDKSettings+Internal.h"
 #import "FBSDKUtility.h"
>>>>>>> origin/develop12

static NSString *const kURLKey = @"url";
static NSString *const kIOSAppStoreIdKey = @"app_store_id";
static NSString *const kIOSAppNameKey = @"app_name";
static NSString *const kWebKey = @"web";
static NSString *const kIOSKey = @"ios";
static NSString *const kIPhoneKey = @"iphone";
static NSString *const kIPadKey = @"ipad";
static NSString *const kShouldFallbackKey = @"should_fallback";
static NSString *const kAppLinksKey = @"app_links";

@interface FBSDKAppLinkResolver ()

@property (nonatomic, strong) NSMutableDictionary<NSURL *, FBSDKAppLink *> *cachedFBSDKAppLinks;
@property (nonatomic, assign) UIUserInterfaceIdiom userInterfaceIdiom;
@end

@implementation FBSDKAppLinkResolver

+ (void)initialize
{
<<<<<<< HEAD
  if (self == [FBSDKAppLinkResolver class]) {
  }
=======
  if (self == [FBSDKAppLinkResolver class]) {}
>>>>>>> origin/develop12
}

- (instancetype)initWithUserInterfaceIdiom:(UIUserInterfaceIdiom)userInterfaceIdiom
{
  if (self = [super init]) {
    self.cachedFBSDKAppLinks = [NSMutableDictionary dictionary];
    self.userInterfaceIdiom = userInterfaceIdiom;
  }
  return self;
}

- (void)appLinkFromURL:(NSURL *)url handler:(FBSDKAppLinkBlock)handler
{
<<<<<<< HEAD
  [self appLinksFromURLs:@[url] handler:^(NSDictionary<NSURL *, FBSDKAppLink *> *urls, NSError * _Nullable error) {
=======
  [self appLinksFromURLs:@[url] handler:^(NSDictionary<NSURL *, FBSDKAppLink *> *urls, NSError *_Nullable error) {
>>>>>>> origin/develop12
    handler(urls[url], error);
  }];
}

- (void)appLinksFromURLs:(NSArray<NSURL *> *)urls handler:(FBSDKAppLinksBlock)handler
{
  if (![FBSDKSettings clientToken] && ![FBSDKAccessToken currentAccessToken]) {
    [FBSDKLogger singleShotLogEntry:FBSDKLoggingBehaviorDeveloperErrors
                           logEntry:@"A user access token or clientToken is required to use FBAppLinkResolver"];
  }
  NSMutableDictionary<NSURL *, FBSDKAppLink *> *appLinks = [NSMutableDictionary dictionary];
  NSMutableArray<NSURL *> *toFind = [NSMutableArray array];
  NSMutableArray<NSString *> *toFindStrings = [NSMutableArray array];

<<<<<<< HEAD
  @synchronized (self.cachedFBSDKAppLinks) {
=======
  @synchronized(self.cachedFBSDKAppLinks) {
>>>>>>> origin/develop12
    for (NSURL *url in urls) {
      if (self.cachedFBSDKAppLinks[url]) {
        [FBSDKTypeUtility dictionary:appLinks setObject:self.cachedFBSDKAppLinks[url] forKey:url];
      } else {
        [FBSDKTypeUtility array:toFind addObject:url];
<<<<<<< HEAD
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        NSString *toFindString = [url.absoluteString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
#pragma clang diagnostic pop
=======
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-declarations"
        NSString *toFindString = [url.absoluteString stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        #pragma clang diagnostic pop
>>>>>>> origin/develop12
        if (toFindString) {
          [FBSDKTypeUtility array:toFindStrings addObject:toFindString];
        }
      }
    }
  }
  if (toFind.count == 0) {
    // All of the URLs have already been found.
    handler(_cachedFBSDKAppLinks, nil);
  }
  NSMutableArray<NSString *> *fields = [NSMutableArray arrayWithObject:kIOSKey];

  NSString *idiomSpecificField = nil;

  switch (self.userInterfaceIdiom) {
    case UIUserInterfaceIdiomPad:
      idiomSpecificField = kIPadKey;
      break;
    case UIUserInterfaceIdiomPhone:
      idiomSpecificField = kIPhoneKey;
      break;
    default:
      break;
  }
  if (idiomSpecificField) {
    [FBSDKTypeUtility array:fields addObject:idiomSpecificField];
  }
  NSString *path = [NSString stringWithFormat:@"?fields=%@.fields(%@)&ids=%@",
                    kAppLinksKey,
                    [fields componentsJoinedByString:@","],
                    [toFindStrings componentsJoinedByString:@","]];
  FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:path
                                                                 parameters:nil
                                                                      flags:FBSDKGraphRequestFlagDoNotInvalidateTokenOnError | FBSDKGraphRequestFlagDisableErrorRecovery];

  [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
    if (error) {
      handler(@{}, error);
      return;
    }
    for (NSURL *url in toFind) {
      id nestedObject = result[url.absoluteString][kAppLinksKey];
      NSMutableArray *rawTargets = [NSMutableArray array];
      if (idiomSpecificField) {
        [rawTargets addObjectsFromArray:nestedObject[idiomSpecificField]];
      }
      [rawTargets addObjectsFromArray:nestedObject[kIOSKey]];

      NSMutableArray<FBSDKAppLinkTarget *> *targets = [NSMutableArray arrayWithCapacity:rawTargets.count];
      for (id rawTarget in rawTargets) {
        [FBSDKTypeUtility array:targets addObject:[FBSDKAppLinkTarget appLinkTargetWithURL:[NSURL URLWithString:rawTarget[kURLKey]]
<<<<<<< HEAD
                                                         appStoreId:rawTarget[kIOSAppStoreIdKey]
                                                            appName:rawTarget[kIOSAppNameKey]]];
=======
                                                                                appStoreId:rawTarget[kIOSAppStoreIdKey]
                                                                                   appName:rawTarget[kIOSAppNameKey]]];
>>>>>>> origin/develop12
      }

      id webTarget = nestedObject[kWebKey];
      NSString *webFallbackString = webTarget[kURLKey];
      NSURL *fallbackUrl = webFallbackString ? [NSURL URLWithString:webFallbackString] : url;

      NSNumber *shouldFallback = webTarget[kShouldFallbackKey];
      if (shouldFallback != nil && !shouldFallback.boolValue) {
        fallbackUrl = nil;
      }

      FBSDKAppLink *link = [FBSDKAppLink appLinkWithSourceURL:url
                                                      targets:targets
                                                       webURL:fallbackUrl];
<<<<<<< HEAD
      @synchronized (self.cachedFBSDKAppLinks) {
=======
      @synchronized(self.cachedFBSDKAppLinks) {
>>>>>>> origin/develop12
        [FBSDKTypeUtility dictionary:self.cachedFBSDKAppLinks setObject:link forKey:url];
      }
      [FBSDKTypeUtility dictionary:appLinks setObject:link forKey:url];
    }
    handler(appLinks, nil);
  }];
}

<<<<<<< HEAD
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
=======
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wdeprecated-declarations"
>>>>>>> origin/develop12
+ (instancetype)resolver
{
  return [[self alloc] initWithUserInterfaceIdiom:UI_USER_INTERFACE_IDIOM()];
}
<<<<<<< HEAD
#pragma clang diagnostic pop
=======

 #pragma clang diagnostic pop
>>>>>>> origin/develop12

@end

#endif
