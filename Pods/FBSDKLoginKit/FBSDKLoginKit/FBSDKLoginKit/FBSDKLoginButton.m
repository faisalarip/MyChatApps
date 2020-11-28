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
#import "FBSDKLoginButton.h"

#ifdef FBSDKCOCOAPODS
#import <FBSDKCoreKit/FBSDKCoreKit+Internal.h>
#else
#import "FBSDKCoreKit+Internal.h"
#endif

#import "FBSDKLoginTooltipView.h"
=======
 #import "FBSDKLoginButton.h"

 #ifdef FBSDKCOCOAPODS
  #import <FBSDKCoreKit/FBSDKCoreKit+Internal.h>
 #else
  #import "FBSDKCoreKit+Internal.h"
 #endif

 #import "FBSDKLoginTooltipView.h"
>>>>>>> origin/develop12

static const CGFloat kFBLogoSize = 16.0;
static const CGFloat kFBLogoLeftMargin = 6.0;
static const CGFloat kButtonHeight = 28.0;
static const CGFloat kRightMargin = 8.0;
static const CGFloat kPaddingBetweenLogoTitle = 8.0;

<<<<<<< HEAD
@interface FBSDKLoginButton() <FBSDKButtonImpressionTracking>
=======
@interface FBSDKLoginButton () <FBSDKButtonImpressionTracking>
>>>>>>> origin/develop12
@end

@implementation FBSDKLoginButton
{
  BOOL _hasShownTooltipBubble;
  FBSDKLoginManager *_loginManager;
  NSString *_userID;
  NSString *_userName;
}

<<<<<<< HEAD
#pragma mark - Object Lifecycle
=======
 #pragma mark - Object Lifecycle
>>>>>>> origin/develop12

- (void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

<<<<<<< HEAD
#pragma mark - Properties
=======
 #pragma mark - Properties
>>>>>>> origin/develop12

- (FBSDKDefaultAudience)defaultAudience
{
  return _loginManager.defaultAudience;
}

- (void)setDefaultAudience:(FBSDKDefaultAudience)defaultAudience
{
  _loginManager.defaultAudience = defaultAudience;
}

- (UIFont *)defaultFont
{
  CGFloat size = 15;

  if (@available(iOS 8.2, *)) {
    return [UIFont systemFontOfSize:size weight:UIFontWeightSemibold];
  } else {
    return [UIFont boldSystemFontOfSize:size];
  }
}

<<<<<<< HEAD
#pragma mark - UIView
=======
 #pragma mark - UIView
>>>>>>> origin/develop12

- (void)didMoveToWindow
{
  [super didMoveToWindow];

<<<<<<< HEAD
  if (self.window &&
      ((self.tooltipBehavior == FBSDKLoginButtonTooltipBehaviorForceDisplay) || !_hasShownTooltipBubble)) {
=======
  if (self.window
      && ((self.tooltipBehavior == FBSDKLoginButtonTooltipBehaviorForceDisplay) || !_hasShownTooltipBubble)) {
>>>>>>> origin/develop12
    [self performSelector:@selector(_showTooltipIfNeeded) withObject:nil afterDelay:0];
    _hasShownTooltipBubble = YES;
  }
}

<<<<<<< HEAD
#pragma mark - Layout
=======
 #pragma mark - Layout
>>>>>>> origin/develop12

- (CGRect)imageRectForContentRect:(CGRect)contentRect
{
  CGFloat centerY = CGRectGetMidY(contentRect);
  CGFloat y = centerY - (kFBLogoSize / 2.0);
  return CGRectMake(kFBLogoLeftMargin, y, kFBLogoSize, kFBLogoSize);
}

- (CGRect)titleRectForContentRect:(CGRect)contentRect
{
  if (self.hidden || CGRectIsEmpty(self.bounds)) {
    return CGRectZero;
  }
  CGRect imageRect = [self imageRectForContentRect:contentRect];
  CGFloat titleX = CGRectGetMaxX(imageRect) + kPaddingBetweenLogoTitle;
  CGRect titleRect = CGRectMake(titleX, 0, CGRectGetWidth(contentRect) - titleX - kRightMargin, CGRectGetHeight(contentRect));

  return titleRect;
}

- (void)layoutSubviews
{
  CGSize size = self.bounds.size;
  CGSize longTitleSize = [self sizeThatFits:size title:[self _longLogInTitle]];
<<<<<<< HEAD
  NSString *title = (longTitleSize.width <= size.width ?
                     [self _longLogInTitle] :
                     [self _shortLogInTitle]);
=======
  NSString *title = (longTitleSize.width <= size.width
    ? [self _longLogInTitle]
    : [self _shortLogInTitle]);
>>>>>>> origin/develop12
  if (![title isEqualToString:[self titleForState:UIControlStateNormal]]) {
    [self setTitle:title forState:UIControlStateNormal];
  }

  [super layoutSubviews];
}

- (CGSize)sizeThatFits:(CGSize)size
{
  if (self.hidden) {
    return CGSizeZero;
  }
  UIFont *font = self.titleLabel.font;

  CGSize selectedSize = FBSDKTextSize([self _logOutTitle], font, size, self.titleLabel.lineBreakMode);
  CGSize normalSize = FBSDKTextSize([self _longLogInTitle], font, size, self.titleLabel.lineBreakMode);
  if (normalSize.width > size.width) {
    normalSize = FBSDKTextSize([self _shortLogInTitle], font, size, self.titleLabel.lineBreakMode);
  }

  CGFloat titleWidth = MAX(normalSize.width, selectedSize.width);
  CGFloat buttonWidth = kFBLogoLeftMargin + kFBLogoSize + kPaddingBetweenLogoTitle + titleWidth + kRightMargin;
  return CGSizeMake(buttonWidth, kButtonHeight);
}

<<<<<<< HEAD
#pragma mark - FBSDKButtonImpressionTracking
=======
 #pragma mark - FBSDKButtonImpressionTracking
>>>>>>> origin/develop12

- (NSDictionary *)analyticsParameters
{
  return nil;
}

- (NSString *)impressionTrackingEventName
{
  return FBSDKAppEventNameFBSDKLoginButtonImpression;
}

- (NSString *)impressionTrackingIdentifier
{
  return @"login";
}

<<<<<<< HEAD
#pragma mark - FBSDKButton
=======
 #pragma mark - FBSDKButton
>>>>>>> origin/develop12

- (void)configureButton
{
  _loginManager = [[FBSDKLoginManager alloc] init];

  NSString *logInTitle = [self _shortLogInTitle];
  NSString *logOutTitle = [self _logOutTitle];

  [self configureWithIcon:nil
<<<<<<< HEAD
                    title:logInTitle
          backgroundColor:self.backgroundColor
         highlightedColor:nil
            selectedTitle:logOutTitle
             selectedIcon:nil
            selectedColor:self.backgroundColor
 selectedHighlightedColor:nil];
=======
                      title:logInTitle
            backgroundColor:self.backgroundColor
           highlightedColor:nil
              selectedTitle:logOutTitle
               selectedIcon:nil
              selectedColor:self.backgroundColor
   selectedHighlightedColor:nil];
>>>>>>> origin/develop12
  self.titleLabel.textAlignment = NSTextAlignmentCenter;
  [self addConstraint:[NSLayoutConstraint constraintWithItem:self
                                                   attribute:NSLayoutAttributeHeight
                                                   relatedBy:NSLayoutRelationEqual
                                                      toItem:nil
                                                   attribute:NSLayoutAttributeNotAnAttribute
                                                  multiplier:1
                                                    constant:kButtonHeight]];
  [self _updateContent];

  [self addTarget:self action:@selector(_buttonPressed:) forControlEvents:UIControlEventTouchUpInside];
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(_accessTokenDidChangeNotification:)
                                               name:FBSDKAccessTokenDidChangeNotification
                                             object:nil];
}

<<<<<<< HEAD
#pragma mark - Helper Methods
=======
 #pragma mark - Helper Methods
>>>>>>> origin/develop12

- (void)_accessTokenDidChangeNotification:(NSNotification *)notification
{
  if (notification.userInfo[FBSDKAccessTokenDidChangeUserIDKey] || notification.userInfo[FBSDKAccessTokenDidExpireKey]) {
    [self _updateContent];
  }
}

- (void)_buttonPressed:(id)sender
{
  [self logTapEventWithEventName:FBSDKAppEventNameFBSDKLoginButtonDidTap parameters:self.analyticsParameters];
  if (FBSDKAccessToken.isCurrentAccessTokenActive) {
    NSString *title = nil;

    if (_userName) {
      NSString *localizedFormatString =
<<<<<<< HEAD
      NSLocalizedStringWithDefaultValue(@"LoginButton.LoggedInAs", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                        @"Logged in as %@",
                                        @"The format string for the FBSDKLoginButton label when the user is logged in");
      title = [NSString localizedStringWithFormat:localizedFormatString, _userName];
    } else {
      NSString *localizedLoggedIn =
      NSLocalizedStringWithDefaultValue(@"LoginButton.LoggedIn", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                        @"Logged in using Facebook",
                                        @"The fallback string for the FBSDKLoginButton label when the user name is not available yet");
      title = localizedLoggedIn;
    }
    NSString *cancelTitle =
    NSLocalizedStringWithDefaultValue(@"LoginButton.CancelLogout", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                      @"Cancel",
                                      @"The label for the FBSDKLoginButton action sheet to cancel logging out");
    NSString *logOutTitle =
    NSLocalizedStringWithDefaultValue(@"LoginButton.ConfirmLogOut", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                      @"Log Out",
                                      @"The label for the FBSDKLoginButton action sheet to confirm logging out");
=======
      NSLocalizedStringWithDefaultValue(
        @"LoginButton.LoggedInAs",
        @"FacebookSDK",
        [FBSDKInternalUtility bundleForStrings],
        @"Logged in as %@",
        @"The format string for the FBSDKLoginButton label when the user is logged in"
      );
      title = [NSString localizedStringWithFormat:localizedFormatString, _userName];
    } else {
      NSString *localizedLoggedIn =
      NSLocalizedStringWithDefaultValue(
        @"LoginButton.LoggedIn",
        @"FacebookSDK",
        [FBSDKInternalUtility bundleForStrings],
        @"Logged in using Facebook",
        @"The fallback string for the FBSDKLoginButton label when the user name is not available yet"
      );
      title = localizedLoggedIn;
    }
    NSString *cancelTitle =
    NSLocalizedStringWithDefaultValue(
      @"LoginButton.CancelLogout",
      @"FacebookSDK",
      [FBSDKInternalUtility bundleForStrings],
      @"Cancel",
      @"The label for the FBSDKLoginButton action sheet to cancel logging out"
    );
    NSString *logOutTitle =
    NSLocalizedStringWithDefaultValue(
      @"LoginButton.ConfirmLogOut",
      @"FacebookSDK",
      [FBSDKInternalUtility bundleForStrings],
      @"Log Out",
      @"The label for the FBSDKLoginButton action sheet to confirm logging out"
    );
>>>>>>> origin/develop12
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                             message:nil
                                                                      preferredStyle:UIAlertControllerStyleActionSheet];
    alertController.popoverPresentationController.sourceView = self;
    alertController.popoverPresentationController.sourceRect = self.bounds;
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:cancelTitle
                                                     style:UIAlertActionStyleCancel
                                                   handler:nil];
    UIAlertAction *logout = [UIAlertAction actionWithTitle:logOutTitle
                                                     style:UIAlertActionStyleDestructive
<<<<<<< HEAD
                                                   handler:^(UIAlertAction * _Nonnull action) {
=======
                                                   handler:^(UIAlertAction *_Nonnull action) {
>>>>>>> origin/develop12
                                                     [self->_loginManager logOut];
                                                     [self.delegate loginButtonDidLogOut:self];
                                                   }];
    [alertController addAction:cancel];
    [alertController addAction:logout];
    UIViewController *topMostViewController = [FBSDKInternalUtility topMostViewController];
    [topMostViewController presentViewController:alertController
                                        animated:YES
                                      completion:nil];
  } else {
    if ([self.delegate respondsToSelector:@selector(loginButtonWillLogin:)]) {
      if (![self.delegate loginButtonWillLogin:self]) {
        return;
      }
    }

    FBSDKLoginManagerLoginResultBlock handler = ^(FBSDKLoginManagerLoginResult *result, NSError *error) {
      if ([self.delegate respondsToSelector:@selector(loginButton:didCompleteWithResult:error:)]) {
        [self.delegate loginButton:self didCompleteWithResult:result error:error];
      }
    };

    [_loginManager logInWithPermissions:self.permissions
                     fromViewController:[FBSDKInternalUtility viewControllerForView:self]
                                handler:handler];
<<<<<<< HEAD

=======
>>>>>>> origin/develop12
  }
}

- (NSString *)_logOutTitle
{
<<<<<<< HEAD
  return NSLocalizedStringWithDefaultValue(@"LoginButton.LogOut", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                           @"Log out",
                                           @"The label for the FBSDKLoginButton when the user is currently logged in");
=======
  return NSLocalizedStringWithDefaultValue(
    @"LoginButton.LogOut",
    @"FacebookSDK",
    [FBSDKInternalUtility bundleForStrings],
    @"Log out",
    @"The label for the FBSDKLoginButton when the user is currently logged in"
  );
>>>>>>> origin/develop12
}

- (NSString *)_longLogInTitle
{
<<<<<<< HEAD
  return NSLocalizedStringWithDefaultValue(@"LoginButton.LogInContinue", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                           @"Continue with Facebook",
                                           @"The long label for the FBSDKLoginButton when the user is currently logged out");
=======
  return NSLocalizedStringWithDefaultValue(
    @"LoginButton.LogInContinue",
    @"FacebookSDK",
    [FBSDKInternalUtility bundleForStrings],
    @"Continue with Facebook",
    @"The long label for the FBSDKLoginButton when the user is currently logged out"
  );
>>>>>>> origin/develop12
}

- (NSString *)_shortLogInTitle
{
<<<<<<< HEAD
  return NSLocalizedStringWithDefaultValue(@"LoginButton.LogIn", @"FacebookSDK", [FBSDKInternalUtility bundleForStrings],
                                           @"Log in",
                                           @"The short label for the FBSDKLoginButton when the user is currently logged out");
=======
  return NSLocalizedStringWithDefaultValue(
    @"LoginButton.LogIn",
    @"FacebookSDK",
    [FBSDKInternalUtility bundleForStrings],
    @"Log in",
    @"The short label for the FBSDKLoginButton when the user is currently logged out"
  );
>>>>>>> origin/develop12
}

- (void)_showTooltipIfNeeded
{
  if ([FBSDKAccessToken currentAccessToken] || self.tooltipBehavior == FBSDKLoginButtonTooltipBehaviorDisable) {
    return;
  } else {
    FBSDKLoginTooltipView *tooltipView = [[FBSDKLoginTooltipView alloc] init];
    tooltipView.colorStyle = self.tooltipColorStyle;
    if (self.tooltipBehavior == FBSDKLoginButtonTooltipBehaviorForceDisplay) {
      tooltipView.forceDisplay = YES;
    }
    [tooltipView presentFromView:self];
  }
}

- (void)_updateContent
{
  BOOL accessTokenIsValid = FBSDKAccessToken.isCurrentAccessTokenActive;
  self.selected = accessTokenIsValid;
  if (accessTokenIsValid) {
    if (![[FBSDKAccessToken currentAccessToken].userID isEqualToString:_userID]) {
      FBSDKGraphRequest *request = [[FBSDKGraphRequest alloc] initWithGraphPath:@"me?fields=id,name"
                                                                     parameters:nil
                                                                          flags:FBSDKGraphRequestFlagDisableErrorRecovery];
      [request startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
        NSString *userID = [FBSDKTypeUtility stringValue:result[@"id"]];
        if (!error && [[FBSDKAccessToken currentAccessToken].userID isEqualToString:userID]) {
          self->_userName = [FBSDKTypeUtility stringValue:result[@"name"]];
          self->_userID = userID;
        }
      }];
    }
  }
}

@end

#endif
