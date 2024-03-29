/*
 * Copyright 2018 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#import <Foundation/Foundation.h>

<<<<<<< HEAD
#import <GoogleDataTransport/GDTCORLifecycle.h>
#import <GoogleDataTransport/GDTCORRegistrar.h>

#import "GDTCORLibrary/Private/GDTCORUploadPackage_Private.h"
=======
#import "GoogleDataTransport/GDTCORLibrary/Public/GoogleDataTransport/GDTCORLifecycle.h"
#import "GoogleDataTransport/GDTCORLibrary/Public/GoogleDataTransport/GDTCORRegistrar.h"
>>>>>>> origin/develop12

@class GDTCORClock;

NS_ASSUME_NONNULL_BEGIN

/** This class connects storage and uploader implementations, providing events to an uploader
 * and informing the storage what events were successfully uploaded or not.
 */
<<<<<<< HEAD
@interface GDTCORUploadCoordinator
    : NSObject <NSSecureCoding, GDTCORLifecycleProtocol, GDTCORUploadPackageProtocol>
=======
@interface GDTCORUploadCoordinator : NSObject <GDTCORLifecycleProtocol>
>>>>>>> origin/develop12

/** The queue on which all upload coordination will occur. Also used by a dispatch timer. */
/** Creates and/or returrns the singleton.
 *
 * @return The singleton instance of this class.
 */
+ (instancetype)sharedInstance;

/** The queue on which all upload coordination will occur. */
@property(nonatomic, readonly) dispatch_queue_t coordinationQueue;

/** A timer that will causes regular checks for events to upload. */
<<<<<<< HEAD
@property(nonatomic, readonly) dispatch_source_t timer;
=======
@property(nonatomic, readonly, nullable) dispatch_source_t timer;
>>>>>>> origin/develop12

/** The interval the timer will fire. */
@property(nonatomic, readonly) uint64_t timerInterval;

/** Some leeway given to libdispatch for the timer interval event. */
@property(nonatomic, readonly) uint64_t timerLeeway;

<<<<<<< HEAD
/** The map of targets to in-flight packages. */
@property(nonatomic, readonly)
    NSMutableDictionary<NSNumber *, GDTCORUploadPackage *> *targetToInFlightPackages;

=======
>>>>>>> origin/develop12
/** The registrar object the coordinator will use. Generally used for testing. */
@property(nonatomic) GDTCORRegistrar *registrar;

/** Forces the backend specified by the target to upload the provided set of events. This should
 * only ever happen when the QoS tier of an event requires it.
 *
 * @param target The target that should force an upload.
 */
- (void)forceUploadForTarget:(GDTCORTarget)target;

/** Starts the upload timer. */
- (void)startTimer;

/** Stops the upload timer from running. */
- (void)stopTimer;

@end

NS_ASSUME_NONNULL_END
