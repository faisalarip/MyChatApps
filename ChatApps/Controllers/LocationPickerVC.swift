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
        configureUIButton()
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
        print("recently coordinates \(coordinates)")
        navigationController?.popViewController(animated: true)
        completion?(coordinates)
    }
    
    private func configureMapUI() {
        mapView.mapType = .standard
        mapView.isZoomEnabled = true
        mapView.isScrollEnabled = true
        mapView.isUserInteractionEnabled = true
                
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
        
        if coordinates == nil {
            navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Share",
                                                                style: .done,
                                                                target: self, action: #selector(didTapShareButton))
            let gesture = UITapGestureRecognizer(target: self,
                                                 action: #selector(didTapMap(_:)))
            gesture.numberOfTouchesRequired = 1
            gesture.numberOfTapsRequired = 1
            mapView.addGestureRecognizer(gesture)
        } else {
            guard let coordinate = coordinates else { return }
            self.setUpPinAnnotation(coordinate: coordinate)
            
            //configure auto zooming level based on region location
            self.setUpCoordinateSpan(coordinate: coordinate)
        }
    }
    
    private func configureUIButton() {
        let locationImage = UIImage(systemName: "location", withConfiguration: UIImage.SymbolConfiguration(weight: .regular))?.withTintColor(.systemTeal, renderingMode: .alwaysOriginal)
        let iconTextButtonViewModel = IconTextViewModel(title: "Get current location", icon: locationImage, backgroundColor: nil)
        let currentLocationButton = CustomButton(frame: CGRect(x: 22.5, y: view.frame.height/6, width: view.frame.width-45, height: 50))
        
        currentLocationButton.configure(with: iconTextButtonViewModel)
        currentLocationButton.addTarget(self, action: #selector(self.didTapCurrentLocationButton(_:)), for: .touchUpInside)
        
        mapView.addSubview(currentLocationButton)
    }
    
    @objc private func didTapCurrentLocationButton(_ sender: UIButton) {
        // Ask for permission thats will be always authorization
        locationManager.requestAlwaysAuthorization()
        // Ask for permission when user in foreground
        locationManager.requestWhenInUseAuthorization()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        
        if CLLocationManager.locationServicesEnabled() {
            locationManager.startUpdatingLocation()
        }
        
        sender.transform = CGAffineTransform(scaleX: 0.9, y: 0.9)

        UIView.animate(withDuration: 2.5,
                       delay: 0,
                       usingSpringWithDamping: CGFloat(0.20),
                       initialSpringVelocity: CGFloat(9.0),
                       options: UIView.AnimationOptions.allowUserInteraction,
                       animations: {
                        sender.transform = CGAffineTransform.identity
                       },
                       completion: { Void in()  }
        )
    }
    
    
    private func setUpPinAnnotation(coordinate: CLLocationCoordinate2D) {
        let location = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
        location.placemark { [weak self] (place, error) in
            guard let placemark = place, let strongSelf = self, error == nil else {
                print("Error", error ?? "error")
                return
            }
            print("Got the place of names \(String(describing: placemark.name))")
            print("Got the place of Address \(String(describing: placemark.postalAddressFormatted))")
            print(location)
            
            let pinAnnotation = MKPointAnnotation()
            pinAnnotation.coordinate = location.coordinate
            pinAnnotation.title = placemark.name
            pinAnnotation.subtitle = placemark.postalAddressFormatted
            
            self?.coordinates = location.coordinate
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
