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


// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder files.


public class MainActivity extends Activity
{


  public static Activity activity;
  public static Handler handler;
  public static Context context;


  @Override
  public void onCreate (Bundle savedInstanceState)
  {
    super.onCreate (savedInstanceState);

    activity = this;
    context = this;

    WebView webview = new WebView (this);
    setContentView (webview);

    webview.loadData (GetVersionNumber (), "text/html", null);

    UpdateGui.run ();
  }


  // A native method that is implemented by the native library which is packaged with this application.
  // Through trial and error it was discovered that there should be no understores (_) in the function name,
  // to avoid a "java.lang.UnsatisfiedLinkError: Native method not found" exception.
  public native String GetVersionNumber ();
    
    
  @Override
  public boolean onCreateOptionsMenu (Menu menu)
  {
    return false;
  }
  

  public void onClickStopService (View view)
  {
    finish ();
  }
    
    
  @Override
  public void onPause () 
  {
    super.onPause ();

    handler = null;
  }


  @Override
  public void onResume () 
  {
    super.onResume();

    UpdateGui.run ();
  }  


  @Override
  protected void onStop () 
  {
    super.onStop();

    handler = null;
  }


  @Override
  protected void onStart () 
  {
    super.onStart();
      
    UpdateGui.run ();
  }


  @Override
  protected void onRestart () 
  {
    super.onRestart();
     
    UpdateGui.run ();
  }


  @Override
  public void onDestroy ()
  {
    super.onDestroy ();

    handler = null;
  }

    
  // This is used to load the native Bibledit library on application startup.
  // Library libjavawrapper calls the Bibledit library.
  // The library has already been unpacked into
  // /data/data/org.bibledit.android/lib/libbjavawrapper.so
  // at installation time by the package manager.
  static {
    System.loadLibrary("javawrapper");
  }

}
