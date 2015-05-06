//
//  ViewController.m
//  Bibledit
//
//  Created by Teus Benschop on 08/04/2015.
//  Copyright (c) 2015 Teus Benschop. All rights reserved.
//

#import "ViewController.h"


@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
    // Update the view, if already loaded.
}

- (IBAction)Button:(id)sender {
    NSString * myurl = @"http://localhost:8080";
    NSURL *url = [NSURL URLWithString:myurl];
    [[NSWorkspace sharedWorkspace] openURL:url];
}

@end
