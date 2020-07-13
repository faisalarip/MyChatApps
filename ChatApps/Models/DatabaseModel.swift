//
//  DatabaseManager.swift
//  ChatApps
//
//  Created by Faisal Arif on 24/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseDatabase

final class DatabaseModel {
    
    static let shared = DatabaseModel()
    
    private let database = Database.database().reference()
    
    public func safeEmail(with emailAddress: String) -> String {
        var safeEmail = emailAddress.replacingOccurrences(of: "@", with: "-")
        safeEmail = safeEmail.replacingOccurrences(of: ".", with: "-")
        return safeEmail
    }
}

// MARK: - Account Management

extension DatabaseModel {
    public func validationNewUser(with email: String, completion: @escaping ((Bool) -> Void)) {
        
        var safeEmail: String {
            var safeEmail = email.replacingOccurrences(of: "@", with: "-")
            safeEmail = safeEmail.replacingOccurrences(of: ".", with: "-")
            return safeEmail
        }
        
        database.child(safeEmail).observeSingleEvent(of: .value, with: { dataSnapshot in
            // If a user account email address has been already exist, then completion handler return true, else if the completion handler return false(the email address can be used)
            guard dataSnapshot.value as? String != nil else {
                completion(false)
                return
            }
            
            completion(true)
        })
        
    }
    
    /// insert new user to database realtime
    public func insertUser(with user: ChatAppsUser, completion: @escaping (Bool) -> Void) {
        database.child(user.safeEmail).setValue([
            "first_name" : user.firstName,
            "last_name" : user.lastName
            ], withCompletionBlock: { error, _ in
                guard error == nil else {
                    print("failed write to database")
                    completion(false)
                    return
                }
                
                /*
                 user => [
                 [
                 "name":
                 "safe_email":
                 ],
                 [
                 "name":
                 "safe_email":
                 ]
                 ]
                 */
                
                self.database.child("users").observeSingleEvent(of: .value) { (dataSnapshot) in
                    if var usersCollection = dataSnapshot.value as? [[String: String]] {
                        // Append to user dictionary
                        let newElement = [
                            "name": user.firstName + " " + user.lastName,
                            "email": user.safeEmail
                        ]
                        usersCollection.append(newElement)
                        
                        self.database.child("users").setValue(usersCollection) { (error, _) in
                            guard error == nil else {
                                print("Failed write array to database")
                                completion(false)
                                return
                            }
                            completion(true)
                        }
                        
                    } else {
                        // Create the array
                        let newCollection: [[String: String]] = [
                            [
                                "name": user.firstName + " " + user.lastName,
                                "email": user.safeEmail
                            ]
                        ]
                        self.database.child("users").setValue(newCollection) { (error, _) in
                            guard error == nil else {
                                print("Failed write array to database")
                                completion(false)
                                return
                            }
                            completion(true)
                        }
                    }
                }
                
                completion(true)
        })
    }
    
    public func getAllUsers(completion: @escaping ((Result<[[String: String]], Error>) -> Void) ) {
        database.child("users").observeSingleEvent(of: .value) { (dataSnapshot) in
            guard let data = dataSnapshot.value as? [[String: String]] else {
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

extension DatabaseModel {
    
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
    public func createNewConversation(with otherUserEmail: String, firstMessage: Message, completion: @escaping (Bool) -> Void) {
        guard let currentUser = UserDefaults.standard.value(forKey: "email") as? String else { return }
        let safeCurrentEmail = DatabaseModel.shared.safeEmail(with: currentUser)
        let references = database.child("\(safeCurrentEmail)")
        references.observeSingleEvent(of: .value) { (dataSnaphot) in
            guard var userNode = dataSnaphot.value as? [String: Any] else {
                completion(false)
                print("user not found")
                return
            }
            
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
            }
            
            let conversationId = "conversation_\(firstMessage.messageId)"
            let newConversationData: [String: Any] = [
                "id": conversationId,
                "other_user_email": otherUserEmail,
                "latest_message": [
                    "date": dateString,
                    "latest_message": message,
                    "is_read": false
                ]
            ]
            
            if var conversations = userNode["conversations"] as? [[String: Any]] {
                // conversation array exist for current user
                // should be append
                conversations.append(newConversationData)
                userNode["conversations"] = conversations
                references.setValue(userNode) { [weak self] (error, _) in
                    guard error == nil else {
                        completion(false)
                        return
                    }
                    self?.finishCreatingConversation(conversationId: conversationId, firstMessage: firstMessage, completion: completion)
                }
            } else {
                // conversation array doesn't exist
                // Create it here
                userNode["conversations"] = [newConversationData]
                references.setValue(userNode) { [weak self] (error, _) in
                    guard error == nil else {
                        completion(false)
                        return 
                    }
                    self?.finishCreatingConversation(conversationId: conversationId, firstMessage: firstMessage, completion: completion)
                }
            }
            
        }
    }
    
    private func finishCreatingConversation(conversationId: String, firstMessage: Message, completion: @escaping (Bool) -> Void) {
        
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
        }
        
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else {
            completion(false)
            return
        }
        
        let currentUserEmail = DatabaseModel.shared.safeEmail(with: email)
        
        let collectionMessage: [String: Any] = [
            "id": firstMessage.messageId,
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
    public func getAllConversation(for email: String, completion: @escaping (Result<String, Error>) -> Void) {
        
    }
    
    /// Gets all messages for given a conversation
    public func getAllMessageForConversation(with id: String, completion: @escaping (Result<String, Error>) -> Void) {
        
    }
    
    /// Sends a message with target conversation and message
    public func sendMessages(with conversation: String, message: Message, completion: @escaping (Bool) -> Void) {
        
    }
}

struct ChatAppsUser {
    let firstName: String
    let lastName: String
    let emailAddress: String
    //    let profileImage: String
    
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
