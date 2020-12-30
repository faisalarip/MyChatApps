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
import SDWebImage


final class ProfileViewController: UIViewController {

    @IBOutlet var tableView: UITableView!
    
    var data = [ProfileViewModel]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        tableView.register(ProfileTableViewCell.self, forCellReuseIdentifier: ProfileTableViewCell.identifier)

        tableView.delegate = self
        tableView.dataSource = self
        tableView.rowHeight = UITableView.automaticDimension
        tableView.estimatedRowHeight = UITableView.automaticDimension
        tableView.tableHeaderView = createTableHeader()
        self.setUpProfileInfo()
    }
    
    private func createTableHeader() -> UIView? {
        
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
        
        StorageManager.shared.downloadURL(for: path) { (result) in
            switch result {
                case .success(let url):
                    imageView.sd_setImage(with: url, completed: nil)
                case .failure(let error):
                print("failed to get download url \(error)")
            }
        }
        
        return headerView
    }
    
    private func setUpProfileInfo() {
        // Returns the value associate with the specified key in the recevier's information property list (info.plist)
        let appVersion = Bundle.main.object(forInfoDictionaryKey: "CFBundleShortVersionString") as? String ?? ""
        
        data.append(ProfileViewModel(viewModelType: .profileInfo,
                                     title: "Name:\n\n\(UserDefaults.standard.value(forKey: "name") as? String ?? "No Name")",
                                     handler: nil))
        data.append(ProfileViewModel(viewModelType: .profileInfo,
                                     title: "Email:\n\n\(UserDefaults.standard.value(forKey: "email") as? String ?? "No Email")", handler: nil))
        data.append(ProfileViewModel(viewModelType: .profileInfo,
                                     title: "App version v\(appVersion)",
                                     handler: nil))
        
        data.append(ProfileViewModel(viewModelType: .logout, title: "Log out", handler: { [weak self] in
            guard let strongSelf = self else { return }
            
            let actionSheet = UIAlertController(title: "Confirm", message: "Are you sure you want to log out of this account?", preferredStyle: .actionSheet)
            
            actionSheet.addAction(UIAlertAction(title: "Log out", style: .destructive, handler: { [weak self] ( _ ) in
                guard let strongSelf = self else { return }
                // Log out Facebook account
                FBSDKLoginKit.LoginManager().logOut()
                // Log out Google account
                GIDSignIn.sharedInstance()?.signOut()
                
                UserDefaults.standard.setValue(nil, forKey: "email")
                UserDefaults.standard.setValue(nil, forKey: "name")
                DispatchQueue.main.async {
                    strongSelf.tableView.reloadData()
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
            
            actionSheet.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
            
            strongSelf.present(actionSheet, animated: true)
            
        }))
    }
    
}

extension ProfileViewController: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return data.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: ProfileTableViewCell.identifier, for: indexPath) as? ProfileTableViewCell else {
            return UITableViewCell(style: .default, reuseIdentifier: "cell")
        }
        
        cell.setUpProfileCell(with: data[indexPath.row])
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        data[indexPath.row].handler?()
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
    func tableView(_ tableView: UITableView, estimatedHeightForRowAt indexPath: IndexPath) -> CGFloat {
        return UITableView.automaticDimension
    }
        
}

class ProfileTableViewCell: UITableViewCell {
    static let identifier = "ProfileTableViewCell"
    
    public func setUpProfileCell(with viewModel: ProfileViewModel) {
        switch viewModel.viewModelType {
        case .profileInfo:
            self.textLabel?.text = viewModel.title
            self.textLabel?.numberOfLines = 0
            
            if viewModel.title.contains("version") {
                self.textLabel?.textAlignment = .center
                self.textLabel?.font = UIFont.preferredFont(forTextStyle: .footnote)
                self.textLabel?.textColor = .secondaryLabel
            } else {
                self.textLabel?.font = UIFont.preferredFont(forTextStyle: .headline)
                self.textLabel?.textAlignment = .natural
                self.selectionStyle = .none
            }
        case .logout:
            self.textLabel?.text = viewModel.title
            self.textLabel?.font = UIFont.systemFont(ofSize: 18, weight: .semibold)
            self.textLabel?.textColor = .red
            self.textLabel?.textAlignment = .center
        }
    }
    
}
