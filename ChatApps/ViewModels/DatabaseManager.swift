//
//  DatabaseManager.swift
//  ChatApps
//
//  Created by Faisal Arif on 24/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseDatabase
import MessageKit
import CoreLocation

/// Manager object to write and read data to real time firebase database
final class DatabaseManager {
    
    /// Shared instance of class
    static let shared = DatabaseManager()
    
    private let database = Database.database().reference()
    
    public func safeEmail(with emailAddress: String) -> String {
        var safeEmail = emailAddress.replacingOccurrences(of: "@", with: "-")
        safeEmail = safeEmail.replacingOccurrences(of: ".", with: "-")
        return safeEmail
    }
    
    public func getDataCurrentUser(safeEmail: String, completion: @escaping (Result<Any, Error>) -> Void) {
        self.database.child("users_conversation/\(safeEmail)").observeSingleEvent(of: .value) { (dataSnapshotUser) in
            guard let value = dataSnapshotUser.value else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return
            }
            completion(.success(value))
        }
    }
    
}

// MARK: - Account Management

extension DatabaseManager {
    
    /// Checks if user exists for given email
    /// Parameter
    /// - `email`:                  Target email to be checked
    /// - `completion`:     Async closure to return with result
    public func validationNewUser(with email: String, completion: @escaping ((Bool) -> Void)) {
        
        var safeEmail: String {
            var safeEmail = email.replacingOccurrences(of: "@", with: "-")
            safeEmail = safeEmail.replacingOccurrences(of: ".", with: "-")
            return safeEmail
        }
        database.child("users/\(safeEmail)").observeSingleEvent(of: .value, with: { dataSnapshotEmail in
            // If a user account email address has been already exist, then completion handler return true, else if the completion handler return false(the email address can be used)
            guard dataSnapshotEmail.value as? String != nil else {
                completion(false)
                return
            }
            completion(true)
        })
        
    }
    
    /// insert new user to database realtime
    public func insertUser(with user: ChatAppsUser, completion: @escaping (Bool) -> Void) {
        
        /*
         users_conversations => [
         "safe_email": [
         "first_name": String
         "last_name": String
         ],
         "safe_email": [
         "first_name": String
         "last_name": String
         ]
         ]
         */
        database.child("users_conversations").observeSingleEvent(of: .value) { [weak self] (dataSnapshotUsersConvo) in
            if var generalUser = dataSnapshotUsersConvo.value as? [String: [String:Any]] {
                // Append to user dictionary
                let newUserConvo: [String: [String:Any]] = [
                    user.safeEmail: [
                        "first_name": user.firstName,
                        "last_name":  user.lastName
                    ]
                ]
                var newUserConvoExist = false
                
                generalUser.forEach { (key, value) in
                    if key == user.safeEmail {
                        newUserConvoExist = true
                    }
                }
                
                if !newUserConvoExist {
                    generalUser.update(other: newUserConvo)
                    
                    self?.database.child("users_conversations").setValue(generalUser, withCompletionBlock: { (error, _) in
                        guard error == nil else {
                            print("Failed to append user conversation to datebase")
                            completion(false)
                            return
                        }
                        completion(true)
                    })
                }
                
            } else {
                // create user conversation to database
                let newUserConvoCollection: [String: [String: Any]] = [
                    user.safeEmail: [
                        "firs_name": user.firstName,
                        "last_name": user.lastName
                    ]
                ]
                
                self?.database.child("users_conversations").setValue(newUserConvoCollection)
                completion(true)
            }
            
            self?.database.child("users").observeSingleEvent(of: .value) { (dataSnapshotUsers) in
                if var usersCollection = dataSnapshotUsers.value as? [[String: String]] {
                    // Append to user dictionary
                    let newElement = [
                        "name": user.firstName + " " + user.lastName,
                        "email": user.safeEmail
                    ]
                    
                    var emailExist = false
                    for users in usersCollection {
                        if users["email"] == user.safeEmail {
                            emailExist = true
                        }
                    }
                    
                    if !emailExist {
                        usersCollection.append(newElement)
                        
                        self?.database.child("users").setValue(usersCollection) { (error, _) in
                            guard error == nil else {
                                print("Failed write array to database")
                                completion(false)
                                return
                            }
                            completion(true)
                        }
                    }
                    
                } else {
                    // Create the array
                    let newCollection: [[String: String]] = [
                        [
                            "name": user.firstName + " " + user.lastName,
                            "email": user.safeEmail
                        ]
                    ]
                    self?.database.child("users").setValue(newCollection)
                    completion(true)
                }
            }
            
        }
        
    }
    
    public func getAllUsers(completion: @escaping ((Result<[[String:String]], Error>) -> Void) ) {
        database.child("users").observeSingleEvent(of: .value) { (dataSnapshotAllUser) in
            guard let data = dataSnapshotAllUser.value as? [[String: String]] else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return
            }
            completion(.success(data))
        }
        
    }
    
    public enum DatabaseErrorHandle: Error {
        case failedToFetch
    }
}

// MARK: - Sending Message / Conversation

extension DatabaseManager {
    
    /*
     
     "exampleId" : {
     "message" : [
     {
     "id": String
     "type": text, photo, video
     "content": String
     "date": Date()
     "sender_email": String
     "isRead": true/false
     }
     ]
     }
     
     conversations => [
     [
     "conversation_id" : "exampleId"
     "other_user_email" :
     "latest_message" : => {
     "date" : Date()
     "message" : "message"
     "isRead" : true/false
     }
     ],
     ]
     */
    
    /// Create a new converssation with target user email and first message sent
    public func createNewConversation(with name: String, otherUserEmail: String, firstMessage: Message, completion: @escaping (Bool) -> Void) {
        guard let currentUserEmail = UserDefaults.standard.value(forKey: "email") as? String,
              let currentUserName = UserDefaults.standard.value(forKey: "name") as? String else { return }
        
        let safeCurrentEmail = DatabaseManager.shared.safeEmail(with: currentUserEmail)
        let references = database.child("users_conversations/\(safeCurrentEmail)")
        references.observeSingleEvent(of: .value) { [weak self] (dataSnaphot) in
            guard var userNode = dataSnaphot.value as? [String: Any] else {
                completion(false)
                print("user not found")
                return
            }
            
            let messageDate = firstMessage.sentDate
            let dateString = ChatViewController.dateFormatter.string(from: messageDate)
            
            /// Content message
            var message = ""
            switch firstMessage.kind {
            case .text(let messageText):
                message = messageText
            case .attributedText(_):
                break
            case .photo(_):
                break
            case .video(_):
                break
            case .location(_):
                break
            case .emoji(_):
                break
            case .audio(_):
                break
            case .contact(_):
                break
            case .custom(_):
                break
            case .linkPreview(_):
                break
            }
            
            let conversationId = "conversation_\(firstMessage.messageId)"
            let senderNewConversationData: [String: Any] = [
                "id": conversationId,
                "other_user_email": otherUserEmail,
                "name": name,
                "content_message": [
                    "date": dateString,
                    "latest_message": message,
                    "is_read": false
                ]
            ]
            
            let recepientNewConversationData: [String: Any] = [
                "id": conversationId,
                "other_user_email": safeCurrentEmail,
                "name": currentUserName,
                "content_message": [
                    "date": dateString,
                    "latest_message": message,
                    "is_read": false
                ]
            ]
            
            // Update recepient current user entry
            self?.database.child("users_conversations/\(otherUserEmail)/conversations").observeSingleEvent(of: .value) { [weak self] (dataSnapshotOtherUser) in
                if var conversations = dataSnapshotOtherUser.value as? [[String: Any]] {
                    conversations.append(recepientNewConversationData)
                    self?.database.child("users_conversations/\(otherUserEmail)/conversations").setValue(conversations)
                } else {
                    // create
                    self?.database.child("users_conversations/\(otherUserEmail)/conversations").setValue([recepientNewConversationData])
                }
            }
            
            // Update current user entry
            if var conversations = userNode["conversations"] as? [[String: Any]] {
                // conversation array exist for current user
                // should be append
                conversations.append(senderNewConversationData)
                userNode["conversations"] = conversations
                references.setValue(userNode) { [weak self] (error, _) in
                    guard error == nil else {
                        completion(false)
                        return
                    }
                    self?.finishCreatingConversation(name: name,
                                                     conversationId: conversationId,
                                                     firstMessage: firstMessage,
                                                     completion: completion)
                }
            } else {
                // conversation array doesn't exist
                // Create it here
                userNode["conversations"] = [senderNewConversationData]
                references.setValue(userNode) { [weak self] (error, _) in
                    guard error == nil else {
                        completion(false)
                        return 
                    }
                    self?.finishCreatingConversation(name: name, conversationId: conversationId, firstMessage: firstMessage, completion: completion)
                }
            }
            
        }
    }
    
    private func finishCreatingConversation(name: String, conversationId: String, firstMessage: Message, completion: @escaping (Bool) -> Void) {
        
        let messageDate = firstMessage.sentDate
        let dateString = ChatViewController.dateFormatter.string(from: messageDate)
        
        var message = ""
        switch firstMessage.kind {
        case .text(let messageText):
            message = messageText
        case .attributedText(_):
            break
        case .photo(_):
            break
        case .video(_):
            break
        case .location(_):
            break
        case .emoji(_):
            break
        case .audio(_):
            break
        case .contact(_):
            break
        case .custom(_):
            break
        case .linkPreview(_):
            break
        }
        
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else {
            completion(false)
            return
        }
        
        let currentUserEmail = DatabaseManager.shared.safeEmail(with: email)
        
        let collectionMessage: [String: Any] = [
            "id": firstMessage.messageId,
            "name": name,
            "type": firstMessage.kind.messageKindString,
            "content": message,
            "date": dateString,
            "sender_email": currentUserEmail,
            "is_read": false
        ]
        
        let value: [String: Any] = [
            "messages": [
                collectionMessage
            ]
        ]
        
        database.child("\(conversationId)").setValue(value) { (error, _) in
            guard error == nil else {
                completion(false)
                return
            }
            completion(true)
        }
    }
    
    /// Fetches and return all conversation for the user with passed in email
    public func getAllConversation(for email: String, completion: @escaping (Result<[Conversation], Error>) -> Void) {
        
        database.child("users_conversations/\(email)/conversations").observe(.value) { (dataSnapshotCurrentConvo) in
            guard let value = dataSnapshotCurrentConvo.value as? [[String: Any]] else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return }
            
            let conversations: [Conversation] = value.compactMap { (dictionary) in
                guard let id = dictionary["id"] as? String,
                      let name = dictionary["name"] as? String,
                      let otherUserEmail = dictionary["other_user_email"] as? String,
                      let contentMessage = dictionary["content_message"] as? [String: Any],
                      let date = contentMessage["date"] as? String,
                      let message = contentMessage["latest_message"] as? String,
                      let isRead = contentMessage["is_read"] as? Bool else {
                    return nil
                }
                
                let latestMessage = LatestMessage(date: date, text: message, isRead: isRead)
                return Conversation(id: id, name: name, otherUserEmail: otherUserEmail, latestMessage: latestMessage)
            }
            completion(.success(conversations))            
        }
    }
    /// Gets all messages for given a conversation
    public func getAllMessageForConversation(with id: String, completion: @escaping (Result<[Message], Error>) -> Void) {
        
        database.child("\(id)/messages").observe(.value) { (dataSnapshotMessages) in
            guard let value = dataSnapshotMessages.value as? [[String: Any]] else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return }
            
            let messages: [Message] = value.compactMap { (dictionary) in
                guard let content = dictionary["content"] as? String,
                      let dateString = dictionary["date"] as? String,
                      let dateFormatter = ChatViewController.dateFormatter.date(from: dateString),
                      let name = dictionary["name"] as? String,
                      let senderEmail = dictionary["sender_email"] as? String,
                      let messageId = dictionary["id"] as? String,
                      let typeMessage = dictionary["type"] as? String,
                      let _ = dictionary["is_read"] as? Bool else {
                    return nil
                }
                
                var kind: MessageKind?
                
                switch typeMessage {
                case "text":
                    kind = .text(content)
                    
                case "photo":
                    guard let url = URL(string: content),
                          let placeholder = UIImage(systemName: "photo.on.rectangle", withConfiguration: UIImage.SymbolConfiguration(weight: .thin))?.withTintColor(.systemGreen ,renderingMode: .alwaysOriginal) else { return nil }
                    
                    let media = Media(url: url,
                                      image: nil,
                                      placeholderImage: placeholder,
                                      size: CGSize(width: 200, height: 200))
                    
                    kind = .photo(media)
                    
                case "video":
                    guard let videoUrl = URL(string: content),
                          let placeholder = UIImage(systemName: "questionmark.video.fill", withConfiguration: UIImage.SymbolConfiguration(weight: .regular))?.withTintColor(.systemGreen ,renderingMode: .alwaysOriginal) else { return nil }
                    
                    let media = Media(url: videoUrl,
                                      image: nil,
                                      placeholderImage: placeholder,
                                      size: CGSize(width: 200, height: 200))
                    
                    kind = .video(media)
                case "location":
                    let locationComponent = content.components(separatedBy: ",")
                    guard let latitude = Double(locationComponent[0]),
                          let longitude = Double(locationComponent[1]) else { return nil }
                    
                    let location = Location(location: CLLocation(latitude: latitude, longitude: longitude),
                                            size: CGSize(width: 200, height: 200))
                    
                    kind = .location(location)
                default:
                    break
                }
                
                guard let finalKind = kind else { return nil }
                let sender = Sender(senderId: senderEmail, photoURL: "", displayName: name)
                
                return Message(sender: sender, messageId: messageId, sentDate: dateFormatter, kind: finalKind)
            }
            completion(.success(messages))
        }
    }
    
    /// Sends a message with target conversation and message
    public func sendMessages(to conversationId: String, otherUserEmail: String, name: String, newMessage: Message, completion: @escaping (Bool) -> Void) {
        //Add new message to messages
        //Update sender latest message
        //Update recepient latest message
        guard let currentEmail = UserDefaults.standard.value(forKey: "email") as? String else { return }
        guard let currentName = UserDefaults.standard.value(forKey: "name") as? String else { return }
        let safeCurrentEmail = DatabaseManager.shared.safeEmail(with: currentEmail)
        
        database.child("\(conversationId)/messages").observeSingleEvent(of: .value) { [weak self] (dataSnapshotCurrentMessages) in
            guard var currentMessage = dataSnapshotCurrentMessages.value as? [[String:Any]], let strongSelf = self else {
                completion(false)
                return
            }
            
            let messageDate = newMessage.sentDate
            let dateString = ChatViewController.dateFormatter.string(from: messageDate)
            
            var message = ""
            switch newMessage.kind {
            case .text(let messageText):
                message = messageText
            case .attributedText(_):
                break
            case .photo(let photoMedia):
                guard let photo = photoMedia.url?.absoluteString else { return }
                message = photo
            case .video(let videoMedia):
                guard let video = videoMedia.url?.absoluteString else{ return }
                message = video
            case .location(let locationData):
                let longitude = locationData.location.coordinate.longitude
                let latitude = locationData.location.coordinate.latitude
                
                message = "\(latitude),\(longitude)"
                break
            case .emoji(_):
                break
            case .audio(_):
                break
            case .contact(_):
                break
            case .custom(_):
                break
            case .linkPreview(_):
                break
            }
            
            let newMessageEntry: [String: Any] = [
                "id": newMessage.messageId,
                "name": name,
                "type": newMessage.kind.messageKindString,
                "content": message,
                "date": dateString,
                "sender_email": safeCurrentEmail,
                "is_read": false
            ]
            
            currentMessage.append(newMessageEntry)
            
            strongSelf.database.child("\(conversationId)/messages").setValue(currentMessage) { (error, _) in
                guard error == nil else {
                    completion(false)
                    return
                }
                
                strongSelf.database.child("users_conversations/\(safeCurrentEmail)/conversations").observeSingleEvent(of: .value) { (dataSnapshotCurrentUser) in
                    
                    var databaseEntryConversationForCurrentUser = [[String:Any]]()
                    
                    let updateLatestMessage: [String: Any] = [
                        "date": dateString,
                        "latest_message": message,
                        "is_read": false
                    ]
                    
                    if var currentUserConvo = dataSnapshotCurrentUser.value as? [[String: Any]] {
                        
                        var targetConversation: [String: Any]?
                        var position = 0
                        for convoDictionary in currentUserConvo {
                            if let currentConvoId = convoDictionary["id"] as? String, currentConvoId == conversationId {
                                targetConversation = convoDictionary
                                break
                            }
                            position += 1
                        }
                                                
                        if var targetConvo = targetConversation {
                            targetConvo["content_message"] = updateLatestMessage
                            currentUserConvo[position] = targetConvo
                            databaseEntryConversationForCurrentUser = currentUserConvo
                        } else {
                            let newConversationData: [String: Any] = [
                                "id": conversationId,
                                "other_user_email": otherUserEmail,
                                "name": name,
                                "content_message": updateLatestMessage
                            ]
                            currentUserConvo.append(newConversationData)
                            databaseEntryConversationForCurrentUser = currentUserConvo
                        }
                        
                    } else {
                        let newConversationData: [String: Any] = [
                            "id": conversationId,
                            "other_user_email": otherUserEmail,
                            "name": name,
                            "content_message": updateLatestMessage
                        ]
                        
                        databaseEntryConversationForCurrentUser = [
                            newConversationData
                        ]
                    }
                    
                    strongSelf.database.child("users_conversations/\(safeCurrentEmail)/conversations").setValue(databaseEntryConversationForCurrentUser) { (error, _) in
                        guard error == nil else {
                            completion(false)
                            return
                        }
                        
                        // Update a latest message recepient
                        strongSelf.database.child("users_conversations/\(otherUserEmail)/conversations").observeSingleEvent(of: .value) { (dataSnapshotOtherUser) in
                            
                            var databaseEntryConversationForOtherUser = [[String:Any]]()
                            
                            let updateLatestMessage: [String: Any] = [
                                "date": dateString,
                                "latest_message": message,
                                "is_read": false
                            ]
                            
                            if var otherUserConvo = dataSnapshotOtherUser.value as? [[String: Any]] {
                                var targetConversation: [String: Any]?
                                var position = 0
                                for convoDictionary in otherUserConvo {
                                    if let otherConvoId = convoDictionary["id"] as? String, otherConvoId == conversationId {
                                        targetConversation = convoDictionary
                                        break
                                    }
                                    position += 1
                                }
                                                        
                                if var targetConvo = targetConversation {
                                    targetConvo["content_message"] = updateLatestMessage
                                    otherUserConvo[position] = targetConvo
                                    databaseEntryConversationForOtherUser = otherUserConvo
                                } else {
                                    let newConversationData: [String: Any] = [
                                        "id": conversationId,
                                        "other_user_email": safeCurrentEmail,
                                        "name": currentName,
                                        "content_message": updateLatestMessage
                                    ]
                                    otherUserConvo.append(newConversationData)
                                    databaseEntryConversationForOtherUser = otherUserConvo
                                }
                                
                            } else {
                                let newConversationData: [String: Any] = [
                                    "id": conversationId,
                                    "other_user_email": safeCurrentEmail,
                                    "name": currentName,
                                    "content_message": updateLatestMessage
                                ]
                                
                                databaseEntryConversationForOtherUser = [
                                    newConversationData
                                ]
                            }
                            
                            strongSelf.database.child("users_conversations/\(otherUserEmail)/conversations").setValue(databaseEntryConversationForOtherUser) { (error, _) in
                                guard error == nil else {
                                    completion(false)
                                    return
                                }                                
                                completion(true)
                            }
                            
                        }
                    }
                    
                }
            }
            
        }
        
    }
    
    /// deleting conversation
    public func deleteConversation(conversationId: String, completion: @escaping (Bool) -> Void) {
        
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else {return}
        let safeEmail = DatabaseManager.shared.safeEmail(with: email)
        
        let referencePath = database.child("users_conversations/\(safeEmail)/conversations")
        referencePath.observeSingleEvent(of: .value) { (dataSnapshotAllConvo) in
            if var conversations = dataSnapshotAllConvo.value as? [[String:Any]] {
                var convoPositionIndex = 0
                for conversation in conversations {
                    if let convoId = conversation["id"] as? String, convoId == conversationId {
                        print("Found conversation id")
                        break
                    }
                    convoPositionIndex += 1
                }
                
                conversations.remove(at: convoPositionIndex)
                
                referencePath.setValue(conversations) { (error, _) in
                    guard error == nil else {
                        print("Failed to write new conversation to firebase")
                        completion(false)
                        return
                    }
                    print("deleted conversation")
                    completion(true)
                }
                                
            }
            
        }
        
    }
    
    // Check a conversation exist or not
    public func conversationExists(with emailRecepient: String, completion: @escaping (Result<String, Error>) -> Void) {
        
        guard let senderEmail = UserDefaults.standard.value(forKey: "email") as? String else { return }
        let safeSenderEmail = DatabaseManager.shared.safeEmail(with: senderEmail)
        let safeRecipientEmail = DatabaseManager.shared.safeEmail(with: emailRecepient)
        
        database.child("users_conversations/\(safeRecipientEmail)/conversations").observeSingleEvent(of: .value) { (dataSnaphot) in
            guard let conversations = dataSnaphot.value as? [[String:Any]] else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return }
            
            if let conversation = conversations.first(where: {
                guard let senderEmail = $0["other_user_email"] as? String else {
                    return false
                }
                return safeSenderEmail == senderEmail
            }) {
                guard let conversationId = conversation["id"] as? String else {
                    completion(.failure(DatabaseErrorHandle.failedToFetch))
                    return
                }                
                completion(.success(conversationId))
                return
            } else {
                completion(.failure(DatabaseErrorHandle.failedToFetch))
                return
            }
        }
        
    }
    
}

struct ChatAppsUser {
    let firstName: String
    let lastName: String
    let emailAddress: String
    
    var safeEmail: String {
        var safeEmail = emailAddress.replacingOccurrences(of: "@", with: "-")
        safeEmail = safeEmail.replacingOccurrences(of: ".", with: "-")
        return safeEmail
    }
    
    var profilePictureFileName: String {
        /*
         /image/faisal-arip10-gmail-com_profile_picture.png
         */
        return "\(safeEmail)_profile_picture.png"
    }
    
}

extension Dictionary {
    mutating func update(other: Dictionary) {
        for (key, value) in other {
            self.updateValue(value, forKey: key)
        }
    }
}
