//
//  LoginViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseAuth
import FBSDKLoginKit
import GoogleSignIn
import JGProgressHUD

class LoginViewController: UIViewController {
    
    private let spinner = JGProgressHUD(style: .dark)
    
    private let scrollView: UIScrollView = {
        let scrollView = UIScrollView()
        scrollView.clipsToBounds = true
        return scrollView
    }()
    
    private let imageView: UIImageView = {
        let imageView = UIImageView()
        imageView.image = UIImage(named: "messenger")
        imageView.contentMode = .scaleAspectFit
        imageView.clipsToBounds = true
        return imageView
    }()
    
    private let emailfield: UITextField = {
        let textFeild = UITextField()
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .continue
        textFeild.layer.cornerRadius = 12
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.placeholder = "Username or Email"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
        return textFeild
    }()
    
    private let passwordField: UITextField = {
        let textFeild = UITextField()
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .done
        textFeild.layer.cornerRadius = 12
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.placeholder = "Password"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
        textFeild.isSecureTextEntry = true
        return textFeild
    }()
    
    private let loginButton: UIButton = {
        let button = UIButton()
        button.setTitle("Log In", for: .normal)
        button.backgroundColor = .secondarySystemBackground
        button.setTitleColor(UIColor.systemOrange, for: .normal)
        button.layer.borderWidth = 1
        button.layer.borderColor = UIColor.systemOrange.cgColor
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 4
        button.titleLabel?.font = UIFont.systemFont(ofSize: 14, weight: .bold)
        return button
    }()
    
    private let fbLoginButton: FBLoginButton = {
        let button = FBLoginButton()
        button.permissions = ["email,public_profile"]
        return button
    }()
    
    private let googleButton = GIDSignInButton()
    
    private var loginObserved: NSObjectProtocol?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        loginObserved = NotificationCenter.default.addObserver(forName: Notification.Name.didLogInNotification, object: nil, queue: .main, using: { [weak self] ( _ ) in
            guard let strongSelf = self else {
                return
            }
            strongSelf.navigationController?.dismiss(animated: true, completion: nil )
        })
        
        title = "Log In"
        view.backgroundColor = .systemBackground
        
        navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Register", style: .done, target: self, action: #selector(didTapRegister))
        
        GIDSignIn.sharedInstance()?.presentingViewController = self
        
        loginButton.addTarget(self, action: #selector(loginButtonTapped), for: .touchUpInside)
        
        emailfield.delegate = self
        passwordField.delegate = self
        fbLoginButton.delegate = self
        
        view.addSubview(scrollView)
        scrollView.addSubview(imageView)
        scrollView.addSubview(emailfield)
        scrollView.addSubview(passwordField)
        scrollView.addSubview(loginButton)
        scrollView.addSubview(fbLoginButton)
        scrollView.addSubview(googleButton)
        
    }
    
    deinit {
        if let observer = loginObserved {
            NotificationCenter.default.removeObserver(observer)
        }
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        
        scrollView.frame = view.bounds
        
        let sizeImage = scrollView.width/3
        imageView.frame = CGRect(x: (scrollView.width-sizeImage)/2, y: 30, width: sizeImage, height: sizeImage)
        
        emailfield.frame = CGRect(x: 30, y: imageView.bottom+45, width: scrollView.width-60, height: 52)
        passwordField.frame = CGRect(x: 30, y: emailfield.bottom+25, width: scrollView.width-60, height: 52)
        loginButton.frame = CGRect(x: 30, y: passwordField.bottom+35, width: scrollView.width-60, height: 42)
        fbLoginButton.frame = CGRect(x: 30, y: loginButton.bottom+25, width: scrollView.width-60, height: 42)
        googleButton.frame = CGRect(x: 30, y: fbLoginButton.bottom+25, width: scrollView.width-60, height: 42)
        
    }
    
    @objc private func loginButtonTapped() {
        
        guard let email = emailfield.text, let password = passwordField.text, !email.isEmpty, !password.isEmpty, password.count >= 6 else {
            alertUserLoginError()
            return }
        
        spinner.show(in: view )
        
        //Firebase Log in
        FirebaseAuth.Auth.auth().signIn(withEmail: email, password: password) { [weak self] (authResult, error) in
            guard error == nil, let strongSelf = self else {
                self?.alertUserLoginError()
                print("Failed to log in \(email) your email not correct")
                return
            }
            DispatchQueue.main.async {
                strongSelf.spinner.dismiss()
            }
            
            let safeEmail = DatabaseManager.shared.safeEmail(with: email)
            DatabaseManager.shared.getDataCurrentUser(safeEmail: safeEmail) { (result) in
                switch result {
                case .success(let data):
                    guard let userData = data as? [String: Any],
                          let firstName = userData["first_name"] as? String,
                          let lastName = userData["last_name"] as? String else {
                        return
                    }
                    UserDefaults.standard.set("\(firstName) \(lastName)", forKey: "name")
                case .failure(let error):
                    print("Failed to read user data with \(error)")
                }
            }            
            UserDefaults.standard.set(email, forKey: "email")
            print("Success to Log in")
            strongSelf.navigationController?.dismiss(animated: true, completion: nil)
        }
        
    }
    
    private func alertUserLoginError() {
        let alert = UIAlertController(title: "Woops..", message: "Please enter all information to log In", preferredStyle: .alert)
        
        alert.addAction(UIAlertAction(title: "Dismiss", style: .cancel, handler: nil))
        present(alert, animated: true)
    }
    
    @objc private func didTapRegister() {
        let registerVC = RegisterViewController()
        registerVC.title = "Create New Account"
        navigationController?.pushViewController(registerVC, animated: true)
    }
    
}

// MARK: - Text Field Delegete

extension LoginViewController: UITextFieldDelegate {
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        
        if textField == emailfield {
            passwordField.becomeFirstResponder()
        } else if textField == passwordField {
            loginButtonTapped()
        }
        
        return true
    }
    
    func textFieldDidBeginEditing(_ textField: UITextField) {
        textField.placeholder = ""
    }
    
    
}

// MARK: - Logion Button Delegete

extension LoginViewController: LoginButtonDelegate {
    
    func loginButtonDidLogOut(_ loginButton: FBLoginButton) {
        // nothing configure
    }
    
    func loginButton(_ loginButton: FBLoginButton, didCompleteWith result: LoginManagerLoginResult?, error: Error?) {
        guard let token = result?.token?.tokenString else {
            print("Failed to login with facebook")
            return
        }
        
        let fbRequest = FBSDKLoginKit.GraphRequest(graphPath: "me", parameters: ["fields": "email, first_name, last_name, picture.type(large)"], tokenString: token, version: nil, httpMethod: .get)
        fbRequest.start(completionHandler: { _, result, error in
            // [String: Any] is a Dictionary data
            guard let result = result as? [String: Any],
                error == nil
                else {
                    print("Failed to make facebook graph request")
                    return
            }
            print(result)
            
            guard let firstName = result["first_name"] as? String,
                let lastName = result["last_name"] as? String,
                let email = result["email"] as? String,
                let picture = result["picture"] as? [String: Any],
                let data = picture["data"] as? [String: Any],
                let pictureUrl = data["url"] as? String else {
                    print("Faield to get email and name from fb result")
                    return
            }
            
            UserDefaults.standard.set(email, forKey: "email")
            UserDefaults.standard.set("\(firstName) \(lastName)", forKey: "name")
            
            DatabaseManager.shared.validationNewUser(with: email) { (exist) in
                if !exist {
                    
                    let chatUser = ChatAppsUser(firstName: firstName, lastName: lastName, emailAddress: email)
                    
                    DatabaseManager.shared.insertUser(with: chatUser, completion: { success in
                        if success {
                            // Upload image
                            guard let url = URL(string: pictureUrl) else {
                                return
                            }
                            print("Downloading data from facebook image")
                            
                            URLSession.shared.dataTask(with: url) { (data, _, error) in
                                guard let data = data else {
                                    fatalError("Failed to downloading data")
                                }
                                
                                print("got data from fb, uploading...")
                                
                                // Upload image
                                let fileName = chatUser.profilePictureFileName
                                StorageManager.shared.uploadProfilePicture(with: data, fileName: fileName) { result in
                                    switch result {
                                        case .success(let downloadURL):
                                            UserDefaults.standard.set(downloadURL, forKey: "profile_picture_url")
                                            print("download picture url returned \(downloadURL)")
                                        case .failure(let errorDownloadURL):
                                            print("Storage manager \(errorDownloadURL)")
                                    }
                                }
                            }.resume()
                            
                        }
                    })
                }
            }
            
            let credential = FacebookAuthProvider.credential(withAccessToken: token)
            FirebaseAuth.Auth.auth().signIn(with: credential) { [weak self] (authResult, error) in
                guard let strongSelf = self else {
                    return
                }
                guard let _ = authResult, error == nil else {
                    if let error = error {
                        fatalError("Facebook credential login failed \(error)")
                    }
                    return
                }
                
                strongSelf.navigationController?.dismiss(animated: true, completion: nil)
            }
        })
        
    }
    
    
}
