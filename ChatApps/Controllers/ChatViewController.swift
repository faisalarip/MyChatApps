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
        }
    }
}

struct Sender: SenderType {
    var senderId: String
    var photoURL: String
    var displayName: String
}


class ChatViewController: MessagesViewController {
    
    public let otherUserEmail: String
    public var isNewConversation = false
    private var conversationsId: String?
    
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
        
        view.backgroundColor = .red
        
        messagesCollectionView.messagesDataSource = self
        messagesCollectionView.messagesLayoutDelegate = self
        messagesCollectionView.messagesDisplayDelegate = self
        messageInputBar.delegate = self
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        messageInputBar.inputTextView.becomeFirstResponder()
        if let convoId = conversationsId {
            listenForMessage(convoId, true)
        }
    }
    
    private func listenForMessage(_ id: String, _ shouldSCrollToBottom: Bool) {
        DatabaseManager.shared.getAllMessageForConversation(with: id) { [weak self] (result) in
            switch result {
                case .success(let messages):
                    guard !messages.isEmpty else { return }
                    self?.messages = messages
                    
                    DispatchQueue.main.async {
                        if shouldSCrollToBottom {
                            self?.messagesCollectionView.scrollToLastItem()
                        }
                        self?.messagesCollectionView.reloadDataAndKeepOffset()
                    }
                case .failure(let error):
                    print("Failed to get message \(error)")
            }
        }
    }
}

extension ChatViewController: InputBarAccessoryViewDelegate {
    func inputBar(_ inputBar: InputBarAccessoryView, didPressSendButtonWith text: String) {
        guard !text.isEmpty, let selfSender = self.selfSender, let messageId = createMessageId() else { return }
        
        if isNewConversation {
            // Create new convo in database
            let message = Message(sender: selfSender, messageId: messageId, sentDate: Date(), kind: .text(text))
            DatabaseManager.shared.createNewConversation(with: self.title ?? "user", otherUserEmail: otherUserEmail, firstMessage: message) { (success) in
                if success {
                    print("success sent a message")
                } else {
                    print("failed sent a message")
                }                
            }
        } else {
            // Append to existing conversation data
        }
    }
    
    private func createMessageId() -> String? {
        // Date, otherUserEmail, senderEmail, randomInt
        guard let currentUserEmail = UserDefaults.standard.value(forKey: "email") as? String else {
            return nil
        }
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
    
}
