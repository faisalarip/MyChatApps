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
    private var primaryRouteNavigation = MKRoute()
    private var routes: [MKRoute] = []
    private let locationManager = CLLocationManager()
    private var resultSearchController: UISearchController?
    private var forSharingLocation: Bool?
    private var searchResultTable = SearchResultTableVC()
    private let currentLocationAddressField = CustomView()
    private let destinationAddressField = CustomView()
    
    private var routeSteps: [MKRoute.Step] = []
    private var stepCounter = 0
    private var navigationStarted = false
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
        title = "Map View"
        
        setupMapUI()
        setupCurrentLocationButton()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(true)
        /// Check if the LocationPickerVC's opened for a sharing location and if true, then variable  forSharingLocation become true, otherwise become false
        if forSharingLocation == false {
            setupNavigationCardView()
        }
        
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        /// Check if the LocationPickerVC's opened for a sharing location and if true, then variable  forSharingLocation become true, otherwise become false
        if forSharingLocation == false {
            mapView.frame = CGRect(x: 0, y: 0, width: self.view.frame.width, height: self.navigationCardView.top)
        } else {
            mapView.frame = view.bounds
        }
        
    }
    
    /// Adding annotation by user tapped on that mapView
    @objc private func didTapMap(_ gesture: UITapGestureRecognizer) {
        if gesture.state == .recognized {
            // Get map coordinate from touch point
            let touchPt: CGPoint = gesture.location(in: mapView)
            let coord: CLLocationCoordinate2D = mapView.convert(touchPt, toCoordinateFrom: mapView)
            let maxMeters: Double = self.setupMetersForDetectPolyline(fromPixel: 22, at: touchPt)
            var nearestDistance: Float = MAXFLOAT
            var nearestPoly: MKPolyline? = nil
            // for every overlay ...
            for overlay: MKOverlay in mapView.overlays {
                // .. if MKPolyline ...
                if (overlay is MKPolyline) {
                    // ... get the distance ...
                    let distance: Float = Float(self.setupDistanceForDetectPolyline(pt: MKMapPoint(coord), toPoly: overlay as! MKPolyline))
                    // ... and find the nearest one
                    if distance < nearestDistance {
                        nearestDistance = distance
                        nearestPoly = overlay as? MKPolyline
                    }
                    
                }
            }
            
            if Double(nearestDistance) <= maxMeters {
                guard let nearestPolyline = nearestPoly else { return }
                print("Touched poly: \(nearestPolyline) distance: \(nearestDistance)")
                
                var position = 0
                for route in routes {
                    if route.polyline == nearestPolyline {
                        self.primaryRouteNavigation = route
                        break
                    }
                    position += 1
                }
                
                // Rearrange primary route from first index to last index
                var newRoutes = routes
                newRoutes.remove(at: position)
                newRoutes.insert(primaryRouteNavigation, at: routes.count-1)
                routes.removeAll()
                
                for newRoute in newRoutes {
                    routes.append(newRoute)
                    mapView.addOverlay(newRoute.polyline, level: .aboveRoads)
                    mapView.setCenter(newRoute.polyline.coordinate, animated: true)
                    mapView.setRegion(MKCoordinateRegion(center: newRoute.polyline.coordinate, latitudinalMeters: newRoute.distance*0.85, longitudinalMeters: newRoute.distance*0.85), animated: true)
                }
                
            } else {
                self.coordinate = coord
                
                // Removing coordinate, if it had tapped more than 1
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
                
                // drop a pin at that coordinate location on the mapView
                self.setupPinAnnotation(coordinate: coord)
            }
        }
        
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
            print("Updating to current location")
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
            self.setupCenterViewToAnnotation(coordinate: currentLocationCoordinate)
            self.setupRouteStepsForMoving(route: primaryRouteNavigation)
            
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
            mapView.setVisibleMapRect(primaryRouteNavigation.polyline.boundingMapRect, edgePadding: UIEdgeInsets(top: 32, left: 32, bottom: 32, right: 32), animated: true)
            
            let topColor = UIColor().hexStringToUIColor(hex: "#11998e")
            let bottomColor = UIColor().hexStringToUIColor(hex: "#38ef7d")
            startNavigationButton.setGradientBackgroundColor(colors: [topColor, bottomColor], axis: .horizontal, cornerRadius: 8, apply: nil)
            getRouteDirectionButton.alpha = 1.0
        }
    }
    
    @objc private func didTapGetRouteDirection(_ sender: UIButton) {
        sender.transform = CGAffineTransform(scaleX: 0.9, y: 0.9)
        UIView().buttonAnimationPopUp(sender)
        mapView.removeOverlays(self.mapView.overlays)
        routes.removeAll()
        routeSteps.removeAll()
        setupNavigationRoute()
        startNavigationButton.alpha = 1.0
    }

}

// MARK: - Setup UI Component of View Controller

extension LocationPickerVC {
    private func setupMapUI() {
        mapView.mapType = .standard
        mapView.showsTraffic = true
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
        
        if forSharingLocation == true {
            navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Share",
                                                                style: .done,
                                                                target: self, action: #selector(didTapShareButton))
        } else {
            guard let coordinate = coordinate else { return }
            
            ///configure pin annotation and automatically zooming a level view based on region location
            self.setupPinAnnotation(coordinate: coordinate)
            self.setupCenterViewToAnnotation(coordinate: coordinate)
            print("pinAnnotation count from viewDidLoad \(mapView.annotations)")
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
        currentLocationAddressField.frame = CGRect(x: 20, y: navigationCardView.top + 10 , width: self.view.width - 120, height: 50)
        lineImageView.frame = CGRect(x: 30, y: currentLocationAddressField.bottom - 2, width: 2, height: 15)
        destinationAddressField.frame = CGRect(x: 20, y: lineImageView.bottom - 2, width: self.view.width - 120, height: 50)
        
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
}

// MARK: - Setup MapView's UI

extension LocationPickerVC {
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
            print("pinAnnotation count from setupPin \(strongSelf.mapView.annotations)")
            
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
    
    private func setupCenterViewToAnnotation(coordinate: CLLocationCoordinate2D) {
        let zoomRect = MKMapRect.null
        let span = MKCoordinateSpan(latitudeDelta: 0.01, longitudeDelta: 0.01)
        let region = MKCoordinateRegion(center: coordinate, span: span)
        
        self.mapView.setVisibleMapRect(zoomRect, edgePadding: UIEdgeInsets(top: 100, left: 100, bottom: 100, right: 100), animated: true)
        self.mapView.setRegion(mapView.regionThatFits(region), animated: true)
    }
    
    private func setupDistanceForDetectPolyline(pt: MKMapPoint, toPoly poly: MKPolyline) -> Double {
        var distance: Double = Double(MAXFLOAT)
        for n in 0..<poly.pointCount - 1 {
            let ptA = poly.points()[n]
            let ptB = poly.points()[n + 1]
            let xDelta: Double = ptB.x - ptA.x
            let yDelta: Double = ptB.y - ptA.y
            if xDelta == 0.0 && yDelta == 0.0 {
                // Points must not be equal
                continue
            }
            let u: Double = ((pt.x - ptA.x) * xDelta + (pt.y - ptA.y) * yDelta) / (xDelta * xDelta + yDelta * yDelta)
            var ptClosest: MKMapPoint
            if u < 0.0 {
                ptClosest = ptA
            }
            else if u > 1.0 {
                ptClosest = ptB
            }
            else {
                ptClosest = MKMapPoint(x: ptA.x + u * xDelta, y: ptA.y + u * yDelta)
            }

            distance = min(distance, ptClosest.distance(to: pt))
        }
        return distance
    }

    private func setupMetersForDetectPolyline(fromPixel px: Int, at pt: CGPoint) -> Double {
        let ptB = CGPoint(x: pt.x + CGFloat(px), y: pt.y)
        let coordA: CLLocationCoordinate2D = mapView.convert(pt, toCoordinateFrom: mapView)
        let coordB: CLLocationCoordinate2D = mapView.convert(ptB, toCoordinateFrom: mapView)
        return MKMapPoint(coordA).distance(to: MKMapPoint(coordB))
    }
    
    private func setupNavigationRoute() {
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
            strongSelf.primaryRouteNavigation = primaryRoute
            
            // Rearrange primary route from first index to last index
            print("First routes\(response.routes)")
            var newRoutes = response.routes
            newRoutes.remove(at: 0)
            newRoutes.insert(primaryRoute, at: newRoutes.count)
            print("second routes\(newRoutes)")
            
            for route in newRoutes {
                
                strongSelf.routes.append(route)
                strongSelf.mapView.addOverlay(route.polyline, level: .aboveRoads)
                strongSelf.mapView.setCenter(route.polyline.coordinate, animated: true)
                strongSelf.mapView.setRegion(MKCoordinateRegion(center: route.polyline.coordinate, latitudinalMeters: route.distance*0.85, longitudinalMeters: route.distance*0.85), animated: true)
            }
            
        }
        
    }
    
    private func setupRouteStepsForMoving(route: MKRoute) {
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
}


// MARK: - Location Manager Delegete

extension LocationPickerVC: CLLocationManagerDelegate {
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {                
        guard let location = locations.last else { return }
        self.coordinate = location.coordinate
        
        ///configure pin annotation and auto zooming level based on region location
        self.setupPinAnnotation(coordinate: location.coordinate)
        self.setupCenterViewToAnnotation(coordinate: location.coordinate)
        print("pinAnnotation count from updatelocation \(mapView.annotations)")
        
        /// Removing coordinate, if it had tapped more than 1
        for pinAnnotation in mapView.annotations {
            print("annotation repeat \(pinAnnotation)")
            if forSharingLocation == true {
                mapView.removeAnnotation(pinAnnotation)
            } else {
                if mapView.annotations.count > 2 {
                    mapView.removeAnnotation(mapView.annotations.first!)
                } else if (pinAnnotation.coordinate.latitude != self.coordinates.first?.latitude) &&
                    (pinAnnotation.coordinate.longitude != self.coordinates.first?.longitude) {
                    mapView.removeAnnotation(pinAnnotation)
                }
                
            }
        }
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
        guard overlay is MKPolyline else {
            return MKOverlayRenderer()
        }
        
        let renderer = MKPolylineRenderer(overlay: overlay)
        renderer.lineWidth = 5
        renderer.lineJoin = .round        
        
        if primaryRouteNavigation == routes.last {
            renderer.strokeColor = UIColor.systemBlue
        } else {
            renderer.strokeColor = UIColor.systemGray3
        }
        
        return renderer
    }
    
    func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
        guard annotation is MKPointAnnotation else { return nil }
        
        let identifier = "Annotation"
        var annotationView = mapView.dequeueReusableAnnotationView(withIdentifier: identifier)
        annotationView = nil
        annotationView?.detailCalloutAccessoryView = nil
        
        if annotationView == nil {
            annotationView = MKPinAnnotationView(annotation: annotation, reuseIdentifier: identifier)
            annotationView?.canShowCallout = true
            
            let detailButton = UIButton(type: .detailDisclosure)
            annotationView?.rightCalloutAccessoryView = detailButton
        }
        
        return annotationView
    }
    
    func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, calloutAccessoryControlTapped control: UIControl) {
        
        if control.state == .highlighted {
            let closeButton = UIButton(type: .custom)
            closeButton.frame = CGRect(origin: .zero, size: CGSize(width: 32, height: 32))
            closeButton.setImage(UIImage(systemName: "xmark", withConfiguration: UIImage.SymbolConfiguration(weight: .semibold))?.withTintColor(.systemBlue, renderingMode: .alwaysOriginal), for: .normal)
            closeButton.isSelected = true
            view.rightCalloutAccessoryView = closeButton
            
            guard let coord = coordinate else { return }
            let rect = CGRect(origin: .zero, size: CGSize(width: 250, height: 200))
            
            let snapshotView = UIView()
            snapshotView.translatesAutoresizingMaskIntoConstraints = false
            
            let mapCamera = MKMapCamera()
            mapCamera.centerCoordinate = coord
            mapCamera.altitude = 500
            mapCamera.heading = 45
            mapCamera.pitch = 45
            
            let options = MKMapSnapshotter.Options()
            options.size = rect.size
            options.mapType = .hybridFlyover
            options.showsBuildings = true
            options.camera = mapCamera
            
            let snapshotter = MKMapSnapshotter(options: options)
            snapshotter.start { snapshot, error in
                guard let snapshot = snapshot, error == nil else {
                    print(error ?? "Unknown error")
                    return
                }
                let imageView = UIImageView(frame: rect)
                imageView.image = snapshot.image
                snapshotView.addSubview(imageView)
            }
            
            view.detailCalloutAccessoryView = snapshotView
            
            NSLayoutConstraint.activate([
                snapshotView.widthAnchor.constraint(equalToConstant: rect.width),
                snapshotView.heightAnchor.constraint(equalToConstant: rect.height)
            ])
        } else {
            view.detailCalloutAccessoryView = nil
            view.rightCalloutAccessoryView = UIButton(type: .detailDisclosure)
        }
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
        ///configure pin annotation and auto zooming level based on region location
        self.setupPinAnnotation(coordinate: placemark.coordinate)
        self.setupCenterViewToAnnotation(coordinate: placemark.coordinate)
        print("pinAnnotation count from handle\(mapView.annotations)")
        /// Removing coordinate, if it had tapped more than 1
        for pinAnnotation in mapView.annotations {
            print("annotation repeat \(pinAnnotation)")
            if forSharingLocation == true {
                mapView.removeAnnotation(pinAnnotation)
            } else {
                if mapView.annotations.count > 2 {
                    mapView.removeAnnotation(mapView.annotations.first!)
                } else if (pinAnnotation.coordinate.latitude != self.coordinates.first?.latitude) &&
                    (pinAnnotation.coordinate.longitude != self.coordinates.first?.longitude) {
                    mapView.removeAnnotation(pinAnnotation)
                }
                
            }
        }
        
    }
}

