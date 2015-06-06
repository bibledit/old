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
    NSString * resources = [BibleditPaths resources];
    NSString * webroot = [BibleditPaths documents];

    
/*
    bibledit_initialize_library ();

    NSArray *components = [NSArray arrayWithObjects:[BibleditPaths documents], @"webroot", nil];
    NSString *path = [NSString pathWithComponents:components];
    const char * document_root = [path UTF8String];
    bibledit_set_web_root (document_root);
    
    float timezoneoffset = ([[NSTimeZone systemTimeZone] secondsFromGMT] / 3600.0);
    bibledit_set_timezone_hours_offset_utc ((int)timezoneoffset);
 */
}


+ (void) bibleditViewHasLoaded:(UIView *)uiview
{
    mainUIView = uiview;
}


+ (void) bibleditInstallResources
{
    /*
    // Display message to user.
    dispatch_async(dispatch_get_main_queue(), ^{
        const char * page = bibledit_get_preparation_notice ();
        NSString *html = [NSString stringWithUTF8String:page];
        [webview loadHTMLString:html baseURL:nil];
    });

    // Run the installation.
    [BibleditInstallation run];
    
    // Open Bibledit main page.
    dispatch_async(dispatch_get_main_queue(), ^{
        [BibleditController bibleditBrowseTo:@"http://localhost:8080"];
    });
     */
}


+ (void) bibleditEnteredForeground
{
    // bibledit_start_library ();
}


+ (void) bibleditOpenWebView
{
    /*
    WKWebViewConfiguration *theConfiguration = [[WKWebViewConfiguration alloc] init];
    webview = [[WKWebView alloc] initWithFrame:mainUIView.frame configuration:theConfiguration];
    [mainUIView addSubview:webview];
     */
}


+ (void) bibleditBrowseTo:(NSString*)urlString
{
    /*
    NSURL *url = [NSURL URLWithString:urlString];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [webview loadRequest:urlRequest];
     */
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
    //bibledit_stop_library ();
    //while (bibledit_is_running ()) { };
}


+ (void) bibleditWillTerminate
{
    //bibledit_shutdown_library ();
}


@end


/*

 // Function is called when the app gets launched.
 @Override
 public void onCreate (Bundle savedInstanceState)
 {
 super.onCreate (savedInstanceState);
 
 // The directory of the external files.
 // On a Nexus 10 this is /storage/emulated/0/Android/data/org.bibledit.android/files
 // Files in this directory cannot be made executable.
 // The system has a protection mechanism for this.
 String externalDirectory = getExternalFilesDir (null).getAbsolutePath ();
 
 // The protected directory that contains files that can be set executable.
 // This would be /data/data/org.bibledit.android/files
 // Files there can be set executable.
 String internalDirectory = getFilesDir ().getAbsolutePath ();
 
 InitializeLibrary (externalDirectory, externalDirectory);
 
 SetTouchEnabled (true);
 
 // Set the timezone in the library.
 TimeZone timezone = TimeZone.getDefault ();
 Calendar calendar = GregorianCalendar.getInstance (timezone);
 int offsetInMillis = timezone.getOffset (calendar.getTimeInMillis());
 int offsetInHours = offsetInMillis / 3600 / 2000;
 SetTimezoneHoursOffsetUtc (offsetInHours);
 
 StartLibrary ();
 
 webview = new WebView (this);
 setContentView (webview);
 webview.getSettings().setJavaScriptEnabled (true);
 webview.setWebViewClient(new WebViewClient());
 webview.loadUrl ("http://localhost:8080");
 
 // Install the assets if needed.
 installAssets (externalDirectory);
 }
 
 
 // A native method that is implemented by the native library which is packaged with this application.
 // Through trial and error it was discovered that there should be no understores (_) in the function name,
 // to avoid a "java.lang.UnsatisfiedLinkError: Native method not found" exception.
 public native String GetVersionNumber ();
 public native void SetTouchEnabled (Boolean enabled);
 public native void SetTimezoneHoursOffsetUtc (int hours);
 public native void InitializeLibrary (String resources, String webroot);
 public native void StartLibrary ();
 public native Boolean IsRunning ();
 public native void StopLibrary ();
 public native void ShutdownLibrary ();
 
 
 @Override
 public boolean onCreateOptionsMenu (Menu menu)
 {
 return false;
 }
 
 
 // Function is called when the app is obscured.
 @Override
 public void onPause ()
 {
 super.onPause ();
 StopLibrary ();
 }
 
 
 // Function is called when the app is moved to the foreground again.
 @Override
 public void onResume ()
 {
 super.onResume();
 StartLibrary ();
 checkUrl ();
 }
 
 
 // Function is called when the user completely leaves the activity.
 @Override
 protected void onStop ()
 {
 super.onStop();
 StopLibrary ();
 }
 
 
 // Function is called when the user starts the app.
 @Override
 protected void onStart ()
 {
 super.onStart();
 StartLibrary ();
 checkUrl ();
 }
 
 
 // Function is called when the user returns to the activity.
 @Override
 protected void onRestart ()
 {
 super.onRestart();
 StartLibrary ();
 checkUrl ();
 }
 
 
 // Function is called when the app gets completely destroyed.
 @Override
 public void onDestroy ()
 {
 super.onDestroy ();
 StopLibrary ();
 while (IsRunning ()) {};
 ShutdownLibrary ();
 }
 
 
 // This is used to load the native Bibledit library on application startup.
 // Library libbibleditjni calls the Bibledit library.
 // The library has already been unpacked into
 // /data/data/org.bibledit.android/lib/libbbibleditjni.so
 // at installation time by the package manager.
 static {
 System.loadLibrary("bibleditjni");
 }
 
 
 private void installAssets (final String webroot)
 {
 
 Thread thread = new Thread ()
 {
 @Override
 public void run ()
 {
 SharedPreferences preferences = getPreferences (Context.MODE_PRIVATE);
 String installedVersion = preferences.getString ("version", "");
 String libraryVersion = GetVersionNumber ();
 if (installedVersion.equals (libraryVersion)) return;
 
 try {
 
 // The assets are not visible in the standard filesystem, but remain inside the .apk file.
 // The manager accesses them.
 AssetManager assetManager = getAssets();
 
 // Read the asset index created by ant.
 String [] files = null;
 try {
 InputStream input;
 input = assetManager.open ("asset.external");
 int size = input.available ();
 byte [] buffer = new byte [size];
 input.read (buffer);
 input.close();
 String text = new String (buffer);
 files = text.split ("\\r?\\n");
 } catch (IOException e) {
 e.printStackTrace ();
 }
 
 // Iterate through the asset files.
 for (String filename : files) {
 try {
 // Read the file into memory.
 InputStream input = assetManager.open ("external/" + filename);
 int size = input.available ();
 byte [] buffer = new byte [size];
 input.read (buffer);
 input.close ();
 // Optionally create output directories.
 File file = new File (filename);
 String parent = file.getParent ();
 if (parent != null) {
 File parentFile = new File (webroot, parent);
 if (!parentFile.exists ()) {
 parentFile.mkdirs ();
 }
 }
 file = null;
 // Write the file to the external webroot directory.
 File outFile = new File (webroot, filename);
 OutputStream out = new FileOutputStream (outFile);
 out.write (buffer, 0, size);
 out.flush ();
 outFile = null;
 out = null;
 //Log.i (filename, webroot);
 } catch(IOException e) {
 e.printStackTrace ();
 }
 }
 
 }
 catch (Exception e) {
 e.printStackTrace ();
 }
 finally {
 }
 preferences.edit ().putString ("version", GetVersionNumber ()).apply ();
 }
 };
 thread.start ();
 }
 
 
 // Checks whether the browser has a Bibledit page opened.
 // If not, it navigates the browser to the Bibledit home page.
 private void checkUrl ()
 {
 Boolean reload = false;
 String url = webview.getUrl ();
 if (url.length () >= 21) {
 url = url.substring (0, 21);
 if (url.compareTo ("http://localhost:8080") != 0) {
 reload = true;
 }
 } else reload = true;
 if (reload) webview.loadUrl ("http://localhost:8080");
 }
 
 }

*/