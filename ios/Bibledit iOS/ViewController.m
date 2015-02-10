/*
Copyright (©) 2003-2014 Teus Benschop.
 
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


#import "ViewController.h"
#import "BibleditPaths.h"
#import "BibleditInstallation.h"
#import "bibledit.h"



@interface ViewController ()

@end

@implementation ViewController

UIWebView * webview;

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    @try {
        
    }
    @catch (NSException *exception) {
        NSLog([exception reason], @"");
    }
    @finally {
    }
    
    webview = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0,
              [[UIScreen mainScreen] applicationFrame].size.width, [[UIScreen mainScreen] applicationFrame].size.height)];
    self.view = webview;
    webview.userInteractionEnabled = YES;
    
    NSLog (@"%@", [BibleditPaths resources]); // Todo
    NSLog (@"%@", [BibleditPaths documents]); // Todo

    // [self performSelectorInBackground:@selector(runWebserver) withObject:nil];
    [self runWebserver];
    
    [self performSelectorInBackground:@selector(runInstallation) withObject:nil];
}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}


- (void) runInstallation
{
    // Display message to user.
    dispatch_async(dispatch_get_main_queue(), ^{
        NSArray *components = [NSArray arrayWithObjects:[BibleditPaths resources], @"setup", @"setup.html", nil];
        NSString *path = [NSString pathWithComponents:components];
        [self browseTo:path];
    });
    // Run the installation.
    [BibleditInstallation run];
    // Open Bibledit-Web main page.
    dispatch_async(dispatch_get_main_queue(), ^{
        [self browseTo:@"http://localhost:8080"];
    });
}


- (void) runWebserver
{
    return;
    NSArray *components = [NSArray arrayWithObjects:[BibleditPaths documents], @"bibledit-web", nil];
    NSString *path = [NSString pathWithComponents:components];
    
    path = [BibleditPaths documents];
    
    const char * document_root = [path UTF8String];

    NSLog(@"%s", (char *) document_root);
    NSLog(@"%s", bibledit_version_number ());

}


- (void)browseTo:(NSString*)urlString
{
    NSURL *url = [NSURL URLWithString:urlString];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [webview loadRequest:urlRequest];
}


- (void)debug
{
    
    NSArray *components = [NSArray arrayWithObjects:[BibleditPaths resources], nil];
    NSString *directory = [NSString pathWithComponents:components];
    
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator *dirEnumerator = [fileManager enumeratorAtPath:directory];
    NSString *file;
    BOOL isDir;
    while (file = [dirEnumerator nextObject]) {
        components = [NSArray arrayWithObjects:directory, file, nil];
        NSString * original = [NSString pathWithComponents:components];
        if ([fileManager fileExistsAtPath:original isDirectory:&isDir]) {
            if (isDir) {
                NSLog(@"%@ is a directory", original);
            } else {
                NSLog(@"%@ is a file", original);
            }
        
        }
    }
    
}


@end
