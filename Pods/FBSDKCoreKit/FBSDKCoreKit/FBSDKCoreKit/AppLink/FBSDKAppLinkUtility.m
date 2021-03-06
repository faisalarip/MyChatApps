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
#import "FBSDKAppLinkUtility.h"

#import "FBSDKAppEventsUtility.h"
#import "FBSDKGraphRequest.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKSettings.h"
#import "FBSDKURL.h"
#import "FBSDKUtility.h"
=======
 #import "FBSDKAppLinkUtility.h"

 #import "FBSDKAppEventsUtility.h"
 #import "FBSDKGraphRequest.h"
 #import "FBSDKInternalUtility.h"
 #import "FBSDKSettings.h"
 #import "FBSDKURL.h"
 #import "FBSDKUtility.h"
>>>>>>> origin/develop12

static NSString *const FBSDKLastDeferredAppLink = @"com.facebook.sdk:lastDeferredAppLink%@";
static NSString *const FBSDKDeferredAppLinkEvent = @"DEFERRED_APP_LINK";

<<<<<<< HEAD
@implementation FBSDKAppLinkUtility {}
=======
@implementation FBSDKAppLinkUtility
{}
>>>>>>> origin/develop12

+ (void)fetchDeferredAppLink:(FBSDKURLBlock)handler
{
  NSAssert([NSThread isMainThread], @"FBSDKAppLink fetchDeferredAppLink: must be invoked from main thread.");

<<<<<<< HEAD
=======
  if ([FBSDKAppEventsUtility shouldDropAppEvent]) {
    if (handler) {
      NSError *error = [[NSError alloc] initWithDomain:@"AdvertiserTrackingEnabled must be enabled" code:-1 userInfo:nil];
      handler(nil, error);
    }
    return;
  }

>>>>>>> origin/develop12
  NSString *appID = [FBSDKSettings appID];

  // Deferred app links are only currently used for engagement ads, thus we consider the app to be an advertising one.
  // If this is considered for organic, non-ads scenarios, we'll need to retrieve the FBAppEventsUtility.shouldAccessAdvertisingID
  // before we make this call.
  NSMutableDictionary *deferredAppLinkParameters =
  [FBSDKAppEventsUtility activityParametersDictionaryForEvent:FBSDKDeferredAppLinkEvent
                                    shouldAccessAdvertisingID:YES];

  FBSDKGraphRequest *deferredAppLinkRequest = [[FBSDKGraphRequest alloc] initWithGraphPath:[NSString stringWithFormat:@"%@/activities", appID, nil]
                                                                                parameters:deferredAppLinkParameters
                                                                               tokenString:nil
                                                                                   version:nil
                                                                                HTTPMethod:FBSDKHTTPMethodPOST];

  [deferredAppLinkRequest startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection,
                                                       id result,
                                                       NSError *error) {
<<<<<<< HEAD
    NSURL *applinkURL = nil;
    if (!error) {
      NSString *appLinkString = result[@"applink_url"];
      if (appLinkString) {
        applinkURL = [NSURL URLWithString:appLinkString];

        NSString *createTimeUtc = result[@"click_time"];
        if (createTimeUtc) {
          // append/translate the create_time_utc so it can be used by clients
          NSString *modifiedURLString = [applinkURL.absoluteString
                                         stringByAppendingFormat:@"%@fb_click_time_utc=%@",
                                         (applinkURL.query) ? @"&" : @"?" ,
                                         createTimeUtc];
          applinkURL = [NSURL URLWithString:modifiedURLString];
        }
      }
    }

    if (handler) {
      dispatch_async(dispatch_get_main_queue(), ^{
        handler(applinkURL, error);
      });
    }
  }];
=======
                                                         NSURL *applinkURL = nil;
                                                         if (!error) {
                                                           NSString *appLinkString = result[@"applink_url"];
                                                           if (appLinkString) {
                                                             applinkURL = [NSURL URLWithString:appLinkString];

                                                             NSString *createTimeUtc = result[@"click_time"];
                                                             if (createTimeUtc) {
                                                               // append/translate the create_time_utc so it can be used by clients
                                                               NSString *modifiedURLString = [applinkURL.absoluteString
                                                                                              stringByAppendingFormat:@"%@fb_click_time_utc=%@",
                                                                                              (applinkURL.query) ? @"&" : @"?",
                                                                                              createTimeUtc];
                                                               applinkURL = [NSURL URLWithString:modifiedURLString];
                                                             }
                                                           }
                                                         }

                                                         if (handler) {
                                                           dispatch_async(dispatch_get_main_queue(), ^{
                                                             handler(applinkURL, error);
                                                           });
                                                         }
                                                       }];
>>>>>>> origin/develop12
}

+ (NSString *)appInvitePromotionCodeFromURL:(NSURL *)url
{
  FBSDKURL *parsedUrl = [FBSDKURL URLWithURL:url];
  NSDictionary *extras = parsedUrl.appLinkExtras;
  if (extras) {
    NSString *deeplinkContextString = extras[@"deeplink_context"];

    // Parse deeplinkContext and extract promo code
    if (deeplinkContextString.length > 0) {
      NSError *error = nil;
      NSDictionary<id, id> *deeplinkContextData = [FBSDKBasicUtility objectForJSONString:deeplinkContextString error:&error];
      if (!error && [deeplinkContextData isKindOfClass:[NSDictionary class]]) {
        return deeplinkContextData[@"promo_code"];
      }
    }
  }

  return nil;
<<<<<<< HEAD

=======
>>>>>>> origin/develop12
}

+ (BOOL)isMatchURLScheme:(NSString *)scheme
{
  if (!scheme) {
    return NO;
  }
<<<<<<< HEAD
  for(NSDictionary *urlType in [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleURLTypes"])
  {
    for(NSString *urlScheme in urlType[@"CFBundleURLSchemes"]) {
      if([urlScheme caseInsensitiveCompare:scheme] == NSOrderedSame) {
=======
  for (NSDictionary *urlType in [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleURLTypes"]) {
    for (NSString *urlScheme in urlType[@"CFBundleURLSchemes"]) {
      if ([urlScheme caseInsensitiveCompare:scheme] == NSOrderedSame) {
>>>>>>> origin/develop12
        return YES;
      }
    }
  }
  return NO;
}

@end

#endif
