//
//  DatabaseManager.swift
//  ChatApps
//
//  Created by Faisal Arif on 24/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation
import FirebaseDatabase

final class DatabaseModel {
    
    static let shared = DatabaseModel()
    
    private let database = Database.database().reference()
    
    public func insertUser(with user: ChatAppsUser) {
        
        database.child(user.emailAddress).setValue([
            "first_name" : user.fristName,
            "last_name" : user.lastName
        ])
        
    }
    
}

struct ChatAppsUser {
    let fristName: String
    let lastName: String
    let emailAddress: String
//    let profileImage: String
}
