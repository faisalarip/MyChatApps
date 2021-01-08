//
//  CustomView.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import MapKit

// MARK: - Custom View for NavigationCarView's

class CustomView: UIView {
    
    let placeImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.translatesAutoresizingMaskIntoConstraints = false
        imageView.contentMode = .scaleAspectFill
        imageView.clipsToBounds = true
        imageView.backgroundColor = .systemBackground
        return imageView
    }()
    
    let titleTextField: UITextField = {
        let textField = UITextField()
        textField.translatesAutoresizingMaskIntoConstraints = false
        textField.placeholder = "Search place or address"
        textField.keyboardType = UIKeyboardType.default
        textField.returnKeyType = UIReturnKeyType.done
        textField.autocorrectionType = UITextAutocorrectionType.no
        textField.font = UIFont.systemFont(ofSize: 13, weight: .medium)
        textField.borderStyle = UITextField.BorderStyle.roundedRect
        textField.clearButtonMode = UITextField.ViewMode.whileEditing;
        textField.contentVerticalAlignment = UIControl.ContentVerticalAlignment.center
        return textField
    }()
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        
        let stackView = UIStackView(arrangedSubviews: [placeImageView, titleTextField])
        stackView.translatesAutoresizingMaskIntoConstraints = false
        stackView.axis = .horizontal
        stackView.alignment = .center
        stackView.spacing = 15
        
        addSubview(stackView)
        
        NSLayoutConstraint.activate([
            titleTextField.heightAnchor.constraint(equalToConstant: 40),

            placeImageView.widthAnchor.constraint(equalToConstant: 22),
            placeImageView.heightAnchor.constraint(equalToConstant: 22),

            stackView.topAnchor.constraint(equalTo: self.topAnchor),
            stackView.bottomAnchor.constraint(equalTo: self.bottomAnchor),
            stackView.leadingAnchor.constraint(equalTo: self.leadingAnchor),
            stackView.trailingAnchor.constraint(equalTo: self.trailingAnchor)

        ])
        
    }
    
    public func configureCustomView(with image: UIImage, placeName: String?) {
        self.placeImageView.image = image
        self.titleTextField.text = placeName
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    
}

struct IconTextViewModel {
    let title: String?
    let icon: UIImage?
    let backgroundColor: UIColor?
}

// MARK: - Custom Button for GetCurrentLocationButton's

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
        layer.shadowOpacity = 0.3
        layer.masksToBounds = false
        layer.cornerRadius = 8        
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

// MARK: - SnapshotAnnotationView for Custom Callout

class SnapshotAnnotationView: MKPinAnnotationView {
    override var annotation: MKAnnotation? { didSet { configureDetailView() } }

    override init(annotation: MKAnnotation?, reuseIdentifier: String?) {
        super.init(annotation: annotation, reuseIdentifier: reuseIdentifier)
        configure()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        configure()
    }
    
    func configure() {
        canShowCallout = true
        configureDetailView()
    }

    func configureDetailView() {
        guard let annotation = annotation else { return }

        let rect = CGRect(origin: .zero, size: CGSize(width: 300, height: 200))

        let snapshotView = UIView()
        snapshotView.translatesAutoresizingMaskIntoConstraints = false

        let options = MKMapSnapshotter.Options()
        options.size = rect.size
        options.mapType = .hybridFlyover
        options.showsBuildings = true
        options.camera = MKMapCamera(lookingAtCenter: annotation.coordinate, fromDistance: 250, pitch: 65, heading: 0)

        let snapshotter = MKMapSnapshotter(options: options)
        snapshotter.start { snapshot, error in
            guard let snapshot = snapshot, error == nil else {
                print(error ?? "Unknown error")
                return
            }

            let imageView = UIImageView(frame: rect)
            imageView.image = snapshot.image
            snapshotView.addSubview(imageView)
        }

        detailCalloutAccessoryView = snapshotView
        
        NSLayoutConstraint.activate([
            snapshotView.widthAnchor.constraint(equalToConstant: rect.width),
            snapshotView.heightAnchor.constraint(equalToConstant: rect.height)
        ])
    }
}

