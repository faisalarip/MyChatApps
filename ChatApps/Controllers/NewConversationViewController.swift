//
//  NewConversationViewController.swift
//  ChatApps
//
//  Created by Faisal Arif on 20/06/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import JGProgressHUD

<<<<<<< HEAD
class NewConversationViewController: UIViewController {
    
    public var completion: (([String: String]) -> Void)?
    
    private let spinner = JGProgressHUD(style: .dark)
    
    private var users = [[String: String]]()
    
    private var result = [[String: String]]()
=======
struct SearchResult {
    let name: String
    let recepientEmail: String
}

class NewConversationViewController: UIViewController {
    
    public var completion: ((SearchResult) -> Void)?
    
    private let spinner = JGProgressHUD(style: .dark)
    
    private var users = [[String:String]]()
    
    private var result = [SearchResult]()
>>>>>>> origin/develop12
    
    private var hasFetched = false
    
    private let searchBar: UISearchBar = {
        let searchBar = UISearchBar()
        searchBar.searchBarStyle = .prominent
        searchBar.placeholder = "Search for user.."
        return searchBar
    }()
    
    private let tableView: UITableView = {
        let tableView = UITableView()
        tableView.isHidden = true
<<<<<<< HEAD
        tableView.register(UITableViewCell.self, forCellReuseIdentifier: "cell")
=======
        tableView.register(NewConversationsCell.self,
                           forCellReuseIdentifier: NewConversationsCell.identifier)
>>>>>>> origin/develop12
        return tableView
    }()
    
    private let noResultLabel: UILabel = {
       let label = UILabel()
        label.isHidden = true
        label.text = "No Result"
        label.textAlignment = .center
        label.textColor = .green
        label.font = .systemFont(ofSize: 16, weight: .medium)
        return label
    }()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.addSubview(noResultLabel)
        view.addSubview(tableView)
        
        tableView.delegate = self
        tableView.dataSource = self
        searchBar.delegate = self
        
        view.backgroundColor = .systemBackground
        navigationController?.navigationBar.topItem?.titleView = searchBar
        navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Cancel", style: .done, target: self, action: #selector(dismissSelf))
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        tableView.frame = view.bounds
        
        noResultLabel.frame = CGRect(x: view.width/4, y: (view.height-20), width: view.width/2, height: 200)
    }
    
    @objc private func dismissSelf() {
        dismiss(animated: true, completion: nil)
    }
    
}

extension NewConversationViewController: UITableViewDelegate, UITableViewDataSource {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return result.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
<<<<<<< HEAD
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        cell.textLabel?.text = result[indexPath.row]["name"]
=======
        guard let cell = tableView.dequeueReusableCell(withIdentifier: NewConversationsCell.identifier,
                                                       for: indexPath) as? NewConversationsCell else {
            return UITableViewCell(style: .default, reuseIdentifier: "cell")
        }
        
        let model = result[indexPath.row]
        cell.configure(with: model)
        
>>>>>>> origin/develop12
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let targetUserData = result[indexPath.row]
//        print("\(targetUserData)")
        dismiss(animated: true, completion: { [weak self] in
            self?.completion?(targetUserData)
        })
        
    }
<<<<<<< HEAD
=======
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 70
    }
>>>>>>> origin/develop12
}

extension NewConversationViewController: UISearchBarDelegate {
    
    func searchBar(_ searchBar: UISearchBar, textDidChange searchText: String) {
        
        searchBar.becomeFirstResponder()
        
        guard !searchText.replacingOccurrences(of: " ", with: "").isEmpty else {
            return
        }
        result.removeAll()
        spinner.show(in: view)
        self.searchUser(query: searchText)
    }
    
    func searchUser(query: String) {
        // check if array has firebase result
        if hasFetched {
            self.filterUsers(with: query)
        } else {
            DatabaseManager.shared.getAllUsers { [weak self] (result) in
                guard let self = self else {
                    return
                }
                switch result {
                    case .success(let usersCollection):
                        self.hasFetched = true
                        self.users = usersCollection
                        self.filterUsers(with: query)
                    case .failure(let error):
                        print("Failed to get users \(error)")
                }
            }
        }
    }
    
    func filterUsers(with term: String) {
<<<<<<< HEAD
        guard hasFetched else {
            return
        }
        
        let results: [[String: String]] = self.users.filter({
=======
        guard let currentUserEmail = UserDefaults.standard.value(forKey: "email") as? String, hasFetched else {
            return
        }
        
        let safeCurrentEmail = DatabaseManager.shared.safeEmail(with: currentUserEmail)

        let results: [SearchResult] = self.users.filter({
    
            guard let email = $0["email"], email != safeCurrentEmail else {
                return false
            }
            
>>>>>>> origin/develop12
            guard let name = $0["name"]?.lowercased() else {
                return false
            }
            return name.contains(term.lowercased())
        }).sorted(by: {
            guard let first: String = ($0["name"] as AnyObject).title else { return false }
            guard let second: String = ($1["name"] as AnyObject).title else { return true }
            return first < second
<<<<<<< HEAD
=======
        }).compactMap({
            
            guard let email = $0["email"], let name = $0["name"] else {
                return nil
            }
                        
            return SearchResult(name: name, recepientEmail: email)
>>>>>>> origin/develop12
        })
        
        self.result = results
        updateUI()
    }
    
    func updateUI() {
        self.spinner.dismiss()
        if result.isEmpty {
            self.noResultLabel.isHidden = false
            self.tableView.isHidden = true
        } else {
            self.noResultLabel.isHidden = true
            self.tableView.isHidden = false
            self.tableView.reloadData()
        }
    }
    
    func searchBarSearchButtonClicked(_ searchBar: UISearchBar) {
        searchBar.resignFirstResponder()
    }
}
