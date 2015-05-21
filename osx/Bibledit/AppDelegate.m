/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#import "AppDelegate.h"
#import "bibledit.h"
#import "Utilities.h"


@interface AppDelegate ()

@end


@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    [Utilities install];
    
    bibledit_initialize_library ();

    NSArray *components = [NSArray arrayWithObjects:[[NSBundle mainBundle] bundlePath], @"Contents", @"Resources", @"webroot", nil];
    NSString *path = [NSString pathWithComponents:components];
    path = [Utilities webrootPath];
    const char * document_root = [path UTF8String];
    bibledit_set_web_root (document_root);

    float timezoneoffset = ([[NSTimeZone systemTimeZone] secondsFromGMT] / 3600.0);
    bibledit_set_timezone_hours_offset_utc ((int)timezoneoffset);
    
    NSLog (@"Old document root %s", document_root); // Todo
    
    NSLog (@"Proper doc root %@", [Utilities webrootPath]);

    bibledit_start_library ();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    bibledit_stop_library ();
    while (bibledit_is_running ()) { };
    bibledit_shutdown_library ();
}

@end
