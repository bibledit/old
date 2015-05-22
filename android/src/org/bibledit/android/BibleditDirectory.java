package org.bibledit.android;


import org.bibledit.android.MainActivity;
import java.lang.String;
import android.content.ContextWrapper;
import java.io.File;
import android.os.Environment;


public class BibleditDirectory
{


  // Returns the directory of the external files.
  public static String external () 
  {
    // /storage/emulated/0/Android/data/org.bibledit.android/files
    String directory = MainActivity.activity.getExternalFilesDir (null).getAbsolutePath ();
    return directory;
  }


  // Returns the protected directory that contains files that can be set executable.
  // Files on /sdcard cannot be made executable. 
  // The system has a protection mechanism for this.
  // Files on /data/data can be made executable.
  public static String internal () 
  {
    // /data/data/org.bibledit.android/files
    File file = MainActivity.activity.getFilesDir ();
    String directory = file.getAbsolutePath ();
    return directory;
  }
  
  
  // Returns the directory of the sdcard.
  public static String storage ()
  {
    String directory = Environment.getExternalStorageDirectory () + "";
    return directory;
  }


  // Palapa Web Server
  // Website: http://alfanla.com/
  // Starts the Bibledit-Web Setup normally.
  // Runs Bibledit-Web well.
  public static String directoryPalapaWebServer ()
  {
    String directory = Environment.getExternalStorageDirectory () + "/pws/www";
    return directory;
  }


  // Bit Web Server (PHP,MySQL,PMA): The app needs to be purchased.
  // AndroPHP: Is a proper web server.
  public static String directoryBitWebServerAndAndroPHP ()
  {
    String directory = Environment.getExternalStorageDirectory () + "/www";
    return directory;
  }


  // PHPRunner: 
  // * It works well to run the PHP scripts. 
  // * Has a good PHP server. 
  // NAMP nginx android web server: 
  // * Trial period of 10 days. 
  // * https://play.google.com/store/apps/details?id=ru.nampltd.namp. Extreme activity in Android logbook.
  // KSWEB: сервер + PHP + MySQL
  // * Trial period of 5 days.
  // * Runs nginx or lighttpd
  // * www.kslabs.ru
  // * Web server works well as to starting Bibledit-Web Setup.
  public static String directoryPhpRunnerAndNampAndKsweb ()
  {
    String directory = Environment.getExternalStorageDirectory () + "/htdocs";
    return directory;
  }


  // Ulti Server: PHP, MySQL, PMA
  // Trial period of 6 days.
  // Web server works well as to starting Bibledit-Web Setup.
  public static String directoryUltiServer ()
  {
    String directory = Environment.getExternalStorageDirectory () + "";
    return directory;
  }


}
