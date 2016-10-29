//
//  UIDelegate.m
//  Bibledit
//
//  Created by Teus Benschop on 12/01/2016.
//  Copyright © 2016 Teus Benschop. All rights reserved.
//

#import "UIDelegate.h"
#import <WebKit/WebKit.h>

@implementation UIDelegate


- (void)webView:(WebView *)sender runOpenPanelForFileButtonWithResultListener:(id < WebOpenPanelResultListener >)resultListener
{
    // Create the File Open Dialog class.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    // Enable the selection of files in the dialog.
    [openDlg setCanChooseFiles:YES];
    
    // Enable the selection of directories in the dialog.
    [openDlg setCanChooseDirectories:NO];
    
    if ( [openDlg runModal] == NSModalResponseOK )
    {
        NSArray* files = [[openDlg URLs]valueForKey:@"relativePath"];
        [resultListener chooseFilenames:files];
    }
}


@end
