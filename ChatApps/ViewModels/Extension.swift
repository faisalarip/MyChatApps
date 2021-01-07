//
//  Extension.swift
//  ChatApps
//
//  Created by Faisal Arif on 08/12/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation
import Contacts
import MapKit

extension CLPlacemark {
    /// street name, eg. Infinite Loop
    var streetName: String? { thoroughfare }
    /// // eg. 1
    var streetNumber: String? { subThoroughfare }
    /// city, eg. Cupertino
    var city: String? { locality }
    /// neighborhood, common name, eg. Mission District
    var neighborhood: String? { subLocality }
    /// state, eg. CA
    var state: String? { administrativeArea }
    /// county, eg. Santa Clara
    var county: String? { subAdministrativeArea }
    /// zip code, eg. 95014
    var zipCode: String? { postalCode }
    /// postal address formatted
    @available(iOS 11.0, *)
    var postalAddressFormatted: String? {
        guard let postalAddress = postalAddress else { return nil }
        return CNPostalAddressFormatter.string(from: postalAddress, style: .mailingAddress)
    }
}

extension CLLocation {
    func placemark(completion: @escaping (_ placemark: CLPlacemark?, _ error: Error?) -> ()) {
        CLGeocoder().reverseGeocodeLocation(self) { completion($0?.first, $1) }
    }
}

extension UIView {
    
    enum axis {
        case vertical, horizontal, custom(angle: CGFloat)
    }

    func setGradientBackgroundColor(colors: [UIColor], axis: axis, cornerRadius: CGFloat? = nil, apply: ((UIView) -> ())? = nil) {
        layer.sublayers?.filter { $0.name == "gradientLayer" }.forEach { $0.removeFromSuperlayer() }
        self.layoutIfNeeded()

        let cgColors = colors.map { $0.cgColor }

        let gradient: CAGradientLayer = CAGradientLayer()
        gradient.colors = cgColors
        gradient.name = "gradientLayer"
        gradient.locations = [0.0, 1.0]

        switch axis {
            case .horizontal:
                gradient.startPoint = CGPoint(x: 0.0, y: 1.0)
                gradient.endPoint = CGPoint(x: 1.0, y: 1.0)
            case .custom(let angle):
                calculatePoints(for: angle, gradient: gradient)
            default:
                break
        }

        gradient.frame = self.bounds
        self.layer.insertSublayer(gradient, at: 0)

        guard let cornerRadius = cornerRadius else { return }

        let circularPath = CGMutablePath()

        circularPath.move(to: CGPoint.init(x: cornerRadius, y: 0))
        circularPath.addLine(to: CGPoint.init(x: self.bounds.width - cornerRadius, y: 0))
        circularPath.addQuadCurve(to: CGPoint.init(x: self.bounds.width, y: cornerRadius), control: CGPoint.init(x: self.bounds.width, y: 0))
        circularPath.addLine(to: CGPoint.init(x: self.bounds.width, y: self.bounds.height - cornerRadius))
        circularPath.addQuadCurve(to: CGPoint.init(x: self.bounds.width - cornerRadius, y: self.bounds.height), control: CGPoint.init(x: self.bounds.width, y: self.bounds.height))
        circularPath.addLine(to: CGPoint.init(x: cornerRadius, y: self.bounds.height))
        circularPath.addQuadCurve(to: CGPoint.init(x: 0, y: self.bounds.height - cornerRadius), control: CGPoint.init(x: 0, y: self.bounds.height))
        circularPath.addLine(to: CGPoint.init(x: 0, y: cornerRadius))
        circularPath.addQuadCurve(to: CGPoint.init(x: cornerRadius, y: 0), control: CGPoint.init(x: 0, y: 0))


        let maskLayer = CAShapeLayer()
        maskLayer.path = circularPath
        maskLayer.fillRule = CAShapeLayerFillRule.evenOdd
        maskLayer.fillColor = UIColor.red.cgColor
        self.layer.mask = maskLayer

        apply?(self)
    }

    func calculatePoints(for angle: CGFloat, gradient: CAGradientLayer) {

        var ang = (-angle).truncatingRemainder(dividingBy: 360)
        if ang < 0 { ang = 360 + ang }
        let n: CGFloat = 0.5

        switch ang {
            case 0...45, 315...360:
                let a = CGPoint(x: 0, y: n * tanx(ang) + n)
                let b = CGPoint(x: 1, y: n * tanx(-ang) + n)
                gradient.startPoint = a
                gradient.endPoint = b
            case 45...135:
                let a = CGPoint(x: n * tanx(ang - 90) + n, y: 1)
                let b = CGPoint(x: n * tanx(-ang - 90) + n, y: 0)
                gradient.startPoint = a
                gradient.endPoint = b
            case 135...225:
                let a = CGPoint(x: 1, y: n * tanx(-ang) + n)
                let b = CGPoint(x: 0, y: n * tanx(ang) + n)
                gradient.startPoint = a
                gradient.endPoint = b
            case 225...315:
                let a = CGPoint(x: n * tanx(-ang - 90) + n, y: 0)
                let b = CGPoint(x: n * tanx(ang - 90) + n, y: 1)
                gradient.startPoint = a
                gradient.endPoint = b
            default:
                let a = CGPoint(x: 0, y: n)
                let b = CGPoint(x: 1, y: n)
                gradient.startPoint = a
                gradient.endPoint = b

        }
    }
    
    private func tanx(_ 𝜽: CGFloat) -> CGFloat {
        return tan(𝜽 * CGFloat.pi / 180)
    }
    
    public func buttonAnimationPopUp(_ view: UIView) {
        
        UIView.animate(withDuration: 2.5,
                       delay: 0,
                       usingSpringWithDamping: CGFloat(0.20),
                       initialSpringVelocity: CGFloat(9.0),
                       options: UIView.AnimationOptions.allowUserInteraction,
                       animations: {
                        view.transform = CGAffineTransform.identity
                       },
                       completion: { Void in()  })
    }
    
    public var width: CGFloat {
        return self.frame.size.width
    }
    
    public var height: CGFloat {
        return self.frame.size.height
    }
    
    public var top: CGFloat {
        return self.frame.origin.y
    }
    
    public var bottom: CGFloat {
        return self.frame.size.height + self.frame.origin.y
    }
    
    public var right: CGFloat {
        return self.frame.size.width + self.frame.origin.x
    }
    
    public var left: CGFloat {
        return self.frame.origin.x
    }
}

extension Notification.Name {
    static let didLogInNotification = Notification.Name("didLogInNotification")
}

extension UIButton {
//    @IBInspectable
    var letterSpacing: CGFloat {
        set {
            let attributedString: NSMutableAttributedString
            if let currentAttrString = attributedTitle(for: .normal) {
                attributedString = NSMutableAttributedString(attributedString: currentAttrString)
            }
            else {
                attributedString = NSMutableAttributedString(string: self.title(for: .normal) ?? "")
                setTitle(.none, for: .normal)
            }

            attributedString.addAttribute(NSAttributedString.Key.kern, value: newValue, range: NSRange(location: 0, length: attributedString.length))
            setAttributedTitle(attributedString, for: .normal)
        }
        get {
            if let currentLetterSpace = attributedTitle(for: .normal)?.attribute(NSAttributedString.Key.kern, at: 0, effectiveRange: .none) as? CGFloat {
                return currentLetterSpace
            }
            else {
                return 0
            }
        }
    }
}

extension UILabel {
    var maxNumberOfLines: Int {
        let maxSize = CGSize(width: frame.size.width, height: CGFloat(MAXFLOAT))
        let text = (self.text ?? "") as NSString
        let textHeight = text.boundingRect(with: maxSize, options: .usesLineFragmentOrigin, attributes: [.font: font ?? ""], context: nil).height
        let lineHeight = font.lineHeight
        return Int(ceil(textHeight / lineHeight))
    }
    
    func setTextSpacingBy(value: Double) {
        if let textString = self.text {
          let attributedString = NSMutableAttributedString(string: textString)
            attributedString.addAttribute(NSAttributedString.Key.kern, value: value, range: NSRange(location: 0, length: attributedString.length - 1))
          attributedText = attributedString
        }
      }
}

extension UIImage {
    func rotate(radians: Float) -> UIImage? {
        var newSize = CGRect(origin: CGPoint.zero, size: self.size).applying(CGAffineTransform(rotationAngle: CGFloat(radians))).size
        // Trim off the extremely small float value to prevent core graphics from rounding it up
        newSize.width = floor(newSize.width)
        newSize.height = floor(newSize.height)
        
        UIGraphicsBeginImageContextWithOptions(newSize, false, self.scale)
        let context = UIGraphicsGetCurrentContext()!
        
        // Move origin to middle
        context.translateBy(x: newSize.width/2, y: newSize.height/2)
        // Rotate around middle
        context.rotate(by: CGFloat(radians))
        // Draw the image at its center
        self.draw(in: CGRect(x: -self.size.width/2, y: -self.size.height/2, width: self.size.width, height: self.size.height))
        
        let newImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return newImage
    }
    
    public var lineLocationImage: UIImage {
        return (UIImage(named: "line")?.withTintColor(.systemTeal, renderingMode:.alwaysOriginal))! }
    
    public var imageLocationCircle: UIImage {
        return (UIImage(systemName: "location.circle", withConfiguration: UIImage.SymbolConfiguration(weight: .regular))?.withTintColor(.systemTeal, renderingMode: .alwaysOriginal).rotate(radians: .pi))! }
    
    public var imageMapPointEllipse: UIImage {
        return (UIImage(systemName: "mappin.and.ellipse", withConfiguration: UIImage.SymbolConfiguration(weight: .regular))?.withTintColor(.systemTeal, renderingMode: .alwaysOriginal))! }
}

extension UIColor {
    public func hexStringToUIColor (hex:String) -> UIColor {
        var cString:String = hex.trimmingCharacters(in: .whitespacesAndNewlines).uppercased()

        if (cString.hasPrefix("#")) {
            cString.remove(at: cString.startIndex)
        }

        if ((cString.count) != 6) {
            return UIColor.gray
        }

        var rgbValue:UInt64 = 0
        Scanner(string: cString).scanHexInt64(&rgbValue)

        return UIColor(
            red: CGFloat((rgbValue & 0xFF0000) >> 16) / 255.0,
            green: CGFloat((rgbValue & 0x00FF00) >> 8) / 255.0,
            blue: CGFloat(rgbValue & 0x0000FF) / 255.0,
            alpha: CGFloat(1.0)
        )
    }
}

