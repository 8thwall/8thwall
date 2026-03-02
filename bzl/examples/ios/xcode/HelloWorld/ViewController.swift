//
//  ViewController.swift
//  VanillaIce
//
//  Created by Erik Murphy-Chutorian on 11/11/16.
//  Copyright © 2016 8th Wall. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
  
  // MARK: Properties
  @IBOutlet weak var nativeCodeLabel: UILabel!

  override func viewDidLoad() {
    super.viewDidLoad()
    // Do any additional setup after loading the view, typically from a nib.
    nativeCodeLabel.text = "Native msg goes here"
    nativeCodeLabel.text = String(c8_exampleInt());
    nativeCodeLabel.text = String(cString:c8_exampleString());

  }

  override func didReceiveMemoryWarning() {
    super.didReceiveMemoryWarning()
    // Dispose of any resources that can be recreated.
  }
}

