package org.bibledit.android;


import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;
import java.io.FileOutputStream;
import android.content.Context;
import android.util.Log;
import java.io.OutputStreamWriter;
import android.content.res.AssetManager;
import java.io.IOException;
import java.lang.String;
import java.io.InputStream;
import java.io.File;
import java.io.OutputStream;
import android.os.Environment;
import android.content.Intent;
import android.view.View;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.NotificationManager;
import android.webkit.WebView;
import java.util.TimeZone;
import java.util.GregorianCalendar;
import java.util.Calendar;
import android.content.res.AssetManager;
import android.webkit.WebViewClient;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.os.Handler;
import android.net.Uri;
import java.util.Timer;
import java.util.TimerTask;
import android.os.Process;


// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder files.


public class MainActivity extends Activity
{
    
    WebView webview;
    int resumecounter = 0;
    String webAppUrl = "http://localhost:8080";
    Timer timer;
    TimerTask timerTask;


    // Function is called when the app gets launched.
    @Override
    public void onCreate (Bundle savedInstanceState)
    {
        super.onCreate (savedInstanceState);
        // Log.d ("Bibledit", "onCreate");

        
        // The directory of the external files.
        // On a Nexus 10 this is /storage/emulated/0/Android/data/org.bibledit.android/files
        // Files in this directory cannot be made executable.
        // The system has a protection mechanism for this.
        String externalDirectory = getExternalFilesDir (null).getAbsolutePath ();
        
        // The protected directory that contains files that can be set executable.
        // This would be /data/data/org.bibledit.android/files
        // Files there can be set executable.
        String internalDirectory = getFilesDir ().getAbsolutePath ();
        
        // Take the external directory for the webroot, if it exists, else the internal directory.
        String webroot = externalDirectory;
        File file = new File (externalDirectory);
        if (!file.exists ()) webroot = internalDirectory;
        
        InitializeLibrary (webroot, webroot);
        
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
        webview.getSettings().setBuiltInZoomControls (true);
        webview.getSettings().setSupportZoom (true);
        webview.setWebViewClient(new WebViewClient());
        webview.loadUrl (webAppUrl);
        
        // Install the assets if needed.
        installAssets (webroot);
        
        // Log information about where to find Bibledit's data.
        Log ("Bibledit data location: " + webroot);
        
        // Keep-awake timer.
        startTimer ();
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
    public native String IsSynchronizing ();
    public native void StopLibrary ();
    public native void ShutdownLibrary ();
    public native void Log (String message);
    public native String GetLastPage ();
    
    
    @Override
    public boolean onCreateOptionsMenu (Menu menu)
    {
        return false;
    }
    
    
    // Function is called when the user starts the app.
    @Override
    protected void onStart ()
    {
        // Log.d ("Bibledit", "onStart");
        super.onStart();
        StartLibrary ();
    }
    
    
    // Function is called when the user returns to the activity.
    @Override
    protected void onRestart ()
    {
        // Log.d ("Bibledit", "onRestart");
        super.onRestart();
        StartLibrary ();
        //stopTimer ();
    }
    
    
    // Function is called when the app is moved to the foreground again.
    @Override
    public void onResume ()
    {
        // Log.d ("Bibledit", "onResume");
        super.onResume();
        StartLibrary ();
        checkUrl ();
        //stopTimer ();
    }
    
    
    // Function is called when the app is obscured.
    @Override
    public void onPause ()
    {
        // Log.d ("Bibledit", "onPause");
        super.onPause ();
        StopLibrary ();
        //startTimer ();
    }
    
    
    // Function is called when the user completely leaves the activity.
    @Override
    protected void onStop ()
    {
        // Log.d ("Bibledit", "onStop");
        super.onStop();
        StopLibrary ();
        //startTimer ();
    }
    
    
    // Function is called when the app gets completely destroyed.
    @Override
    public void onDestroy ()
    {
        // Log.d ("Bibledit", "onDestroy");
        super.onDestroy ();
        StopLibrary ();
        // Crashes: while (IsRunning ()) {};
        ShutdownLibrary ();
    }
    
    
    // Function is called on device orientation and keyboard hide.
    // At least, it should be called. But it does not seem to happen.
    // Anyway the call is not needed because the webview reconfigures itself.
    // The app used to crash on device rotation.
    // The fix was adding
    // android:configChanges="orientation|keyboardHidden"
    // to the <activity> element in AndroidManifest.xml.
    // The app used to restart after a Bluetooth keyboard came on or went off.
    // This is according to the specifications.
    // But then the editor would go away, and the app would go back to the home screen after the restart.
    // The fix was to add "keyboard" to the above "configChanges" element.
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        // Log.d ("Bibledit", "onConfigurationChanged");
        super.onConfigurationChanged(newConfig);
    }
    
    
    // This is used to load the native Bibledit library on application startup.
    // Library libbibleditjni calls the Bibledit library.
    // The library has already been unpacked into
    // /data/data/org.bibledit.android/lib/libbbibleditjni.so
    // at installation time by the package manager.
    static {
        System.loadLibrary("gnustl_shared");
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
        // Bail out on the activity's first resume.
        // Else it crashes on checking its URL.
        resumecounter++;
        if (resumecounter <= 1) return;
        
        Boolean load_index = false;
        String url = webview.getUrl ();
        if (url.length () >= 21) {
            String bit = url.substring (0, 21);
            if (bit.compareTo (webAppUrl) != 0) {
                load_index = true;
            }
        } else load_index = true;
        if (load_index) {
            // Load the index page.
            webview.loadUrl (webAppUrl);
        } else {
            // Just to be sure that any javascript runs, reload the loaded URL.
            webview.loadUrl (url);
        }
    }

    
    /*
     
     There was an idea that the app would shut down itself after it would be in the background for a while.
     This works well when another app is started and thus Bibledit goes to the background.
     But when the screen is powered off, then when Bibledit quits itself, Android keeps restarting it.
     And when the screen is powered on again, then Bibledit cannot find the page.
     Thus since this does not work well, it was not implemented.
     
     System.runFinalizersOnExit (true);
     this.finish ();
     Process.killProcess (Process.myPid());
     System.exit (0);
     
    */
    

    private void startTimer ()
    {
        stopTimer ();
        timer = new Timer();
        initializeTimerTask();
        timer.schedule (timerTask, 1000);
    }
    
    
    private void stopTimer ()
    {
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }
    
    private void initializeTimerTask() {
        timerTask = new TimerTask() {
            public void run() {
                Log.d ("Bibledit syncing", IsSynchronizing ());
                Log.d ("Bibledit version", GetVersionNumber ());
                startTimer ();
            }
        };
    }

    
    @Override
    public void onBackPressed() {
        // The Android back button navigates back in the web view.
        // That is the behaviour people expect.
        if (webview.canGoBack()) {
            webview.goBack();
            return;
        }
        
        // Otherwise defer to system default behavior.
        super.onBackPressed();
    }

}
