//
//  PhotoViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import SDWebImage
import JGProgressHUD

final class PhotoViewController: UIViewController {
    
    private var imageUrl: URL?
    private let spinner = JGProgressHUD(style: .dark)
    
    init(with url: URL) {
        imageUrl = url
        super.init(nibName: nil, bundle: nil)
    }
    
    let imageView: UIImageView = {
        let imageView = UIImageView()
        imageView.contentMode = .scaleAspectFit
        return imageView
    }()
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.backgroundColor = .systemBackground
        title = "Photo"
        
        let shareImage = UIImage(systemName: "arrowshape.turn.up.right.fill", withConfiguration: UIImage.SymbolConfiguration(weight: .heavy))?.withTintColor(.systemYellow, renderingMode: .alwaysOriginal)
        let editButton = UIBarButtonItem(image: shareImage, style: .done, target: self, action: #selector(didTapShareButton(_:)))
        
        navigationItem.largeTitleDisplayMode = .never
        navigationItem.rightBarButtonItem = editButton
        view.addSubview(imageView)
        imageView.sd_setImage(with: imageUrl, completed: nil)
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        imageView.frame = view.bounds
    }
    
    @objc private func didTapShareButton(_ sender: UIButton) {
        
        if let imageURL = imageUrl, let photo = imageView.image {
            let actionVC = UIActivityViewController(activityItems: [
                photo,
                imageURL
            ], applicationActivities: nil)
            
            // Ipad support
            actionVC.popoverPresentationController?.sourceView = sender
            actionVC.popoverPresentationController?.sourceRect = sender.frame
            // present action sheet
            self.present(actionVC, animated: true, completion: nil)
        }
        
    }
    
}
