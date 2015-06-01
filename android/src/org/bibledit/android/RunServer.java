package org.bibledit.android;


import android.app.Activity;
import java.io.FileOutputStream;
import android.content.Context;
import java.io.OutputStreamWriter;
import android.content.res.AssetManager;
import java.io.IOException;
import java.lang.String;
import java.io.InputStream;
import java.io.File;
import java.io.OutputStream;
import android.os.Environment;
import org.bibledit.android.BibleditDirectory;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import android.app.Notification;
import android.app.PendingIntent;
import org.bibledit.android.MainActivity;
import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;
import android.os.SystemClock;
import java.util.Calendar;
import android.os.Handler;
import org.apache.http.util.EntityUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.HttpStatus;
import org.apache.http.client.params.ClientPNames;
import java.net.CookiePolicy;
import org.apache.http.client.CookieStore;
import org.apache.http.impl.client.BasicCookieStore;
import org.apache.http.cookie.CookieSpecFactory;
import org.apache.http.impl.cookie.BrowserCompatSpec;
import org.apache.http.cookie.MalformedCookieException;
import org.apache.http.cookie.CookieOrigin;
import org.apache.http.cookie.Cookie;
import org.apache.http.cookie.CookieSpec;
import org.apache.http.params.HttpParams;
import java.util.Set;
import java.io.FilenameFilter;
import java.io.FileReader;
import java.util.HashSet;
import java.util.ArrayList;
import java.util.List;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.NameValuePair;
import java.util.LinkedList;
import org.apache.http.client.utils.URLEncodedUtils;
import java.util.TimeZone;


// Runs the regular tasks.


public class RunServer extends Service // Todo remove it all.
{


  private Handler handler;
  private Runnable runnableMinute;
  private Runnable runnableSecond;
  private Set <String> runningTasks = new HashSet <String> ();
  

  @Override
	public IBinder onBind (Intent arg0)
    {
      return null;
	}


	@Override
	public void onCreate ()
    {
        
	  Toast.makeText (this, "Bibledit Service Started", Toast.LENGTH_LONG).show ();

      makeSettings ();
    
      handler = new Handler ();

      runnableMinute = new Runnable ()
    {
      @Override
      public void run ()
      {
        handler.postDelayed (this, 60000);
        runScript ("http://localhost:8080/bibledit-web/timer/index.php", "", true);
      }
    };
    handler.postDelayed (runnableMinute, 1000);
    
    runnableSecond = new Runnable () 
    {
      @Override
      public void run ()
      {
        handler.postDelayed (this, 1000);
        checkTasks ();
      }
    };
    handler.postDelayed (runnableSecond, 1000);
    
	}


	@Override
	public void onStart (Intent intent, int startId)
  {
	}


	@Override
	public void onDestroy ()
  {
    handler.removeCallbacks (runnableMinute);
    handler.removeCallbacks (runnableSecond);
		Toast.makeText (this, "Bibledit Service Stopped", Toast.LENGTH_LONG).show();
	}
  
  
  private void runScript (final String url, final String task, final boolean record)
  {
    Thread thread = new Thread ()
    {
      @Override
      public void run () 
      {
        runningTasks.add (task);
        try {
          Log.i ("Bibledit", "Fetch url " + url);
          DefaultHttpClient client = new DefaultHttpClient ();
          CookieStore cookieStore = new BasicCookieStore ();
          client.setCookieStore(cookieStore);
          CookieSpecFactory csf = new CookieSpecFactory () {
            public CookieSpec newInstance (HttpParams params) {
              return new BrowserCompatSpec () {
                @Override
                public void validate (Cookie cookie, CookieOrigin origin) throws MalformedCookieException {
                }
              };
            }
          };
          client.getCookieSpecs ().register ("easy", csf);
          client.getParams ().setParameter (ClientPNames.COOKIE_POLICY, "easy"); 
          HttpGet httpGet = new HttpGet (url);
          HttpResponse response = client.execute (httpGet);
          HttpEntity entity = response.getEntity ();
          String string = EntityUtils.toString (entity);
          if (record) {
            if (!string.isEmpty ()) {
              // Log the url of this script.
              String recorder = "http://localhost:8080/bibledit-web/journal/log.php?";
              List <NameValuePair> query = new LinkedList <NameValuePair> ();
              query.add (new BasicNameValuePair ("msg", url));
              String s = URLEncodedUtils.format (query, "utf-8");
              runScript (recorder + s, "", false);
              SystemClock.sleep (10);
              // Log the output of this script, line by line.
              String [] messages = string.split ("\n");
              for (String message : messages) {
                query = new LinkedList <NameValuePair> ();
                query.add (new BasicNameValuePair ("msg", message));
                s = URLEncodedUtils.format (query, "utf-8");
                runScript (recorder + s, "", false);
                SystemClock.sleep (10);
              }
            }
          } else {
            Log.i ("Bibledit response for " + url, string);
          }
        } 
        catch (Exception e) {
          e.printStackTrace ();
        }
        finally {
          runningTasks.remove (task);
        }
      }    
    };
    thread.start ();
  }
  
  
  private void checkTasks ()
  {
    // Run no more than so many scripts simultaneously.
    if (runningTasks.size () >= 2) return;

    try {

      // The directory to read the tasks from.
      String directory = Preferences.webRoot () + "/bibledit-web/processes";
      File folder = new File (directory);

      // Get the files except .htaccess.
      FilenameFilter filenameFilter = new FilenameFilter () {
        @Override
        public boolean accept (File dir, String name) {
          if (name.equals (".htaccess")) return false;
          return true;
        }
      };      
      String [] names = folder.list (filenameFilter);
      
      // Bail out if there are no tasks.
      if (names.length == 0) return;

      // Get first task file.
      String name = names [0];

      // Path to the task file.
      String path = directory + "/" + name;

      // Read the contents of the task, and delete the corresponding file.
      BufferedReader reader = null;
      try {
        reader = new BufferedReader (new FileReader (path));
        String line = reader.readLine ();
        if (!line.equals ("")) {
          runTask (line);
        }
      }
      catch (Exception e) {
        e.printStackTrace ();
      }
      finally {
        reader.close ();
      }
      File file = new File (path);
      file.delete ();
    } 
    catch (Exception e) {
      e.printStackTrace ();
    }
  }


  private void runTask (String task)
  {
    try {

      // Get the executable from the task.
      // Only proceed if the executable is "php".
      // Other executables don't work through the web server.
      int i = task.indexOf (" ");
      if (i < 1) return;
      String executable = task.substring (0, i);
      if (!executable.equals ("php")) return;

      // Get all the parameters. The parameters are in single quotes.
      // The first parameter refers to the script, that is web page, to call.
      String parameterString = task.substring (i + 1);
      parameterString = parameterString.trim ();
      i = parameterString.indexOf ("'");
      if (i < 0) return;
      String [] parameters = parameterString.split ("'");

      // Get the page to call, plus their query parameters.
      String page = "";
      List <NameValuePair> queryParams = new LinkedList <NameValuePair> ();
      i = 0;
      for (String parameter : parameters) {
        parameter = parameter.trim ();
        if (parameter.equals ("")) continue;
        if (i == 0) {
          page = parameter;
        } else {
          queryParams.add (new BasicNameValuePair (String.valueOf (i), parameter));     
        }
        i++;
      }
      
      // Update the page to call.
      // Currrently it will look like e.g. this: /storage/emulated/0/www/bibledit-web/journal/rotate.php
      // We need something like e.g. this: http://localhost:8080/bibledit-web/journal/rotate.php
      i = page.indexOf ("bibledit-web");
      if (i < 0) return;
      page = "http://localhost:8080/" + page.substring (i);
      
      // Assemble and add the query bit for the URL.
      String query = URLEncodedUtils.format (queryParams, "utf-8");
      if (!query.isEmpty ()) page += "?" + query;
      
      // Fetch the web page so running the script.
      Log.i ("Bibledit page", page);
      runScript (page, task, true);

    } 
    catch (Exception e) {
      e.printStackTrace ();
    }
  }
  
  
  private void makeSettings ()
  {
  }
  

}
