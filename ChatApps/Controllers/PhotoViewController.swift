//
//  PhotoViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import SDWebImage

class PhotoViewController: UIViewController {
    
    private var imageUrl: URL?
    
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
        
        view.backgroundColor = .black
        title = "Photo"
        navigationItem.largeTitleDisplayMode = .never
        view.addSubview(imageView)
        imageView.sd_setImage(with: imageUrl, completed: nil)
        
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        imageView.frame = view.bounds
    }
    
}
