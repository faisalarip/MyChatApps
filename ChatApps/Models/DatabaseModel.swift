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
    public func insertUser(with user: ChatAppsUser) {
        database.child(user.safeEmail).setValue([
            "first_name" : user.firstName,
            "last_name" : user.lastName
        ])
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
}
