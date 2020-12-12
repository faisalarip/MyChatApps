//
//  ViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import FirebaseAuth
import JGProgressHUD

/// Controller class for showing list conversation
final class ConversationViewController: UIViewController {
    
    private let spinner = JGProgressHUD(style: .dark)
    
    private var conversations: [Conversation] = []
    
    private let tableView: UITableView = {
        let table = UITableView()
        table.isHidden = true
        table.register(ConversationCell.self, forCellReuseIdentifier: ConversationCell.identifier)        
        return table
    }()
    
    private let noConversationFirstLabel: UILabel = {
       let label  = UILabel()
        label.text = "Oppss, you doesn't have any conversation with anyone"
        label.textAlignment = .center
        label.numberOfLines = 0
        label.lineBreakMode = .byWordWrapping
        label.textColor = .label
        label.font = .systemFont(ofSize: 16, weight: .medium)
        label.isHidden = true
        return label
    }()
    
    private let noConversationSecondLabel: UILabel = {
       let label  = UILabel()
        label.text = "Tap here to start a conversation"
        label.textAlignment = .center
        label.isUserInteractionEnabled = true
        label.clipsToBounds = true
        label.textColor = .link
        label.font = .systemFont(ofSize: 16, weight: .semibold)
        label.isHidden = true
        return label
    }()
    
    private var loginObserved: NSObjectProtocol?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        navigationItem.rightBarButtonItem = UIBarButtonItem(barButtonSystemItem: .compose, target: self, action: #selector(didTapComposeButton))
        
        view.addSubview(tableView)
                
        tableView.dataSource = self
        tableView.delegate = self
        startListeningForConversation()
        
        let gesture = UITapGestureRecognizer(target: self, action: #selector(didTapComposeButton))
        gesture.numberOfTapsRequired = 1
        gesture.numberOfTouchesRequired = 1
        noConversationSecondLabel.addGestureRecognizer(gesture)
        
        
        loginObserved = NotificationCenter.default.addObserver(forName: .didLogInNotification, object: nil, queue: .main, using: { [weak self] ( _ ) in
            guard let strongSelf = self else {
                return
            }
            strongSelf.startListeningForConversation()
        })
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        validateLogIn()
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        tableView.frame = view.bounds
    }
    
    private func startListeningForConversation() {
        guard let email = UserDefaults.standard.value(forKey: "email") as? String else { return }
        
        if let oberserver = loginObserved {
            NotificationCenter.default.removeObserver(oberserver)
        }
        
        print("Starting conversations fetch...")
        let safeEmail = DatabaseManager.shared.safeEmail(with: email)
        DatabaseManager.shared.getAllConversation(for: safeEmail) { [weak self] (result) in
            switch result {
                case .success(let conversations):
                    print("successfully get conversation model")
                    guard !conversations.isEmpty else { return }
                    self?.tableView.isHidden = false
                    self?.noConversationFirstLabel.isHidden = true
                    self?.noConversationSecondLabel.isHidden = true
                    self?.conversations = conversations
                    DispatchQueue.main.async {
                        self?.tableView.reloadData()
                    }
                case .failure(let error):
                    //Reloading data for deleted conversations interface
                    self?.tableView.isHidden = true
                    self?.setUpLayoutNoConversationLabel()
                    print("Failed to get conversations \(error)")
            }
        }
        
    }
    
    @objc private func didTapComposeButton() {
        let vc = NewConversationViewController()
        vc.completion = { [weak self] result in
            print("\(result)")

            let currentConvo = self?.conversations
                        
            if let targetConvo = currentConvo?.first(where: {
                $0.otherUserEmail == DatabaseManager.shared.safeEmail(with: result.recepientEmail)
            }) {
                let chatVC = ChatViewController(with: targetConvo.otherUserEmail, id: targetConvo.id)
                chatVC.isNewConversation = false
                chatVC.title = targetConvo.name
                chatVC.navigationItem.largeTitleDisplayMode = .never
                self?.navigationController?.pushViewController(chatVC, animated: true)
            } else {
                // create new conversation
                self?.createNewConversation(with: result)
            }
            
        }
        let nav = UINavigationController(rootViewController: vc)
        nav.modalPresentationStyle = .popover
        present(nav, animated: true)
    }
    
    private func createNewConversation(with result: SearchResult) {
        let name = result.name
        let emailRecepient = DatabaseManager.shared.safeEmail(with: result.recepientEmail)
        
        // Check in database if conversation with these two users exists
        // if it does, reuse conversation Id
        // otherwise, user existing code        
        DatabaseManager.shared.conversationExists(with: emailRecepient) { [weak self] (result) in
            switch result {
            case .success(let conversationId):
                let vc = ChatViewController(with: emailRecepient, id: conversationId)
                vc.isNewConversation = false
                vc.title = name
                vc.navigationItem.largeTitleDisplayMode = .never
                self?.navigationController?.pushViewController(vc, animated: true)
            case .failure(_):
                let vc = ChatViewController(with: emailRecepient, id: nil)
                vc.isNewConversation = true
                vc.title = name
                vc.navigationItem.largeTitleDisplayMode = .never
                self?.navigationController?.pushViewController(vc, animated: true)
            }
            
        }
        
    }
    
    private func validateLogIn() {
        // Persistance data user for login
        if FirebaseAuth.Auth.auth().currentUser == nil {
            let loginVC = LoginViewController()
            let nav = UINavigationController(rootViewController: loginVC)
            nav.modalPresentationStyle = .fullScreen
            present(nav, animated: true)
        }
    }
    
    private func setUpLayoutNoConversationLabel() {
        noConversationFirstLabel.isHidden = false
        noConversationSecondLabel.isHidden = false
        let stackview = UIStackView()
        stackview.axis = .vertical
        stackview.spacing = 8
        stackview.distribution = .equalSpacing
        stackview.translatesAutoresizingMaskIntoConstraints = false
        stackview.addArrangedSubview(noConversationFirstLabel)
        noConversationFirstLabel.widthAnchor.constraint(equalToConstant: 230).isActive = true
        noConversationFirstLabel.heightAnchor.constraint(equalToConstant: 70).isActive = true
        
        stackview.addArrangedSubview(noConversationSecondLabel)
        noConversationSecondLabel.widthAnchor.constraint(equalToConstant: self.view.frame.width).isActive = true
        noConversationSecondLabel.heightAnchor.constraint(equalToConstant: 30).isActive = true
        
        self.view.addSubview(stackview)
        stackview.centerYAnchor.constraint(equalTo: view.centerYAnchor).isActive = true
        stackview.centerXAnchor.constraint(equalTo: view.centerXAnchor).isActive = true
    }
    
}

// MARK: - Table View Delegate and Data Source

extension ConversationViewController: UITableViewDelegate, UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {        
        return conversations.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        
        let model = conversations[indexPath.row]
        let cell = tableView.dequeueReusableCell(withIdentifier: ConversationCell.identifier, for: indexPath) as! ConversationCell
        
        cell.configure(with: model)
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let model = conversations[indexPath.row]

        let vc = ChatViewController(with: model.otherUserEmail, id: model.id)
        vc.title = model.name
        vc.navigationItem.largeTitleDisplayMode = .never
        navigationController?.pushViewController(vc, animated: true)
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 85
    }
    
    func tableView(_ tableView: UITableView, editingStyleForRowAt indexPath: IndexPath) -> UITableViewCell.EditingStyle {
        return .delete
    }
    
    func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCell.EditingStyle, forRowAt indexPath: IndexPath) {
        if editingStyle == .delete {
            tableView.beginUpdates()
            
            let conversationId = conversations[indexPath.row].id
            self.conversations.remove(at: indexPath.row)
            tableView.deleteRows(at: [indexPath], with: .left)
            
            DatabaseManager.shared.deleteConversation(conversationId: conversationId) { [weak self] (success) in
                if !success {
                    // add model and row back and show error
                }
            }

            tableView.endUpdates()
        }
    }
    
}
