//
//  ChatViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 26/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import MessageKit
import InputBarAccessoryView
import SDWebImage
import AVFoundation
import AVKit
import CoreLocation

struct Message: MessageType {
    public var sender: SenderType
    public var messageId: String
    public var sentDate: Date
    public var kind: MessageKind
}

extension MessageKind {
    var messageKindString: String {
        switch self {
            case .text(_):
            return "text"
            case .attributedText(_):
            return "attributed_text"
            case .photo(_):
            return "photo"
            case .video(_):
            return "video"
            case .location(_):
            return "location"
            case .emoji(_):
            return "emoji"
            case .audio(_):
            return "audio"
            case .contact(_):
            return "contact"
            case .custom(_):
            return "custom"
            case .linkPreview(_):
            return "linkPreview"
        }
    }
}

struct Sender: SenderType {
    var senderId: String
    var photoURL: String
    var displayName: String
}

struct Location: LocationItem {
    var location: CLLocation
    var size: CGSize
}

struct Media: MediaItem {
    var url: URL?
    var image: UIImage?
    var placeholderImage: UIImage
    var size: CGSize
}

class ChatViewController: MessagesViewController {
    
    public let otherUserEmail: String
    public var isNewConversation = false
    private var conversationsId: String?
    
    private var senderPhotoProfileUrl: URL?
    private var recepientPhotoProfileUrl: URL?
    
    public static let dateFormatter: DateFormatter = {
        let dateFormatter = DateFormatter()
        dateFormatter.dateStyle = .medium
        dateFormatter.timeStyle = .long
        dateFormatter.timeZone = .current
        return dateFormatter
    }()
    
    private var messages = [Message]()
    
    private var selfSender: Sender? {
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else {
            return nil
        }
        let safeEmail = DatabaseManager.shared.safeEmail(with: email)
        return Sender(senderId: safeEmail, photoURL: "", displayName: "Me")
    }
    
    init(with email: String, id: String?) {
        self.otherUserEmail = email
        self.conversationsId = id
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        messagesCollectionView.messagesDataSource = self
        messagesCollectionView.messagesLayoutDelegate = self
        messagesCollectionView.messagesDisplayDelegate = self
        messagesCollectionView.messageCellDelegate = self
        messagesCollectionView.delegate = self
        messageInputBar.delegate = self
        
        setupInputButton()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        messageInputBar.inputTextView.becomeFirstResponder()
        if let convoId = conversationsId {
            listenForMessage(convoId, true)
        }
    }
    
    private func setupInputButton() {
        
        let button = InputBarButtonItem()
        button.setSize(CGSize(width: 34, height: 34), animated: true)
        button.setImage(UIImage(systemName: "paperclip"), for: .normal)
        button.onTouchUpInside { [weak self] (_) in
            self?.presentInputActionSheet()
        }
        messageInputBar.setLeftStackViewWidthConstant(to: 36, animated: true)
        messageInputBar.setStackViewItems([button], forStack: .left, animated: true)
        
    }
    
    private func presentInputActionSheet() {
        let alert = UIAlertController(title: "Attach Media", message: "What would you like to attach?", preferredStyle: .actionSheet)
        alert.addAction(UIAlertAction(title: "Photo", style: .default, handler: { [weak self] (_) in
            self?.presentPhotoInputActionSheet()
        }))
        alert.addAction(UIAlertAction(title: "Video", style: .default, handler: { [weak self] (_) in
            self?.presentVideoInputActionSheet()
        }))
        alert.addAction(UIAlertAction(title: "Audio", style: .default, handler: { (_) in
            
        }))
        alert.addAction(UIAlertAction(title: "Location", style: .default, handler: { [weak self] (_) in
            self?.presentMapActionSheet()
        }))
        alert.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
        
        present(alert, animated: true)
    }
    
    private func presentPhotoInputActionSheet() {
        let alert = UIAlertController(title: "Attach Photo", message: "Where would you like to attach a photo from?", preferredStyle: .actionSheet)
        alert.addAction(UIAlertAction(title: "Camera", style: .default, handler: { [weak self] (_) in
            let imagePicker = UIImagePickerController()
            imagePicker.sourceType = .camera
            imagePicker.allowsEditing = true
            imagePicker.delegate = self
            self?.present(imagePicker, animated: true)
        }))
        alert.addAction(UIAlertAction(title: "Photo Library", style: .default, handler: { [weak self] (_) in
            let imagePicker = UIImagePickerController()
            imagePicker.sourceType = .photoLibrary
            imagePicker.allowsEditing = true
            imagePicker.delegate = self
            self?.present(imagePicker, animated: true)
        }))
        alert.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
        
        present(alert, animated: true)
    }
    
    private func presentVideoInputActionSheet() {
        let alert = UIAlertController(title: "Attach Video", message: "Where would you like to attach a video from?", preferredStyle: .actionSheet)
        alert.addAction(UIAlertAction(title: "Camera", style: .default, handler: { [weak self] (_) in
            let imagePicker = UIImagePickerController()
            imagePicker.sourceType = .camera
            imagePicker.allowsEditing = true
            imagePicker.mediaTypes = ["public.movie"]
            imagePicker.videoQuality = .typeMedium
            imagePicker.delegate = self
            self?.present(imagePicker, animated: true)
        }))
        alert.addAction(UIAlertAction(title: "Video Library", style: .default, handler: { [weak self] (_) in
            let imagePicker = UIImagePickerController()
            imagePicker.sourceType = .photoLibrary
            imagePicker.allowsEditing = true
            imagePicker.mediaTypes = ["public.movie"]
            imagePicker.videoQuality = .typeMedium
            imagePicker.delegate = self
            self?.present(imagePicker, animated: true)
        }))
        alert.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
        
        present(alert, animated: true)
    }
    
    private func presentMapActionSheet() {
        let locationVC = LocationPickerVC(coordinates: nil)
        locationVC.navigationItem.largeTitleDisplayMode = .never
        locationVC.completion = { [weak self] selectedLocation in
            
            guard  let strongSelf = self,
                   let messageId = strongSelf.createMessageId(),
                   let convoId = strongSelf.conversationsId,
                   let otherUserName = strongSelf.title,
                   let selfSender = strongSelf.selfSender else {
                return
            }
            
            let longitude: Double = selectedLocation.longitude
            let latitude: Double = selectedLocation.latitude
            print("get a coordinate location, long: \(longitude) | lat: \(latitude)")
            
            let location = Location(location: CLLocation(latitude: latitude, longitude: longitude),
                                 size: .zero)
            
            let message = Message(sender: selfSender,
                                  messageId: messageId,
                                  sentDate: Date(),
                                  kind: .location(location))
            
            DatabaseManager.shared.sendMessages(to: convoId, otherUserEmail: strongSelf.otherUserEmail, name: otherUserName, newMessage: message) { [weak self] (success) in
                if success {
                    self?.messageInputBar.inputTextView.text = nil
                    print("Success sent location message")
                } else {
                    print("Failed to send a location message")
                }
            }
            
        }
        navigationController?.pushViewController(locationVC, animated: true)
    }
    
    private func listenForMessage(_ id: String,_ shouldSCrollToBottom: Bool) {
        DatabaseManager.shared.getAllMessageForConversation(with: id) { [weak self] (result) in
            switch result {
                case .success(let messages):
                    guard !messages.isEmpty, let strongSelf = self else {
                        print("messages Empty")
                        return
                    }
                    self?.messages = messages
                    DispatchQueue.main.async {
                        strongSelf.messagesCollectionView.reloadDataAndKeepOffset()
                        if shouldSCrollToBottom {
                            strongSelf.messagesCollectionView.scrollToLastItem()
                        }
                    }
            case .failure(let error):
                print("Failed to get message \(error)")
            }
        }
    }
}

extension ChatViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    
    func imagePickerControllerDidCancel(_ picker: UIImagePickerController) {
        picker.dismiss(animated: true, completion: nil)
    }
    
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        picker.dismiss(animated: true, completion: nil)
        guard let messageId = createMessageId(),
              let convoId = conversationsId,
              let otherUserName = self.title,
              let selfSender = selfSender else {
            return
        }
        
        let imageFileName = "photo_message_" + messageId.replacingOccurrences(of: " ", with: "-") + ".png"
        
        if let image = info[.editedImage] as? UIImage, let imagePngData = image.pngData() {
            // Upload image to firebase storage
            StorageManager.shared.uploadPhotosMessage(with: imagePngData, filename: imageFileName) { [weak self] (result) in
                guard let strongSelf = self else { return }
                switch result {
                case .success(let urlString):
                    // Send photo message
                    print("Uploaded photo message \(urlString)")
                    
                    guard let url = URL(string: urlString),
                          let placeholder = UIImage(systemName: "photo.on.rectangle", withConfiguration: UIImage.SymbolConfiguration(weight: .thin))?.withTintColor(.systemGreen ,renderingMode: .alwaysOriginal) else { return }
                    
                    let media = Media(url: url,
                                      image: nil,
                                      placeholderImage: placeholder,
                                      size: .zero)
                    
                    let message = Message(sender: selfSender,
                                          messageId: messageId,
                                          sentDate: Date(),
                                          kind: .photo(media))
                    
                    DatabaseManager.shared.sendMessages(to: convoId, otherUserEmail: strongSelf.otherUserEmail, name: otherUserName, newMessage: message) { (success) in
                        if success {
                            self?.messageInputBar.inputTextView.text = nil
                            print("Success sent message")
                        } else {
                            print("Failed to send a photo message")
                        }
                    }
                case .failure(let error):
                    print("Upload photo message error with \(error)")
                }
            }
        } else if let videoUrl = info[.mediaURL] as? URL {
            // Upload video to firebase storage
            
            let videoFileName = "video_message_" + messageId.replacingOccurrences(of: " ", with: "-") + ".mov"
            
            StorageManager.shared.uploadVideosMessage(with: videoUrl, filename: videoFileName) { [weak self] (result) in
                guard let strongSelf = self else { return }
                switch result {
                case .success(let urlString):
                    // Send video message
                    print("Uploaded video message \(urlString)")
                    
                    guard let url = URL(string: urlString),
                          let placeholder = UIImage(systemName: "photo.on.rectangle", withConfiguration: UIImage.SymbolConfiguration(weight: .thin))?.withTintColor(.systemGreen ,renderingMode: .alwaysOriginal) else { return }
                    
                    let media = Media(url: url,
                                      image: nil,
                                      placeholderImage: placeholder,
                                      size: .zero)
                    
                    let message = Message(sender: selfSender,
                                          messageId: messageId,
                                          sentDate: Date(),
                                          kind: .video(media))
                    
                    DatabaseManager.shared.sendMessages(to: convoId, otherUserEmail: strongSelf.otherUserEmail, name: otherUserName, newMessage: message) { (success) in
                        if success {
                            self?.messageInputBar.inputTextView.text = nil
                            print("Success sent message")
                        } else {
                            print("Failed to send a photo message")
                        }
                    }
                case .failure(let error):
                    print("Upload photo message error with \(error)")
                }
            }
        }
        
    }
    
}

extension ChatViewController: InputBarAccessoryViewDelegate {
    func inputBar(_ inputBar: InputBarAccessoryView, didPressSendButtonWith text: String) {
        guard !text.isEmpty, let selfSender = self.selfSender, let messageId = createMessageId() else { return }
        
        let message = Message(sender: selfSender,
                              messageId: messageId,
                              sentDate: Date(),
                              kind: .text(text))
        
        if isNewConversation {
            // Create new convo in database
            DatabaseManager.shared.createNewConversation(with: self.title ?? "user", otherUserEmail: otherUserEmail, firstMessage: message) { [weak self] (success) in
                if success {
                    print("success sent a message")
                    self?.isNewConversation = false
                    let newConversationId = "conversation_\(message.messageId)"
                    self?.conversationsId = newConversationId
                    self?.listenForMessage(newConversationId, true)
                    
                } else {
                    print("failed sent a message")
                }                
            }
        } else {
//             Append to existing conversation data
            guard let conversationId = conversationsId, let name = self.title else { return }
            DatabaseManager.shared.sendMessages(to: conversationId, otherUserEmail: otherUserEmail, name: name,newMessage: message) { [weak self] (succes) in
                if succes {
                    self?.messageInputBar.inputTextView.text = nil
                    print("message sent")
                } else {
                    print("Failed send a message")
                }
            }
        }
    }
    
    private func createMessageId() -> String? {
        // Date, otherUserEmail, senderEmail, randomInt
        guard let currentUserEmail = UserDefaults.standard.value(forKey: "email") as? String else { return nil }
        let safeCurrentEmail = DatabaseManager.shared.safeEmail(with: currentUserEmail)
        
        let dateString = Self.dateFormatter.string(from: Date())
        let newIdentifier = "\(otherUserEmail)_\(safeCurrentEmail)_\(dateString)"
        print("create new message \(newIdentifier)")
        return newIdentifier
    }
}

extension ChatViewController: MessagesDataSource, MessagesLayoutDelegate, MessagesDisplayDelegate {
    
    func currentSender() -> SenderType {
        if let sender = selfSender {
            return sender
        }
        fatalError("Self sender is nil, email should be chaced")
    }
    
    func messageForItem(at indexPath: IndexPath, in messagesCollectionView: MessagesCollectionView) -> MessageType {
        return messages[indexPath.section]
    }
    
    func numberOfSections(in messagesCollectionView: MessagesCollectionView) -> Int {
        return messages.count
    }
    
    func configureMediaMessageImageView(_ imageView: UIImageView, for message: MessageType, at indexPath: IndexPath, in messagesCollectionView: MessagesCollectionView) {
        guard let message = message as? Message else { return }
        
        switch message.kind {
        case .photo(let media):
            guard let imageUrl = media.url else { return }
            imageView.sd_setImage(with: imageUrl, completed: nil)
        default:
            break
        }
    }
    
    func backgroundColor(for message: MessageType, at indexPath: IndexPath, in messagesCollectionView: MessagesCollectionView) -> UIColor {
        let senderId = message.sender.senderId
        
        if selfSender?.senderId == senderId {
            return .systemBlue
        }
        return .secondarySystemBackground
    }
    
    func configureAvatarView(_ avatarView: AvatarView, for message: MessageType, at indexPath: IndexPath, in messagesCollectionView: MessagesCollectionView) {
        let senderId = message.sender.senderId
        
        if selfSender?.senderId == senderId {
            //show sender photo profile
            
            if senderPhotoProfileUrl != nil {
                avatarView.sd_setImage(with: senderPhotoProfileUrl, completed: nil)
            } else {
                
                guard let currentUserEmail = UserDefaults.standard.value(forKey: "email") as? String else { return }
                let safeCurrentEmail = DatabaseManager.shared.safeEmail(with: currentUserEmail)
                let fileName = "image/\(safeCurrentEmail)_profile_picture.png"
                
                StorageManager.shared.downloadURL(for: fileName) { [weak self] (result) in
                    switch result {
                    case .success(let photoUrl):
                        self?.senderPhotoProfileUrl = photoUrl
                        DispatchQueue.main.async {
                            avatarView.sd_setImage(with: photoUrl, completed: nil)
                        }
                    case .failure(let error):
                        print("Failed get a photo profile url \(error)")
                    }
                    
                }
            }
            
        } else {
            // show recepient photo profile
            if recepientPhotoProfileUrl != nil {
                avatarView.sd_setImage(with: recepientPhotoProfileUrl, completed: nil)
            } else {
                let fileName = "image/\(otherUserEmail)_profile_picture.png"
                StorageManager.shared.downloadURL(for: fileName) { [weak self] (result) in
                    switch result {
                    case .success(let photoUrl):
                        self?.recepientPhotoProfileUrl = photoUrl
                        DispatchQueue.main.async {
                            avatarView.sd_setImage(with: photoUrl, completed: nil)
                        }
                    case .failure(let error):
                        print("Failed get a photo profile url \(error)")
                    }
                    
                }
                
            }
        }
        
    }
}

extension ChatViewController: MessageCellDelegate {
    
    func didTapMessage(in cell: MessageCollectionViewCell) {
        guard let indexPath = messagesCollectionView.indexPath(for: cell) else { return }
        
        let message = messages[indexPath.section]
        switch message.kind {
        case .location(let locationData):
            let coordinates = locationData.location.coordinate
            let locationVC = LocationPickerVC(coordinates: coordinates)
            locationVC.title = "Location"
            
            self.navigationController?.pushViewController(locationVC, animated: true)
        default:
            break
        }
    }
    
    func didTapImage(in cell: MessageCollectionViewCell) {
        guard let indexPath = messagesCollectionView.indexPath(for: cell) else { return }
        
        let message = messages[indexPath.section]
        switch message.kind {
        case .photo(let media):
            guard let imageUrl = media.url else { return }
            
            let vc = PhotoViewController(with: imageUrl)
            self.navigationController?.pushViewController(vc, animated: true)
        case .video(let media):
            guard let videoUrl = media.url else { return }
            
            let video = AVPlayerViewController()
            video.player = AVPlayer(url: videoUrl)
            video.player?.play()
            present(video, animated: true, completion: nil)
            
        default:
            break
        }
    }
    
}
