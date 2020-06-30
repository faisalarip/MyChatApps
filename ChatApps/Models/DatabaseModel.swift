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

// Mark: - Account Management

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
                    let newElement =
                        [
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
