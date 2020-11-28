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

class LocationPickerVC: UIViewController {
    
    public var completion: ((CLLocationCoordinate2D) -> Void)?
    private var coordinates: CLLocationCoordinate2D?
    private var isPickable = true
    private let map = MKMapView()
    
    init(coordinates: CLLocationCoordinate2D?) {
        self.coordinates = coordinates
        self.isPickable  = false
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()

        view.addSubview(map)
        
        title = "Pick Location"
        map.isUserInteractionEnabled = true
        if isPickable {
            navigationItem.rightBarButtonItem = UIBarButtonItem(title: "Save",
                                                                style: .done,
                                                                target: self, action: #selector(saveButtonTapped))
            let gesture = UITapGestureRecognizer(target: self,
                                                 action: #selector(didTapMap(_:)))
            gesture.numberOfTouchesRequired = 1
            gesture.numberOfTapsRequired = 1
            map.addGestureRecognizer(gesture)
        } else {
            guard let coordinate = coordinates else { return }
            let pin = MKPointAnnotation()
            pin.coordinate = coordinate
            map.addAnnotation(pin)
            
        }
                
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        map.frame = view.bounds
    }
    
    @objc private func didTapMap(_ gesture: UITapGestureRecognizer) {
        let locationInView = gesture.location(in: map)
        let coordinate = map.convert(locationInView, toCoordinateFrom: map)
        self.coordinates = coordinate
        
        for pinAnnotation in map.annotations {
            map.removeAnnotation(pinAnnotation)
        }
        //drop a pin on the location
        let pin = MKPointAnnotation()
        pin.coordinate = coordinate
        map.addAnnotation(pin)
    }
    
    @objc private func saveButtonTapped() {
        guard let coordinates = coordinates else { return }
        navigationController?.popViewController(animated: true)
        completion?(coordinates)
    }
    
}
