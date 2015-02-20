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
#import <mach/mach.h>


@interface ViewController ()

@end

@implementation ViewController

UIWebView * webview;
NSTimer *timer;

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
  
  [self displayBrowser];
  
  [self runWebserver];
  
  [self performSelectorInBackground:@selector(runInstallation) withObject:nil];
  
  timer = [NSTimer scheduledTimerWithTimeInterval:600.0 target:self selector:@selector(timerTick:) userInfo:nil repeats:YES];
}


- (void)didReceiveMemoryWarning
{
  [super didReceiveMemoryWarning];
  // There is are huge memory leaks in ios UIWebView.
  // The memory usage keeps creeping up over time when it displays dynamic pages.
  // iOS sends a few memory warnings after an hour or so, then iOS kills the app.
  [self logMemoryUsage];
  bibledit_log ("The device runs low on memory. Resetting the user interface to release memory.");
  // A suggestion to release memory is the following:
  // [webview loadHTMLString: @"" baseURL: nil];
  // [self browseTo:@"http://localhost:8080"];
  // A suggestion to release memory is to reload the webview
  [webview reload];
  [self logMemoryUsage];
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
  NSArray *components = [NSArray arrayWithObjects:[BibleditPaths documents], @"webroot", nil];
  NSString *path = [NSString pathWithComponents:components];
  const char * document_root = [path UTF8String];
  bibledit_set_web_root (document_root);
  bibledit_start_server ();
  // NSLog(@"Document root: %s", (char *) document_root);
}


- (void)browseTo:(NSString*)urlString
{
  NSURL *url = [NSURL URLWithString:urlString];
  NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
  [webview loadRequest:urlRequest];
}


- (void)displayBrowser
{
  webview = [[UIWebView alloc] initWithFrame:CGRectMake(0, 0, [[UIScreen mainScreen] applicationFrame].size.width, [[UIScreen mainScreen] applicationFrame].size.height)];
  self.view = webview;
  webview.userInteractionEnabled = YES;
}


- (void) timerTick:(NSTimer *)incomingTimer
{
  [self logMemoryUsage];
}


- (void)logMemoryUsage
{
  struct mach_task_basic_info info;
  mach_msg_type_number_t size = MACH_TASK_BASIC_INFO_COUNT;
  kern_return_t kerr = task_info (mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &size);
  if (kerr == KERN_SUCCESS) {
    NSString *string = [NSString stringWithFormat:@"Memory in use: %lld Mb", info.resident_size / 1024 / 1024];
    const char * message = [string UTF8String];
    bibledit_log (message);
  }
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
