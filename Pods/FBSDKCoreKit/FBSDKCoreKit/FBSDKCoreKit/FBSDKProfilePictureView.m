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
#import "FBSDKProfilePictureView.h"

#import "FBSDKAccessToken.h"
#import "FBSDKInternalUtility.h"
#import "FBSDKMaleSilhouetteIcon.h"
#import "FBSDKMath.h"
#import "FBSDKUtility.h"
=======
 #import "FBSDKProfilePictureView.h"

 #import "FBSDKAccessToken.h"
 #import "FBSDKInternalUtility.h"
 #import "FBSDKMaleSilhouetteIcon.h"
 #import "FBSDKMath.h"
 #import "FBSDKUtility.h"
>>>>>>> origin/develop12

@interface FBSDKProfilePictureViewState : NSObject

- (instancetype)initWithProfileID:(NSString *)profileID
                             size:(CGSize)size
                            scale:(CGFloat)scale
                      pictureMode:(FBSDKProfilePictureMode)pictureMode
                   imageShouldFit:(BOOL)imageShouldFit;

<<<<<<< HEAD
@property (nonatomic, assign, readonly) BOOL imageShouldFit;
@property (nonatomic, assign, readonly) FBSDKProfilePictureMode pictureMode;
@property (nonatomic, copy, readonly) NSString *profileID;
@property (nonatomic, assign, readonly) CGFloat scale;
@property (nonatomic, assign, readonly) CGSize size;
=======
@property (nonatomic, readonly, assign) BOOL imageShouldFit;
@property (nonatomic, readonly, assign) FBSDKProfilePictureMode pictureMode;
@property (nonatomic, readonly, copy) NSString *profileID;
@property (nonatomic, readonly, assign) CGFloat scale;
@property (nonatomic, readonly, assign) CGSize size;
>>>>>>> origin/develop12

- (BOOL)isEqualToState:(FBSDKProfilePictureViewState *)other;
- (BOOL)isValidForState:(FBSDKProfilePictureViewState *)other;

@end

@implementation FBSDKProfilePictureViewState

- (instancetype)initWithProfileID:(NSString *)profileID
                             size:(CGSize)size
                            scale:(CGFloat)scale
                      pictureMode:(FBSDKProfilePictureMode)pictureMode
                   imageShouldFit:(BOOL)imageShouldFit
{
  if ((self = [super init])) {
    _profileID = [profileID copy];
    _size = size;
    _scale = scale;
    _pictureMode = pictureMode;
    _imageShouldFit = imageShouldFit;
  }
  return self;
}

- (NSUInteger)hash
{
  NSUInteger subhashes[] = {
    (NSUInteger)_imageShouldFit,
    (NSUInteger)_size.width,
    (NSUInteger)_size.height,
    (NSUInteger)_scale,
    (NSUInteger)_pictureMode,
    _profileID.hash,
  };
  return [FBSDKMath hashWithIntegerArray:subhashes count:sizeof(subhashes) / sizeof(subhashes[0])];
}

- (BOOL)isEqual:(id)object
{
  if (![object isKindOfClass:[FBSDKProfilePictureViewState class]]) {
    return NO;
  }
  FBSDKProfilePictureViewState *other = (FBSDKProfilePictureViewState *)object;
  return [self isEqualToState:other];
}

- (BOOL)isEqualToState:(FBSDKProfilePictureViewState *)other
{
<<<<<<< HEAD
  return ([self isValidForState:other] &&
          CGSizeEqualToSize(_size, other->_size) &&
          (_scale == other->_scale));
=======
  return ([self isValidForState:other]
    && CGSizeEqualToSize(_size, other->_size)
    && (_scale == other->_scale));
>>>>>>> origin/develop12
}

- (BOOL)isValidForState:(FBSDKProfilePictureViewState *)other
{
<<<<<<< HEAD
  return (other != nil &&
          (_imageShouldFit == other->_imageShouldFit) &&
          (_pictureMode == other->_pictureMode) &&
          [FBSDKInternalUtility object:_profileID isEqualToObject:other->_profileID]);
=======
  return (other != nil
    && (_imageShouldFit == other->_imageShouldFit)
    && (_pictureMode == other->_pictureMode)
    && [FBSDKInternalUtility object:_profileID isEqualToObject:other->_profileID]);
>>>>>>> origin/develop12
}

@end

@implementation FBSDKProfilePictureView
{
  BOOL _hasProfileImage;
  UIImageView *_imageView;
  FBSDKProfilePictureViewState *_lastState;
  BOOL _needsImageUpdate;
  BOOL _placeholderImageIsValid;
}

<<<<<<< HEAD
#pragma mark - Object Lifecycle
=======
 #pragma mark - Object Lifecycle
>>>>>>> origin/develop12

- (instancetype)initWithFrame:(CGRect)frame
{
  if ((self = [super initWithFrame:frame])) {
    [self _configureProfilePictureView];
  }
  return self;
}

- (instancetype)initWithCoder:(NSCoder *)decoder
{
  if ((self = [super initWithCoder:decoder])) {
    [self _configureProfilePictureView];
  }
  return self;
}

- (instancetype)initWithFrame:(CGRect)frame
                      profile:(FBSDKProfile *)profile
{
  if ((self = [super initWithFrame:frame])) {
    _profileID = [profile.userID copy];

    [self setNeedsImageUpdate];
  }
  return self;
}

- (instancetype)initWithProfile:(FBSDKProfile *)profile
{
  return [self initWithFrame:CGRectZero profile:profile];
}

- (void)dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

<<<<<<< HEAD
#pragma mark - Properties
=======
 #pragma mark - Properties
>>>>>>> origin/develop12

- (void)setBounds:(CGRect)bounds
{
  dispatch_async(dispatch_get_main_queue(), ^{
    CGRect currentBounds = self.bounds;
    if (!CGRectEqualToRect(currentBounds, bounds)) {
      super.bounds = bounds;
      if (!CGSizeEqualToSize(currentBounds.size, bounds.size)) {
        self->_placeholderImageIsValid = NO;
        [self setNeedsImageUpdate];
      }
    }
  });
}

- (UIViewContentMode)contentMode
{
  return _imageView.contentMode;
}

- (void)setContentMode:(UIViewContentMode)contentMode
{
  if (_imageView.contentMode != contentMode) {
    _imageView.contentMode = contentMode;
    super.contentMode = contentMode;
    [self setNeedsImageUpdate];
  }
}

- (void)setMode:(FBSDKProfilePictureMode)pictureMode
{
  if (_pictureMode != pictureMode) {
    _pictureMode = pictureMode;
    [self setNeedsImageUpdate];
  }
}

- (void)setProfileID:(NSString *)profileID
{
  if (![FBSDKInternalUtility object:_profileID isEqualToObject:profileID]) {
    _profileID = [profileID copy];
    _placeholderImageIsValid = NO;
    [self setNeedsImageUpdate];
  }
}

<<<<<<< HEAD
#pragma mark - Public Methods
=======
 #pragma mark - Public Methods
>>>>>>> origin/develop12

- (void)setNeedsImageUpdate
{
  dispatch_async(dispatch_get_main_queue(), ^{
    if (!self->_imageView || CGRectIsEmpty(self.bounds)) {
      // we can't do anything with an empty view, so just bail out until we have a size
      return;
    }

    // ensure that we have an image.  do this here so we can draw the placeholder image synchronously if we don't have one
    if (!self->_placeholderImageIsValid && !self->_hasProfileImage) {
      [self _setPlaceholderImage];
    }

    // debounce calls to needsImage against the main runloop
    if (self->_needsImageUpdate) {
      return;
    }
    self->_needsImageUpdate = YES;
    [self _needsImageUpdate];
  });
}

<<<<<<< HEAD
#pragma mark - Helper Methods
=======
 #pragma mark - Helper Methods
>>>>>>> origin/develop12

- (void)_accessTokenDidChangeNotification:(NSNotification *)notification
{
  if (![_profileID isEqualToString:@"me"] || !notification.userInfo[FBSDKAccessTokenDidChangeUserIDKey]) {
    return;
  }
  _lastState = nil;
  [self setNeedsImageUpdate];
}

- (void)_configureProfilePictureView
{
  _imageView = [[UIImageView alloc] initWithFrame:self.bounds];
  _imageView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight);
  [self addSubview:_imageView];

  _profileID = @"me";
  self.backgroundColor = [UIColor whiteColor];
  self.contentMode = UIViewContentModeScaleAspectFit;
  self.userInteractionEnabled = NO;
  [[NSNotificationCenter defaultCenter] addObserver:self
                                           selector:@selector(_accessTokenDidChangeNotification:)
                                               name:FBSDKAccessTokenDidChangeNotification
                                             object:nil];

  [self setNeedsImageUpdate];
}

- (BOOL)_imageShouldFit
{
  switch (self.contentMode) {
    case UIViewContentModeBottom:
    case UIViewContentModeBottomLeft:
    case UIViewContentModeBottomRight:
    case UIViewContentModeCenter:
    case UIViewContentModeLeft:
    case UIViewContentModeRedraw:
    case UIViewContentModeRight:
    case UIViewContentModeScaleAspectFit:
    case UIViewContentModeTop:
    case UIViewContentModeTopLeft:
    case UIViewContentModeTopRight:
      return YES;
    case UIViewContentModeScaleAspectFill:
    case UIViewContentModeScaleToFill:
      return NO;
  }
}

- (CGSize)_imageSize:(BOOL)imageShouldFit scale:(CGFloat)scale
{
  // get the image size based on the contentMode and pictureMode
  CGSize size = self.bounds.size;
  switch (_pictureMode) {
<<<<<<< HEAD
    case FBSDKProfilePictureModeSquare:{
=======
    case FBSDKProfilePictureModeSquare: {
>>>>>>> origin/develop12
      CGFloat imageSize;
      if (imageShouldFit) {
        imageSize = MIN(size.width, size.height);
      } else {
        imageSize = MAX(size.width, size.height);
      }
      size = CGSizeMake(imageSize, imageSize);
      break;
    }
    case FBSDKProfilePictureModeNormal:
<<<<<<< HEAD
=======
    case FBSDKProfilePictureModeAlbum:
    case FBSDKProfilePictureModeSmall:
    case FBSDKProfilePictureModeLarge:
>>>>>>> origin/develop12
      // use the bounds size
      break;
  }

  // adjust for the screen scale
  size = CGSizeMake(size.width * scale, size.height * scale);

  return size;
}

- (void)_needsImageUpdate
{
  _needsImageUpdate = NO;

  if (!_profileID) {
    if (!_placeholderImageIsValid) {
      [self _setPlaceholderImage];
    }
    return;
  }

  // if the current image is no longer representative of the current state, clear the current value out; otherwise,
  // leave the current value until the new resolution image is downloaded
  BOOL imageShouldFit = [self _imageShouldFit];
  UIScreen *screen = self.window.screen ?: [UIScreen mainScreen];
  CGFloat scale = screen.scale;
  CGSize imageSize = [self _imageSize:imageShouldFit scale:scale];
  FBSDKProfilePictureViewState *state = [[FBSDKProfilePictureViewState alloc] initWithProfileID:_profileID
                                                                                           size:imageSize
                                                                                          scale:scale
                                                                                    pictureMode:_pictureMode
                                                                                 imageShouldFit:imageShouldFit];
  if (![_lastState isValidForState:state]) {
    [self _setPlaceholderImage];
  }
  _lastState = state;

  FBSDKAccessToken *accessToken = [FBSDKAccessToken currentAccessToken];
  if ([state.profileID isEqualToString:@"me"] && !accessToken) {
    return;
  }

  NSString *path = [[NSString alloc] initWithFormat:@"/%@/picture", [FBSDKUtility URLEncode:state.profileID]];
  CGSize size = state.size;
  NSMutableDictionary<NSString *, id> *parameters = [[NSMutableDictionary alloc] init];
  [FBSDKTypeUtility dictionary:parameters setObject:@(size.width) forKey:@"width"];
  [FBSDKTypeUtility dictionary:parameters setObject:@(size.height) forKey:@"height"];
  [FBSDKTypeUtility dictionary:parameters setObject:accessToken.tokenString forKey:@"access_token"];
  NSURL *imageURL = [FBSDKInternalUtility facebookURLWithHostPrefix:@"graph" path:path queryParameters:parameters error:NULL];

  __weak FBSDKProfilePictureView *weakSelf = self;

  NSURLRequest *request = [[NSURLRequest alloc] initWithURL:imageURL];
  NSURLSession *session = [NSURLSession sharedSession];
  [[session
    dataTaskWithRequest:request
    completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
      if (!error && data.length) {
        [weakSelf _updateImageWithData:data state:state];
      }
    }] resume];
}

- (void)_setPlaceholderImage
{
<<<<<<< HEAD
  UIColor *fillColor = [UIColor colorWithRed:157.0/255.0 green:177.0/255.0 blue:204.0/255.0 alpha:1.0];
=======
  UIColor *fillColor = [UIColor colorWithRed:157.0 / 255.0 green:177.0 / 255.0 blue:204.0 / 255.0 alpha:1.0];
>>>>>>> origin/develop12
  _placeholderImageIsValid = YES;
  _hasProfileImage = NO;

  dispatch_async(dispatch_get_main_queue(), ^{
    self->_imageView.image = [[[FBSDKMaleSilhouetteIcon alloc] initWithColor:fillColor] imageWithSize:self->_imageView.bounds.size];
  });
}

- (void)_updateImageWithData:(NSData *)data state:(FBSDKProfilePictureViewState *)state
{
  // make sure we haven't updated the state since we began fetching the image
  if (![state isValidForState:_lastState]) {
    return;
  }

  UIImage *image = [[UIImage alloc] initWithData:data scale:state.scale];
  if (image) {
    _hasProfileImage = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
      self->_imageView.image = image;
    });
  } else {
    _hasProfileImage = NO;
    _placeholderImageIsValid = NO;
    [self setNeedsImageUpdate];
  }
}

@end

#endif
