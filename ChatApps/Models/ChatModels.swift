//
//  ChatModels.swift
//  ChatApps
//
//  Created by Faisal Arif on 11/12/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation
import MessageKit
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
