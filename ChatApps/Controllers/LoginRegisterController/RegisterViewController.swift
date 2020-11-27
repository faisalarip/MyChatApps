//
//  RegisterViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseAuth
import JGProgressHUD

class RegisterViewController: UIViewController {
    
    private let spinner = JGProgressHUD(style: .dark)
    
    private let scrollView: UIScrollView = {
        let scrollView = UIScrollView()
        scrollView.clipsToBounds = true
        return scrollView
    }()
    
    private let imageView: UIImageView = {
        let imageView = UIImageView()
        imageView.image = UIImage(systemName: "person.circle")
        imageView.backgroundColor = .systemBackground
        imageView.contentMode = .scaleAspectFit
        imageView.clipsToBounds = true
        imageView.layer.masksToBounds = true
        //        imageView.layer.borderWidth = 2
        //        imageView.layer.borderColor = UIColor.lightGray.cgColor
        return imageView
    }()
    
    private let firstNameField: UITextField = {
        let textFeild = UITextField()
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .continue
        textFeild.layer.cornerRadius = 12
        textFeild.placeholder = "First name"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
        return textFeild
    }()
    
    private let lastNameField: UITextField = {
        let textFeild = UITextField()
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .continue
        textFeild.layer.cornerRadius = 12
        textFeild.placeholder = "Last name"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
        return textFeild
    }()
    
    private let emailfield: UITextField = {
        let textFeild = UITextField()
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .continue
        textFeild.layer.cornerRadius = 12
        textFeild.placeholder = "Username or Email"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
        return textFeild
    }()
    
    private let passwordField: UITextField = {
        let textFeild = UITextField()
        textFeild.layer.borderWidth = 1
        textFeild.layer.borderColor = UIColor.lightGray.cgColor
        textFeild.autocapitalizationType = .none
        textFeild.autocorrectionType = .no
        textFeild.returnKeyType = .done
        textFeild.layer.cornerRadius = 12
        textFeild.placeholder = "Password"
        textFeild.leftView = UIView(frame: CGRect(x: 0, y: 0, width: 10, height: 0))
        textFeild.leftViewMode = .always
//        textFeild.isSecureTextEntry = true        
        return textFeild
    }()
    
    private let registerButton: UIButton = {
        let button = UIButton()
        button.setTitle("Register", for: .normal)
        button.backgroundColor = .link
        button.setTitleColor(.white, for: .normal)
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 28
        button.titleLabel?.font = UIFont.systemFont(ofSize: 16, weight: .bold)
        return button
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        title = "Create New Account"
        view.backgroundColor = .systemBackground
        
        registerButton.addTarget(self, action: #selector(registerButtonTapped), for: .touchUpInside)
        
        emailfield.delegate = self
        passwordField.delegate = self
        firstNameField.delegate = self
        lastNameField.delegate = self        
        
        imageView.isUserInteractionEnabled = true
        scrollView.isUserInteractionEnabled = true
        
//        DispatchQueue.main.async {
//            self.firstNameField.addBottomBorderWithColor(color: UIColor.lightGray, width: 1.5)
//            self.lastNameField.addBottomBorderWithColor(color: UIColor.lightGray, width: 1.5)
//            self.emailfield.addBottomBorderWithColor(color: UIColor.lightGray, width: 1.5)
//            self.passwordField.addBottomBorderWithColor(color: UIColor.lightGray, width: 1.5)
//        }
        
        view.addSubview(scrollView)
        scrollView.addSubview(imageView)
        scrollView.addSubview(firstNameField)
        scrollView.addSubview(lastNameField)
        scrollView.addSubview(emailfield)
        scrollView.addSubview(passwordField)
        scrollView.addSubview(registerButton)
        
        let gesture = UITapGestureRecognizer(target: self, action: #selector(didTapChangeProfile))
        //        gesture.numberOfTouches =
        gesture.numberOfTapsRequired = 1
        imageView.addGestureRecognizer(gesture)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        
        scrollView.frame = view.bounds
        
        let sizeImage = scrollView.width/4
        imageView.frame = CGRect(x: (scrollView.width-sizeImage)/2, y: 30, width: sizeImage, height: sizeImage)
        imageView.layer.cornerRadius = imageView.width/2.0
        
        firstNameField.frame = CGRect(x: 30, y: imageView.bottom+45, width: scrollView.width-60, height: 52)
        lastNameField.frame = CGRect(x: 30, y: firstNameField.bottom+25, width: scrollView.width-60, height: 52)
        emailfield.frame = CGRect(x: 30, y: lastNameField.bottom+25, width: scrollView.width-60, height: 52)
        passwordField.frame = CGRect(x: 30, y: emailfield.bottom+25, width: scrollView.width-60, height: 52)
        registerButton.frame = CGRect(x: 30, y: passwordField.bottom+45, width: scrollView.width-60, height: 52)
    }
    
    @objc private func didTapChangeProfile() {
        print("Change photo profile called")
        presentPhotoActionSheet()
    }
    
    @objc private func registerButtonTapped() {
        
        firstNameField.resignFirstResponder()
        lastNameField.resignFirstResponder()
        emailfield.resignFirstResponder()
        passwordField.resignFirstResponder()
        
        guard let email = emailfield.text, let password = passwordField.text, let firstName = firstNameField.text,  let lastName = lastNameField.text, !firstName.isEmpty, !lastName.isEmpty, !email.isEmpty, !password.isEmpty, password.count >= 6 else {
            alertUserLoginError(message: "Please enter all information to log In")

            return }
        print(email)
        
        spinner.show(in: view)
        
        /// Firebase Register Management
        DatabaseManager.shared.validationNewUser(with: email) { [weak self] (newUser) in
            guard let strongSelf = self else {
                return
            }
            
            DispatchQueue.main.async {
                strongSelf.spinner.dismiss()
            }
                        
            guard !newUser else {
                // Email already exist
                strongSelf.alertUserLoginError(message: "Looks like a user account for that email address already exist. ")
                return
            }
            
            /// Adding new account from register to database
            FirebaseAuth.Auth.auth().createUser(withEmail: email, password: password) { (authResult, error) in
                
                guard authResult != nil,
                    error == nil else {
                    print("Oppss.. Failed creating account")
                    return
                }
                
                let chatUser = ChatAppsUser(firstName: firstName,
                                            lastName: lastName,
                                            emailAddress: email)
                DatabaseManager.shared.insertUser(with: chatUser, completion: { success in
                    if success {
                        // Upload Image
                        guard let image = strongSelf.imageView.image, let data = image.pngData() else {
                            return
                        }
                        
                        let fileName = chatUser.profilePictureFileName
                        StorageManager.shared.uploadProfilePicture(with: data, fileName: fileName) { result in
                            switch result {
                                case .success(let downloadURL):
                                    UserDefaults.standard.set(downloadURL, forKey: "profile_picture_url")
                                print("\(downloadURL)")
                                
                                case .failure(let errorDownloadURL):
                                print("Storage manager \(errorDownloadURL)")
                            }
                        }
                    }
                    })
                
                strongSelf.navigationController?.dismiss(animated: true, completion: nil)
            }
        }
    }
    
    private func alertUserLoginError(message: String) {
        let alert = UIAlertController(title: "Woops..", message: message, preferredStyle: .alert)
        
        alert.addAction(UIAlertAction(title: "Dismiss", style: .cancel, handler: nil))
        present(alert, animated: true)
    }
    
}

extension RegisterViewController: UITextFieldDelegate {
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if textField == firstNameField {
            lastNameField.becomeFirstResponder()
        } else if textField == lastNameField {
            lastNameField.resignFirstResponder()
            emailfield.becomeFirstResponder()
        } else if textField == emailfield {
            emailfield.resignFirstResponder()
            passwordField.becomeFirstResponder()
        } else if textField == passwordField {
            passwordField.resignFirstResponder()
        }
        
        return true
    }
    
}

extension RegisterViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    
    func presentPhotoActionSheet() {
        
        let actionSheet = UIAlertController(title: "Profile pciture", message: "How would you like to select a picture?", preferredStyle: .actionSheet)
        actionSheet.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: nil))
        actionSheet.addAction(UIAlertAction(title: "Take photo", style: .default, handler: { [weak self] (_) in
            self?.presentCamera()
        }))
        actionSheet.addAction(UIAlertAction(title: "Choose photo", style: .default, handler: { [weak self] (_) in
            self?.presentPhotoPicker()
        }))
        
        present(actionSheet, animated: true)
    }
    
    func presentCamera() {
        let vc =  UIImagePickerController()
        vc.sourceType = .camera
        vc.delegate = self
        vc.allowsEditing = true
        present(vc, animated: true)
    }
    
    func presentPhotoPicker() {
        let vc = UIImagePickerController()
        vc.sourceType = .photoLibrary
        vc.delegate = self
        vc.allowsEditing = true
        present(vc, animated: true)
    }
    
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [UIImagePickerController.InfoKey : Any]) {
        picker.dismiss(animated: true, completion: nil)
        
        guard let selectedImage = info[UIImagePickerController.InfoKey.editedImage] as? UIImage else {
            return
        }
        imageView.image = selectedImage
    }
    
    func imagePickerControllerDidCancel(_ picker: UIImagePickerController) {
        picker.dismiss(animated: true, completion: nil)
    }
    
}

extension UIView {
    func addBottomBorderWithColor(color: UIColor, width: CGFloat) {
        let border = CALayer()
        border.backgroundColor = color.cgColor
        border.frame = CGRect(x: 0, y: self.frame.size.height - width,
                              width: self.frame.size.width, height: width)
        self.layer.addSublayer(border)
    }
}
