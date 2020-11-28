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
#import "FBSDKGraphErrorRecoveryProcessor.h"

#import "FBSDKCoreKit+Internal.h"
#import "FBSDKErrorRecoveryAttempter.h"

@interface FBSDKGraphErrorRecoveryProcessor()
=======
 #import "FBSDKGraphErrorRecoveryProcessor.h"

 #import "FBSDKCoreKit+Internal.h"
 #import "FBSDKErrorRecoveryAttempter.h"

@interface FBSDKGraphErrorRecoveryProcessor ()
>>>>>>> origin/develop12
{
  FBSDKErrorRecoveryAttempter *_recoveryAttempter;
  NSError *_error;
}

<<<<<<< HEAD
@property (nonatomic, strong, nullable) id<FBSDKGraphErrorRecoveryProcessorDelegate>delegate;
=======
@property (nullable, nonatomic, strong) id<FBSDKGraphErrorRecoveryProcessorDelegate> delegate;
>>>>>>> origin/develop12

@end

@implementation FBSDKGraphErrorRecoveryProcessor

- (BOOL)processError:(NSError *)error request:(FBSDKGraphRequest *)request delegate:(id<FBSDKGraphErrorRecoveryProcessorDelegate>)delegate
{
  self.delegate = delegate;
  if ([self.delegate respondsToSelector:@selector(processorWillProcessError:error:)]) {
    if (![self.delegate processorWillProcessError:self error:error]) {
      return NO;
    }
  }

  FBSDKGraphRequestError errorCategory = [error.userInfo[FBSDKGraphRequestErrorKey] unsignedIntegerValue];
  switch (errorCategory) {
<<<<<<< HEAD
    case FBSDKGraphRequestErrorTransient :
      [self.delegate processorDidAttemptRecovery:self didRecover:YES error:nil];
      self.delegate = nil;
      return YES;
    case FBSDKGraphRequestErrorRecoverable :
=======
    case FBSDKGraphRequestErrorTransient:
      [self.delegate processorDidAttemptRecovery:self didRecover:YES error:nil];
      self.delegate = nil;
      return YES;
    case FBSDKGraphRequestErrorRecoverable:
>>>>>>> origin/develop12
      if ([request.tokenString isEqualToString:[FBSDKAccessToken currentAccessToken].tokenString]) {
        _recoveryAttempter = error.recoveryAttempter;

        // Set up a block to do the typical recovery work so that we can chain it for ios auth special cases.
        // the block returns YES if recovery UI is started (meaning we wait for the alertviewdelegate to resume control flow).
<<<<<<< HEAD
        BOOL (^standardRecoveryWork)(void) = ^BOOL{
=======
        BOOL (^standardRecoveryWork)(void) = ^BOOL {
>>>>>>> origin/develop12
          NSArray *recoveryOptionsTitles = error.userInfo[NSLocalizedRecoveryOptionsErrorKey];
          if (recoveryOptionsTitles.count > 0 && self->_recoveryAttempter) {
            NSString *recoverySuggestion = error.userInfo[NSLocalizedRecoverySuggestionErrorKey];
            self->_error = error;
            dispatch_async(dispatch_get_main_queue(), ^{
              [self displayAlertWithRecoverySuggestion:recoverySuggestion recoveryOptionsTitles:recoveryOptionsTitles];
            });
            return YES;
          }
          return NO;
        };

        return standardRecoveryWork();
      }
      return NO;
<<<<<<< HEAD
    case FBSDKGraphRequestErrorOther :
=======
    case FBSDKGraphRequestErrorOther:
>>>>>>> origin/develop12
      if ([request.tokenString isEqualToString:[FBSDKAccessToken currentAccessToken].tokenString]) {
        NSString *message = error.userInfo[FBSDKErrorLocalizedDescriptionKey];
        NSString *title = error.userInfo[FBSDKErrorLocalizedTitleKey];
        if (message) {
          dispatch_async(dispatch_get_main_queue(), ^{
            NSString *localizedOK =
<<<<<<< HEAD
            NSLocalizedStringWithDefaultValue(@"ErrorRecovery.Alert.OK", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                              @"OK",
                                              @"The title of the label to dismiss the alert when presenting user facing error messages");
=======
            NSLocalizedStringWithDefaultValue(
              @"ErrorRecovery.Alert.OK",
              @"FacebookSDK",
              [FBSDKInternalUtility bundleForStrings],
              @"OK",
              @"The title of the label to dismiss the alert when presenting user facing error messages"
            );
>>>>>>> origin/develop12
            [self displayAlertWithTitle:title message:message cancelButtonTitle:localizedOK];
          });
        }
      }
      return NO;
  }
  return NO;
}

<<<<<<< HEAD
#pragma mark - UIAlertController support
=======
 #pragma mark - UIAlertController support
>>>>>>> origin/develop12

- (void)displayAlertWithRecoverySuggestion:(NSString *)recoverySuggestion recoveryOptionsTitles:(NSArray<NSString *> *)recoveryOptionsTitles
{
  UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil
                                                                           message:recoverySuggestion
                                                                    preferredStyle:UIAlertControllerStyleAlert];
  for (NSUInteger i = 0; i < recoveryOptionsTitles.count; i++) {
    NSString *title = [FBSDKTypeUtility array:recoveryOptionsTitles objectAtIndex:i];
    UIAlertAction *option = [UIAlertAction actionWithTitle:title
                                                     style:UIAlertActionStyleDefault
<<<<<<< HEAD
                                                   handler:^(UIAlertAction * _Nonnull action) {
=======
                                                   handler:^(UIAlertAction *_Nonnull action) {
>>>>>>> origin/develop12
                                                     [self->_recoveryAttempter attemptRecoveryFromError:self->_error
                                                                                            optionIndex:i
                                                                                               delegate:self
                                                                                     didRecoverSelector:@selector(didPresentErrorWithRecovery:contextInfo:)
                                                                                            contextInfo:nil];
                                                   }];
    [alertController addAction:option];
  }
  UIViewController *topMostViewController = [FBSDKInternalUtility topMostViewController];
  [topMostViewController presentViewController:alertController
                                      animated:YES
                                    completion:nil];
}

- (void)displayAlertWithTitle:(NSString *)title message:(NSString *)message cancelButtonTitle:(NSString *)localizedOK
{
  UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil
                                                                           message:message
                                                                    preferredStyle:UIAlertControllerStyleAlert];
  UIAlertAction *OKAction = [UIAlertAction actionWithTitle:localizedOK
                                                     style:UIAlertActionStyleCancel
<<<<<<< HEAD
                                                   handler:^(UIAlertAction * _Nonnull action) {
=======
                                                   handler:^(UIAlertAction *_Nonnull action) {
>>>>>>> origin/develop12
                                                     [self->_recoveryAttempter attemptRecoveryFromError:self->_error
                                                                                            optionIndex:0
                                                                                               delegate:self
                                                                                     didRecoverSelector:@selector(didPresentErrorWithRecovery:contextInfo:)
                                                                                            contextInfo:nil];
                                                   }];
  [alertController addAction:OKAction];
  UIViewController *topMostViewController = [FBSDKInternalUtility topMostViewController];
  [topMostViewController presentViewController:alertController
                                      animated:YES
                                    completion:nil];
}

<<<<<<< HEAD
#pragma mark - FBSDKErrorRecoveryAttempting "delegate"
=======
 #pragma mark - FBSDKErrorRecoveryAttempting "delegate"
>>>>>>> origin/develop12

- (void)didPresentErrorWithRecovery:(BOOL)didRecover contextInfo:(void *)contextInfo
{
  [self.delegate processorDidAttemptRecovery:self didRecover:didRecover error:_error];
  self.delegate = nil;
}

@end

#endif
