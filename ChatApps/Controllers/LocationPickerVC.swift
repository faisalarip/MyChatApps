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
import AVKit

protocol HandleMapSearch {
    func dropPinZoomIn(placemark:MKPlacemark)
}

class LocationPickerVC: UIViewController {
    
    public var completion: ((CLLocationCoordinate2D) -> Void)?
    private var coordinates: [CLLocationCoordinate2D] = []
    private var coordinate: CLLocationCoordinate2D?
    private let mapView = MKMapView()
    private var primaryRouteNavigate = MKRoute()
    private var routes: [MKRoute] = []
    private let locationManager = CLLocationManager()
    private var resultSearchController: UISearchController?
    private var forSharingLocation: Bool?
    private var searchResultTable = SearchResultTableVC()
    private let currentLocationAddressField = CustomView()
    private let destinationAddressField = CustomView()
    
    private var routeSteps: [MKRoute.Step] = []
    private var stepCounter = 0
    private let locationDistance: CLLocationDistance = 300
    private var navigationStarted = false
    private var getRoute = false
    private var speechSynthesizer = AVSpeechSynthesizer()
    
    init(coordinate: CLLocationCoordinate2D?, forShareLocation: Bool?) {
        super.init(nibName: nil, bundle: nil)
        self.coordinate = coordinate
        self.forSharingLocation = forShareLocation
    }
    
    private let startNavigationButton: UIButton = {
        let button = UIButton()
        button.isUserInteractionEnabled = true
        button.alpha = 0.3
        button.setTitle("START", for: .normal)
        button.titleLabel?.font = .systemFont(ofSize: 14, weight: .medium)
        button.titleLabel?.setTextSpacingBy(value: 1.5)
        button.setTitleColor(.systemBackground, for: .normal)
        return button
    }()
    
    private var getRouteDirectionButton: UIButton = {
        let button = UIButton()
        button.isUserInteractionEnabled = true
        button.titleLabel?.numberOfLines = 0
        button.alpha = 0.3
        button.setTitle("GET ROUTE", for: .normal)
        button.titleLabel?.font = .systemFont(ofSize: 11, weight: .medium)
        button.titleLabel?.setTextSpacingBy(value: 0.5)
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
        view.backgroundColor = .systemBackground
        view.addSubview(mapView)
        title = "Pick Location"

        setupMapUI()
        setupCurrentLocationButton()
        
    }
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        
        /// Check if that LocationPickerVC's opened for a sharing location, if that variable  forSharingLocation become true, otherwise become false
        if forSharingLocation == false {
            setupNavigationCardView()
        }
        
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        /// Check if that LocationPickerVC's opened for a sharing location, if that variable  forSharingLocation become true, otherwise become false
        if forSharingLocation == false {
            mapView.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: self.navigationCardView.top)
        } else {
            mapView.frame = view.bounds
        }
        
    }
    
    /// Adding annotation by user tapped on that mapView
    @objc private func didTapMap(_ gesture: UITapGestureRecognizer) {
        let locationInView = gesture.location(in: mapView)
        let coordinate = mapView.convert(locationInView, toCoordinateFrom: mapView)
        self.coordinate = coordinate
        
        /// Removing coordinate, if it had tapped more than 1
        for pinAnnotation in mapView.annotations {
            if forSharingLocation == true {
                mapView.removeAnnotation(pinAnnotation)
            } else {
                if (pinAnnotation.coordinate.latitude != self.coordinates.first?.latitude) &&
                    (pinAnnotation.coordinate.longitude != self.coordinates.first?.longitude) {
                    mapView.removeAnnotation(pinAnnotation)
                }
            }
        }
        
        /// drop a pin at that coordinate location on the mapView
        self.setupPinAnnotation(coordinate: coordinate)
        
    }
    
    @objc private func didTapShareButton() {
        guard let coordinate = coordinate else { return }
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
            locationManager.startUpdatingLocation()
        }
        
        sender.transform = CGAffineTransform(scaleX: 0.9, y: 0.9)
        UIView().buttonAnimationPopUp(sender)
        
    }
    
    @objc private func didTapStartNavigation(_ sender: UIButton) {
        sender.transform = CGAffineTransform(scaleX: 0.9, y: 0.9)
        UIView().buttonAnimationPopUp(sender)
        
        if !navigationStarted {
            navigationStarted = true
            guard let currentLocationCoordinate = coordinates.first else { return }
            self.centerViewToUserLocation(with: currentLocationCoordinate)
            self.getRouteSteps(with: primaryRouteNavigate)
            
            /// Change button view's
            sender.setTitle("CANCEL", for: .normal)
            sender.titleLabel?.setTextSpacingBy(value: 1.5)
            sender.setTitleColor(.systemRed, for: .normal)
            sender.layer.borderColor = UIColor.systemRed.cgColor
            sender.layer.borderWidth = 1.5
            sender.setGradientBackgroundColor(colors: [.white, .white], axis: .horizontal, cornerRadius: 8, apply: nil)
            
            getRouteDirectionButton.alpha = 0.3
        } else {
            navigationStarted = false
            self.mapView.removeOverlays(self.mapView.overlays)
            self.routeSteps.removeAll()
            self.stepCounter = 0
            
            /// Change button view's
            startNavigationButton.alpha = 0.3
            sender.setTitle("START", for: .normal)
            sender.layer.borderWidth = 0
            sender.setTitleColor(.systemBackground, for: .normal)
            mapView.setVisibleMapRect(primaryRouteNavigate.polyline.boundingMapRect, edgePadding: UIEdgeInsets(top: 32, left: 32, bottom: 32, right: 32), animated: true)
            
            let topColor = UIColor().hexStringToUIColor(hex: "#11998e")
            let bottomColor = UIColor().hexStringToUIColor(hex: "#38ef7d")
            startNavigationButton.setGradientBackgroundColor(colors: [topColor, bottomColor], axis: .horizontal, cornerRadius: 8, apply: nil)
            getRouteDirectionButton.alpha = 1.0
        }
    }
    
    @objc private func didTapGetRouteDirection(_ sender: UIButton) {
        sender.transform = CGAffineTransform(scaleX: 0.9, y: 0.9)
        UIView().buttonAnimationPopUp(sender)
        self.mapView.removeOverlays(self.mapView.overlays)
        self.configureStartNavigationRoute()
        self.startNavigationButton.alpha = 1.0
    }
    
    private func setupMapUI() {
        mapView.mapType = .standard
        mapView.delegate = self
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
            self.setupPinAnnotation(coordinate: coordinate)

            //configure auto zooming level based on region location
            self.setupCoordinateSpan(coordinate: coordinate)
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
    
    private func setupNavigationCardView() {
        currentLocationAddressField.frame = CGRect(x: 20, y: navigationCardView.top + 10 , width: 250, height: 50)
        lineImageView.frame = CGRect(x: 30, y: currentLocationAddressField.bottom - 2, width: 2, height: 15)
        destinationAddressField.frame = CGRect(x: 20, y: lineImageView.bottom - 2, width: 250, height: 50)

        getRouteDirectionButton.frame = CGRect(x: currentLocationAddressField.right + 10, y: navigationCardView.top + 18 , width: 80, height: 35)
        startNavigationButton.frame = CGRect(x: destinationAddressField.right + 10, y: getRouteDirectionButton.bottom + 25 , width: 80, height: 35)
        
        let topColor = UIColor().hexStringToUIColor(hex: "#11998e")
        let bottomColor = UIColor().hexStringToUIColor(hex: "#38ef7d")
        
        startNavigationButton.setGradientBackgroundColor(colors: [topColor, bottomColor], axis: .horizontal, cornerRadius: 8, apply: nil)
        startNavigationButton.addTarget(self, action: #selector(didTapStartNavigation(_:)), for: .touchUpInside)
        
        getRouteDirectionButton.setGradientBackgroundColor(colors: [topColor, bottomColor], axis: .horizontal, cornerRadius: 8, apply: nil)
        getRouteDirectionButton.addTarget(self, action: #selector(didTapGetRouteDirection(_:)), for: .touchUpInside)
        if self.mapView.overlays.isEmpty {
            getRouteDirectionButton.alpha = 1.0
        } else {
            getRouteDirectionButton.alpha = 0.3
        }
        
        lineImageView.image = UIImage().lineLocationImage
        
        navigationCardView.addSubview(currentLocationAddressField)
        navigationCardView.addSubview(lineImageView)
        navigationCardView.addSubview(destinationAddressField)
        navigationCardView.addSubview(startNavigationButton)
        navigationCardView.addSubview(getRouteDirectionButton)
        
        self.view.addSubview(navigationCardView)
                
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
    
    private func configureStartNavigationRoute() {
        guard let destinationCoordinate = coordinate,
              let currentLocCoordinate = coordinates.last else { return }
        
        let currentLocationPlacemark = MKPlacemark(coordinate: currentLocCoordinate)
        let destinationLocationPlaceMARK = MKPlacemark(coordinate: destinationCoordinate)
        
        let currentLocationMapItem = MKMapItem(placemark: currentLocationPlacemark)
        let destinationLocationMapItem = MKMapItem(placemark: destinationLocationPlaceMARK)
        
        let routeRequest = MKDirections.Request()
        routeRequest.source = currentLocationMapItem
        routeRequest.destination = destinationLocationMapItem
        routeRequest.transportType = .any
        routeRequest.requestsAlternateRoutes = true
        
        let directions = MKDirections(request: routeRequest)
        directions.calculate { [weak self] (response, error) in
            
            guard let response = response,
                  let primaryRoute = response.routes.first,
                  let strongSelf = self,
                  error == nil else {
                print("Error with \(String(describing: error?.localizedDescription))")
                return
            }
            strongSelf.primaryRouteNavigate = primaryRoute
            
            // Rearrange primary route from first index to last index
            var newRoutes = response.routes
            newRoutes.remove(at: 0)
            newRoutes.insert(primaryRoute, at: newRoutes.count)
            
            for route in newRoutes {
                strongSelf.routes.append(route)
                strongSelf.mapView.addOverlay(route.polyline, level: .aboveRoads)
                strongSelf.mapView.setCenter(route.polyline.coordinate, animated: true)
                strongSelf.mapView.setRegion(MKCoordinateRegion(center: route.polyline.coordinate, latitudinalMeters: route.distance*0.75, longitudinalMeters: route.distance*0.75), animated: true)
            }
            
        }
        
    }
    
    private func getRouteSteps(with route: MKRoute) {
        for monitoringRegion in locationManager.monitoredRegions {
            locationManager.stopMonitoring(for: monitoringRegion)
        }
        
        self.routeSteps = route.steps
        
        for i in 0..<routeSteps.count {
            let step = routeSteps[i]
            
            let region = CLCircularRegion(center: step.polyline.coordinate, radius: 20, identifier: "\(i)")
            locationManager.startMonitoring(for: region)
        }
        
        stepCounter += 1
        let firstMessageDistanceAndInstruction = "In \(routeSteps[stepCounter].distance.binade) meters \(routeSteps[stepCounter].instructions) then in \(routeSteps[stepCounter + 1].distance.binade) meters \(routeSteps[stepCounter + 1].instructions)"
        
        let speechUtterance = AVSpeechUtterance(string: firstMessageDistanceAndInstruction)
        speechSynthesizer.speak(speechUtterance)
    }
    
    private func centerViewToUserLocation(with center: CLLocationCoordinate2D) {
        let region = MKCoordinateRegion(center: center, latitudinalMeters: locationDistance, longitudinalMeters: locationDistance)
        mapView.setRegion(region, animated: true)
    }
    
    private func setupPinAnnotation(coordinate: CLLocationCoordinate2D) {
        let location = CLLocation(latitude: coordinate.latitude, longitude: coordinate.longitude)
        location.placemark { [weak self] (place, error) in
            guard let placemark = place,
                  let strongSelf = self,
                    error == nil else {
                print("Error", error ?? "error")
                return
            }
            strongSelf.coordinate = location.coordinate
            
            let pinAnnotation = MKPointAnnotation()
            pinAnnotation.coordinate = location.coordinate
            pinAnnotation.title = placemark.name
            pinAnnotation.subtitle = placemark.postalAddressFormatted
            strongSelf.mapView.addAnnotation(pinAnnotation)
            
            if strongSelf.coordinates.isEmpty {
                strongSelf.coordinates.append(coordinate)
                strongSelf.currentLocationAddressField.configureCustomView(with: UIImage().imageLocationCircle, placeName: placemark.name)
                strongSelf.destinationAddressField.configureCustomView(with: UIImage().imageMapPointEllipse, placeName: nil)
            } else {
                strongSelf.destinationAddressField.configureCustomView(with: UIImage().imageMapPointEllipse, placeName: placemark.name)
                strongSelf.getRouteDirectionButton.alpha = 1.0
            }
                        
            
        }
                
    }
    
    private func setupCoordinateSpan(coordinate: CLLocationCoordinate2D) {
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
        print("location has updated")
        
        guard let location = locations.last else { return }
        self.coordinate = location.coordinate
        self.setupPinAnnotation(coordinate: location.coordinate)
        
        ///configure auto zooming level based on region location
        self.setupCoordinateSpan(coordinate: location.coordinate)
    }
    
    func locationManager(_ manager: CLLocationManager, didChangeAuthorization status: CLAuthorizationStatus) {
        if status == .authorizedWhenInUse {
            locationManager.requestLocation()
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("Error: \(error)")
    }
    
    func locationManager(_ manager: CLLocationManager, didEnterRegion region: CLRegion) {
        print("didEnterRegion")
        stepCounter += 1
        
        if stepCounter < routeSteps.count {
            let counterMessageDistanceAndInstruction = "then in \(routeSteps[stepCounter].distance.binade) meters \(routeSteps[stepCounter].instructions)"
            
            let speechUtterance = AVSpeechUtterance(string: counterMessageDistanceAndInstruction)
            speechSynthesizer.speak(speechUtterance)
        } else {
            let finalMessage = "You have arrived at your destination"
            let speechUtterance = AVSpeechUtterance(string: finalMessage)
            speechSynthesizer.speak(speechUtterance)
            
            stepCounter = 0
            for monitorRegion in locationManager.monitoredRegions {
                locationManager.stopMonitoring(for: monitorRegion)
            }
            
        }
        
    }
}

// MARK: - MapView Delegate

extension LocationPickerVC: MKMapViewDelegate {
    
    func mapView(_ mapView: MKMapView, rendererFor overlay: MKOverlay) -> MKOverlayRenderer {
        if overlay is MKCircle {
            let renderer = MKCircleRenderer(overlay: overlay)
            renderer.fillColor = UIColor.black.withAlphaComponent(0.1)
            renderer.strokeColor = UIColor.systemGray
            renderer.lineWidth = 2
            renderer.lineJoin = .round
            return renderer
            
        } else if overlay is MKPolyline {
            let renderer = MKPolylineRenderer(overlay: overlay)
            renderer.lineWidth = 5
            renderer.lineJoin = .round
            if routes.last == primaryRouteNavigate {
                renderer.strokeColor = UIColor.systemBlue
            } else {
                renderer.strokeColor = UIColor.systemGray3
            }
            return renderer
        } else if overlay is MKPolygon {
            let renderer = MKPolygonRenderer(polygon: overlay as! MKPolygon)
            renderer.fillColor = UIColor.black.withAlphaComponent(0.5)
            renderer.strokeColor = UIColor.systemGray
            renderer.lineWidth = 2
            renderer.lineJoin = .round
            return renderer
        }
        
        return MKOverlayRenderer()
    }
    
    func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
        guard annotation is MKPointAnnotation else { return nil }

            let identifier = "Annotation"
        var annotationView = mapView.dequeueReusableAnnotationView(withIdentifier: identifier)

            if annotationView == nil {
                annotationView = MKPinAnnotationView(annotation: annotation, reuseIdentifier: identifier)
                annotationView?.canShowCallout = true
                
                annotationView?.rightCalloutAccessoryView = UIButton(type: .detailDisclosure)
                annotationView?.leftCalloutAccessoryView = UIButton(type: .detailDisclosure)
            } else {
                annotationView?.annotation = annotation
            }

            return annotationView
    }
    
    func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, calloutAccessoryControlTapped control: UIControl) {
        print("Tapped")
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
        
        /// Removing coordinate, if it had tapped more than 1
        for pinAnnotation in mapView.annotations {
            if forSharingLocation == true {
                mapView.removeAnnotation(pinAnnotation)
            } else {
                if (pinAnnotation.coordinate.latitude != self.coordinates.first?.latitude) &&
                    (pinAnnotation.coordinate.longitude != self.coordinates.first?.longitude) {
                    mapView.removeAnnotation(pinAnnotation)
                } else if mapView.annotations.count > 2 {
                    mapView.removeAnnotation(mapView.annotations[1])
                }
            }
        }
        
        self.setupPinAnnotation(coordinate: placemark.coordinate)
        self.setupCoordinateSpan(coordinate: placemark.coordinate)        
    }
}
