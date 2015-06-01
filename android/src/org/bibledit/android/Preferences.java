package org.bibledit.android;


import org.bibledit.android.MainActivity;
import java.lang.String;
import android.content.Context;
import android.content.SharedPreferences;
import java.util.HashSet;
import java.util.Set;


public class Preferences // Todo remove it all.
{


  // Sets and gets the web root.
  public static void webRoot (String value) 
  {
    setString ("web-root", value);
  }
  public static String webRoot ()
  {
    return getString ("web-root", "/sdcard/www");
  }


  // Stores and retrieves a String.
  private static void setString (String key, String value)
  {
    SharedPreferences preferences = MainActivity.activity.getPreferences (Context.MODE_PRIVATE);
    preferences.edit ().putString (key, value).apply ();
  }
  private static String getString (String key, String fallback)
  {
    SharedPreferences preferences = MainActivity.activity.getPreferences (Context.MODE_PRIVATE);
    String value = preferences.getString (key, fallback);
    return value;
  }


  // Stores and retrieves a String Set.
  private static void setStringSet (String key, Set <String> values)
  {
    SharedPreferences preferences = MainActivity.activity.getPreferences (Context.MODE_PRIVATE);
    preferences.edit ().putStringSet (key, values).apply ();
  }
  private static Set <String> getStringSet (String key)
  {
    SharedPreferences preferences = MainActivity.activity.getPreferences (Context.MODE_PRIVATE);
    Set <String> values = preferences.getStringSet (key, new HashSet <String> ());
    return values;
  }


}
