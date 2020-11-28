/*
 * Copyright 2019 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import "FirebaseAuth/Sources/Backend/RPC/MultiFactor/Enroll/FIRStartMFAEnrollmentRequest.h"

#import "FirebaseAuth/Sources/Backend/RPC/Proto/Phone/FIRAuthProtoStartMFAPhoneRequestInfo.h"

static NSString *const kStartMFAEnrollmentEndPoint = @"accounts/mfaEnrollment:start";

<<<<<<< HEAD
@implementation FIRStartMFAEnrollmentRequest

- (nullable instancetype)initWithIDToken:(NSString *)IDToken
                     multiFactorProvider:(NSString *)multiFactorProvider
=======
/** @var kTenantIDKey
    @brief The key for the tenant id value in the request.
 */
static NSString *const kTenantIDKey = @"tenantId";

@implementation FIRStartMFAEnrollmentRequest

- (nullable instancetype)initWithIDToken:(NSString *)IDToken
>>>>>>> origin/develop12
                          enrollmentInfo:(FIRAuthProtoStartMFAPhoneRequestInfo *)enrollmentInfo
                    requestConfiguration:(FIRAuthRequestConfiguration *)requestConfiguration {
  self = [super initWithEndpoint:kStartMFAEnrollmentEndPoint
            requestConfiguration:requestConfiguration
             useIdentityPlatform:YES
                      useStaging:NO];
  if (self) {
    _IDToken = IDToken;
<<<<<<< HEAD
    _multiFactorProvider = multiFactorProvider;
=======
>>>>>>> origin/develop12
    _enrollmentInfo = enrollmentInfo;
  }
  return self;
}

- (nullable id)unencodedHTTPRequestBodyWithError:(NSError *__autoreleasing _Nullable *)error {
  NSMutableDictionary *postBody = [NSMutableDictionary dictionary];
  if (_IDToken) {
    postBody[@"idToken"] = _IDToken;
  }
<<<<<<< HEAD
  if (_multiFactorProvider) {
    postBody[@"mfaProvider"] = _multiFactorProvider;
  }
=======
>>>>>>> origin/develop12
  if (_enrollmentInfo) {
    if ([_enrollmentInfo isKindOfClass:[FIRAuthProtoStartMFAPhoneRequestInfo class]]) {
      postBody[@"phoneEnrollmentInfo"] = [_enrollmentInfo dictionary];
    }
  }
<<<<<<< HEAD
=======
  if (self.tenantID) {
    postBody[kTenantIDKey] = self.tenantID;
  }
>>>>>>> origin/develop12
  return [postBody copy];
}

@end
