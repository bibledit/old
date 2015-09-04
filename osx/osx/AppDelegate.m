//
//  AppDelegate.m
//  osx
//
//  Created by Teus Benschop on 28/05/2015.
//  Copyright (c) 2015 Teus Benschop. All rights reserved.
//

#import "AppDelegate.h"
#import <WebKit/WebKit.h>
#import "bibledit.h"

@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;

@property (strong) id activity;

@end


@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {

    // When the Bibledit app is in the background, OS X puts it to sleep.
    // This is the "App Nap".
    // It has been noticed that even after coming out of the nap, Bibledit remains slowish,
    // and uses a lot of CPU resources.
    // Disable App Nap:
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
        self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:0x00FFFFFF reason:@"runs a web server"];
    }
    
    NSArray *components = [NSArray arrayWithObjects:[[NSBundle mainBundle] resourcePath], @"webroot", nil];
    NSString *packagePath = [NSString pathWithComponents:components];

    NSString * documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    components = [NSArray arrayWithObjects:documents, @"webroot", nil];
    NSString *webrootPath = [NSString pathWithComponents:components];

    const char * package = [packagePath UTF8String];
    const char * webroot = [webrootPath UTF8String];
    
    bibledit_initialize_library (package, webroot);
    bibledit_open_browser ();
    bibledit_start_library ();
    
    // Open the web app in the web view
    NSURL *url = [NSURL URLWithString:@"http://localhost:9876"];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
    [self.window setContentView:self.webview];
    
    float timezoneoffset = ([[NSTimeZone systemTimeZone] secondsFromGMT] / 3600.0);
    bibledit_set_timezone_hours_offset_utc ((int)timezoneoffset);
    
    [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector (checkForExternalBrowser) userInfo:nil repeats:YES];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    bibledit_stop_library ();
    while (bibledit_is_running ()) {}
    bibledit_shutdown_library ();
}

- (void)checkForExternalBrowser {
    if (bibledit_open_browser ()) {
        NSURL *url = [NSURL URLWithString:@"http://localhost:9876"];
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
    return YES;
}

@end
