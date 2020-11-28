[![Image](./Screenshots/Preview.gif)]()

# InputBarAccessoryView

### Features

- [x] Autocomplete text with @mention, #hashtag or any other prefix 
- [x] A self-sizing `UITextView` with an optional fixed height (can be replaced with any other view)
- [x] Image paste support   
- [x] Autocomplete attributed text highlighting
- [x] Reactive components that respond to given events
- [x] Top/Bottom/Left/Right `InputStackView`s that act as toolbars to place buttons
- [x] `RxSwift`/`RxCocoa` Support with `RxExtensions` Cocoapod subspec
- [x] Drop in attachment view for file/photo management
- [x] Plugin support for your own `InputPlugin`s
- [x] Compatible with all iPhones and iPads
- [x] RTL Support

<<<<<<< HEAD
### Installation via CocoaPods

```ruby
# Swift 5.0
pod 'InputBarAccessoryView'
# Swift 4.2
pod 'InputBarAccessoryView', '4.2.2'
=======
### Installation via Swift Package Manager (SPM)

The [Swift Package Manager](https://swift.org/package-manager/) is a tool for automating the distribution of Swift code and is integrated into the `swift` compiler.
Once you have your Swift package set up, adding InputBarAccessoryView as a dependency is as easy as adding it to the dependencies value of your Package.swift.

```
dependencies: [
    .package(url: "https://github.com/nathantannar4/InputBarAccessoryView.git", .upToNextMajor(from: "4.5.0"))
]
```
You can also add it via XCode SPM editor with URL:
```
https://github.com/nathantannar4/InputBarAccessoryView.git
```
To make `RxSwift`/`RxCocoa` extensions work you need to explicitly import `Rx` dependencies.

### Installation via CocoaPods

```ruby
# Swift 5.3
pod 'InputBarAccessoryView'

# Swift 5.0
pod 'InputBarAccessoryView', '5.1.0'
>>>>>>> origin/develop12
```

### Installation via Carthage

```ruby
<<<<<<< HEAD
# Swift 5.0
github "nathantannar4/InputBarAccessoryView"
# Swift 4.2
github "nathantannar4/InputBarAccessoryView" "4.2.2"
=======
# Swift 5.3
github "nathantannar4/InputBarAccessoryView"

# Swift 5.0
github "nathantannar4/InputBarAccessoryView" "5.1.0"
>>>>>>> origin/develop12
```

### Requirements

<<<<<<< HEAD
iOS 9.0+
Swift 5.0
=======
iOS 12.0+
Swift 5.3

> The latest iOS 11 release is v5.1.0

> The latest iOS 9 + iOS 10 release is v4.3.3

> The latest Swift 5.0 release is v5.1.0 
>>>>>>> origin/develop12

> The latest Swift 4.2 release is v4.2.2 

### Documentation

[Getting Started](./GETTING_STARTED.md)

> See the Example project to see how you can make the iMessage, Slack, Facebook and GitHawk input bars!

### Example Usage

<img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotA.png" width="242" height="432"> <img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotB.png" width="242" height="432"> <img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotC.png" width="242" height="432"> <img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotD.png" width="242" height="432"> <img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotE.png" width="242" height="432"> <img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/ScreenshotF.png" width="242" height="432">

### Featured In

Add your app to the list of apps using this library and make a pull request.

- [MessageKit](https://github.com/MessageKit/MessageKit) *(renamed to MessageInputBar)*
<p>
  <img src="https://raw.githubusercontent.com/MessageKit/MessageKit/master/Assets/mklogo.png" title="MessageKit Logo" height="50">
</p>

- [MessageViewController](https://github.com/GitHawkApp/MessageViewController) *(Autocomplete Highlighting Algorithm)*
<p>
  <img src="https://avatars3.githubusercontent.com/u/32285710?s=200&v=4" title="GitHawk Logo" height="50">
</p>

### See Also

iMessage style [TypingIndicator](https://github.com/nathantannar4/TypingIndicator) for chat apps

## Latest Releases
<<<<<<< HEAD
- 4.3.2
    - Fixed a bug where editing immediately before a tag prefix would break the tag
- 4.3.1
    - Add `RxSwift`/`RxCocoa` support through extensions and delegate proxies, requires Cocoapods installation of  `InputBarAccessoryView/RxExtensions`
- 4.3.0
    - Swift 5 support
- 4.2.2
    - Fixed image paste orientation issue in `InputTextView`
- 4.2.1
    - Fixed autocompletes of completions that contain spaces
    - Depricated `isCaseSensitive` in favor of a function asignment to pre-process autocompletions
- 4.2.0
    - Added new API for overriding the main middle view, normally the `InputTextView` so it can be replaced with views such as a "Join" button", `setMiddleContentView(_ view: UIView?, animated: Bool)`
- 4.1.2
        - Add `InputBarViewController` which contains an `InputBarAccessoryView` as the `inputAccessoryView` by default with a convenient `isInputBarHidden: Bool` property to show/hide it 
- 4.1.1
        - Add `frameInsets: HorizontalEdgePadding` property to `InputBarAccessoryView` to inset the view to be compatible with `UISplitViewController` or other custom containers where the view should not be the full width
- 4.1.0
        - Fix issue where setting long strings in `viewDidLoad` broke inital layout, Issue #41
        - Add `deleteCompletionByParts: Bool` flag to `AutocompleteManager` to allow for partial deletions of autocompletes rather than just the entire substring. 
        - Add `InputBarSendButton` to use as the `sendButton` in `InputBarAccessoryView`. This subclass of `InputBarButtonItem` has a `UIActivityIndicatorView` to show a spinner when making HTTP requests

### [CHANGELOG](./CHANGELOG.md)
=======
5.2.0
   - Drop support for iOS 11 and bump minimum version to iOS 12+
   - Support Swift 5.3 and higher for XCode 12
- 5.1.0
    - Added support for smooth height transitions when the text view expands, set `shouldAnimateTextDidChangeLayout` to `true`
    - Fixed accessibility of `HorizontalEdgePadding` initializers and a typo in its filename
    - Added support for Dark Mode on iOS 13+
- 5.0.0
    - Drop support for iOS 9 and iOS 10
    - Remove `RxSwift`/`RxCocoa`  from SPM
    - Fix image paste logic
    - Fixed Getting started preview and hashtag typo
    - Update documentation for setStackViewItems
    
See [CHANGELOG](./CHANGELOG.md) for more details and older releases.
>>>>>>> origin/develop12

**Find a bug? Open an issue!**

## Layout

The layout of the `InputBarAccessoryView` is made of of 4  `InputStackView`'s and an `InputTextView`. The padding of the subviews can be easily adjusted by changing the `padding` and `textViewPadding` properties. The constraints will automatically be updated.

<img src="https://raw.githubusercontent.com/nathantannar4/InputBarAccessoryView/master/Screenshots/Layout.png">

It is important to note that each of the `InputStackView `'s to the left and right of the `InputTextView` are anchored by a width constraint. This way the `InputTextView` will always fill the space inbetween in addition to providing methods that can easily be called to hide all buttons to the right or left of the `InputTextView` by setting the width constraint constant to 0. The bottom and top stack views are not height constraint and rely on their `intrinsicContentSize`

```swift
func setLeftStackViewWidthConstant(to newValue: CGFloat, animated: Bool)

func setRightStackViewWidthConstant(to newValue: CGFloat, animated: Bool)
```

### Reactive Hooks

Each `InputBarButtonItem` has properties that can hold actions that will be executed during various hooks such as the button being touched, the `UITextView `text changing and more! Thanks to these easy hooks with a few lines of code the items can be easily resized and animated similar to that of the Facebook messenger app.

```swift
// MARK: - Hooks
    
public typealias InputBarButtonItemAction = ((InputBarButtonItem) -> Void)    
    
private var onTouchUpInsideAction: InputBarButtonItemAction?
private var onKeyboardEditingBeginsAction: InputBarButtonItemAction?
private var onKeyboardEditingEndsAction: InputBarButtonItemAction?
private var onKeyboardSwipeGestureAction: ((InputBarButtonItem, UISwipeGestureRecognizer) -> Void)?
private var onTextViewDidChangeAction: ((InputBarButtonItem, InputTextView) -> Void)?
private var onSelectedAction: InputBarButtonItemAction?
private var onDeselectedAction: InputBarButtonItemAction?
private var onEnabledAction: InputBarButtonItemAction?
private var onDisabledAction: InputBarButtonItemAction?
```

## Author
<p>
	<img src="https://github.com/nathantannar4/NTComponents/raw/master/NTComponents/Assets/Nathan.png" width="100" height="100">
</p>

**Nathan Tannar** - [https://nathantannar.me](https://nathantannar.me)

## License

Distributed under the MIT license. See ``LICENSE`` for more information.
