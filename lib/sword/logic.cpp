/*
 Copyright (©) 2003-2015 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include <sword/logic.h>
#include <webserver/request.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/archive.h>
#include <filter/shell.h>
#include <locale/translate.h>
#include <config/logic.h>
#include <client/logic.h>
#include <database/logs.h>
#include <database/books.h>
#include <database/cache.h>
#include <database/config/general.h>
#include <database/versifications.h>
#include <sync/resources.h>
#include <tasks/logic.h>
#include <demo/logic.h>
#include <sword/installmgr.h>
#include <config.h>
#ifdef HAVE_SWORD
#include <swmgr.h>
#include <swlog.h>
#include <installmgr.h>
#include <filemgr.h>
#include <swmodule.h>
#else
#endif


mutex sword_logic_installer_mutex;
bool sword_logic_installing_module = false;


string sword_logic_get_path ()
{
  string sword_path = ".";
  char * home = getenv ("HOME");
  if (home) sword_path = home;
  sword_path.append ("/.sword/InstallMgr");
  return sword_path;
}


void sword_logic_refresh_module_list ()
{
  Database_Logs::log ("Refreshing SWORD module list");
  
  string out_err;
  vector <string> lines;
  
  // Initialize SWORD directory structure and configuration.
  string sword_path = sword_logic_get_path ();
  filter_url_mkdir (sword_path);
  string swordconf = "[Install]\n"
                     "DataPath=" + sword_path + "/\n";
  filter_url_file_put_contents (filter_url_create_path (sword_path, "sword.conf"), swordconf);
  string config_files_path = filter_url_create_root_path ("sword");
  filter_shell_run ("cp -r " + config_files_path + "/locales.d " + sword_path, out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) Database_Logs::log (line);
  filter_shell_run ("cp -r " + config_files_path + "/mods.d " + sword_path, out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) Database_Logs::log (line);
  
  // Initialize basic user configuration.
#ifdef HAVE_SWORD
  sword_logic_installmgr_initialize ();
#else
  filter_shell_run ("echo yes | installmgr -init", out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) Database_Logs::log (line);
#endif
  
  // Sync the configuration with the online known remote repository list.
#ifdef HAVE_SWORD
  sword_logic_installmgr_synchronize_configuration_with_master ();
#else
  filter_shell_run ("echo yes | installmgr -sc", out_err);
  filter_string_replace_between (out_err, "WARNING", "enable? [no]", "");
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) Database_Logs::log (line);
#endif
  
  // List the remote sources.
  vector <string> remote_sources;
#ifdef HAVE_SWORD
  sword_logic_installmgr_list_remote_sources (remote_sources);
#else
  filter_shell_run ("installmgr -s", out_err);
  lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    Database_Logs::log (line);
    line = filter_string_trim (line);
    if (line.find ("[") != string::npos) {
      line.erase (0, 1);
      if (line.find ("]") != string::npos) {
        line.erase (line.length () - 1, 1);
        remote_sources.push_back (line);
      }
    }
  }
#endif
  
  vector <string> sword_modules;
  
  for (auto remote_source : remote_sources) {
    
#ifdef HAVE_SWORD
    sword_logic_installmgr_refresh_remote_source (remote_source);
#else
    filter_shell_run ("echo yes | installmgr -r \"" + remote_source + "\"", out_err);
    filter_string_replace_between (out_err, "WARNING", "type yes at the prompt", "");
    Database_Logs::log (out_err);
#endif

    vector <string> modules;
#ifdef HAVE_SWORD
    sword_logic_installmgr_list_remote_modules (remote_source, modules);
    for (auto & module : modules) {
      sword_modules.push_back ("[" + remote_source + "]" + " " + module);
    }
#else
    filter_shell_run ("installmgr -rl \"" + remote_source + "\"", out_err);
    lines = filter_string_explode (out_err, '\n');
    for (auto line : lines) {
      line = filter_string_trim (line);
      if (line.empty ()) continue;
      if (line.find ("[") == string::npos) continue;
      if (line.find ("]") == string::npos) continue;
      modules.push_back ("[" + remote_source + "]" + " " + line);
    }
#endif
    for (auto module : modules) {
      sword_modules.push_back (module);
    }
    Database_Logs::log (remote_source + ": " + convert_to_string (modules.size ()) + " modules");
  }
  
  // Store the list of remote sources and their modules.
  // It is stored in the client files area.
  // Clients can access it from there too.
  string path = sword_logic_module_list_path ();
  filter_url_file_put_contents (path, filter_string_implode (sword_modules, "\n"));
  
  Database_Logs::log ("Ready refreshing SWORD module list");
}


string sword_logic_module_list_path ()
{
  return filter_url_create_root_path ("databases", "client", "sword_modules.txt");
}


// Gets the name of the remote source of the $line like this:
// [CrossWire] *[Shona] (1.1) - Shona Bible
string sword_logic_get_source (string line)
{
  if (line.length () < 10) return "";
  line.erase (0, 1);
  size_t pos = line.find ("]");
  if (pos == string::npos) return "";
  line.erase (pos);
  return line;
}


// Gets the module name of the $line like this:
// [CrossWire] *[Shona] (1.1) - Shona Bible
string sword_logic_get_remote_module (string line)
{
  if (line.length () < 10) return "";
  line.erase (0, 2);
  if (line.length () < 10) return "";
  size_t pos = line.find ("[");
  if (pos == string::npos) return "";
  line.erase (0, pos + 1);
  pos = line.find ("]");
  if (pos == string::npos) return "";
  line.erase (pos);
  return line;
}


// Gets the module name of the $line like this:
// [Shona]  (1.1)  - Shona Bible
string sword_logic_get_installed_module (string line)
{
  line = filter_string_trim (line);
  if (line.length () > 10) {
    line.erase (0, 1);
    size_t pos = line.find ("]");
    if (pos != string::npos) line.erase (pos);
  }
  return line;
}


// Gets the version number of a module of the $line like this:
// [Shona]  (1.1)  - Shona Bible
string sword_logic_get_version (string line)
{
  line = filter_string_trim (line);
  if (line.length () > 10) {
    line.erase (0, 3);
  }
  if (line.length () > 10) {
    size_t pos = line.find ("(");
    if (pos != string::npos) line.erase (0, pos + 1);
    pos = line.find (")");
    if (pos != string::npos) line.erase (pos);
  }
  return line;
}


// Gets the human-readable name of a $line like this:
// [CrossWire] *[Shona] (1.1) - Shona Bible
string sword_logic_get_name (string line)
{
  vector <string> bits = filter_string_explode (line, '-');
  if (bits.size () >= 2) {
    bits.erase (bits.begin ());
  }
  line = filter_string_implode (bits, "-");
  line = filter_string_trim (line);
  return line;
}


void sword_logic_install_module (string source_name, string module_name) // Todo
{
  Database_Logs::log ("Install SWORD module " + module_name + " from source " + source_name);
  string sword_path = sword_logic_get_path ();

  /* Installation through SWORD InstallMgr does not yet work.
#ifdef HAVE_SWORD

  sword::SWMgr *mgr = new sword::SWMgr();
  
  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);
  
  sword::InstallSourceMap::iterator source = installMgr->sources.find(source_name.c_str ());
  if (source == installMgr->sources.end()) {
    Database_Logs::log ("Could not find remote source " + source_name);
  } else {
    sword::InstallSource *is = source->second;
    sword::SWMgr *rmgr = is->getMgr();
    sword::SWModule *module;
    sword::ModMap::iterator it = rmgr->Modules.find(module_name.c_str());
    if (it == rmgr->Modules.end()) {
      Database_Logs::log ("Remote source " + source_name + " does not make available module " + module_name);
    } else {
      module = it->second;
      int error = installMgr->installModule(mgr, 0, module->getName(), is);
      if (error) {
        Database_Logs::log ("Error installing module " + module_name);
      } else {
        Database_Logs::log ("Installed module [" + module_name);
      }
    }
  }

  delete installMgr;
  delete mgr;
  
#else
   */
  
  string out_err;
  filter_shell_run ("cd " + sword_path + "; echo yes | installmgr -ri \"" + source_name + "\" \"" + module_name + "\"", out_err);
  vector <string> lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    line = filter_string_trim (line);
    if (line.empty ()) continue;
    Database_Logs::log (line);
  }
  
//#endif
}


void sword_logic_uninstall_module (string module)
{
  Database_Logs::log ("Uninstall SWORD module " + module);
  string out_err;
  string sword_path = sword_logic_get_path ();
  filter_shell_run ("cd " + sword_path + "; installmgr -u \"" + module + "\"", out_err);
  vector <string> lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    line = filter_string_trim (line);
    if (line.empty ()) continue;
    Database_Logs::log (line);
  }
}


// Get available SWORD modules.
vector <string> sword_logic_get_available ()
{
  string contents = filter_url_file_get_contents (sword_logic_module_list_path ());
  return filter_string_explode (contents, '\n');
}


// Get installed SWORD modules.
vector <string> sword_logic_get_installed ()
{
  vector <string> modules;
  string out_err;
  string sword_path = sword_logic_get_path ();
  filter_shell_run ("cd " + sword_path + "; installmgr -l", out_err);
  vector <string> lines = filter_string_explode (out_err, '\n');
  for (auto line : lines) {
    line = filter_string_trim (line);
    if (line.empty ()) continue;
    if (line.find ("[") == string::npos) continue;
    modules.push_back (line);
  }
  return modules;
}


string sword_logic_get_text (string source, string module, int book, int chapter, int verse)
{
  if (config_logic_client_prepared ()) {

    // Client checks for and optionally creates the cache for this SWORD module.
    if (!Database_Cache::exists (module)) {
      Database_Cache::create (module);
    }

    // If this module/passage exists in the cache, return it (it updates the access days in the cache).
    if (Database_Cache::exists (module, book, chapter, verse)) {
      return Database_Cache::retrieve (module, book, chapter, verse);
    }

    // Fetch this SWORD resource from the server.
    string address = Database_Config_General::getServerAddress ();
    int port = Database_Config_General::getServerPort ();
    if (!client_logic_client_enabled ()) {
      // If the client has not been connected to a cloud instance,
      // fetch the SWORD content from the Bibledit Cloud demo.
      address = demo_address ();
      port = demo_port ();
    }
    string url = client_logic_url (address, port, sync_resources_url ());
    string resource = "[" + source + "][" + module + "]";
    url = filter_url_build_http_query (url, "r", resource);
    url = filter_url_build_http_query (url, "b", convert_to_string (book));
    url = filter_url_build_http_query (url, "c", convert_to_string (chapter));
    url = filter_url_build_http_query (url, "v", convert_to_string (verse));
    string error;
    string html = filter_url_http_get (url, error);
    
    // In case of an error, don't cache that error, but let the user see it.
    if (!error.empty ()) return error;

    // Client caches this info for later.
    // Except when the Cloud is now installing the SWORD module.
    if (html != sword_logic_installing_module_text ()) {
      Database_Cache::cache (module, book, chapter, verse, html);
    }
    
    return html;
    
  } else {

    // The virtual URL for caching purposes.
    string url = sword_logic_virtual_url (module, book, chapter, verse);

    // The module text.
    string output;

    if (database_cache_exists (url)) {

      // Access cache for speed.
      output = database_cache_get (url);
      
    } else {

      // The server fetches the module text as follows:
      // diatheke -b KJV -k Jn 3:16
      string sword_path = sword_logic_get_path ();
      string osis = Database_Books::getOsisFromId (book);
      string command = "cd " + sword_path + "; diatheke -b " + module + " -k " + osis + " " + convert_to_string (chapter) + ":" + convert_to_string (verse);
      filter_shell_run (command, output);

    }
    
    // If the module has not been installed, the output of "diatheke" will be empty.
    // If the module was installed, but the requested passage is out of range,
    // the output of "diatheke" contains the module name, so it won't be empty.
    if (output.empty ()) {
      
      // Check whether the SWORD module exists.
      vector <string> modules = sword_logic_get_available ();
      string smodules = filter_string_implode (modules, "");
      if (smodules.find ("[" + module + "]") != string::npos) {
        // Schedule SWORD module installation.
        // (It used to be the case that this function, to get the text,
        // would wait till the SWORD module was installed, and then after installation,
        // return the text from that module.
        // But due to long waiting on Bibledit demo, while it would install multiple modules,
        // the Bibledit demo would become unresponsive.
        // So, it's better to return immediately with an informative text.)
        tasks_logic_queue (INSTALLSWORDMODULE, {source, module});
        // Return standard 'installing' information. Client knows not to cache this.
        return sword_logic_installing_module_text ();
      } else {
        return "Cannot find SWORD module " + module;
      }
    }
    
    // The server hits the cache for recording the last day the module was queried.
    // It hits passage 0.0.0 because the installed SWORD module is one data unit.
    string module_url = sword_logic_virtual_url (module, 0, 0, 0);
    if (!database_cache_exists (module_url)) {
      database_cache_put (module_url, "accessed");
    }
    database_cache_get (module_url);
    
    // If the module verse output was not cached yet, cache it here.
    if (!database_cache_exists (url)) {
      database_cache_put (url, output);
    }
    
    // The standard output of a Bible verse starts with the passage, like so:
    // Ruth 1:2:
    // Remove that.
    size_t pos = output.find (":");
    if (pos != string::npos) {
      output.erase (0, pos + 1);
    }
    pos = output.find (":");
    if (pos != string::npos) {
      output.erase (0, pos + 1);
    }
    
    // The standard output ends with the module name, like so:
    // (KJV)
    // Remove that.
    output = filter_string_str_replace ("(" + module + ")", "", output);
    
    // Remove any OSIS elements.
    filter_string_replace_between (output, "<", ">", "");
    
    // Clean whitespace away.
    output = filter_string_trim (output);
    
    return output;
  }

  return "";
}


// Checks the installed modules, whether they need to be updated.
void sword_logic_update_installed_modules ()
{
  Database_Logs::log ("Updating installed SWORD modules");

  vector <string> available_modules = sword_logic_get_available ();

  vector <string> installed_modules = sword_logic_get_installed ();
  for (auto & installed_module : installed_modules) {
    string module = sword_logic_get_installed_module (installed_module);
    string installed_version = sword_logic_get_version (installed_module);
    for (auto & available_module : available_modules) {
      if (sword_logic_get_remote_module (available_module) == module) {
        if (sword_logic_get_version (available_module) != installed_version) {
          string source = sword_logic_get_source (available_module);
          // Uninstall module.
          sword_logic_uninstall_module (module);
          // Clear the cache.
          Database_Versifications database_versifications;
          vector <int> books = database_versifications.getMaximumBooks ();
          for (auto & book : books) {
            vector <int> chapters = database_versifications.getMaximumChapters (book);
            for (auto & chapter : chapters) {
              vector <int> verses = database_versifications.getMaximumVerses (book, chapter);
              for (auto & verse : verses) {
                string schema = sword_logic_virtual_url (module, book, chapter, verse);
                database_cache_remove (schema);
              }
            }
          }
          // Schedule module installation.
          tasks_logic_queue (INSTALLSWORDMODULE, {source, module});
        }
        continue;
      }
    }
  }
  
  Database_Logs::log ("Ready updating installed SWORD modules");
}


// Trims the installed SWORD modules.
void sword_logic_trim_modules ()
{
  if (!config_logic_client_prepared ()) {
    Database_Logs::log ("Trimming the installed SWORD modules");
    vector <string> modules = sword_logic_get_installed ();
    for (auto module : modules) {
      module = sword_logic_get_installed_module (module);
      string url = sword_logic_virtual_url (module, 0, 0, 0);
      if (!database_cache_exists (url)) {
        sword_logic_uninstall_module (module);
      }
    }
    Database_Logs::log ("Ready trimming the SWORD caches and modules");
  }
}


// Text saying that the Cloud will install the requested SWORD module.
string sword_logic_installing_module_text ()
{
  return "The requested SWORD module is not yet installed. Bibledit Cloud will install it shortly. Please check back after a few minutes.";
}


// The virtual URL for caching purposes.
string sword_logic_virtual_url (const string & module, int book, int chapter, int verse)
{
  string url = "sword_" + module + "_" + convert_to_string (book) + "_chapter_" + convert_to_string (chapter) + "_verse_" + convert_to_string (verse);
  return url;
}


// The functions runs a scheduled module installation.
// The purpose of this function is that only one module installation occurs at a time,
// rather than simultaneously installing modules, which clogs the system.
void sword_logic_run_scheduled_module_install (string source, string module)
{
  // If a module is being installed,
  // and a call is made for another module installation,
  // re-schedule this module installation to postpone it,
  // till after this one is ready.
  sword_logic_installer_mutex.lock ();
  bool installing = sword_logic_installing_module;
  sword_logic_installer_mutex.unlock ();
  if (installing) {
    tasks_logic_queue (INSTALLSWORDMODULE, {source, module});
    return;
  }

  // Set flag for current module installation running.
  sword_logic_installer_mutex.lock ();
  sword_logic_installing_module = true;
  sword_logic_installer_mutex.unlock ();

  // Run the installer.
  sword_logic_install_module (source, module);
  
  // Clear flag as current module installation is ready.
  sword_logic_installer_mutex.lock ();
  sword_logic_installing_module = false;
  sword_logic_installer_mutex.unlock ();
}


void sword_logic_installmgr_initialize ()
{
#ifdef HAVE_SWORD
  sword::SWMgr *mgr = new sword::SWMgr();
  if (!mgr->config) Database_Logs::log ("ERROR: Please configure SWORD first.");

  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);
  
  sword::SWBuf confPath = baseDir + "/InstallMgr.conf";
  sword::FileMgr::createParent (confPath.c_str());
  remove(confPath.c_str());
  
  sword::InstallSource is("FTP");
  is.caption = "CrossWire";
  is.source = "ftp.crosswire.org";
  is.directory = "/pub/sword/raw";
  
  sword::SWConfig config(confPath.c_str());
  config["General"]["PassiveFTP"] = "true";
  config["Sources"]["FTPSource"] = is.getConfEnt();
  config.Save();

  delete installMgr;
  delete mgr;
#endif
}


void sword_logic_installmgr_synchronize_configuration_with_master ()
{
#ifdef HAVE_SWORD
  sword::SWMgr *mgr = new sword::SWMgr();
  
  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);
  
  if (!installMgr->refreshRemoteSourceConfiguration()) {
    //Database_Logs::log ("Synchronized SWORD configuration with the master remote source list");
  } else {
    Database_Logs::log ("Failed to synchronize SWORD configuration with the master remote source list");
  }
  
  delete installMgr;
  delete mgr;
#endif
}


void sword_logic_installmgr_list_remote_sources (vector <string> & sources)
{
#ifdef HAVE_SWORD
  sword::SWMgr *mgr = new sword::SWMgr();
  
  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);
  
  for (sword::InstallSourceMap::iterator it = installMgr->sources.begin(); it != installMgr->sources.end(); it++) {
    string caption (it->second->caption);
    sources.push_back (caption);
    /*
    string description;
    description.append (caption);
    description.append (" - ");
    description.append (it->second->type);
    description.append (" - ");
    description.append (it->second->source);
    description.append (" - ");
    description.append (it->second->directory);
    Database_Logs::log (description);
    */
  }
  
  delete installMgr;
  delete mgr;
#endif
}


void sword_logic_installmgr_refresh_remote_source (string name)
{
#ifdef HAVE_SWORD
  sword::SWMgr *mgr = new sword::SWMgr();
  
  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);

  sword::InstallSourceMap::iterator source = installMgr->sources.find(name.c_str ());
  if (source == installMgr->sources.end()) {
    Database_Logs::log ("Could not find remote source " + name);
  } else {
    if (!installMgr->refreshRemoteSource(source->second)) {
      //Database_Logs::log ("Remote source refreshed: " + name);
    } else {
      Database_Logs::log ("Error refreshing remote source " + name);
    }
  }
  
  delete installMgr;
  delete mgr;
#endif
}


void sword_logic_installmgr_list_remote_modules (string source_name, vector <string> & modules)
{
#ifdef HAVE_SWORD
  sword::SWMgr *mgr = new sword::SWMgr();
  
  sword::SWBuf baseDir = sword_logic_get_path ().c_str ();
  
  sword::InstallMgr *installMgr = new sword::InstallMgr (baseDir, NULL);
  installMgr->setUserDisclaimerConfirmed (true);
  
  sword::InstallSourceMap::iterator source = installMgr->sources.find(source_name.c_str ());
  if (source == installMgr->sources.end()) {
    Database_Logs::log ("Could not find remote source " + source_name);
  } else {
    sword::SWMgr *otherMgr = source->second->getMgr();
    sword::SWModule *module;
    if (!otherMgr) otherMgr = mgr;
    std::map<sword::SWModule *, int> mods = sword::InstallMgr::getModuleStatus(*mgr, *otherMgr);
    for (std::map<sword::SWModule *, int>::iterator it = mods.begin(); it != mods.end(); it++) {
      module = it->first;
      sword::SWBuf version = module->getConfigEntry("Version");
      sword::SWBuf status = " ";
      if (it->second & sword::InstallMgr::MODSTAT_NEW) status = "*";
      if (it->second & sword::InstallMgr::MODSTAT_OLDER) status = "-";
      if (it->second & sword::InstallMgr::MODSTAT_UPDATED) status = "+";
      string module_name (status);
      module_name.append ("[");
#ifdef HAVE_SWORD16
      module_name.append (module->Name());
#else
      module_name.append (module->getName());
#endif
      module_name.append ("]  \t(");
      module_name.append (version);
      module_name.append (")  \t- ");
#ifdef HAVE_SWORD16
      module_name.append (module->Description());
#else
      module_name.append (module->getDescription());
#endif
      // Check if the module is a verse-based Bible or commentary.
      bool verse_based = false;
#ifdef HAVE_SWORD16
      string module_type = module->Type ();
#else
      string module_type = module->getType ();
#endif
      if (module_type == "Biblical Texts") verse_based = true;
      if (module_type == "Commentaries") verse_based = true;
      if (verse_based) modules.push_back (module_name);
    }
  }
  
  delete installMgr;
  delete mgr;
#endif
}
