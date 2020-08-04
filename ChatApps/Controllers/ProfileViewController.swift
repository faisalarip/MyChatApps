//
//  ProfileViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseAuth
import FBSDKLoginKit
import GoogleSignIn

class ProfileViewController: UIViewController {

    @IBOutlet var tableView: UITableView!
    
    var data = ["Log out"]
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: "cell")
        
        tableView.delegate = self
        tableView.dataSource = self
        tableView.tableHeaderView = createTableHeader()
    }
    
    func createTableHeader() -> UIView? {
        
        let headerView = UIView(frame: CGRect(x: 0, y: 0, width: self.view.width, height: 150))
        headerView.backgroundColor = .systemBackground
        let imageView = UIImageView(frame: CGRect(x: (headerView.width-75) / 2, y: 25, width: 75   , height: 75))
        imageView.contentMode = .scaleAspectFill
        imageView.layer.cornerRadius = imageView.frame.height / 2
        imageView.backgroundColor = .white
        imageView.layer.masksToBounds = true
        headerView.addSubview(imageView)
        
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else {
            return nil
        }        
        let safeEmail = DatabaseManager.shared.safeEmail(with: email)
        let fileName = "\(safeEmail)_profile_picture.png"
        let path = "image/\(fileName)"
        
        StorageManager.shared.downloadURL(for: path) { [weak self] (result) in
            guard let strongSelf = self else {
                return
            }
            
            switch result {
                case .success(let url):
                    strongSelf.downloadImage(with: imageView, url: url)
                case .failure(let error):
                print("failed to get download url \(error)")
            }
        }
        
        return headerView
    }
    
    private func downloadImage(with imageView: UIImageView, url: URL) {
        URLSession.shared.dataTask(with: url) { (data, _, error) in
            guard let data = data else {
                return
            }
            DispatchQueue.main.async {
                let image = UIImage(data: data)
                imageView.image = image
                self.reloadInputViews()
            }
        }.resume()
    }
}

extension ProfileViewController: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return data.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        cell.textLabel?.text = data[indexPath.row]
        cell.textLabel?.textAlignment = .center
        cell.textLabel?.textColor = .red
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        
        let actionSheet = UIAlertController(title: "", message: "", preferredStyle: .actionSheet)
        actionSheet.addAction(UIAlertAction(title: "Log out", style: .destructive, handler: { [weak self] ( _ ) in
            
            guard let self = self else {
                return
            }
            
            // Log out Facebook account
            FBSDKLoginKit.LoginManager().logOut()
            
            // Log out Google account
            GIDSignIn.sharedInstance()?.signOut()
            
            do {
                tableView.deselectRow(at: indexPath, animated: true)
                try FirebaseAuth.Auth.auth().signOut()
                
                let vc = LoginViewController()
                let nav = UINavigationController(rootViewController: vc)
                nav.modalPresentationStyle = .fullScreen
                self.present(nav, animated: true)
            } catch {
                print("Failed to log out")
            }
            
        }))
        
        actionSheet.addAction(UIAlertAction(title: "", style: .cancel, handler: nil))
        present(actionSheet, animated: true)
    }
    
}
