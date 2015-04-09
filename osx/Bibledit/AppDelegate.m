//
//  AppDelegate.m
//  Bibledit
//
//  Created by Teus Benschop on 08/04/2015.
//  Copyright (c) 2015 Teus Benschop. All rights reserved.
//

#import "AppDelegate.h"
#import "bibledit.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    bibledit_initialize_library ();

    NSArray *components = [NSArray arrayWithObjects:[[NSBundle mainBundle] bundlePath], @"Contents", @"Resources", @"webroot", nil];
    NSString *path = [NSString pathWithComponents:components];
    const char * document_root = [path UTF8String];
    bibledit_set_web_root (document_root);

    float timezoneoffset = ([[NSTimeZone systemTimeZone] secondsFromGMT] / 3600.0);
    bibledit_set_timezone_hours_offset_utc ((int)timezoneoffset);
    
    bibledit_start_library ();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    bibledit_stop_library ();
    while (bibledit_is_running ()) { };
    bibledit_shutdown_library ();
}

@end
