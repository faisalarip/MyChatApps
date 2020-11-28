//
//  ConversationCell.swift
//  ChatApps
//
//  Created by Faisal Arif on 03/08/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import SDWebImage

class ConversationCell: UITableViewCell {
    
    static let identifier = "ConversationCell"
    
    private let userImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.contentMode = .scaleAspectFill
        imageView.layer.cornerRadius = 25
        imageView.layer.masksToBounds = true
        return imageView
    }()
    
    private let userNameLabel: UILabel = {
        let namelabel = UILabel()
        namelabel.font = .systemFont(ofSize: 18, weight: .medium)
        return namelabel
    }()
    
    private let userMessageLabel: UILabel = {
        let messageLabel = UILabel()
        messageLabel.font = .systemFont(ofSize: 14, weight: .regular)
<<<<<<< HEAD
        messageLabel.numberOfLines = 0
=======
        messageLabel.numberOfLines = 2
>>>>>>> origin/develop12
        return messageLabel
    }()
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        contentView.addSubview(userImageView)
        contentView.addSubview(userNameLabel)
        contentView.addSubview(userMessageLabel)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        
        userImageView.frame = CGRect(x: 10,
                                     y: 10,
                                     width: 50,
                                     height: 50)
<<<<<<< HEAD
        userNameLabel.frame = CGRect(x: userImageView.right + 5,
                                     y: 5,
                                     width: contentView.width - 10 - userImageView.width,
                                     height: (contentView.height-10)/2)
        userMessageLabel.frame = CGRect(x: userImageView.right + 5,
                                        y: userNameLabel.bottom + 5,
                                        width: contentView.width - 10 - userImageView.width,
                                        height: (contentView.height-10)/2)
=======
        userNameLabel.frame = CGRect(x: userImageView.right + 15,
                                     y: 15,
                                     width: contentView.width - 10 - userImageView.width,
                                     height: (contentView.height-5)/4)
        userMessageLabel.frame = CGRect(x: userImageView.right + 15,
                                        y: userNameLabel.bottom + 5,
                                        width: contentView.width - 24 - userImageView.width,
                                        height: (contentView.height-5)/4)
>>>>>>> origin/develop12
        
    }
    
    func configure(with model: Conversation) {
        self.userNameLabel.text = model.name
        self.userMessageLabel.text = model.latestMessage.text
        
        let pathImage = "image/\(model.otherUserEmail)_profile_picture.png"
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
