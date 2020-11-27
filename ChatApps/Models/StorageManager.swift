//
//  StorageManager.swift
//  ChatApps
//
//  Created by Faisal Arif on 27/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import Foundation
import FirebaseStorage

final class StorageManager {
    
    static let shared = StorageManager()
    
    private let storage = Storage.storage().reference()
    
    /*
     /image/faisal-arip10-gmail-com_profile_picture.png
     */
    
    public typealias uploadPictureCompletion = (Result<String, Error>) -> Void
    
    /// Upload picture to firebase storage and returns completion with url string to download
    public func uploadProfilePicture(with data: Data, fileName: String, completion: @escaping uploadPictureCompletion) {
        storage.child("image/\(fileName)").putData(data, metadata: nil) { [weak self] (metadata, error) in
            guard error == nil else {
                print("Failed upload to firebase storage for picture")
                completion(.failure(StorageError.failedToUpload))
                return
            }
            
            self?.storage.child("image/\(fileName)").downloadURL { (url, error) in
                guard let url = url else {
                    print("Failed to get download url")
                    completion(.failure(StorageError.failedToGetDownloadUrl))
                    return
                }
                
                let urlString = url.absoluteString
                print("Got a download urlString \(urlString)")
                completion(.success(urlString))
            }
            
        }
    }
    
    /// Upload photo message to firebase storage    
    public func uploadPhotosMessage(with data: Data, filename: String, completion: @escaping uploadPictureCompletion) {
        
        storage.child("message_images/\(filename)").putData(data, metadata: nil) { [weak self] (metadata, error) in
            guard error == nil else {
                print("Failed to upload data to firebase for picture ")
                completion(.failure(StorageError.failedToUpload))
                return
            }
            
            self?.storage.child("message_images/\(filename)").downloadURL(completion: { (url, error) in
                guard let url = url, error == nil else {
                    print("Failed to get download url from firebase storage")
                    completion(.failure(StorageError.failedToGetDownloadUrl))
                    return
                }
                
                let urlString = url.absoluteString
                print("Got a urlString from firebase storage \(urlString)")
                completion(.success(urlString))
            })
            
        }
        
    }
    
    /// Upload video message to firebase storage
    public func uploadVideosMessage(with fileUrl: URL, filename: String, completion: @escaping uploadPictureCompletion) {
        
        storage.child("message_videos/\(filename)").putFile(from: fileUrl, metadata: nil) { [weak self] (metadata, error) in
            guard error == nil else {
                print("Failed to upload video file to firebase for picture ")
                completion(.failure(StorageError.failedToUpload))
                return
            }
            
            self?.storage.child("message_videos/\(filename)").downloadURL(completion: { (url, error) in
                guard let url = url, error == nil else {
                    print("Failed to get download url video from firebase storage")
                    completion(.failure(StorageError.failedToGetDownloadUrl))
                    return
                }
                
                let urlString = url.absoluteString
                print("Got a urlString from firebase storage \(urlString)")
                completion(.success(urlString))
            })
            
        }
        
    }
    
    public enum StorageError: Error {
        case failedToUpload
        case failedToGetDownloadUrl
    }
    
    public func downloadURL(for path: String, completion: @escaping (Result<URL, Error>) -> Void) {        
        let reference = storage.child(path)
        
        reference.downloadURL { (url, error) in
            guard let url = url, error == nil else {
                completion(.failure(StorageError.failedToGetDownloadUrl))
                return
            }
            completion(.success(url))
        }
        
    }
}
