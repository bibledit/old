/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <filter/url.h>
#include <webserver/http.h>
#include <webserver/request.h>
#include <config/globals.h>
#include <filter/UriCodec.cpp>
#include <filter/string.h>
#include <filter/date.h>
#include <database/books.h>
#include <database/logs.h>
#ifdef HAVE_CLIENT
#else
#include <curl/curl.h>
#endif
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"


// SSL/TLS globals.
mbedtls_entropy_context filter_url_mbed_tls_entropy;
mbedtls_ctr_drbg_context filter_url_mbed_tls_ctr_drbg;
mbedtls_x509_crt filter_url_mbed_tls_cacert;


// Gets the base URL of current Bibledit installation.
string get_base_url (void * webserver_request)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;
  string scheme;
  string port;
  if (request->secure || config_globals_enforce_https_browser) {
    scheme = "https";
    port = convert_to_string (config_logic_https_network_port ());
  } else {
    scheme = "http";
    port = config_logic_http_network_port ();
  }
  string url = scheme + "://" + request->host + ":" + port + "/";
  return url;
}


// This function redirects the browser to "path".
// "path" is an absolute value.
void redirect_browser (void * webserver_request, string path)
{
  Webserver_Request * request = (Webserver_Request *) webserver_request;

  // A location header should contain an absolute url, like http://localhost/some/path.
  // See 14.30 in the specification https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html.
  
  // The absolute location contains the user-facing URL, when the administrator entered it.
  // This is needed in case of a proxy server,
  // where Bibledit may not be able to obtain the user-facing URL of the website.
  string location = config_logic_site_url (webserver_request);
  
  // If the request was secure, or supposed to be secure,
  // ensure the location contains https rather than plain http,
  // plus the correct secure port.
  if (request->secure || config_globals_enforce_https_browser) {
    location = filter_string_str_replace ("http:", "https:", location);
    string plainport = config_logic_http_network_port ();
    string secureport = convert_to_string (config_logic_https_network_port ());
    location = filter_string_str_replace (":" + plainport, ":" + secureport, location);
  }
  
  location.append (path);

  request->header = "Location: " + location;
  request->response_code = 302;
}


// C++ replacement for the dirname function, see http://linux.die.net/man/3/dirname.
string filter_url_dirname (string url)
{
  if (!url.empty ()) {
    if (url.find_last_of (DIRECTORY_SEPARATOR) == url.length () - 1) {
      // Remove trailing slash.
      url = url.substr (0, url.length () - 1);
    }
    size_t pos = url.find_last_of (DIRECTORY_SEPARATOR);
    if (pos != string::npos) url = url.substr (0, pos);
    else url = "";
  }
  if (url.empty ()) url = ".";
  return url;
}


// C++ replacement for the basename function, see http://linux.die.net/man/3/basename.
string filter_url_basename (string url)
{
  if (!url.empty ()) {
    if (url.find_last_of (DIRECTORY_SEPARATOR) == url.length () - 1) {
      // Remove trailing slash.
      url = url.substr (0, url.length () - 1);
    }
    size_t pos = url.find_last_of (DIRECTORY_SEPARATOR);
    if (pos != string::npos) url = url.substr (pos + 1);
  }
  return url;
}


void filter_url_unlink (string filename)
{
  unlink (filename.c_str ());
}


void filter_url_rename (const string& oldfilename, const string& newfilename)
{
  rename (oldfilename.c_str(), newfilename.c_str());
}


// Creates a file path out of the components.
string filter_url_create_path (string part1, string part2, string part3, string part4, string part5, string part6)
{
  string path (part1);
  if (part2.length()) path += DIRECTORY_SEPARATOR + part2;
  if (part3.length()) path += DIRECTORY_SEPARATOR + part3;
  if (part4.length()) path += DIRECTORY_SEPARATOR + part4;
  if (part5.length()) path += DIRECTORY_SEPARATOR + part5;
  if (part6.length()) path += DIRECTORY_SEPARATOR + part6;
  return path;
}


// Creates a file path out of the variable list of components, relative to the server's document root.
string filter_url_create_root_path (string part1, string part2, string part3, string part4, string part5)
{
  string path = config_globals_document_root;
  if (part1.length()) path += DIRECTORY_SEPARATOR + part1;
  if (part2.length()) path += DIRECTORY_SEPARATOR + part2;
  if (part3.length()) path += DIRECTORY_SEPARATOR + part3;
  if (part4.length()) path += DIRECTORY_SEPARATOR + part4;
  if (part5.length()) path += DIRECTORY_SEPARATOR + part5;
  return path;
}


// If $path contains the global document root,
// then this function removes it, and returns the result.
string filter_url_remove_root_path (string path)
{
  size_t pos = path.find (config_globals_document_root);
  if (pos != string::npos) path.erase (0, config_globals_document_root.length ());
  if (!path.empty ()) if (path.substr (0, 1) == DIRECTORY_SEPARATOR) path.erase (0, 1);
  return path;
}


// Gets the file / url extension, e.g. /home/joe/file.txt returns "txt".
string filter_url_get_extension (string url)
{
  string extension;
  size_t pos = url.find_last_of (".");
  if (pos != string::npos) {
    extension = url.substr (pos + 1);
  }
  return extension;
}


// Returns true if the file in "url" exists.
bool file_exists (string url)
{
  struct stat buffer;   
  return (stat (url.c_str(), &buffer) == 0);
}


// Wrapper for the mkdir function: make a directory. CheckWindows
// Creates parents where needed.
void filter_url_mkdir (string directory)
{
  int status;
#ifdef HAVE_MINGW
  status = mkdir (directory.c_str());
#else
  status = mkdir (directory.c_str(), 0777);
#endif
  if (status != 0) {
    vector <string> paths;
    paths.push_back (directory);
    directory = filter_url_dirname (directory);
    while (directory.length () > 2) {
      paths.push_back (directory);
      directory = filter_url_dirname (directory);
    }
    reverse (paths.begin (), paths.end ());
    for (unsigned int i = 0; i < paths.size (); i++) {
#ifdef HAVE_MINGW
      mkdir (paths[i].c_str ());
#else
      mkdir (paths[i].c_str (), 0777);
#endif
    }
  }
}


// Removes directory recursively.
void filter_url_rmdir (string directory)
{
  DIR *dir;
  struct dirent *entry;
  char path[PATH_MAX];
  dir = opendir(directory.c_str());
  if (dir == NULL) return;
  while ((entry = readdir (dir)) != NULL) {
    if (strcmp (entry->d_name, ".") && strcmp (entry->d_name, "..")) {
      snprintf (path, (size_t) PATH_MAX, "%s/%s", directory.c_str(), entry->d_name);
      // It used to test on entry->d_type == DT_DIR but this did not work within Mingw:
      // error: 'struct dirent' has no member named 'd_type'; did you mean 'd_name'?
      // if (entry->d_type == DT_DIR)
      // error: 'DT_DIR' was not declared in this scope
      // if (entry->d_type == DT_DIR)
      if (filter_url_is_dir (path)) {
        filter_url_rmdir (path);
      }
      remove (path);
    }
  }
  closedir(dir);
  remove (directory.c_str());
}


// Returns true is $path points to a directory.
bool filter_url_is_dir (string path)
{
  struct stat sb;
  stat (path.c_str(), &sb);
  return (sb.st_mode & S_IFMT) == S_IFDIR;
}


bool filter_url_get_write_permission (string path)
{
  int result = access (path.c_str(), W_OK);
  if (result == 0) return true;
  return false;
}


void filter_url_set_write_permission (string path)
{
  chmod (path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
}


// C++ rough equivalent for PHP'sfilter_url_file_get_contents.
string filter_url_file_get_contents (string filename)
{
  if (!file_exists (filename)) return "";
  try {
    ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
    streamoff filesize = ifs.tellg();
    ifs.seekg (0, ios::beg);
    vector <char> bytes ((int)filesize);
    ifs.read (&bytes[0], (int)filesize);
    return string (&bytes[0], (int)filesize);
  } catch (...) {
    return "";
  }
}


// C++ rough equivalent for PHP'sfilter_url_file_put_contents.
void filter_url_file_put_contents (string filename, string contents)
{
  try {
    ofstream file;  
    file.open (filename, ios::binary | ios::trunc);
    file << contents;
    file.close ();
  } catch (...) {
  }
}


// C++ rough equivalent for PHP'sfilter_url_file_put_contents.
// Appends the data if the file exists.
void filter_url_file_put_contents_append (string filename, string contents)
{
  try {
    ofstream file;  
    file.open (filename, ios::binary | ios::app);  
    file << contents;
    file.close ();
  } catch (...) {
  }
}


// Copies the contents of file named "input" to file named "output".
// It is assumed that the folder where "output" will reside exists.
bool filter_url_file_cp (string input, string output)
{
  try {
    ifstream source (input, ios::binary);
    ofstream dest (output, ios::binary | ios::trunc);
    dest << source.rdbuf();
    source.close();
    dest.close();
  } catch (...) {
    return false;
  }
  return true;
}


// Copies the entire directory $input to a directory named $output.
// It will recursively copy the inner directories also.
void filter_url_dir_cp (const string & input, const string & output)
{
  // Create the output directory.
  filter_url_mkdir (output);
  // Check on all files in the input directory.
  vector <string> files = filter_url_scandir (input);
  for (auto & file : files) {
    string input_path = filter_url_create_path (input, file);
    string output_path = filter_url_create_path (output, file);
    if (filter_url_is_dir (input_path)) {
      // Create output directory.
      filter_url_mkdir (output_path);
      // Handle the new input directory.
      filter_url_dir_cp (input_path, output_path);
    } else {
      // Copy input file to output.
      filter_url_file_cp (input_path, output_path);
    }
  }
}


// A C++ equivalent for PHP's filesize function.
int filter_url_filesize (string filename)
{
  struct stat stat_buf;
  int rc = stat (filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : 0;
}


// A C++ near equivalent for PHP's scandir function.
vector <string> filter_url_scandir (string folder)
{
  vector <string> files;
  DIR * dir = opendir (folder.c_str());
  if (dir) {
    struct dirent * direntry;
    while ((direntry = readdir (dir)) != NULL) {
      string name = direntry->d_name;
      if (name.substr (0, 1) == ".") continue;
      if (name == "gitflag") continue;
      files.push_back (name);
    }
    closedir (dir);
  }
  sort (files.begin(), files.end());
  return files;
}


// Recursively scans a directory for directories and files.
void filter_url_recursive_scandir (string folder, vector <string> & paths)
{
  vector <string> files = filter_url_scandir (folder);
  for (auto & file : files) {
    string path = filter_url_create_path (folder, file);
    paths.push_back (path);
    if (filter_url_is_dir (path)) {
      filter_url_recursive_scandir (path, paths);
    }
  }
}


// A C++ near equivalent for PHP's filemtime function.
int filter_url_filemtime (string filename)
{
  struct stat attributes;
  stat (filename.c_str(), &attributes);
  return (int) attributes.st_mtime;
}


// A C++ near equivalent for PHP's urldecode function.
string filter_url_urldecode (string url)
{
  url = UriDecode (url);
  replace (url.begin (), url.end (), '+', ' ');
  return url;
}


// A C++ near equivalent for PHP's urlencode function.
string filter_url_urlencode (string url)
{
  url = UriEncode (url);
  return url;
}


// Returns the name of a temporary file.
string filter_url_tempfile (const char * directory)
{
  string filename = convert_to_string (filter_date_seconds_since_epoch ()) + convert_to_string (filter_date_numerical_microseconds ()) + convert_to_string (filter_string_rand (10000000, 99999999));
  if (directory) {
    filename = filter_url_create_path (directory, filename);
  } else {
    filename = filter_url_create_root_path ("tmp", filename);
  }
  return filename;
}


// C++ equivalent for PHP's escapeshellarg function.
string filter_url_escape_shell_argument (string argument)
{
  argument = filter_string_str_replace ("'", "\\'", argument);
  argument.insert (0, "'");
  argument.append ("'");
  return argument;
}


// The function accepts a $path.
// The function may add a numerical suffix 
// to ensure that the $path does not yet exist in the filesystem.
string filter_url_unique_path (string path)
{
  if (!file_exists (path)) return path;
  for (size_t i = 1; i < 100; i++) {
    string uniquepath = path + "." + convert_to_string (i);
    if (!file_exists (uniquepath)) return uniquepath;
  }
  return path + "." + convert_to_string (filter_string_rand (100, 1000));
}


// Returns true if the email address is valid.
bool filter_url_email_is_valid (string email)
{
  const string valid_set ("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._-");
  // The @ character should appear only once.
  vector <string> atbits = filter_string_explode (email, '@');
  if (atbits.size() != 2) return false;
  // The characters on the left of @ should be from the valid set.
  string left = atbits [0];
  for (unsigned int i = 0; i < left.size(); i++) {
    char c = left [i];
    if (valid_set.find (c) == string::npos) return false;
  }
  // The characters on the right of @ should be from the valid set.
  string right = atbits [1];
  for (unsigned int i = 0; i < right.size(); i++) {
    char c = right [i];
    if (valid_set.find (c) == string::npos) return false;
  }
  // The character . should appear at least once to the right of @.
  vector <string> dotbits = filter_string_explode (right, '.');
  if (dotbits.size () < 2) return false;
  // The email address is valid.
  return true;
}


string filter_url_build_http_query (string url, const string& parameter, const string& value)
{
  size_t pos = url.find ("?");
  if (pos == string::npos) url.append ("?");
  else url.append ("&");
  url.append (parameter);
  url.append ("=");
  url.append (value);
  return url;
}


size_t filter_url_curl_write_function (void *ptr, size_t size, size_t count, void *stream)
{
  ((string *) stream)->append ((char *) ptr, 0, size * count);
  return size * count;
}


// Sends a http GET request to the $url.
// It returns the response from the server.
// It writes any error to $error.
string filter_url_http_get (string url, string& error, bool check_certificate)
{
  string response;
#ifdef HAVE_CLIENT
  response = filter_url_http_request_mbed (url, error, {}, "", check_certificate);
#else
  (void) check_certificate;
  CURL *curl = curl_easy_init ();
  if (curl) {
    curl_easy_setopt (curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, filter_url_curl_write_function);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    //curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
    // Because a Bibledit client should work even over very bad networks,
    // pass some timeout options to curl so it properly deals with such networks.
    filter_url_curl_set_timeout (curl);
    CURLcode res = curl_easy_perform (curl);
    if (res == CURLE_OK) {
      error.clear ();
      long http_code = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code != 200) {
        response.append ("http code " + convert_to_string ((int)http_code));
      }
    } else {
      response.clear ();
      error = curl_easy_strerror (res);
    }
    curl_easy_cleanup (curl);
  }
#endif
  return response;
}


// Sends a http POST request to $url.
// burst: Set connection timing for burst mode, where the response comes after a relatively long silence.
// It posts the $values.
// It returns the response from the server.
// It writes any error to $error.
string filter_url_http_post (string url, map <string, string> values, string& error, bool burst, bool check_certificate)
{
  string response;
#ifdef HAVE_CLIENT
  (void) burst;
  response = filter_url_http_request_mbed (url, error, values, "", check_certificate);
#else
  (void) check_certificate;
  // Get a curl handle.
  CURL *curl = curl_easy_init ();
  if (curl) {
    // First set the URL that is about to receive the POST.
    // This can be http or https.
    curl_easy_setopt (curl, CURLOPT_URL, url.c_str());
    // Generate the post data.
    string postdata;
    for (auto & element : values) {
      if (!postdata.empty ()) postdata.append ("&");
      postdata.append (element.first);
      postdata.append ("=");
      postdata.append (filter_url_urlencode (element.second));
    }
    // Specify the POST data to curl, e.g.: "name=foo&project=bar"
    curl_easy_setopt (curl, CURLOPT_POSTFIELDS, postdata.c_str());
    // Callback for the server response.
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, filter_url_curl_write_function);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);
    // Further options.
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    // curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
    // Timeouts for very bad networks, see the GET routine above for an explanation.
    filter_url_curl_set_timeout (curl, burst);
    // Perform the request.
    CURLcode res = curl_easy_perform (curl);
    // Result check.
    if (res == CURLE_OK) {
      error.clear ();
      long http_code = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code != 200) {
        error.append ("Server response " + filter_url_http_response_code_text (http_code));
      }
    } else {
      error = curl_easy_strerror (res);
    }
    // Always cleanup.
    curl_easy_cleanup (curl);
  }
#endif
  return response;
}


// Sends a http POST request to $url as from a <form> with enctype="multipart/form-data".
// It posts the $values.
// It uploads $filename.
// It returns the response from the server.
// It writes any error to $error.
string filter_url_http_upload (string url, map <string, string> values, string filename, string& error)
{
  string response;

#ifdef HAVE_CLIENT
  (void) url;
  (void) values;
  (void) filename;
  error = "Not implemented in client configuration";
#else

  // Coded while looking at http://curl.haxx.se/libcurl/c/postit2.html.
  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;

  // Fill in the text fields to submit.
  for (auto & element : values) {
    curl_formadd (&formpost, &lastptr,
                  CURLFORM_COPYNAME, element.first.c_str (),
                  CURLFORM_COPYCONTENTS, element.second.c_str(),
                  CURLFORM_END);
  }

  // Fill in the file upload field to submit.
  curl_formadd(&formpost, &lastptr,
               CURLFORM_COPYNAME, "uploadedZipFile",
               CURLFORM_FILE, filename.c_str(),
               CURLFORM_END);

  // Get a curl handle.
  CURL *curl = curl_easy_init ();
  if (curl) {
    // First set the URL that is about to receive the POST.
    curl_easy_setopt (curl, CURLOPT_URL, url.c_str());
    // Specify the POST data to curl.
    curl_easy_setopt (curl, CURLOPT_HTTPPOST, formpost);
    // Callback for the server response.
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, filter_url_curl_write_function);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &response);
    // Further options.
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    // curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
    // Timeouts for very bad networks, see the GET routine above for an explanation.
    filter_url_curl_set_timeout (curl, false);
    // Perform the request.
    CURLcode res = curl_easy_perform (curl);
    // Result check.
    if (res == CURLE_OK) {
      error.clear ();
      long http_code = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code != 200) {
        error.append ("Server response " + filter_url_http_response_code_text (http_code));
      }
    } else {
      error = curl_easy_strerror (res);
    }
    // Always cleanup cURL.
    curl_easy_cleanup (curl);
    // Then cleanup the formpost chain.
    curl_formfree (formpost);
  }
#endif

  return response;
}


string filter_url_http_response_code_text (int code)
{
  string text = convert_to_string (code);
  text.append (" ");
  switch (code) {
    case 100: text += "Continue"; break;
    case 101: text += "Switching Protocols"; break;
    case 200: text += "OK"; break;
    case 201: text += "Created"; break;
    case 202: text += "Accepted"; break;
    case 203: text += "Non-Authoritative Information"; break;
    case 204: text += "No Content"; break;
    case 205: text += "Reset Content"; break;
    case 206: text += "Partial Content"; break;
    case 300: text += "Multiple Choices"; break;
    case 301: text += "Moved Permanently"; break;
    case 302: text += "Found"; break;
    case 303: text += "See Other"; break;
    case 304: text += "Not Modified"; break;
    case 305: text += "Use Proxy"; break;
    case 307: text += "Temporary Redirect"; break;
    case 308: text += "Permanent Redirect"; break;
    case 400: text += "Bad Request"; break;
    case 401: text += "Unauthorized"; break;
    case 402: text += "Payment Required"; break;
    case 403: text += "Forbidden"; break;
    case 404: text += "Not Found"; break;
    case 405: text += "Method Not Allowed"; break;
    case 406: text += "Not Acceptable"; break;
    case 407: text += "Proxy Authentication Required"; break;
    case 408: text += "Request Timeout"; break;
    case 409: text += "Conflict"; break;
    case 410: text += "Gone"; break;
    case 411: text += "Length Required"; break;
    case 412: text += "Precondition Failed"; break;
    case 413: text += "Request Entity Too Large"; break;
    case 414: text += "Request-URI Too Long"; break;
    case 415: text += "Unsupported Media Type"; break;
    case 416: text += "Requested Range Not Satisfiable"; break;
    case 417: text += "Expectation Failed"; break;
    case 426: text += "Upgrade Required"; break;
    case 428: text += "Precondition Required"; break;
    case 429: text += "Too Many Requests"; break;
    case 431: text += "Request Header Fields Too Large"; break;
    case 500: text += "Internal Server Error"; break;
    case 501: text += "Not Implemented"; break;
    case 502: text += "Bad Gateway"; break;
    case 503: text += "Service Unavailable"; break;
    case 504: text += "Gateway Timeout"; break;
    case 505: text += "HTTP Version Not Supported"; break;
    case 511: text += "Network Authentication Required"; break;
    default:  text += "Error"; break;
  }
  return text;
}


// Downloads the file at $url, and stores it at $filename.
void filter_url_download_file (string url, string filename, string& error, bool check_certificate)
{
#ifdef HAVE_CLIENT
  filter_url_http_request_mbed (url, error, {}, filename, check_certificate);
#else
  (void) check_certificate;
  CURL *curl = curl_easy_init ();
  if (curl) {
    curl_easy_setopt (curl, CURLOPT_URL, url.c_str());
    FILE* file = fopen (filename.c_str(), "w");
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    // curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
    filter_url_curl_set_timeout (curl);
    CURLcode res = curl_easy_perform (curl);
    if (res == CURLE_OK) {
      error.clear ();
      long http_code = 0;
      curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code != 200) {
        error.append ("http code " + convert_to_string ((int)http_code));
      }
    } else {
      error = curl_easy_strerror (res);
    }
    curl_easy_cleanup (curl);
    fclose (file);
  }
#endif
}


/*
 * The function returns the filename for a html file for a Bible.
 * $path    - The path where to store the files.
 * $book    - The book identifier.
 * $chapter - The chapter number.
 */
string filter_url_html_file_name_bible (string path, int book, int chapter)
{
  string filename;
  
  // If a path is given, prefix it.
  if (path != "") {
    filename = path + "/";
  }
  
  // No book ID given: Return the name for the index file for the Bible.
  if (book == 0) {
    filename += "index.html";
    return filename;
  }
  
  // Add the name for the book. No spaces.
  filename += filter_string_fill (convert_to_string (book), 2, '0');
  string sbook = Database_Books::getEnglishFromId (book);
  sbook = filter_string_str_replace (" ", "", sbook);
  filename += '-' + sbook;
  
  // Chapter given: Provide name for the chaper.
  if (chapter >= 0) {
    filename += '-' + filter_string_fill (convert_to_string (chapter), 3, '0');
  }
  
  filename += ".html";
  
  return filename;
}


// Callback function for logging cURL debug information.
#ifdef HAVE_CLIENT
#else
int filter_url_curl_debug_callback (void *curl_handle, int curl_info_type, char *data, size_t size, void *userptr)
{
  if (curl_handle && userptr) {};
  bool log = true;
  curl_infotype type = (curl_infotype) curl_info_type;
  if (type == CURLINFO_SSL_DATA_OUT) log = false;
  if (type == CURLINFO_SSL_DATA_OUT) log = false;
  if (log) {
    string message (data, size);
    Database_Logs::log (message);
  }
  return 0;
}
#endif


// Sets timeouts for cURL operations.
// burst: When true, the server gives a burst response, that is, all data arrives at once after a delay.
//        When false, the data is supposed to be downloaded gradually.
// Without these timeouts, the Bibledit client will hang on stalled sync operations.
#ifdef HAVE_CLIENT
#else
void filter_url_curl_set_timeout (void *curl_handle, bool burst)
{
  CURL * handle = (CURL *) curl_handle;
  
  // There is a timeout on establishing a connection.
  curl_easy_setopt (handle, CURLOPT_CONNECTTIMEOUT, 10);
  
  // There is a also a transfer timeout for normal speeds.
  curl_easy_setopt (handle, CURLOPT_TIMEOUT, 600);
  
  // There is also a shorter transfer timeout for low speeds,
  // because low speeds indicate a stalled connection.
  // But when the server needs to do a lot of processing and then sends then response at once,
  // the low speed timeouts should be disabled,
  // else it times out before the response has come.
  if (!burst) {
    curl_easy_setopt (handle, CURLOPT_LOW_SPEED_LIMIT, 100);
    curl_easy_setopt (handle, CURLOPT_LOW_SPEED_TIME, 10);
  }
  
  // Timing out may use signals, which is not what we want.
  curl_easy_setopt (handle, CURLOPT_NOSIGNAL, 1L);
}
#endif



// When the client POSTs + sign to the server,
// the + sign is replaced with a space in the process.
// Therefore first convert the + to a TAG before sending it off.
string filter_url_plus_to_tag (string data)
{
  return filter_string_str_replace ("+", "PLUSSIGN", data);
}


// When POSTing a + sign via jQuery to the server,
// the + sign is replaced with a space in the process.
// Javascript first converts the + to a TAG before sending it off.
// This function reverts the TAG to the original + sign.
string filter_url_tag_to_plus (string data)
{
  return filter_string_str_replace ("PLUSSIGN", "+", data);
}


// This filter removes the username and password components from the $url.
string filter_url_remove_username_password (string url)
{
  string slashes = "//";
  size_t pos = url.find (slashes);

  // Consider the following URL for github:
  // https://username:password@github.com/username/repository.git
  if (filter_string_replace_between (url, slashes, ":", "")) {
    if (pos != string::npos) url.insert (pos, slashes);
  }
  if (filter_string_replace_between (url, slashes, "@", "")) {
    if (pos != string::npos) url.insert (pos, slashes);
  }
  
  return url;
}


// A very simple function that sends a HTTP GET or POST request and reads the response.
// It handles plain and secure http.
// $url: The URL including host / port / path.
// $error: To store any error messages.
// $post: Value pairs for a POST request.
// $filename: The filename to save the data to.
// $check_certificate: Whether the check the server certificate in case of secure http.
string filter_url_http_request_mbed (string url, string& error, const map <string, string>& post, const string& filename, bool check_certificate) // CheckWindows
{
  // The "http" scheme is used to locate network resources via the HTTP protocol.
  // $url = "http(s):" "//" host [ ":" port ] [ abs_path [ "?" query ]]
  
  
  // Whether this is a secure http request.
  bool secure = url.find ("https:") != string::npos;
  
  
  // Remove the scheme: http(s).
  size_t pos = url.find ("://");
  if (pos != string::npos) {
    url.erase (0, pos + 3);
  }

  
  // Extract the host.
  pos = url.find (":");
  if (pos == string::npos) pos = url.find ("/");
  if (pos == string::npos) pos = url.length () + 1;
  string hostname = url.substr (0, pos);
  url.erase (0, hostname.length ());

  
  // Default port numbers for plain or secure http.
  int port = 80;
  if (secure) port = 443;
  // Extract the port number if any.
  pos = url.find (":");
  if (pos != string::npos) {
    url.erase (0, 1);
    size_t pos2 = url.find ("/");
    if (pos2 == string::npos) pos2 = url.length () + 1;
    string p = url.substr (0, pos2);
    port = convert_to_int (p);
    url.erase (0, p.length ());
  }
  
  
  // Empty URL results in a slash.
  if (url.empty ()) url = "/";
  
  
  // The absolute path plus optional query remain after extracting the preceding stuff.
  

  bool connection_healthy = true;
  
  
  // SSL/TLS configuration and context.
  // The configuration is local, because options may not be the same for every website.
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  if (secure) {
    mbedtls_ssl_init (&ssl);
    mbedtls_ssl_config_init (&conf);
  }
  
  
  // Resolve the host.
  struct addrinfo hints;
  struct addrinfo * address_results;
  bool address_info_resolved = false;
  if (!secure) {
    memset (&hints, 0, sizeof (struct addrinfo));
    // Allow IPv4 and IPv6.
    hints.ai_family = AF_UNSPEC;
    // TCP/IP socket.
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    // Select protocol that matches with the socket type.
    hints.ai_protocol = 0;
    // The 'service' is actually the port number.
    string service = convert_to_string (port);
    // Get a list of address structures. There can be several of them.
    int res = getaddrinfo (hostname.c_str(), service.c_str (), &hints, &address_results);
    if (res != 0) {
      error = hostname + ": ";
      error.append (gai_strerror (res));
      connection_healthy = false;
    } else {
      address_info_resolved = true;
    }
  }
  
  
  // Secure connection setup.
  mbedtls_net_context fd;
  if (secure) {

    // Secure socket setup.
    if (connection_healthy) {
      mbedtls_net_init (&fd);
    }

    // SSL/TLS connection configuration.
    if (connection_healthy) {
      int ret = mbedtls_ssl_config_defaults (&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
      if (ret != 0) {
        filter_url_display_mbed_tls_error (ret, &error, false);
        connection_healthy = false;
      }
      mbedtls_ssl_conf_authmode (&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
      mbedtls_ssl_conf_ca_chain (&conf, &filter_url_mbed_tls_cacert, NULL);
      mbedtls_ssl_conf_rng (&conf, mbedtls_ctr_drbg_random, &filter_url_mbed_tls_ctr_drbg);
      ret = mbedtls_ssl_setup (&ssl, &conf);
      if (ret != 0) {
        filter_url_display_mbed_tls_error (ret, &error, false);
        connection_healthy = false;
      }
      // The hostname it connects to, and verifies the certificate for.
      ret = mbedtls_ssl_set_hostname (&ssl, hostname.c_str ());
      if (ret != 0) {
        filter_url_display_mbed_tls_error (ret, &error, false);
        connection_healthy = false;
      }
      mbedtls_ssl_set_bio (&ssl, &fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    }
    
    // Secure connect to host.
    if (connection_healthy) {
      const char * server_port = convert_to_string (port).c_str ();
      int ret = mbedtls_net_connect (&fd, hostname.c_str(), server_port, MBEDTLS_NET_PROTO_TCP);
      if (ret != 0) {
        filter_url_display_mbed_tls_error (ret, &error, false);
        connection_healthy = false;
      }
    }
  }
  
  
  // Plain connection setup.
  int sock = 0;
  if (!secure) {
    
    // Iterate over the list of address structures.
    vector <string> errors;
    struct addrinfo * rp = NULL;
    if (connection_healthy) {
      for (rp = address_results; rp != NULL; rp = rp->ai_next) {
        // Try to get a socket for this address structure.
        sock = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        // If it fails, try the next one.
        if (sock < 0) {
          string err = "Creating socket: ";
          err.append (strerror (errno));
          errors.push_back (err);
          continue;
        }
        // Try to connect.
        int res = connect (sock, rp->ai_addr, rp->ai_addrlen);
        // Test and record error.
        if (res < 0) {
          string err = hostname + ":" + convert_to_string (port) + ": ";
          err.append (strerror (errno));
          errors.push_back (err);
        }
        // If success: Done.
        if (res != -1) break;
        // Failure: Socket should be closed: This is done in the code below.
      }
    }
    
    // Check whether no address succeeded.
    if (connection_healthy) {
      if (rp == NULL) {
        error = filter_string_implode (errors, " | ");
        connection_healthy = false;
      }
    }
    
    // No longer needed: Only to be freed when the address was resolved.
    if (address_info_resolved) freeaddrinfo (address_results);
  }
  
  
  // A Bibledit client should work even over very bad networks,
  // so set a timeout on the network connection.
  if (connection_healthy) {
    // Socket, whether plain or secure http.
    int comm_sock = sock;
    if (secure) comm_sock = fd.fd;
    // Make the timeout not too short, so it can support very slow networks. CheckWindows
#ifdef HAVE_MINGW
    // Windows: Timeout value is a DWORD in milliseconds, address passed to setsockopt() is const char *
    const char * tv = "600000";
#else
    // Linux: Timeout value is a struct timeval, address passed to setsockopt() is const void *
    struct timeval tv;
    tv.tv_sec = 600;
    tv.tv_usec = 0;
#endif
    // Check on the result of setting the socket options.
    // If it cannot be set, record it in the journal,
    // but still proceed with the connection, because this is not fatal.
    int ret;
#ifdef HAVE_MINGW
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_RCVTIMEO, tv, sizeof (tv));
#else
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
#endif
    if (ret != 0) Database_Logs::log (strerror (errno));
#ifdef HAVE_MINGW
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_SNDTIMEO, tv, sizeof (tv));
#else
    ret = setsockopt (comm_sock, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
#endif
    if (ret != 0) Database_Logs::log (strerror (errno));
  }
  
  
  // SSL/TLS handshake.
  if (secure) {
    int ret;
    while (connection_healthy && ((ret = mbedtls_ssl_handshake (&ssl)) != 0)) {
      if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
      if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
      filter_url_display_mbed_tls_error (ret, &error, false);
      connection_healthy = false;
    }
  }
  
  
  // Optionally verify the server certificate.
  if (connection_healthy && secure && check_certificate) {
    uint32_t flags = mbedtls_ssl_get_verify_result (&ssl);
    if (flags != 0) {
      char vrfy_buf [512];
      mbedtls_x509_crt_verify_info (vrfy_buf, sizeof (vrfy_buf), " ! ", flags);
      error = vrfy_buf;
      connection_healthy = false;
    }
  }

  
  // Assemble the data to POST, if any.
  string postdata;
  for (auto & element : post) {
    if (!postdata.empty ()) postdata.append ("&");
    postdata.append (element.first);
    postdata.append ("=");
    postdata.append (filter_url_urlencode (element.second));
  }
  
  
  // Send the request.
  if (connection_healthy) {
    string request = "GET";
    if (!post.empty ()) request = "POST";
    request.append (" ");
    request.append (url);
    request.append (" ");
    request.append ("HTTP/1.1");
    request.append ("\r\n");
    request.append ("Host: ");
    request.append (hostname);
    request.append ("\r\n");
    // Close connection, else it's harder to locate the end of the response.
    request.append ("Connection: close");
    request.append ("\r\n");
    if (!post.empty ()) {
      request.append ("Content-Type: application/x-www-form-urlencoded");
      request.append ("\r\n");
      request.append ("Content-Length: " + convert_to_string (postdata.length()));
      request.append ("\r\n");
    }
    request.append ("\r\n");
    request.append (postdata);
    if (secure) {
      
      // Write the secure http request to the server.
      const char * output = request.c_str();
      const unsigned char * buf = (const unsigned char *) output;
      // The C function strlen () fails on null characters in the request, so take string::size()
      size_t len = request.size ();
      while (connection_healthy && (len > 0)) {
        // Function
        // int ret = mbedtls_ssl_write (&ssl, buf, len)
        // will do partial writes in some cases.
        // If the return value is non-negative but less than length,
        // the function must be called again with updated arguments:
        // buf + ret, len - ret
        // until it returns a value equal to the last 'len' argument.
        int ret = mbedtls_ssl_write (&ssl, buf, len);
        if (ret > 0) {
          buf += ret;
          len -= ret;
        } else {
          // When it returns MBEDTLS_ERR_SSL_WANT_WRITE/READ,
          // it must be called later with the *same* arguments,
          // until it returns a positive value.
          if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
          if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) continue;
          filter_url_display_mbed_tls_error (ret, &error, false);
          connection_healthy = false;
        }
      }
      
    } else {

      // Send plain http.
      if (send (sock, request.c_str(), request.length(), 0) != (int) request.length ()) {
        error = "Sending request: ";
        error.append (strerror (errno));
        connection_healthy = false;
      }
      
    }
  }
  
  
  // Read the response headers and body.
  string headers;
  string response;
  if (connection_healthy) {

    bool reading = true;
    bool reading_body = false;
    char prev = 0;
    char cur;
    FILE * file = NULL;
    if (!filename.empty ()) file = fopen (filename.c_str(), "w");
    
    do {
      int ret = 0;
      if (secure) {
        unsigned char buffer [1];
        memset (&buffer, 0, 1);
        ret = mbedtls_ssl_read (&ssl, buffer, 1);
        cur = buffer [0];
      } else {
        ret = read (sock, &cur, 1);
      }
      if (ret > 0) {
        if (reading_body) {
          if (file) fwrite (&cur, 1, 1, file);
          else response += cur;
        } else {
          if (cur == '\r') continue;
          headers += cur;
          if ((cur == '\n') && (prev == '\n')) reading_body = true;
          prev = cur;
        }
      } else if (!secure && (ret < 0)) {
        error = "Receiving: ";
        error.append (strerror (errno));
        connection_healthy = false;
      } else if (secure && (ret == MBEDTLS_ERR_SSL_WANT_READ)) {
      } else if (secure && (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
      } else if (secure && (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
      } else if (secure && (ret < 0)) {
        filter_url_display_mbed_tls_error (ret, &error, false);
        connection_healthy = false;
      } else {
        // Probably EOF.
        reading = false;
      }
    } while (reading && connection_healthy);

    if (file) fclose (file);
  }
  
  
  if (secure) {
    mbedtls_ssl_close_notify (&ssl);
    mbedtls_net_free (&fd);
    mbedtls_ssl_free (&ssl);
    mbedtls_ssl_config_free (&conf);
  } else {
    // Only close the socket if it was open.
    // It used to close (0) in error,
    // and on Android and iOS, when this was done a couple of times, it would crash the app.
    if (sock > 0) close (sock);
  }

  
  // Check the response headers.
  vector <string> lines = filter_string_explode (headers, '\n');
  for (auto & line : lines) {
    if (line.empty ()) continue;
    if (line.find ("HTTP") != string::npos) {
      size_t pos = line.find (" ");
      if (pos != string::npos) {
        line.erase (0, pos + 1);
        int response_code = convert_to_int (line);
        if (response_code != 200) {
          error = "Response code: " + line;
          return "";
        }
      } else {
        error = "Invalid response: " + line;
        return "";
      }
    }
  }
  
  
  // Done.
  if (!connection_healthy) response.clear ();
  return response;
}


// Initialize the SSL/TLS system once.
void filter_url_ssl_tls_initialize ()
{
  int ret = 0;
  // Random number generator.
  mbedtls_ctr_drbg_init (&filter_url_mbed_tls_ctr_drbg);
  mbedtls_entropy_init (&filter_url_mbed_tls_entropy);
  const char *pers = "Bibledit Client";
  ret = mbedtls_ctr_drbg_seed (&filter_url_mbed_tls_ctr_drbg,
                               mbedtls_entropy_func, &filter_url_mbed_tls_entropy,
                               (const unsigned char *) pers, strlen (pers));
  filter_url_display_mbed_tls_error (ret, NULL, false);
  // Read the trusted root certificates.
  string path = filter_url_create_root_path ("filter", "cas.crt");
  ret = mbedtls_x509_crt_parse_file (&filter_url_mbed_tls_cacert, path.c_str ());
  filter_url_display_mbed_tls_error (ret, NULL, false);
}


// Finalize the SSL/TLS system once.
void filter_url_ssl_tls_finalize ()
{
  mbedtls_ctr_drbg_free (&filter_url_mbed_tls_ctr_drbg);
  mbedtls_entropy_free (&filter_url_mbed_tls_entropy);
  mbedtls_x509_crt_free (&filter_url_mbed_tls_cacert);
}


// This logs the $ret (return) value, converted to readable text, to the journal.
// If $error is given, it is stored there instead.
// It $server is true, it suppresses additional error codes.
void filter_url_display_mbed_tls_error (int & ret, string * error, bool server)
{
  // Local copy of the return value, and clear the original return value.
  int local_return = ret;
  ret = 0;
  
  // Everything OK
  if (local_return == 0) return;

  // The server suppresses a couple of error messages caused by rogue clients or spiders.
  // The reason for suppressing them is to prevent them from flooding the Journal.
  if (server) {
    // SSL - Processing of the ClientHello handshake message failed (-30976)
    if (local_return == MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO) return;
    // SSL - The connection indicated an EOF (-29312)
    if (local_return == MBEDTLS_ERR_SSL_CONN_EOF) return;
    // NET - Reading information from the socket failed (-76)
    if (local_return == MBEDTLS_ERR_NET_RECV_FAILED) return;
    // NET - Connection was reset by peer (-80)
    if (local_return == MBEDTLS_ERR_NET_CONN_RESET) return;
  }
  
  // There's an error: Display it.
  char error_buf [100];
  mbedtls_strerror (local_return, error_buf, 100);
  string msg = error_buf;
  msg.append (" (");
  msg.append (convert_to_string (local_return));
  msg.append (")");
  if (error) {
    error->assign (msg);
  } else {
    Database_Logs::log (msg);
  }
}


// This takes $url, removes any scheme (http / https) it has,
// then sets the correct scheme based on $secure,
// and returns the URL, e.g. as http://localhost or https://localhost.
string filter_url_set_scheme (string url, bool secure)
{
  // Remove whitespace.
  url = filter_string_trim (url);
  // Remove amy existing scheme: http(s) or whatever.
  size_t pos = url.find ("://");
  if (pos != string::npos) {
    url.erase (0, pos + 3);
  }
  // Produce the correct scheme.
  string scheme = "http";
  if (secure) scheme.append ("s");
  scheme.append ("://");
  // Insert the scheme.
  url = scheme + url;
  // Done.
  return url;
}


// Replace invalid characters in Windows filenames with valid abbreviations.
string filter_url_clean_filename (string name)
{
  name = filter_string_str_replace ("\\", "b2", name);
  name = filter_string_str_replace ("/",  "sl", name);
  name = filter_string_str_replace (":",  "co", name);
  name = filter_string_str_replace ("*",  "as", name);
  name = filter_string_str_replace ("?",  "qu", name);
  name = filter_string_str_replace ("\"", "ba", name);
  name = filter_string_str_replace ("<",  "sm", name);
  name = filter_string_str_replace (">",  "la", name);
  name = filter_string_str_replace ("|",  "ve", name);
  return name;
}


// Replace invalid characters in Windows filenames with valid abbreviations.
// In contrast with the above function, the $name in this function can be "uncleaned" again.
// The next function does the "unclean" operation, to get the original $name back.
string filter_url_filename_clean (string name)
{
  name = filter_string_str_replace ("\\", "___b2___", name);
  name = filter_string_str_replace ("/",  "___sl___", name);
  name = filter_string_str_replace (":",  "___co___", name);
  name = filter_string_str_replace ("*",  "___as___", name);
  name = filter_string_str_replace ("?",  "___qu___", name);
  name = filter_string_str_replace ("\"", "___ba___", name);
  name = filter_string_str_replace ("<",  "___sm___", name);
  name = filter_string_str_replace (">",  "___la___", name);
  name = filter_string_str_replace ("|",  "___ve___", name);
  return name;
}


// Take $name, and undo the "clean" function in the above.
string filter_url_filename_unclean (string name)
{
  name = filter_string_str_replace ("___b2___", "\\", name);
  name = filter_string_str_replace ("___sl___", "/",  name);
  name = filter_string_str_replace ("___co___", ":",  name);
  name = filter_string_str_replace ("___as___", "*",  name);
  name = filter_string_str_replace ("___qu___", "?",  name);
  name = filter_string_str_replace ("___ba___", "\"", name);
  name = filter_string_str_replace ("___sm___", "<",  name);
  name = filter_string_str_replace ("___la___", ">",  name);
  name = filter_string_str_replace ("___ve___", "|",  name);
  return name;
}
