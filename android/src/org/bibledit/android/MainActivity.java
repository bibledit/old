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


// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder files.


public class MainActivity extends Activity
{


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
        
        WebView webview = new WebView (this);
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
    }
    
    
    // Function is called when the user returns to the activity.
    @Override
    protected void onRestart ()
    {
        super.onRestart();
        StartLibrary ();
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
    

}
