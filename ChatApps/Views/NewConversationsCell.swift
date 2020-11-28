//
//  NewConversationsCell.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/11/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit

import SDWebImage

class NewConversationsCell: UITableViewCell {
    
    static let identifier = "NewConversationsCell"
    
    private let userImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.contentMode = .scaleAspectFill
        imageView.layer.cornerRadius = 25
        imageView.layer.masksToBounds = true
        return imageView
    }()
    
    private let userNameLabel: UILabel = {
        let namelabel = UILabel()
        namelabel.font = .systemFont(ofSize: 16, weight: .regular)
        return namelabel
    }()
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        contentView.addSubview(userImageView)
        contentView.addSubview(userNameLabel)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        
        userImageView.frame = CGRect(x: 15,
                                     y: 15,
                                     width: 50,
                                     height: 50)
        userNameLabel.frame = CGRect(x: userImageView.right + 15,
                                     y: 5,
                                     width: contentView.width - 10 - userImageView.width,
                                     height: (contentView.height-10))
        
    }
    
    func configure(with model: SearchResult) {
        self.userNameLabel.text = model.name        
        
        let pathImage = "image/\(model.recepientEmail)_profile_picture.png"
        StorageManager.shared.downloadURL(for: pathImage) { (result) in
            switch result {
                case .success(let url):
                    
                    DispatchQueue.main.async {
                        self.userImageView.sd_setImage(with: url, completed: nil)
                    }
                    
                    break
                case .failure(let error):
                print("Failed to get image url \(error)")
            }
            
        }
    }
}
