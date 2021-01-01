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
    
    private var coordinate: CLLocationCoordinate2D?
    private var coordinates: [CLLocationCoordinate2D]?
    private let mapView = MKMapView()
    private let locationManager = CLLocationManager()
    private var resultSearchController: UISearchController?
    private var searchResultTable = SearchResultTableVC()
    
    init(coordinate: CLLocationCoordinate2D?) {
        self.coordinate = coordinate
        super.init(nibName: nil, bundle: nil)
    }
    
    private let startNavigationButton: UIButton = {
        let button = UIButton()
        button.isUserInteractionEnabled = true
        button.setTitle("START", for: .normal)
        button.titleLabel?.font = .systemFont(ofSize: 14, weight: .semibold)
        button.letterSpacing = 2.0
        button.setTitleColor(.systemBackground, for: .normal)
        return button
    }()
    
    private let navigationCardView: UIView = {
        let view = UIView()
        view.translatesAutoresizingMaskIntoConstraints = false
        view.backgroundColor = .systemBackground
        return view
    }()
    
    private let lineImageView: UIImageView = {
        let imageView = UIImageView()
        imageView.contentMode = .scaleAspectFill
        imageView.clipsToBounds = false
        imageView.backgroundColor = .systemBackground
        return imageView
    }()
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        view.addSubview(mapView)
        title = "Pick Location"

        setupMapUI()
        setupCurrentLocationButton()
//        setupStartNavigationUI()
        
    }
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        bottomRectangleView()
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        mapView.frame = view.bounds
        
        let customView1 = CustomView(frame: CGRect(x: 20, y: navigationCardView.top + 10 , width: 250, height: 50))
        customView1.configureCustomView(with: UIImage().imageLocationCircle, placeName: "Pakuwon mall")
        lineImageView.frame = CGRect(x: 30, y: customView1.bottom - 2, width: 2, height: 15)
        let customView2 = CustomView(frame: CGRect(x: 20, y: lineImageView.bottom - 2, width: 250, height: 50))
        customView2.configureCustomView(with: UIImage().imageMapPointEllipse, placeName: "Pakuwon mall")
        
        let topColor = hexStringToUIColor(hex: "#11998e")
        let bottomColor = hexStringToUIColor(hex: "#38ef7d")
        
        startNavigationButton.frame = CGRect(x: customView1.right + 10, y: navigationCardView.top + 20 , width: 80, height: 35)
        
        startNavigationButton.setGradientBackgroundColor(colors: [topColor, bottomColor], axis: .horizontal, cornerRadius: 8, apply: nil)
        
        lineImageView.image = UIImage().lineLocationImage
        
        navigationCardView.addSubview(customView1)
        navigationCardView.addSubview(lineImageView)
        navigationCardView.addSubview(customView2)
        navigationCardView.addSubview(startNavigationButton)
        
        mapView.addSubview(navigationCardView)
    }
    
    @objc private func didTapMap(_ gesture: UITapGestureRecognizer) {
        let locationInView = gesture.location(in: mapView)
        let coordinate = mapView.convert(locationInView, toCoordinateFrom: mapView)
        self.coordinate = coordinate
        
        for pinAnnotation in mapView.annotations {
            mapView.removeAnnotation(pinAnnotation)
        }
        
        //drop a pin on the location
        self.setUpPinAnnotation(coordinate: coordinate)
        
    }
    
    @objc private func didTapShareButton() {
        guard let coordinate = coordinate else { return }
        self.coordinates?.append(coordinate)
        print("recently coordinates \(coordinate)")
        navigationController?.popViewController(animated: true)
        completion?(coordinate)
    }
    
    @objc private func didTapCurrentLocationButton(_ sender: UIButton) {
        // Ask for permission thats will be always authorization
        locationManager.requestAlwaysAuthorization()
        // Ask for permission when user in foreground
        locationManager.requestWhenInUseAuthorization()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        
        if CLLocationManager.locationServicesEnabled() {
            print("Updating")
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
    
    @objc private func didTapStartNavigation(_ sender: UIButton) {
        print("Start Navigation has tapped")
        if !sender.isSelected {
            sender.isSelected = true
            sender.setTitleColor(.systemGray5, for: .selected)
            let alertController = UIAlertController(title: "Oppss, The destination's not yet", message: "Before you start a navigation, you must add a destination that you will to navigate", preferredStyle: .alert)
            alertController.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            alertController.addAction(UIAlertAction(title: "Cancel", style: .cancel, handler: { (_) in
                self.dismiss(animated: true, completion: nil)
            }))
            self.present(alertController, animated: true, completion: nil)
            
        } else {
            sender.isSelected = false
            sender.setTitleColor(.systemTeal, for: .selected)
//            configureStartNavigationRoute()
        }
        
    }
    
    private func setupMapUI() {
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
        
        let gesture = UITapGestureRecognizer(target: self,
                                             action: #selector(didTapMap(_:)))
        gesture.numberOfTouchesRequired = 1
        gesture.numberOfTapsRequired = 1
        mapView.addGestureRecognizer(gesture)
        
        if coordinate == nil {
            navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Share",
                                                                style: .done,
                                                                target: self, action: #selector(didTapShareButton))
        } else {
            guard let coordinate = coordinate else { return }
            self.setUpPinAnnotation(coordinate: coordinate)
            
            //configure auto zooming level based on region location
            self.setUpCoordinateSpan(coordinate: coordinate)
        }
    }
    
    private func setupCurrentLocationButton() {
        let locationImage = UIImage(systemName: "location", withConfiguration: UIImage.SymbolConfiguration(weight: .regular))?.withTintColor(.systemTeal, renderingMode: .alwaysOriginal)
        let iconTextButtonViewModel = IconTextViewModel(title: "Get current location", icon: locationImage, backgroundColor: nil)
        let currentLocationButton = CustomButton(frame: CGRect(x: 22.5, y: view.frame.height/5.5, width: view.frame.width-45, height: 50))
        
        currentLocationButton.configure(with: iconTextButtonViewModel)
        currentLocationButton.addTarget(self, action: #selector(self.didTapCurrentLocationButton(_:)), for: .touchUpInside)
        
        mapView.addSubview(currentLocationButton)
    }
    
    private func setupStartNavigationUI() {
        mapView.addSubview(startNavigationButton)
                
        startNavigationButton.backgroundColor = .tertiarySystemBackground
        startNavigationButton.translatesAutoresizingMaskIntoConstraints = false
        startNavigationButton.addTarget(self, action: #selector(self.didTapStartNavigation(_:)), for: .touchUpInside)
        
        if #available(iOS 11.0, *) {
            let guide = self.view.safeAreaLayoutGuide
            NSLayoutConstraint.activate([
                startNavigationButton.trailingAnchor.constraint(equalTo: guide.trailingAnchor),
                startNavigationButton.leadingAnchor.constraint(equalTo: guide.leadingAnchor),
                startNavigationButton.bottomAnchor.constraint(equalTo: guide.bottomAnchor),
                
                startNavigationButton.heightAnchor.constraint(equalToConstant: 80)
            ])
        }
        else {
            NSLayoutConstraint(item: startNavigationButton, attribute: .bottom, relatedBy: .equal, toItem: view, attribute: .bottom, multiplier: 1.0, constant: 0).isActive = true
            NSLayoutConstraint(item: startNavigationButton, attribute: .leading, relatedBy: .equal, toItem: view, attribute: .leading, multiplier: 1.0, constant: 0).isActive = true
            NSLayoutConstraint(item: startNavigationButton, attribute: .trailing, relatedBy: .equal, toItem: view, attribute: .trailing, multiplier: 1.0, constant: 0).isActive = true
            
            startNavigationButton.heightAnchor.constraint(equalToConstant: 80).isActive = true
        }
        
    }
    
    private func bottomRectangleView() {
        if #available(iOS 11.0, *) {
            let guide = self.view.safeAreaLayoutGuide
            NSLayoutConstraint.activate([
                
                navigationCardView.trailingAnchor.constraint(equalTo: guide.trailingAnchor),
                navigationCardView.leadingAnchor.constraint(equalTo: guide.leadingAnchor),
                navigationCardView.bottomAnchor.constraint(equalTo: guide.bottomAnchor),
                
                navigationCardView.widthAnchor.constraint(equalToConstant: view.frame.width),
                navigationCardView.heightAnchor.constraint(equalToConstant: 130)
            ])
        }
        else {
            NSLayoutConstraint(item: navigationCardView, attribute: .bottom, relatedBy: .equal, toItem: view, attribute: .bottom, multiplier: 1.0, constant: 0).isActive = true
            NSLayoutConstraint(item: navigationCardView, attribute: .leading, relatedBy: .equal, toItem: view, attribute: .leading, multiplier: 1.0, constant: 0).isActive = true
            NSLayoutConstraint(item: navigationCardView, attribute: .trailing, relatedBy: .equal, toItem: view, attribute: .trailing, multiplier: 1.0, constant: 0).isActive = true

            navigationCardView.heightAnchor.constraint(equalToConstant: 130).isActive = true
        }
    }
    
    private func setUpPinAnnotation(coordinate: CLLocationCoordinate2D) {
        let location = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
        location.placemark { [weak self] (place, error) in
            guard let placemark = place, let strongSelf = self, error == nil else {
                print("Error", error ?? "error")
                return
            }
            
            let pinAnnotation = MKPointAnnotation()
            pinAnnotation.coordinate = location.coordinate
            pinAnnotation.title = placemark.name
            pinAnnotation.subtitle = placemark.postalAddressFormatted
            
            self?.coordinate = location.coordinate
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
        self.coordinate = location.coordinate
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

// MARK: - Gradient UIColor

extension LocationPickerVC {
    func setGradientBackground(colorTop: UIColor, colorBottom: UIColor){
            let gradientLayer = CAGradientLayer()
            gradientLayer.colors = [colorBottom.cgColor, colorTop.cgColor]
            gradientLayer.startPoint = CGPoint(x: 0.5, y: 1.0)
            gradientLayer.endPoint = CGPoint(x: 0.5, y: 0.0)
            gradientLayer.locations = [0, 1]
            gradientLayer.frame = startNavigationButton.bounds

            startNavigationButton.layer.insertSublayer(gradientLayer, at: 0)
    }
    
    func hexStringToUIColor (hex:String) -> UIColor {
        var cString:String = hex.trimmingCharacters(in: .whitespacesAndNewlines).uppercased()

        if (cString.hasPrefix("#")) {
            cString.remove(at: cString.startIndex)
        }

        if ((cString.count) != 6) {
            return UIColor.gray
        }

        var rgbValue:UInt64 = 0
        Scanner(string: cString).scanHexInt64(&rgbValue)

        return UIColor(
            red: CGFloat((rgbValue & 0xFF0000) >> 16) / 255.0,
            green: CGFloat((rgbValue & 0x00FF00) >> 8) / 255.0,
            blue: CGFloat(rgbValue & 0x0000FF) / 255.0,
            alpha: CGFloat(1.0)
        )
    }
}
