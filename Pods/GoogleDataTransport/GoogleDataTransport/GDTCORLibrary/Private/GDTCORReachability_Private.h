/*
 * Copyright 2019 Google
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

<<<<<<< HEAD
#import "GDTCORLibrary/Public/GDTCORReachability.h"

@interface GDTCORReachability ()

#if !TARGET_OS_WATCH
/** Allows manually setting the flags for testing purposes. */
@property(nonatomic, readwrite) SCNetworkReachabilityFlags flags;
#endif
=======
#import "GoogleDataTransport/GDTCORLibrary/Public/GoogleDataTransport/GDTCORReachability.h"

@interface GDTCORReachability ()

/** Allows manually setting the flags for testing purposes. */
@property(nonatomic, readwrite) GDTCORNetworkReachabilityFlags flags;
>>>>>>> origin/develop12

/** Creates/returns the singleton instance of this class.
 *
 * @return The singleton instance of this class.
 */
+ (instancetype)sharedInstance;

@end
