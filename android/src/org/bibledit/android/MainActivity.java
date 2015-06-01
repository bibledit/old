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
import org.bibledit.android.InstallAssets;
import org.bibledit.android.UpdateGui;
import org.bibledit.android.RunServer;
import android.os.Handler;
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


// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder files.


public class MainActivity extends Activity
{


    // Todo all goes out.
  public static Activity activity;
  public static Handler handler;
  public static Context context;


  // Function is called when the app gets launched.
  @Override
  public void onCreate (Bundle savedInstanceState)
  {
    super.onCreate (savedInstanceState);
      Log.d ("%s", "onCreate"); // Todo

    activity = this;
    context = this;

      
    SetTouchEnabled (true);

    // Set the timezone in the library.
    TimeZone timezone = TimeZone.getDefault ();
    Calendar calendar = GregorianCalendar.getInstance (timezone);
    int offsetInMillis = timezone.getOffset (calendar.getTimeInMillis());
    int offsetInHours = offsetInMillis / 3600 / 2000;
    SetTimezoneHoursOffsetUtc (offsetInHours);
      
    WebView webview = new WebView (this);
    setContentView (webview);
    webview.getSettings().setJavaScriptEnabled(true);
    webview.loadUrl ("http://www.google.nl");
    // Todo The WebView should call an URL, start with bibledit.org, then later change to http://localhost:8080.
    // If the URL opens in Chrome, then Bibledit should always display the splash screen for beauty.
      
      
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
      Log.d ("%s", "onPause"); // Todo

    handler = null;
  }


  // Function is called when the app is moved to the foreground again.
  @Override
  public void onResume () 
  {
    super.onResume();
      Log.d ("%s", "onResume"); // Todo

    UpdateGui.run ();
  }  


  // Function is called when the user completely leaves the activity.
  @Override
  protected void onStop () 
  {
    super.onStop();
      Log.d ("%s", "onStop"); // Todo


    handler = null;
  }


  // Function is called when the user starts the app.
  @Override
  protected void onStart () 
  {
    super.onStart();
      Log.d ("%s", "onStart"); // Todo
      
    UpdateGui.run ();
  }


  // Function is called when the user returns to the activity.
  @Override
  protected void onRestart () 
  {
    super.onRestart();
      Log.d ("%s", "onRestart"); // Todo

     
    UpdateGui.run ();
  }


  // Function is called when the app gets completely destroyed.
  @Override
  public void onDestroy ()
  {
    super.onDestroy ();
      Log.d ("%s", "onDestroy"); // Todo


    handler = null;
  }

    
  // This is used to load the native Bibledit library on application startup.
  // Library libbibleditjni calls the Bibledit library.
  // The library has already been unpacked into
  // /data/data/org.bibledit.android/lib/libbbibleditjni.so
  // at installation time by the package manager.
  static {
    System.loadLibrary("bibleditjni");
  }

}
