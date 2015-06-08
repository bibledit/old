//
//  BibleditInstallation.m
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//


#import "BibleditInstallation.h"
#import "BibleditPaths.h"
#import "bibledit.h"


@implementation BibleditInstallation


+ (void) run
{
    // Bail out in case the installed version equals the bibledit library version.
    NSString * version = [self libraryVersion];
    if ([version isEqualToString:[self installedVersion]]) return;
    
    // Wait for some time till the Bibledit library has installed its resources.
    [NSThread sleepForTimeInterval:90];
    //NSLog(@"%s", "bibleditInstallResources after delay");

    NSString * webroot = [BibleditPaths documents];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *directoryEnumerator = [fileManager enumeratorAtPath:webroot];
    NSString *file;
    BOOL isDir;
    while (file = [directoryEnumerator nextObject]) {
        NSArray * components = [NSArray arrayWithObjects:webroot, file, nil];
        NSString * path = [NSString pathWithComponents:components];
        if ([fileManager fileExistsAtPath:path isDirectory:&isDir]) {
            // Exclude path from the iCloud backup.
            NSString * schemePath = [NSString stringWithFormat:@"file://%@", path];
            NSURL *url = [[NSURL alloc] initWithString:schemePath];
            [url setResourceValue: [NSNumber numberWithBool: YES] forKey: NSURLIsExcludedFromBackupKey error: nil];
        }
    }
    
    // Store the installed version in the preferences.
    [self installedVersion:version];
}


+ (NSString *) libraryVersion
{
    // Retieve the version of the Bibledit library.
    NSString *version = [NSString stringWithFormat:@"%s", bibledit_get_version_number ()];
    return version;
}


+ (NSString *) installedVersion
{
    // Retieve the version of Bibledit-Web that has been installed.
    NSString * version = [[NSUserDefaults standardUserDefaults] stringForKey:@"bibledit-version"];
    return version;
}


+ (void) installedVersion:(NSString *)version
{
    // Store the Bibledit-Web verion in the preferences.
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setObject:version forKey:@"bibledit-version"];
    [userDefaults synchronize];
}


@end
