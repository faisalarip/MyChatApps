//
//  CustomView.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit

extension UIView {
    
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

struct IconTextViewModel {
    let title: String?
    let icon: UIImage?
    let backgroundColor: UIColor?
}

final class CustomButton: UIButton {
    
    private let title: UILabel = {
        let label = UILabel()
        label.numberOfLines = 0
        label.textAlignment = .center
        label.font = .systemFont(ofSize: 18, weight: .regular)
        label.textColor = .systemTeal
        return label
    }()
    
    private let iconView: UIImageView = {
        let iconView = UIImageView()
        iconView.clipsToBounds = true
        iconView.contentMode = .scaleAspectFit
        iconView.tintColor = .systemBackground
        return iconView
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        addSubview(title)
        addSubview(iconView)
        layer.shadowColor = UIColor.black.cgColor
        layer.shadowOffset = CGSize(width: 0.0, height: 2.0)
        layer.shadowRadius = 8
        layer.shadowOpacity = 0.4
        layer.masksToBounds = false
        layer.cornerRadius = 8
        layer.borderWidth = 1
        layer.borderColor = UIColor.tertiarySystemBackground.cgColor
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    public func configure(with viewModel: IconTextViewModel) {
        title.text = viewModel.title
        iconView.image = viewModel.icon
        self.backgroundColor = .tertiarySystemBackground
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        title.sizeToFit()
        let iconSize: CGFloat = 25
        let iconRightPadding: CGFloat = 12
        let iconX: CGFloat = (frame.size.width - title.frame.size.width - iconSize - 5) / 2
        
        iconView.frame = CGRect(x: iconX,
                                y: (frame.size.height-iconSize)/2,
                                width: iconSize,
                                height: iconSize)
        title.frame = CGRect(x: iconX + iconSize + iconRightPadding,
                             y: 0,
                             width: title.frame.size.width,
                             height: frame.size.height)
    }
}
