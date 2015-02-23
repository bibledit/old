//
//  BibleditPaths.m
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import "BibleditController.h"
#import "BibleditPaths.h"
#import "BibleditInstallation.h"
#import "bibledit.h"
#import <mach/mach.h>


@implementation BibleditController


UIView * mainUIView = NULL;
WKWebView *webview;


+ (void) bibleditAppLaunched
{
    bibledit_initialize_library ();

    NSArray *components = [NSArray arrayWithObjects:[BibleditPaths documents], @"webroot", nil];
    NSString *path = [NSString pathWithComponents:components];
    const char * document_root = [path UTF8String];
    bibledit_set_web_root (document_root);
}


+ (void) bibleditViewHasLoaded:(UIView *)uiview
{
    mainUIView = uiview;
}


+ (void) bibleditInstallResources
{
    NSLog (@"%s", "install"); // Todo
    // Display message to user.
    dispatch_async(dispatch_get_main_queue(), ^{
        NSArray *components = [NSArray arrayWithObjects:[BibleditPaths resources], @"setup", @"setup.html", nil];
        NSString *path = [NSString pathWithComponents:components];
        path = [path stringByAppendingString:@"file://"];
        NSLog (path, @""); // Todo
        [BibleditController bibleditBrowseTo:path];
    });
    // Run the installation.
    [BibleditInstallation run];
    // Open Bibledit-Web main page.
    dispatch_async(dispatch_get_main_queue(), ^{
        [BibleditController bibleditBrowseTo:@"http://localhost:8080"];
    });
}


+ (void) bibleditEnteredForeground
{
    bibledit_start_library ();
}


+ (void) bibleditOpenWebView
{
    WKWebViewConfiguration *theConfiguration = [[WKWebViewConfiguration alloc] init];
    webview = [[WKWebView alloc] initWithFrame:mainUIView.frame configuration:theConfiguration];
    [mainUIView addSubview:webview];
}


+ (void) bibleditBrowseTo:(NSString*)urlString
{
    NSURL *url = [NSURL URLWithString:urlString];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [webview loadRequest:urlRequest];
}


+ (void) bibleditReceivedMemoryWarning
{
    // There are huge memory leaks in UIWebView.
    // The memory usage keeps creeping up over time when it displays dynamic pages.
    // iOS sends a few memory warnings after an hour or so, then iOS kills the app.
    // WKWeb​View is new on iOS 8 and uses and leaks far less memory.
    // It uses the webkit rendering engine, and a faster javascript engine.
    // The best solution to the above memory problems is to use WKWebView.
    // That has been implemented.

    bibledit_log ("The device runs low on memory.");
    
    struct mach_task_basic_info info;
    mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info (mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &size);
    if (kerr == KERN_SUCCESS) {
        NSString *string = [NSString stringWithFormat:@"Memory in use: %lld Mb", info.resident_size / 1024 / 1024];
        const char * message = [string UTF8String];
        bibledit_log (message);
    }
}


+ (void) bibleditWillEnterBackground
{
    // Before the app enters the background, suspend the library, and wait till done.
    bibledit_stop_library ();
    while (bibledit_is_running ()) { };
}


+ (void) bibleditWillTerminate
{
    bibledit_shutdown_library ();
}


@end
