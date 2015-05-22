package org.bibledit.android;


import android.app.Activity;
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
import org.bibledit.android.BibleditDirectory;
import org.bibledit.android.MainActivity;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.List;



/*
This class installs the assets.
Binaries on /sdcard cannot be made executable.
Therefore the web server is installed on internal memory, so it can be executed.
As it now uses an external web server app, the above information no longer applies.
The bibledit-web PHP code is on the /sdcard because it can grow big.
*/


public class InstallAssets
{


  private static final String TAG = "Bibledit Assets";


  public static void run (String directory) 
  {
    // Retrieve the version number of the Bibledit-Web scripts in the directory.
    String bibleditVersion = MainActivity.activity.getString (R.string.version_number);
    String scriptVersion = version (directory);
    Log.i ("Bibledit-Web version", scriptVersion);

    // Bail out if the version of the scripts matches the version of the Bibledit app.
    if (bibleditVersion.equals (scriptVersion)) return;

    // Look for existing Bibledit-Web scripts, and move them to the correct web server document root.
    String scripts = scripts ();
    if (scripts != null) {
      String destination = directory + "/bibledit-web";
      if (!scripts.equals (destination)) {
        Log.i ("Move Bibledit-Web scripts from", scripts);
        Log.i ("Move Bibledit-Web scripts to", destination);
        File fileOrigin = new File (scripts);
        File fileDestination = new File (destination);
        fileOrigin.renameTo (fileDestination);
      }
    }

    // Bail out if the version of the scripts matches the version of the Bibledit app.
    scriptVersion = version (directory);
    if (bibleditVersion.equals (scriptVersion)) return;

    // Install or update the Bibledit-Web scripts.
    Log.i ("Updating Bibledit-Web to version", bibleditVersion);
    install (directory);
  }


  // This function reads and returns the version number of the Bibledit-Web scripts.
  private static String version (String directory)
  {
    String path = directory + "/bibledit-web/config/version.php";
    String version = "";
    try {
      BufferedReader br = new BufferedReader (new FileReader (path));
      StringBuilder sb = new StringBuilder();
      String line = br.readLine();
      while (line != null) {
        sb.append(line);
        sb.append("\n");
        line = br.readLine();
      }
      version = sb.toString ();
      br.close ();
      version = version.substring (version.indexOf ("\"") + 1, version.lastIndexOf ("\""));
    } catch (IOException e) {
    }
    return version;
  }


  // This looks through known locations for the Bibledit-Web scripts.
  // If found, it returns the path to them.
  // Else it returns null.
  private static String scripts ()
  {
    List<String> directories = new ArrayList<String>();
    directories.add (BibleditDirectory.directoryPalapaWebServer ());
    directories.add (BibleditDirectory.directoryBitWebServerAndAndroPHP ());
    directories.add (BibleditDirectory.directoryPhpRunnerAndNampAndKsweb ());
    directories.add (BibleditDirectory.directoryUltiServer ());
    for (String directory : directories) {
      directory += "/bibledit-web"; 
      File file = new File (directory);
      Log.i ("Testing script directory", directory);
      if (file.isDirectory ()) {
        return directory;
      }
    }
    return null;
  }
    

  private static void install (String directory)
  {
    // The assets are not visible in the standard filesystem, but remain inside the .apk file.
    // The manager accesses them.
    AssetManager assetManager = MainActivity.activity.getAssets();


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
          File parentFile = new File (directory, parent);
          if (!parentFile.exists ()) {
            parentFile.mkdirs ();
          }
        }
        file = null;
        // Rename "dotdot.*" to ".*" in e.g. "dotdot.htaccess" to ".htaccess".
        filename = filename.replace ("dotdot.", ".");
        // Write the file to the external files directory.
        File outFile = new File (directory, filename);
        OutputStream out = new FileOutputStream (outFile);
        out.write (buffer, 0, size);
        out.flush ();
        outFile = null;
        out = null;
      } catch(IOException e) {
        e.printStackTrace ();
      }       
    }
  }


}
