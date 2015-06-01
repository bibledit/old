package org.bibledit.android;


import org.bibledit.android.MainActivity;
import java.lang.String;
import android.content.ContextWrapper;
import java.io.File;
import android.os.Environment;


public class BibleditDirectory // Todo remove it.
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


}
