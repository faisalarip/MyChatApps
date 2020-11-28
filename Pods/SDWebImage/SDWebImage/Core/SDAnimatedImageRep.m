/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "SDAnimatedImageRep.h"

#if SD_MAC

#import "SDImageIOAnimatedCoderInternal.h"
#import "SDImageGIFCoder.h"
#import "SDImageAPNGCoder.h"
#import "SDImageHEICCoder.h"
<<<<<<< HEAD
#import "SDImageHEICCoderInternal.h"
=======
#import "SDImageAWebPCoder.h"
>>>>>>> origin/develop12

@implementation SDAnimatedImageRep {
    CGImageSourceRef _imageSource;
}

- (void)dealloc {
    if (_imageSource) {
        CFRelease(_imageSource);
        _imageSource = NULL;
    }
}

<<<<<<< HEAD
// `NSBitmapImageRep`'s `imageRepWithData:` is not designed initlizer
=======
// `NSBitmapImageRep`'s `imageRepWithData:` is not designed initializer
>>>>>>> origin/develop12
+ (instancetype)imageRepWithData:(NSData *)data {
    SDAnimatedImageRep *imageRep = [[SDAnimatedImageRep alloc] initWithData:data];
    return imageRep;
}

<<<<<<< HEAD
// We should override init method for `NSBitmapImageRep` to do initlize about animated image format
=======
// We should override init method for `NSBitmapImageRep` to do initialize about animated image format
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunguarded-availability"
>>>>>>> origin/develop12
- (instancetype)initWithData:(NSData *)data {
    self = [super initWithData:data];
    if (self) {
        CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef) data, NULL);
        if (!imageSource) {
            return self;
        }
        _imageSource = imageSource;
        NSUInteger frameCount = CGImageSourceGetCount(imageSource);
        if (frameCount <= 1) {
            return self;
        }
        CFStringRef type = CGImageSourceGetType(imageSource);
        if (!type) {
            return self;
        }
        if (CFStringCompare(type, kUTTypeGIF, 0) == kCFCompareEqualTo) {
            // GIF
            // Fix the `NSBitmapImageRep` GIF loop count calculation issue
            // Which will use 0 when there are no loop count information metadata in GIF data
            NSUInteger loopCount = [SDImageGIFCoder imageLoopCountWithSource:imageSource];
            [self setProperty:NSImageLoopCount withValue:@(loopCount)];
        } else if (CFStringCompare(type, kUTTypePNG, 0) == kCFCompareEqualTo) {
            // APNG
<<<<<<< HEAD
            // Do initilize about frame count, current frame/duration and loop count
=======
            // Do initialize about frame count, current frame/duration and loop count
>>>>>>> origin/develop12
            [self setProperty:NSImageFrameCount withValue:@(frameCount)];
            [self setProperty:NSImageCurrentFrame withValue:@(0)];
            NSUInteger loopCount = [SDImageAPNGCoder imageLoopCountWithSource:imageSource];
            [self setProperty:NSImageLoopCount withValue:@(loopCount)];
        } else if (CFStringCompare(type, kSDUTTypeHEICS, 0) == kCFCompareEqualTo) {
            // HEIC
<<<<<<< HEAD
            // Do initilize about frame count, current frame/duration and loop count
=======
            // Do initialize about frame count, current frame/duration and loop count
>>>>>>> origin/develop12
            [self setProperty:NSImageFrameCount withValue:@(frameCount)];
            [self setProperty:NSImageCurrentFrame withValue:@(0)];
            NSUInteger loopCount = [SDImageHEICCoder imageLoopCountWithSource:imageSource];
            [self setProperty:NSImageLoopCount withValue:@(loopCount)];
<<<<<<< HEAD
=======
        } else if (CFStringCompare(type, kSDUTTypeWebP, 0) == kCFCompareEqualTo) {
            // WebP
            // Do initialize about frame count, current frame/duration and loop count
            [self setProperty:NSImageFrameCount withValue:@(frameCount)];
            [self setProperty:NSImageCurrentFrame withValue:@(0)];
            NSUInteger loopCount = [SDImageAWebPCoder imageLoopCountWithSource:imageSource];
            [self setProperty:NSImageLoopCount withValue:@(loopCount)];
>>>>>>> origin/develop12
        }
    }
    return self;
}

// `NSBitmapImageRep` will use `kCGImagePropertyGIFDelayTime` whenever you call `setProperty:withValue:` with `NSImageCurrentFrame` to change the current frame. We override it and use the actual `kCGImagePropertyGIFUnclampedDelayTime` if need.
- (void)setProperty:(NSBitmapImageRepPropertyKey)property withValue:(id)value {
    [super setProperty:property withValue:value];
    if ([property isEqualToString:NSImageCurrentFrame]) {
        // Access the image source
        CGImageSourceRef imageSource = _imageSource;
        if (!imageSource) {
            return;
        }
        // Check format type
        CFStringRef type = CGImageSourceGetType(imageSource);
        if (!type) {
            return;
        }
        NSUInteger index = [value unsignedIntegerValue];
        NSTimeInterval frameDuration = 0;
        if (CFStringCompare(type, kUTTypeGIF, 0) == kCFCompareEqualTo) {
            // GIF
            frameDuration = [SDImageGIFCoder frameDurationAtIndex:index source:imageSource];
        } else if (CFStringCompare(type, kUTTypePNG, 0) == kCFCompareEqualTo) {
            // APNG
            frameDuration = [SDImageAPNGCoder frameDurationAtIndex:index source:imageSource];
        } else if (CFStringCompare(type, kSDUTTypeHEICS, 0) == kCFCompareEqualTo) {
            // HEIC
            frameDuration = [SDImageHEICCoder frameDurationAtIndex:index source:imageSource];
<<<<<<< HEAD
=======
        } else if (CFStringCompare(type, kSDUTTypeWebP, 0) == kCFCompareEqualTo) {
            // WebP
            frameDuration = [SDImageAWebPCoder frameDurationAtIndex:index source:imageSource];
>>>>>>> origin/develop12
        }
        if (!frameDuration) {
            return;
        }
        // Reset super frame duration with the actual frame duration
        [super setProperty:NSImageCurrentFrameDuration withValue:@(frameDuration)];
    }
}
<<<<<<< HEAD
=======
#pragma clang diagnostic pop
>>>>>>> origin/develop12

@end

#endif
