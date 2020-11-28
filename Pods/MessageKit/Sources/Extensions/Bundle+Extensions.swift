/*
 MIT License

<<<<<<< HEAD
 Copyright (c) 2017-2019 MessageKit
=======
 Copyright (c) 2017-2020 MessageKit
>>>>>>> origin/develop12

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

import Foundation

internal extension Bundle {
<<<<<<< HEAD

    static func messageKitAssetBundle() -> Bundle {
        let podBundle = Bundle(for: MessagesViewController.self)
        
        guard let resourceBundleUrl = podBundle.url(forResource: "MessageKitAssets", withExtension: "bundle") else {
            fatalError(MessageKitError.couldNotCreateAssetsPath)
        }
        
        guard let resourceBundle = Bundle(url: resourceBundleUrl) else {
            fatalError(MessageKitError.couldNotLoadAssetsBundle)
        }
        
        return resourceBundle
    }

=======
    #if IS_SPM
    static var messageKitAssetBundle: Bundle = Bundle.module
    #else
    static var messageKitAssetBundle: Bundle = Bundle(for: MessagesViewController.self)
    #endif
>>>>>>> origin/develop12
}
