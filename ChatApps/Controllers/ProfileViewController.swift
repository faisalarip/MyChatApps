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
<<<<<<< HEAD
=======
import SDWebImage

enum ProfileViewModelType {
    case profileInfo, logout
}

struct ProfileViewModel {
    let viewModel: ProfileViewModelType
    let title: String
    let handler: (() -> Void)?
}
>>>>>>> origin/develop12

class ProfileViewController: UIViewController {

    @IBOutlet var tableView: UITableView!
    
<<<<<<< HEAD
    var data = ["Log out"]
=======
    var data = [ProfileViewModel]()
>>>>>>> origin/develop12
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
<<<<<<< HEAD
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: "cell")
=======
        tableView.register(ProfileTableViewCell.self, forCellReuseIdentifier: ProfileTableViewCell.identifier)
        
        data.append(ProfileViewModel(viewModel: .profileInfo,
                                     title: "Name: \(UserDefaults.standard.value(forKey: "name") as? String ?? "No Name")",
                                     handler: nil))
        data.append(ProfileViewModel(viewModel: .profileInfo,
                                     title: "Email: \(UserDefaults.standard.value(forKey: "email") as? String ?? "No Email")",
                                     handler: nil))
        data.append(ProfileViewModel(viewModel: .logout, title: "Log out", handler: { [weak self] in
            guard let strongSelf = self else { return }
            
            let actionSheet = UIAlertController(title: "", message: "", preferredStyle: .actionSheet)
            actionSheet.addAction(UIAlertAction(title: "Log out", style: .destructive, handler: { [weak self] ( _ ) in
                guard let strongSelf = self else { return }
                // Log out Facebook account
                FBSDKLoginKit.LoginManager().logOut()
                // Log out Google account
                GIDSignIn.sharedInstance()?.signOut()
                
                UserDefaults.standard.setValue(nil, forKey: "email")
                UserDefaults.standard.setValue(nil, forKey: "name")
                DispatchQueue.main.async {
                    self?.tableView.reloadData()
                }
                do {
                    try FirebaseAuth.Auth.auth().signOut()
                    
                    let loginVC = LoginViewController()
                    let nav = UINavigationController(rootViewController: loginVC)
                    nav.modalPresentationStyle = .fullScreen
                    strongSelf.present(nav, animated: true)
                } catch {
                    print("Failed to log out")
                }
                
            }))
            
            actionSheet.addAction(UIAlertAction(title: "", style: .cancel, handler: nil))
            strongSelf.present(actionSheet, animated: true)
            
        }))
>>>>>>> origin/develop12
        
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
        
<<<<<<< HEAD
        StorageManager.shared.downloadURL(for: path) { [weak self] (result) in
            guard let strongSelf = self else {
                return
            }
            
            switch result {
                case .success(let url):
                    strongSelf.downloadImage(with: imageView, url: url)
=======
        StorageManager.shared.downloadURL(for: path) { (result) in
            switch result {
                case .success(let url):
                    imageView.sd_setImage(with: url, completed: nil)
>>>>>>> origin/develop12
                case .failure(let error):
                print("failed to get download url \(error)")
            }
        }
        
        return headerView
    }
<<<<<<< HEAD
    
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
=======
>>>>>>> origin/develop12
}

extension ProfileViewController: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return data.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
<<<<<<< HEAD
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        cell.textLabel?.text = data[indexPath.row]
        cell.textLabel?.textAlignment = .center
        cell.textLabel?.textColor = .red
=======
        guard let cell = tableView.dequeueReusableCell(withIdentifier: ProfileTableViewCell.identifier, for: indexPath) as? ProfileTableViewCell else {
            return UITableViewCell(style: .default, reuseIdentifier: "cell")
        }
        
        cell.setUpProfileCell(with: data[indexPath.row])
>>>>>>> origin/develop12
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
<<<<<<< HEAD
        
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
=======
        data[indexPath.row].handler?()
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
}

class ProfileTableViewCell: UITableViewCell {
    static let identifier = "ProfileTableViewCell"
    
    public func setUpProfileCell(with viewModel: ProfileViewModel) {
        switch viewModel.viewModel {
        case .profileInfo:
            self.textLabel?.text = viewModel.title
            self.textLabel?.textAlignment = .natural
            self.selectionStyle = .none
        case .logout:
            self.textLabel?.text = viewModel.title
            self.textLabel?.textColor = .red
            self.textLabel?.textAlignment = .center
        }
>>>>>>> origin/develop12
    }
    
}
