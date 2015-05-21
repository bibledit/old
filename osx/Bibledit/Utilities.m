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


#import <Foundation/Foundation.h>
#import "Utilities.h"
#import "bibledit.h"


@implementation Utilities


+ (NSString *)resourcesPath
{
    NSString * path = [[NSBundle mainBundle] resourcePath];
    return path;
}


+ (NSString *)webrootPath
{
    NSString * documents = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSArray *components = [NSArray arrayWithObjects:documents, @"webroot", nil];
    NSString *path = [NSString pathWithComponents:components];
    return path;
}


// In the sandboxed app, the app cannot write to the webroot folder in the Resources included.
// Therefore the webroot folder will be copied to the sandboxed Documents folder.
// This is done once when the version number changes.
+ (void) install
{
    // Bail out in case the installed version equals the bibledit library version.
    NSString * version = [self libraryVersion];
    if ([version isEqualToString:[self installedVersion]]) return;
    
    NSArray *inputComponents = [NSArray arrayWithObjects:[Utilities resourcesPath], @"webroot", nil];
    NSString *inputDirectory = [NSString pathWithComponents:inputComponents];
    
    NSString *outputDirectory = [Utilities webrootPath];
            
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
                // Create directory.
                [fileManager createDirectoryAtPath:outputPath withIntermediateDirectories:YES attributes:nil error:nil];
            } else {
                // Copy file to destination. First remove old item to prevent a copy error.
                [fileManager removeItemAtPath:outputPath error:nil];
                [fileManager copyItemAtPath:inputPath toPath:outputPath error:nil];
            }
            // Exclude path from the iCloud backup.
            NSString * schemePath = [NSString stringWithFormat:@"file://%@", outputPath];
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
    // Retieve the version of Bibledit that has been installed.
    NSString * version = [[NSUserDefaults standardUserDefaults] stringForKey:@"bibledit-version"];
    return version;
}


+ (void) installedVersion:(NSString *)version
{
    // Store the Bibledit verion in the preferences.
    NSUserDefaults *userDefaults = [NSUserDefaults standardUserDefaults];
    [userDefaults setObject:version forKey:@"bibledit-version"];
    [userDefaults synchronize];
}


@end
