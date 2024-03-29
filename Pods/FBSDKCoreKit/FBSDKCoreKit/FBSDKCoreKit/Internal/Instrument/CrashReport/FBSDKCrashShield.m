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

#import "FBSDKCrashShield.h"

#import "FBSDKFeatureManager.h"
#import "FBSDKGraphRequest.h"
#import "FBSDKGraphRequestConnection.h"
<<<<<<< HEAD
#import "FBSDKSettings.h"
#import "FBSDKSettings+Internal.h"
#import "FBSDKTypeUtility.h"
=======
#import "FBSDKInternalUtility.h"
#import "FBSDKSettings.h"
#import "FBSDKSettings+Internal.h"
>>>>>>> origin/develop12

@implementation FBSDKCrashShield

static NSDictionary<NSString *, NSArray<NSString *> *> *_featureMapping;

+ (void)initialize
{
  if (self == [FBSDKCrashShield class]) {
    _featureMapping =
    @{
      @"AAM" : @[
<<<<<<< HEAD
          @"FBSDKMetadataIndexer",
      ],
      @"CodelessEvents" : @[
          @"FBSDKCodelessIndexer",
          @"FBSDKEventBinding",
          @"FBSDKEventBindingManager",
          @"FBSDKViewHierarchy",
          @"FBSDKCodelessPathComponent",
          @"FBSDKCodelessParameterComponent",
      ],
      @"RestrictiveDataFiltering" : @[
          @"FBSDKRestrictiveDataFilterManager",
      ],
      @"ErrorReport" : @[
          @"FBSDKErrorReport",
      ],
      @"PrivacyProtection" : @[
          @"FBSDKModelManager",
      ],
      @"SuggestedEvents" : @[
          @"FBSDKSuggestedEventsIndexer",
          @"FBSDKFeatureExtractor",
      ],
      @"IntelligentIntegrity" : @[
          @"FBSDKIntegrityManager",
      ],
      @"EventDeactivation" : @[
          @"FBSDKEventDeactivationManager",
      ],
      @"Monitoring" : @[
          @"FBSDKMonitor",
=======
        @"FBSDKMetadataIndexer",
      ],
      @"CodelessEvents" : @[
        @"FBSDKCodelessIndexer",
        @"FBSDKEventBinding",
        @"FBSDKEventBindingManager",
        @"FBSDKViewHierarchy",
        @"FBSDKCodelessPathComponent",
        @"FBSDKCodelessParameterComponent",
      ],
      @"RestrictiveDataFiltering" : @[
        @"FBSDKRestrictiveDataFilterManager",
      ],
      @"ErrorReport" : @[
        @"FBSDKErrorReport",
      ],
      @"PrivacyProtection" : @[
        @"FBSDKModelManager",
      ],
      @"SuggestedEvents" : @[
        @"FBSDKSuggestedEventsIndexer",
        @"FBSDKFeatureExtractor",
      ],
      @"IntelligentIntegrity" : @[
        @"FBSDKIntegrityManager",
      ],
      @"EventDeactivation" : @[
        @"FBSDKEventDeactivationManager",
      ],
      @"SKAdNetworkConversionValue" : @[
        @"FBSDKSKAdNetworkReporter",
        @"FBSDKSKAdNetworkConversionConfiguration",
        @"FBSDKSKAdNetworkRule",
        @"FBSDKSKAdNetworkEvent",
      ],
      @"Monitoring" : @[
        @"FBSDKMonitor",
>>>>>>> origin/develop12
      ],
    };
  }
}

+ (void)analyze:(NSArray<NSDictionary<NSString *, id> *> *)crashLogs
{
  NSMutableSet<NSString *> *disabledFeatues = [NSMutableSet set];
  for (NSDictionary<NSString *, id> *crashLog in crashLogs) {
    NSArray<NSString *> *callstack = crashLog[@"callstack"];
    NSString *featureName = [self getFeature:callstack];
<<<<<<< HEAD
      if (featureName) {
        [FBSDKFeatureManager disableFeature:featureName];
        [disabledFeatues addObject:featureName];
        continue;
      }
=======
    if (featureName) {
      [FBSDKFeatureManager disableFeature:featureName];
      [disabledFeatues addObject:featureName];
      continue;
    }
>>>>>>> origin/develop12
  }
  if ([FBSDKSettings isDataProcessingRestricted]) {
    return;
  }
  if (disabledFeatues.count > 0) {
<<<<<<< HEAD
    NSDictionary<NSString *, id> *disabledFeatureLog = @{@"feature_names":[disabledFeatues allObjects],
                                                         @"timestamp":[NSString stringWithFormat:@"%.0lf", [[NSDate date] timeIntervalSince1970]],
    };
=======
    NSDictionary<NSString *, id> *disabledFeatureLog = @{@"feature_names" : [disabledFeatues allObjects],
                                                         @"timestamp" : [NSString stringWithFormat:@"%.0lf", [[NSDate date] timeIntervalSince1970]], };
>>>>>>> origin/develop12
    NSData *jsonData = [FBSDKTypeUtility dataWithJSONObject:disabledFeatureLog options:0 error:nil];
    if (jsonData) {
      NSString *disabledFeatureReport = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
      if (disabledFeatureReport) {
        FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:[NSString stringWithFormat:@"%@/instruments", [FBSDKSettings appID]]
<<<<<<< HEAD
                                                                       parameters:@{@"crash_shield":disabledFeatureReport}
=======
                                                                       parameters:@{@"crash_shield" : disabledFeatureReport}
>>>>>>> origin/develop12
                                                                       HTTPMethod:FBSDKHTTPMethodPOST];

        [request startWithCompletionHandler:nil];
      }
    }
  }
}

+ (nullable NSString *)getFeature:(NSArray<NSString *> *)callstack
{
  NSArray<NSString *> *featureNames = _featureMapping.allKeys;
  for (NSString *entry in callstack) {
    NSString *className = [self getClassName:entry];
    for (NSString *featureName in featureNames) {
      NSArray<NSString *> *classArray = [FBSDKTypeUtility dictionary:_featureMapping objectForKey:featureName ofType:NSObject.class];
      if (className && [classArray containsObject:className]) {
        return featureName;
      }
    }
  }
  return nil;
}

+ (nullable NSString *)getClassName:(NSString *)entry
{
  NSArray<NSString *> *items = [entry componentsSeparatedByString:@" "];
  NSString *className = nil;
  // parse class name only from an entry in format "-[className functionName]+offset"
  // or "+[className functionName]+offset"
  if (items.count > 0 && ([[FBSDKTypeUtility array:items objectAtIndex:0] hasPrefix:@"+["] || [[FBSDKTypeUtility array:items objectAtIndex:0] hasPrefix:@"-["])) {
    className = [[FBSDKTypeUtility array:items objectAtIndex:0] substringFromIndex:2];
  }
  return className;
}

@end
