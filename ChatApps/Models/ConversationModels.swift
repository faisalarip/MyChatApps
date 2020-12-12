//
//  Conversaiton.swift
//  ChatApps
//
//  Created by Faisal Arif on 03/08/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation

struct Conversation {
    let id: String
    let name: String
    let otherUserEmail: String
    let latestMessage: LatestMessage
}

struct LatestMessage {
    let date: String
    let text: String
    let isRead: Bool
}
