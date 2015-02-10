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
    // Todo if ([version isEqualToString:[self installedVersion]]) return;

    NSLog (@"%@", [BibleditPaths resources]); // Todo
    
    NSArray *inputComponents = [NSArray arrayWithObjects:[BibleditPaths resources], @"webroot", nil];
    NSString *inputDirectory = [NSString pathWithComponents:inputComponents];
    
    NSString *outputDirectory = [BibleditPaths documents];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *directoryEnumerator = [fileManager enumeratorAtPath:inputDirectory];
    NSString *file;
    BOOL isDir;
    while (file = [directoryEnumerator nextObject]) {
        inputComponents = [NSArray arrayWithObjects:inputDirectory, file, nil];
        NSString * inputPath = [NSString pathWithComponents:inputComponents];
        NSArray * outputComponents = [NSArray arrayWithObjects:outputDirectory, file, nil];
        NSString * outputPath = [NSString pathWithComponents:outputComponents];
        if ([fileManager fileExistsAtPath:inputPath isDirectory:&isDir]) {
            if (isDir) {
                [fileManager createDirectoryAtPath:outputPath withIntermediateDirectories:YES attributes:nil error:nil];
            } else {
                // Copy file to destination. First remove old item to prevent a copy error.
                [fileManager removeItemAtPath:outputPath error:nil];
                [fileManager copyItemAtPath:inputPath toPath:outputPath error:nil];
                //NSLog (@"%@", outputPath); // Todo
            }
            
        }
    }
    
    NSLog (@"%@", [BibleditPaths documents]); // Todo
    // Store the installed version in the preferences.
    [self installedVersion:version];
}


+ (NSString *) libraryVersion
{
    // Retieve the version of the Bibledit library.
    NSString *version = [NSString stringWithFormat:@"%s", bibledit_version_number ()];
    
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
