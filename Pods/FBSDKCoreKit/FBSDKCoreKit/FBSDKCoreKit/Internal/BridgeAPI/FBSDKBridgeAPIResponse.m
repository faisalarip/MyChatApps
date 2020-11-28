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
#import "FBSDKBridgeAPIResponse.h"

#import "FBSDKBridgeAPIProtocol.h"
#import "FBSDKBridgeAPIProtocolType.h"
#import "FBSDKBridgeAPIRequest+Private.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKTypeUtility.h"
=======
 #import "FBSDKBridgeAPIResponse.h"

 #import "FBSDKBridgeAPIProtocol.h"
 #import "FBSDKBridgeAPIProtocolType.h"
 #import "FBSDKBridgeAPIRequest+Private.h"
 #import "FBSDKInternalUtility.h"
>>>>>>> origin/develop12

@interface FBSDKBridgeAPIResponse ()
- (instancetype)initWithRequest:(FBSDKBridgeAPIRequest *)request
             responseParameters:(NSDictionary *)responseParameters
                      cancelled:(BOOL)cancelled
                          error:(NSError *)error
<<<<<<< HEAD
NS_DESIGNATED_INITIALIZER;
=======
  NS_DESIGNATED_INITIALIZER;
>>>>>>> origin/develop12
@end

@implementation FBSDKBridgeAPIResponse

<<<<<<< HEAD
#pragma mark - Class Methods
=======
 #pragma mark - Class Methods
>>>>>>> origin/develop12

+ (instancetype)bridgeAPIResponseWithRequest:(FBSDKBridgeAPIRequest *)request error:(NSError *)error
{
  return [[self alloc] initWithRequest:request
                    responseParameters:nil
                             cancelled:NO
                                 error:error];
}

+ (instancetype)bridgeAPIResponseWithRequest:(FBSDKBridgeAPIRequest *)request
                                 responseURL:(NSURL *)responseURL
                           sourceApplication:(NSString *)sourceApplication
                                       error:(NSError *__autoreleasing *)errorRef
{
  FBSDKBridgeAPIProtocolType protocolType = request.protocolType;
  if (@available(iOS 13.0, *)) {
    // SourceApplication is not available in iOS 13.
    // https://forums.developer.apple.com/thread/119118
  } else {
    switch (protocolType) {
<<<<<<< HEAD
      case FBSDKBridgeAPIProtocolTypeNative:{
=======
      case FBSDKBridgeAPIProtocolTypeNative: {
>>>>>>> origin/develop12
        if (![FBSDKInternalUtility isFacebookBundleIdentifier:sourceApplication]) {
          return nil;
        }
        break;
      }
<<<<<<< HEAD
      case FBSDKBridgeAPIProtocolTypeWeb:{
=======
      case FBSDKBridgeAPIProtocolTypeWeb: {
>>>>>>> origin/develop12
        if (![FBSDKInternalUtility isSafariBundleIdentifier:sourceApplication]) {
          return nil;
        }
        break;
      }
    }
  }
  NSDictionary<NSString *, NSString *> *const queryParameters = [FBSDKBasicUtility dictionaryWithQueryString:responseURL.query];
  if (!queryParameters) {
    return nil;
  }
  id<FBSDKBridgeAPIProtocol> protocol = request.protocol;
  BOOL cancelled;
  NSError *error;
  NSDictionary *responseParameters = [protocol responseParametersForActionID:request.actionID
                                                             queryParameters:queryParameters
                                                                   cancelled:&cancelled
                                                                       error:&error];
  if (errorRef != NULL) {
    *errorRef = error;
  }
  if (!responseParameters) {
    return nil;
  }
  return [[self alloc] initWithRequest:request
                    responseParameters:responseParameters
                             cancelled:cancelled
                                 error:error];
}

+ (instancetype)bridgeAPIResponseCancelledWithRequest:(FBSDKBridgeAPIRequest *)request
{
  return [[self alloc] initWithRequest:request
                    responseParameters:nil
                             cancelled:YES
                                 error:nil];
}

<<<<<<< HEAD
#pragma mark - Object Lifecycle
=======
 #pragma mark - Object Lifecycle
>>>>>>> origin/develop12

- (instancetype)initWithRequest:(FBSDKBridgeAPIRequest *)request
             responseParameters:(NSDictionary *)responseParameters
                      cancelled:(BOOL)cancelled
                          error:(NSError *)error
{
  if ((self = [super init])) {
    _request = [request copy];
    _responseParameters = [responseParameters copy];
    _cancelled = cancelled;
    _error = [error copy];
  }
  return self;
}

<<<<<<< HEAD
#pragma mark - NSCopying
=======
 #pragma mark - NSCopying
>>>>>>> origin/develop12

- (id)copyWithZone:(NSZone *)zone
{
  return self;
}

@end

#endif
