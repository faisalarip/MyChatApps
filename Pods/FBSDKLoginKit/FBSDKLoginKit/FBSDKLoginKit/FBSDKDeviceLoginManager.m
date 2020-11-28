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

#import "FBSDKDeviceLoginManager.h"
<<<<<<< HEAD
#import "FBSDKDeviceLoginManagerResult+Internal.h"

#ifdef FBSDKCOCOAPODS
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKCoreKit/FBSDKCoreKit+Internal.h>
#else
#import "FBSDKCoreKit+Internal.h"
=======

#import "FBSDKDeviceLoginManagerResult+Internal.h"

#ifdef FBSDKCOCOAPODS
 #import <FBSDKCoreKit/FBSDKCoreKit.h>
 #import <FBSDKCoreKit/FBSDKCoreKit+Internal.h>
#else
 #import "FBSDKCoreKit+Internal.h"
>>>>>>> origin/develop12
#endif

#import "FBSDKDeviceLoginCodeInfo+Internal.h"
#import "FBSDKLoginConstants.h"

static NSMutableArray<FBSDKDeviceLoginManager *> *g_loginManagerInstances;

<<<<<<< HEAD
@implementation FBSDKDeviceLoginManager {
  FBSDKDeviceLoginCodeInfo *_codeInfo;
  BOOL _isCancelled;
  NSNetService * _loginAdvertisementService;
=======
@implementation FBSDKDeviceLoginManager
{
  FBSDKDeviceLoginCodeInfo *_codeInfo;
  BOOL _isCancelled;
  NSNetService *_loginAdvertisementService;
>>>>>>> origin/develop12
  BOOL _isSmartLoginEnabled;
}

+ (void)initialize
{
  if (self == [FBSDKDeviceLoginManager class]) {
    g_loginManagerInstances = [NSMutableArray array];
  }
}

- (instancetype)initWithPermissions:(NSArray<NSString *> *)permissions enableSmartLogin:(BOOL)enableSmartLogin
{
  if ((self = [super init])) {
    _permissions = [permissions copy];
    _isSmartLoginEnabled = enableSmartLogin;
  }
  return self;
}

- (void)start
{
  [FBSDKInternalUtility validateAppID];
  [FBSDKTypeUtility array:g_loginManagerInstances addObject:self];

  NSDictionary *parameters = @{
<<<<<<< HEAD
                               @"scope": [self.permissions componentsJoinedByString:@","] ?: @"",
                               @"redirect_uri": self.redirectURL.absoluteString ?: @"",
                               FBSDK_DEVICE_INFO_PARAM: [FBSDKDeviceRequestsHelper getDeviceInfo],
                               };
=======
    @"scope" : [self.permissions componentsJoinedByString:@","] ?: @"",
    @"redirect_uri" : self.redirectURL.absoluteString ?: @"",
    FBSDK_DEVICE_INFO_PARAM : [FBSDKDeviceRequestsHelper getDeviceInfo],
  };
>>>>>>> origin/develop12
  FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:@"device/login"
                                                                 parameters:parameters
                                                                tokenString:[FBSDKInternalUtility validateRequiredClientAccessToken]
                                                                 HTTPMethod:@"POST"
                                                                      flags:FBSDKGraphRequestFlagNone];
  [request setGraphErrorRecoveryDisabled:YES];
  [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
    if (error) {
      [self _processError:error];
      return;
    }

    self->_codeInfo = [[FBSDKDeviceLoginCodeInfo alloc]
<<<<<<< HEAD
                                          initWithIdentifier:result[@"code"]
                                          loginCode:result[@"user_code"]
                                          verificationURL:[NSURL URLWithString:result[@"verification_uri"]]
                                          expirationDate:[[NSDate date] dateByAddingTimeInterval:[result[@"expires_in"] doubleValue]]
                                          pollingInterval:[result[@"interval"] integerValue]];
=======
                       initWithIdentifier:result[@"code"]
                       loginCode:result[@"user_code"]
                       verificationURL:[NSURL URLWithString:result[@"verification_uri"]]
                       expirationDate:[[NSDate date] dateByAddingTimeInterval:[result[@"expires_in"] doubleValue]]
                       pollingInterval:[result[@"interval"] integerValue]];
>>>>>>> origin/develop12

    if (self->_isSmartLoginEnabled) {
      [FBSDKDeviceRequestsHelper startAdvertisementService:self->_codeInfo.loginCode
                                              withDelegate:self
      ];
    }

    [self.delegate deviceLoginManager:self startedWithCodeInfo:self->_codeInfo];
    [self _schedulePoll:self->_codeInfo.pollingInterval];
  }];
<<<<<<< HEAD
 }
=======
}
>>>>>>> origin/develop12

- (void)cancel
{
  [FBSDKDeviceRequestsHelper cleanUpAdvertisementService:self];
  _isCancelled = YES;
  [g_loginManagerInstances removeObject:self];
}

#pragma mark - Private impl

- (void)_notifyError:(NSError *)error
{
  [FBSDKDeviceRequestsHelper cleanUpAdvertisementService:self];
  [self.delegate deviceLoginManager:self
                completedWithResult:nil
                              error:error];
  [g_loginManagerInstances removeObject:self];
}

<<<<<<< HEAD
- (void)_notifyToken:(NSString *)tokenString
{
  [FBSDKDeviceRequestsHelper cleanUpAdvertisementService:self];
  void(^completeWithResult)(FBSDKDeviceLoginManagerResult *) = ^(FBSDKDeviceLoginManagerResult *result) {
=======
- (void)_notifyToken:(NSString *)tokenString withExpirationDate:(NSDate *)expirationDate withDataAccessExpirationDate:(NSDate *)dataAccessExpirationDate
{
  [FBSDKDeviceRequestsHelper cleanUpAdvertisementService:self];
  void (^completeWithResult)(FBSDKDeviceLoginManagerResult *) = ^(FBSDKDeviceLoginManagerResult *result) {
>>>>>>> origin/develop12
    [self.delegate deviceLoginManager:self completedWithResult:result error:nil];
    [g_loginManagerInstances removeObject:self];
  };

  if (tokenString) {
    FBSDKGraphRequest *permissionsRequest =
    [[FBSDKGraphRequest alloc] initWithGraphPath:@"me"
<<<<<<< HEAD
                                      parameters:@{@"fields": @"id,permissions"}
=======
                                      parameters:@{@"fields" : @"id,permissions"}
>>>>>>> origin/develop12
                                     tokenString:tokenString
                                      HTTPMethod:@"GET"
                                           flags:FBSDKGraphRequestFlagDisableErrorRecovery];
    [permissionsRequest startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id permissionRawResult, NSError *error) {
      NSString *userID = permissionRawResult[@"id"];
      NSDictionary *permissionResult = permissionRawResult[@"permissions"];
<<<<<<< HEAD
      if (error ||
          !userID ||
          !permissionResult) {
#if TARGET_TV_OS
=======
      if (error
          || !userID
          || !permissionResult) {
      #if TARGET_TV_OS
>>>>>>> origin/develop12
        NSError *wrappedError = [FBSDKError errorWithDomain:FBSDKShareErrorDomain
                                                       code:FBSDKErrorTVOSUnknown
                                                    message:@"Unable to fetch permissions for token"
                                            underlyingError:error];
<<<<<<< HEAD
#else
=======
      #else
>>>>>>> origin/develop12
        NSError *wrappedError = [FBSDKError errorWithDomain:FBSDKLoginErrorDomain
                                                       code:FBSDKErrorUnknown
                                                    message:@"Unable to fetch permissions for token"
                                            underlyingError:error];
<<<<<<< HEAD
#endif
=======
      #endif
>>>>>>> origin/develop12
        [self _notifyError:wrappedError];
      } else {
        NSMutableSet<NSString *> *permissions = [NSMutableSet set];
        NSMutableSet<NSString *> *declinedPermissions = [NSMutableSet set];
        NSMutableSet<NSString *> *expiredPermissions = [NSMutableSet set];

        [FBSDKInternalUtility extractPermissionsFromResponse:permissionResult
                                          grantedPermissions:permissions
                                         declinedPermissions:declinedPermissions
<<<<<<< HEAD
                                         expiredPermissions:expiredPermissions];
=======
                                          expiredPermissions:expiredPermissions];
>>>>>>> origin/develop12
        FBSDKAccessToken *accessToken = [[FBSDKAccessToken alloc] initWithTokenString:tokenString
                                                                          permissions:permissions.allObjects
                                                                  declinedPermissions:declinedPermissions.allObjects
                                                                   expiredPermissions:expiredPermissions.allObjects
                                                                                appID:[FBSDKSettings appID]
                                                                               userID:userID
<<<<<<< HEAD
                                                                       expirationDate:nil
                                                                          refreshDate:nil
                                                             dataAccessExpirationDate:nil
                                                                          graphDomain:nil];
        FBSDKDeviceLoginManagerResult *result = [[FBSDKDeviceLoginManagerResult alloc] initWithToken:accessToken
                                                                                         isCancelled:NO];
=======
                                                                       expirationDate:expirationDate
                                                                          refreshDate:nil
                                                             dataAccessExpirationDate:dataAccessExpirationDate
                                                                          graphDomain:nil];
        FBSDKDeviceLoginManagerResult *result = [[FBSDKDeviceLoginManagerResult alloc] initWithToken:accessToken
                                                                                         isCancelled:NO];
        [FBSDKAccessToken setCurrentAccessToken:accessToken];
>>>>>>> origin/develop12
        completeWithResult(result);
      }
    }];
  } else {
    _isCancelled = YES;
    FBSDKDeviceLoginManagerResult *result = [[FBSDKDeviceLoginManagerResult alloc] initWithToken:nil isCancelled:YES];
    completeWithResult(result);
  }
}

- (void)_processError:(NSError *)error
{
  FBSDKDeviceLoginError code = [error.userInfo[FBSDKGraphRequestErrorGraphErrorSubcodeKey] unsignedIntegerValue];
  switch (code) {
    case FBSDKDeviceLoginErrorAuthorizationPending:
      [self _schedulePoll:_codeInfo.pollingInterval];
      break;
    case FBSDKDeviceLoginErrorCodeExpired:
    case FBSDKDeviceLoginErrorAuthorizationDeclined:
<<<<<<< HEAD
      [self _notifyToken:nil];
=======
      [self _notifyToken:nil withExpirationDate:nil withDataAccessExpirationDate:nil];
>>>>>>> origin/develop12
      break;
    case FBSDKDeviceLoginErrorExcessivePolling:
      [self _schedulePoll:_codeInfo.pollingInterval * 2];
    default:
      [self _notifyError:error];
      break;
  }
}

- (void)_schedulePoll:(NSUInteger)interval
{
<<<<<<< HEAD
  dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
    if (self->_isCancelled) {
      return;
    }

    NSDictionary *parameters = @{ @"code": self->_codeInfo.identifier };
    FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:@"device/login_status"
                                                                   parameters:parameters
                                                                  tokenString:[FBSDKInternalUtility validateRequiredClientAccessToken]
                                                                   HTTPMethod:@"POST"
                                                                        flags:FBSDKGraphRequestFlagNone];
    [request setGraphErrorRecoveryDisabled:YES];
    [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
      if (self->_isCancelled) {
        return;
      }
      if (error) {
        [self _processError:error];
      } else {
        NSString *tokenString = result[@"access_token"];
        if (tokenString) {
          [self _notifyToken:tokenString];
        } else {
          NSError *unknownError = [FBSDKError errorWithDomain:FBSDKLoginErrorDomain
                                                         code:FBSDKErrorUnknown
                                                      message:@"Device Login poll failed. No token nor error was found."];
          [self _notifyError:unknownError];
        }
      }
    }];
  });
=======
  dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(interval * NSEC_PER_SEC)),
    dispatch_get_main_queue(), ^{
      if (self->_isCancelled) {
        return;
      }

      NSDictionary *parameters = @{ @"code" : self->_codeInfo.identifier };
      FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:@"device/login_status"
                                                                     parameters:parameters
                                                                    tokenString:[FBSDKInternalUtility validateRequiredClientAccessToken]
                                                                     HTTPMethod:@"POST"
                                                                          flags:FBSDKGraphRequestFlagNone];
      [request setGraphErrorRecoveryDisabled:YES];
      [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
        if (self->_isCancelled) {
          return;
        }
        if (error) {
          [self _processError:error];
        } else {
          NSString *tokenString = result[@"access_token"];
          NSDate *expirationDate = [NSDate distantFuture];
          if ([result[@"expires_in"] integerValue] > 0) {
            expirationDate = [NSDate dateWithTimeIntervalSinceNow:[result[@"expires_in"] integerValue]];
          }

          NSDate *dataAccessExpirationDate = [NSDate distantFuture];
          if ([result[@"data_access_expiration_time"] integerValue] > 0) {
            dataAccessExpirationDate = [NSDate dateWithTimeIntervalSince1970:[result[@"data_access_expiration_time"] integerValue]];
          }

          if (tokenString) {
            [self _notifyToken:tokenString withExpirationDate:expirationDate withDataAccessExpirationDate:dataAccessExpirationDate];
          } else {
            NSError *unknownError = [FBSDKError errorWithDomain:FBSDKLoginErrorDomain
                                                           code:FBSDKErrorUnknown
                                                        message:@"Device Login poll failed. No token nor error was found."];
            [self _notifyError:unknownError];
          }
        }
      }];
    });
>>>>>>> origin/develop12
}

- (void)netService:(NSNetService *)sender
     didNotPublish:(NSDictionary<NSString *, NSNumber *> *)errorDict
{
  // Only cleanup if the publish error is from our advertising service
<<<<<<< HEAD
  if ([FBSDKDeviceRequestsHelper isDelegate:self forAdvertisementService:sender])
  {
=======
  if ([FBSDKDeviceRequestsHelper isDelegate:self forAdvertisementService:sender]) {
>>>>>>> origin/develop12
    [FBSDKDeviceRequestsHelper cleanUpAdvertisementService:self];
  }
}

@end
