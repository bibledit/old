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


// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder files.


public class MainActivity extends Activity
{


  public static Activity activity;
  public static Handler handler;
  public static Context context;
  //private long notificationId;


  @Override
  public void onCreate (Bundle savedInstanceState)
  {
    super.onCreate (savedInstanceState);

    activity = this;
    context = this;

    setContentView (R.layout.main);

    UpdateGui.run ();

		startService (new Intent (this, RunServer.class));

/*
    int icon = R.drawable.ic_launcher;
    long when = System.currentTimeMillis ();
    notificationId = System.currentTimeMillis ();
    String appname = context.getResources().getString(R.string.app_name);
    NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
    Intent intent = new Intent(context, MainActivity.class);
    PendingIntent contentIntent = PendingIntent.getActivity (context, 0, intent, 0);
    String message = "Bibledit service running";
    Notification notification = new Notification(icon, message, when);
    notification.setLatestEventInfo (context, appname, message, contentIntent);
    notification.flags = Notification.FLAG_FOREGROUND_SERVICE;
    notificationManager.notify((int) notificationId, notification);
*/
  }


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

    stopService (new Intent (this, RunServer.class));

/*
    NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
    notificationManager.cancel ((int)notificationId);
*/
  }


}
