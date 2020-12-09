//
//  SearchResultTableVC.swift
//  ChatApps
//
//  Created by Faisal Arif on 07/12/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import MapKit

class SearchResultTableVC: UITableViewController {
    
    public var mapView: MKMapView? = nil
    public var handleMapSearchDelegate:HandleMapSearch? = nil
    private var places:[MKMapItem] = []
    private var searchCompleter: MKLocalSearchCompleter?
    private var searchRegion: MKCoordinateRegion = MKCoordinateRegion(MKMapRect.world)
    
    var completerResults: [MKLocalSearchCompletion]?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        tableView.register(SuggestionCompletionTableViewCell.self, forCellReuseIdentifier: SuggestionCompletionTableViewCell.identifier)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        startProvidingCompletions()
    }
    
    override func viewDidDisappear(_ animated: Bool) {
        super.viewDidDisappear(animated)
        stopProvidingCompletions()
    }
    
    private func startProvidingCompletions() {
        searchCompleter = MKLocalSearchCompleter()
        searchCompleter?.delegate = self
        searchCompleter?.resultTypes = .pointOfInterest
        searchCompleter?.region = searchRegion
    }
    
    private func stopProvidingCompletions() {
        searchCompleter = nil
    }
    
    private func searchBySuggestedCompletion(for suggestedCompletion: MKLocalSearchCompletion) {
        let suggested = MKLocalSearch.Request(completion: suggestedCompletion)
        searchRequest(using: suggested)
    }
    
    func searchByText(for queryString: String?) {
        let searchQuery = MKLocalSearch.Request()
        searchQuery.naturalLanguageQuery = queryString
        searchRequest(using: searchQuery)
    }
    
    /// - Tag: SearchRequest
    private func searchRequest(using searchRequest: MKLocalSearch.Request) {
        // Confine the map search area to an area around the user's current location.
        searchRequest.region = searchRegion
        
        // Include only point of interest results. This excludes results based on address matches.
        searchRequest.resultTypes = .pointOfInterest
                
        let localSearch = MKLocalSearch(request: searchRequest)
        localSearch.start { [unowned self] (response, error) in
            guard let response = response, error == nil else {
                self.displaySearchError(error)
                return
            }
            
            self.places = response.mapItems
            print("This results of response item \(response.mapItems)")
            
            let selectedItem = places[0].placemark
            handleMapSearchDelegate?.dropPinZoomIn(placemark: selectedItem)
            // Used when setting the map's region in `prepareForSegue`.
            let updatedRegion = response.boundingRegion
            self.searchRegion = updatedRegion
            
        }
    }
    
    private func displaySearchError(_ error: Error?) {
        if let error = error as NSError?, let errorString = error.userInfo[NSLocalizedDescriptionKey] as? String {
            let alertController = UIAlertController(title: "Could not find any places.", message: errorString, preferredStyle: .alert)
            alertController.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            present(alertController, animated: true, completion: nil)
        }
    }
}

extension SearchResultTableVC {
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return completerResults?.count ?? 0
    }
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        guard let suggestion = completerResults?[indexPath.row] else {
            self.displaySearchError(.none)
            return
        }
        
        self.searchBySuggestedCompletion(for: suggestion)
        dismiss(animated: true, completion: nil)
    }
    
    /// - Tag: HighlightFragment
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: SuggestionCompletionTableViewCell.identifier, for: indexPath)

        if let suggestion = completerResults?[indexPath.row] {
            // Each suggestion is a MKLocalSearchCompletion with a title, subtitle, and ranges describing what part of the title
            // and subtitle matched the current query string. The ranges can be used to apply helpful highlighting of the text in
            // the completion suggestion that matches the current query fragment.
            cell.textLabel?.attributedText = createHighlightedString(text: suggestion.title, rangeValues: suggestion.titleHighlightRanges)
            cell.detailTextLabel?.attributedText = createHighlightedString(text: suggestion.subtitle, rangeValues: suggestion.subtitleHighlightRanges)
        }

        return cell
    }
    
    private func createHighlightedString(text: String, rangeValues: [NSValue]) -> NSAttributedString {
        let attributes = [NSAttributedString.Key.backgroundColor: UIColor(named: "suggestionHighlight") ?? UIColor.systemGray3]
        let highlightedString = NSMutableAttributedString(string: text)
        
        // Each `NSValue` wraps an `NSRange` that can be used as a style attribute's range with `NSAttributedString`.
        let ranges = rangeValues.map { $0.rangeValue }
        ranges.forEach { (range) in
            highlightedString.addAttributes(attributes, range: range)
        }
        
        return highlightedString
    }
}

extension SearchResultTableVC: UISearchResultsUpdating {
    /// - Tag: UpdateQuery
    func updateSearchResults(for searchController: UISearchController) {
        // Ask `MKLocalSearchCompleter` for new completion suggestions based on the change in the text entered in `UISearchBar`.
        searchCompleter?.queryFragment = searchController.searchBar.text ?? ""
    }
    
}

extension SearchResultTableVC: MKLocalSearchCompleterDelegate {
    /// - Tag: QueryResults
    func completerDidUpdateResults(_ completer: MKLocalSearchCompleter) {
        // As the user types, new completion suggestions are continuously returned to this method.
        // Overwrite the existing results, and then refresh the UI with the new results.
        completerResults = completer.results
        tableView.reloadData()
    }
    
    func completer(_ completer: MKLocalSearchCompleter, didFailWithError error: Error) {
        // Handle any errors returned from MKLocalSearchCompleter.
        if let error = error as NSError? {
            print("MKLocalSearchCompleter encountered an error: \(error.localizedDescription). The query fragment is: \"\(completer.queryFragment)\"")
        }
    }}

private class SuggestionCompletionTableViewCell: UITableViewCell {
    
    static let identifier = "SuggestionCompletionTableViewCell"
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: .subtitle, reuseIdentifier: reuseIdentifier)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
}
