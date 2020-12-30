//
//  ProfileModels.swift
//  ChatApps
//
//  Created by Faisal Arif on 11/12/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation

enum ProfileViewModelType {
    case profileInfo, logout
}

struct ProfileViewModel {
    let viewModelType: ProfileViewModelType
    let title: String
    let handler: (() -> Void)?
}
