//
//  LocationVC.swift
//  ChatApps
//
//  Created by Faisal Arif on 25/11/20.
//  Copyright © 2020 AppBrewery. All rights reserved.
//

import UIKit
import CoreLocation
import MapKit

protocol HandleMapSearch {
    func dropPinZoomIn(placemark:MKPlacemark)
}

class LocationPickerVC: UIViewController {
    
    public var completion: ((CLLocationCoordinate2D) -> Void)?
    
    private var coordinates: CLLocationCoordinate2D?
    private let mapView = MKMapView()
    private let locationManager = CLLocationManager()
    private var resultSearchController: UISearchController?
    private var searchResultTable = SearchResultTableVC()
    
    init(coordinates: CLLocationCoordinate2D?) {
        self.coordinates = coordinates
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.addSubview(mapView)
        
        title = "Pick Location"
        
        configureMapUI()
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        mapView.frame = view.bounds
    }
    
    @objc private func didTapMap(_ gesture: UITapGestureRecognizer) {
        let locationInView = gesture.location(in: mapView)
        let coordinate = mapView.convert(locationInView, toCoordinateFrom: mapView)
        self.coordinates = coordinate
        
        for pinAnnotation in mapView.annotations {
            mapView.removeAnnotation(pinAnnotation)
        }
        
        //drop a pin on the location
        self.setUpPinAnnotation(coordinate: coordinate)
    }
    
    @objc private func didTapShareButton() {
        guard let coordinates = coordinates else { return }
        navigationController?.popViewController(animated: true)
        completion?(coordinates)
    }
    
    private func configureMapUI() {
        mapView.mapType = .standard
        mapView.isZoomEnabled = true
        mapView.isScrollEnabled = true
        mapView.isUserInteractionEnabled = true
        
        if coordinates == nil {
            navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Share",
                                                                style: .done,
                                                                target: self, action: #selector(didTapShareButton))
            let gesture = UITapGestureRecognizer(target: self,
                                                 action: #selector(didTapMap(_:)))
            gesture.numberOfTouchesRequired = 1
            gesture.numberOfTapsRequired = 1
            mapView.addGestureRecognizer(gesture)
            
            resultSearchController?.searchBar.delegate = self
            resultSearchController = UISearchController(searchResultsController: searchResultTable)
            resultSearchController?.searchResultsUpdater = searchResultTable
            
            let searchBar = resultSearchController?.searchBar
            searchBar?.sizeToFit()
            searchBar?.placeholder = "Search for places"
            navigationItem.searchController = resultSearchController
            
            resultSearchController?.hidesNavigationBarDuringPresentation = false
            definesPresentationContext = true
            
            searchResultTable.mapView = mapView
            searchResultTable.handleMapSearchDelegate = self
            
            presentActionSheet()
        } else {
            guard let coordinate = coordinates else { return }
            self.setUpPinAnnotation(coordinate: coordinate)
            
            //configure auto zooming level based on region location
            self.setUpCoordinateSpan(coordinate: coordinate)
        }
    }
    
    private func presentActionSheet() {
        
        let alertController = UIAlertController(title: "Share Location", message: "Would you like to share your current location?", preferredStyle: .alert)
        let searchAction = UIAlertAction(title: "OK", style: .default) { [weak self] (_) in
            guard let strongSelf = self else { return }
            // Ask for permission thats will be always authorization
            strongSelf.locationManager.requestAlwaysAuthorization()
            // Ask for permission when user in foreground
            strongSelf.locationManager.requestWhenInUseAuthorization()
            strongSelf.locationManager.delegate = self
            strongSelf.locationManager.desiredAccuracy = kCLLocationAccuracyBest
            
            if CLLocationManager.locationServicesEnabled() {
                strongSelf.locationManager.startUpdatingLocation()
            }
        }
        let currentLocation = UIAlertAction(title: "Cancel", style: .cancel) { [weak self] (_) in            
            self?.dismiss(animated: true, completion: nil)
        }
        alertController.addAction(searchAction)
        alertController.addAction(currentLocation)        
        self.present(alertController, animated: true, completion: nil)        
    }
    
    private func setUpPinAnnotation(coordinate: CLLocationCoordinate2D) {
        let location = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
        location.placemark { [weak self] (place, error) in
            guard let placemark = place, let strongSelf = self, error == nil else {
                print("Error", error ?? "error")
                return
            }
            print("Success to get a place names \(String(describing: placemark.name))")
            print("Success to get a place Address \(String(describing: placemark.postalAddressFormatted))")
            
            let pinAnnotation = MKPointAnnotation()
            pinAnnotation.coordinate = location.coordinate
            pinAnnotation.title = placemark.name
            pinAnnotation.subtitle = placemark.postalAddressFormatted
            
            strongSelf.mapView.addAnnotation(pinAnnotation)
        }
                
    }
    
    private func setUpCoordinateSpan(coordinate: CLLocationCoordinate2D) {
        let zoomRect = MKMapRect.null
        let span = MKCoordinateSpan(latitudeDelta: 0.01, longitudeDelta: 0.01)
        let region = MKCoordinateRegion(center: coordinate, span: span)
        
        self.mapView.setVisibleMapRect(zoomRect, edgePadding: UIEdgeInsets(top: 100, left: 100, bottom: 100, right: 100), animated: true)
        self.mapView.setRegion(mapView.regionThatFits(region), animated: true)
    }
    
}

// MARK: - Location Manager Delegete

extension LocationPickerVC: CLLocationManagerDelegate {
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        
        guard let location = locations.last else { return }
        self.coordinates = location.coordinate
        self.setUpPinAnnotation(coordinate: location.coordinate)
        
        //configure auto zooming level based on region location
        self.setUpCoordinateSpan(coordinate: location.coordinate)
    }
    
    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        if status == .authorizedWhenInUse {
            locationManager.requestLocation()
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("Error: \(error)")
    }
}

// MARK: - Search Bar Delegete

extension LocationPickerVC: UISearchBarDelegate {
    func searchBarCancelButtonClicked(_ searchBar: UISearchBar) {
        searchBar.resignFirstResponder()
    }
    
    func searchBarTextDidBeginEditing(_ searchBar: UISearchBar) {
        searchBar.setShowsCancelButton(true, animated: true)
    }
    
    func searchBarTextDidEndEditing(_ searchBar: UISearchBar) {
        searchBar.setShowsCancelButton(false, animated: true)
    }
    
    func searchBarSearchButtonClicked(_ searchBar: UISearchBar) {
        searchBar.resignFirstResponder()
        dismiss(animated: true, completion: nil)
        
        // The user tapped search on the `UISearchBar` or on the keyboard. Since they didn't
        // select a row with a suggested completion, run the search with the query text in the search field.
        print("THIS IS SEARCH RESULTS \(searchBar.text ?? "")")
        searchResultTable.searchByText(for: searchBar.text)
    }
}

// MARK: - Handle Map Search

extension LocationPickerVC: HandleMapSearch {
    func dropPinZoomIn(placemark:MKPlacemark){
        // clear existing pins
        mapView.removeAnnotations(mapView.annotations)
        
        self.setUpPinAnnotation(coordinate: placemark.coordinate)
        self.setUpCoordinateSpan(coordinate: placemark.coordinate)        
    }
}
