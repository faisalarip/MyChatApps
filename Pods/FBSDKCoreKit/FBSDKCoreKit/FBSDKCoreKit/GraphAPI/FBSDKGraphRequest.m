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

#import "FBSDKGraphRequest+Internal.h"

#import <UIKit/UIKit.h>

#import "FBSDKAccessToken.h"
#import "FBSDKCoreKit.h"
#import "FBSDKGraphRequestConnection.h"
#import "FBSDKGraphRequestDataAttachment.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKLogger.h"
#import "FBSDKSettings+Internal.h"

// constants
FBSDKHTTPMethod FBSDKHTTPMethodGET = @"GET";
FBSDKHTTPMethod FBSDKHTTPMethodPOST = @"POST";
FBSDKHTTPMethod FBSDKHTTPMethodDELETE = @"DELETE";

<<<<<<< HEAD
@interface FBSDKGraphRequest()
@property (nonatomic, assign) FBSDKGraphRequestFlags flags;
@property (nonatomic, copy, readwrite) FBSDKHTTPMethod HTTPMethod;
=======
@interface FBSDKGraphRequest ()
@property (nonatomic, assign) FBSDKGraphRequestFlags flags;
@property (nonatomic, readwrite, copy) FBSDKHTTPMethod HTTPMethod;
>>>>>>> origin/develop12
@end

@implementation FBSDKGraphRequest

@synthesize HTTPMethod;

<<<<<<< HEAD
- (instancetype)initWithGraphPath:(NSString *)graphPath {
=======
- (instancetype)initWithGraphPath:(NSString *)graphPath
{
>>>>>>> origin/develop12
  return [self initWithGraphPath:graphPath parameters:@{}];
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
<<<<<<< HEAD
                       HTTPMethod:(FBSDKHTTPMethod)method {
=======
                       HTTPMethod:(FBSDKHTTPMethod)method
{
>>>>>>> origin/develop12
  return [self initWithGraphPath:graphPath parameters:@{} HTTPMethod:method];
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
<<<<<<< HEAD
                       parameters:(NSDictionary *)parameters {
=======
                       parameters:(NSDictionary *)parameters
{
>>>>>>> origin/develop12
  return [self initWithGraphPath:graphPath
                      parameters:parameters
                           flags:FBSDKGraphRequestFlagNone];
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
                       parameters:(NSDictionary *)parameters
<<<<<<< HEAD
                       HTTPMethod:(FBSDKHTTPMethod)method {
=======
                       HTTPMethod:(FBSDKHTTPMethod)method
{
>>>>>>> origin/develop12
  return [self initWithGraphPath:graphPath
                      parameters:parameters
                     tokenString:[FBSDKAccessToken currentAccessToken].tokenString
                         version:nil
                      HTTPMethod:method];
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
                       parameters:(NSDictionary *)parameters
<<<<<<< HEAD
                            flags:(FBSDKGraphRequestFlags)flags {
=======
                            flags:(FBSDKGraphRequestFlags)flags
{
>>>>>>> origin/develop12
  return [self initWithGraphPath:graphPath
                      parameters:parameters
                     tokenString:[FBSDKAccessToken currentAccessToken].tokenString
                      HTTPMethod:FBSDKHTTPMethodGET
                           flags:flags];
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
                       parameters:(NSDictionary *)parameters
                      tokenString:(NSString *)tokenString
                       HTTPMethod:(FBSDKHTTPMethod)method
<<<<<<< HEAD
                            flags:(FBSDKGraphRequestFlags)flags {
=======
                            flags:(FBSDKGraphRequestFlags)flags
{
>>>>>>> origin/develop12
  if ((self = [self initWithGraphPath:graphPath
                           parameters:parameters
                          tokenString:tokenString
                              version:[FBSDKSettings graphAPIVersion]
                           HTTPMethod:method])) {
    self.flags |= flags;
  }
  return self;
}

- (instancetype)initWithGraphPath:(NSString *)graphPath
                       parameters:(NSDictionary *)parameters
                      tokenString:(NSString *)tokenString
                          version:(NSString *)version
<<<<<<< HEAD
                       HTTPMethod:(FBSDKHTTPMethod)method {
=======
                       HTTPMethod:(FBSDKHTTPMethod)method
{
>>>>>>> origin/develop12
  if ((self = [super init])) {
    _tokenString = tokenString ? [tokenString copy] : nil;
    _version = version ? [version copy] : [FBSDKSettings graphAPIVersion];
    _graphPath = [graphPath copy];
    self.HTTPMethod = method.length > 0 ? [method copy] : FBSDKHTTPMethodGET;
    _parameters = parameters ?: @{};
    if (!FBSDKSettings.isGraphErrorRecoveryEnabled) {
      _flags = FBSDKGraphRequestFlagDisableErrorRecovery;
    }
  }
  return self;
}

- (BOOL)isGraphErrorRecoveryDisabled
{
  return (self.flags & FBSDKGraphRequestFlagDisableErrorRecovery);
}

- (void)setGraphErrorRecoveryDisabled:(BOOL)disable
{
  if (disable) {
    self.flags |= FBSDKGraphRequestFlagDisableErrorRecovery;
  } else {
    self.flags &= ~FBSDKGraphRequestFlagDisableErrorRecovery;
  }
}

- (BOOL)hasAttachments
{
  __block BOOL hasAttachments = NO;
  [FBSDKTypeUtility dictionary:self.parameters enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
    if ([FBSDKGraphRequest isAttachment:obj]) {
      hasAttachments = YES;
      *stop = YES;
    }
  }];
  return hasAttachments;
}

+ (BOOL)isAttachment:(id)item
{
<<<<<<< HEAD
  return ([item isKindOfClass:[UIImage class]] ||
          [item isKindOfClass:[NSData class]] ||
          [item isKindOfClass:[FBSDKGraphRequestDataAttachment class]]);
}


+ (NSString *)serializeURL:(NSString *)baseUrl
                    params:(NSDictionary *)params {
=======
  return ([item isKindOfClass:[UIImage class]]
    || [item isKindOfClass:[NSData class]]
    || [item isKindOfClass:[FBSDKGraphRequestDataAttachment class]]);
}

+ (NSString *)serializeURL:(NSString *)baseUrl
                    params:(NSDictionary *)params
{
>>>>>>> origin/develop12
  return [self serializeURL:baseUrl params:params httpMethod:FBSDKHTTPMethodGET];
}

+ (NSString *)serializeURL:(NSString *)baseUrl
                    params:(NSDictionary *)params
<<<<<<< HEAD
                httpMethod:(NSString *)httpMethod {
=======
                httpMethod:(NSString *)httpMethod
{
>>>>>>> origin/develop12
  return [self serializeURL:baseUrl params:params httpMethod:httpMethod forBatch:NO];
}

+ (NSString *)serializeURL:(NSString *)baseUrl
                    params:(NSDictionary *)params
                httpMethod:(NSString *)httpMethod
<<<<<<< HEAD
                  forBatch:(BOOL)forBatch {
  params = [self preprocessParams: params];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  NSURL *parsedURL = [NSURL URLWithString:[baseUrl stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
#pragma clang pop
=======
                  forBatch:(BOOL)forBatch
{
  params = [self preprocessParams:params];

  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
  NSURL *parsedURL = [NSURL URLWithString:[baseUrl stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
  #pragma clang pop
>>>>>>> origin/develop12

  if ([httpMethod isEqualToString:FBSDKHTTPMethodPOST] && !forBatch) {
    return baseUrl;
  }

  NSString *queryPrefix = parsedURL.query ? @"&" : @"?";

<<<<<<< HEAD
  NSString *query = [FBSDKBasicUtility queryStringWithDictionary:params error:NULL invalidObjectHandler:^id(id object, BOOL *stop) {
=======
  NSString *query = [FBSDKBasicUtility queryStringWithDictionary:params error:NULL invalidObjectHandler:^id (id object, BOOL *stop) {
>>>>>>> origin/develop12
    if ([self isAttachment:object]) {
      if ([httpMethod isEqualToString:FBSDKHTTPMethodGET]) {
        [FBSDKLogger singleShotLogEntry:FBSDKLoggingBehaviorDeveloperErrors logEntry:@"can not use GET to upload a file"];
      }
      return nil;
    }
    return object;
  }];
  return [NSString stringWithFormat:@"%@%@%@", baseUrl, queryPrefix, query];
}

+ (NSDictionary *)preprocessParams:(NSDictionary *)params
{
  NSString *debugValue = [FBSDKSettings graphAPIDebugParamValue];
  if (debugValue) {
    NSMutableDictionary *mutableParams = [NSMutableDictionary dictionaryWithDictionary:params];
    [FBSDKTypeUtility dictionary:mutableParams setObject:debugValue forKey:@"debug"];
    return mutableParams;
  }

  return params;
}

- (FBSDKGraphRequestConnection *)startWithCompletionHandler:(FBSDKGraphRequestBlock)handler
{
  FBSDKGraphRequestConnection *connection = [[FBSDKGraphRequestConnection alloc] init];
  [connection addRequest:self completionHandler:handler];
  [connection start];
  return connection;
}

#pragma mark - Debugging helpers

- (NSString *)description
{
  NSMutableString *result = [NSMutableString stringWithFormat:@"<%@: %p",
                             NSStringFromClass([self class]),
                             self];
  if (self.graphPath) {
    [result appendFormat:@", graphPath: %@", self.graphPath];
  }
  if (self.HTTPMethod) {
    [result appendFormat:@", HTTPMethod: %@", self.HTTPMethod];
  }
  [result appendFormat:@", parameters: %@>", self.parameters.description];
  return result;
}

@end
