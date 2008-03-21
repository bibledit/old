/*
** Copyright (©) 2003-2008 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "libraries.h"
#include "utilities.h"
#include "httpd.h"
#include <glib/gthread.h>
#include "bible.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "uname.h"
#include "books.h"
#include "settings.h"
#include <errno.h>
#include "directories.h"


Httpd::Httpd (bool dummy)
// This is a very basic webserver tailored to Bibledit's specific needs.
// It has been written using bits of code from awhttpd.
{
#ifndef WIN32
  g_thread_create (GThreadFunc (thread_start), gpointer (this), false, NULL);
#endif
}


Httpd::~Httpd ()
{
#ifndef WIN32
  // When the object is destroyed, a flag is set indicating this. The socket 
  // times out, and the webserver sees the flags, and shuts down.
  thread_stop = true;
  // Wait a bit longer than the server's timeout on the non blocking port.
  g_usleep (200000);
  // Just to be sure, and solve a problem of sockets not closed that has been 
  // seen randomly, we close the socket and connection here.
  close (conn);
  close (sock);
#endif
}


#ifndef WIN32


void Httpd::thread_start (gpointer data)
{
  ((Httpd*) data)->thread_main();
}


void Httpd::thread_main ()
{
  // Get the host.
  // Convert the host address to an IP number, possibly looking in DNS.
  struct hostent *host = gethostbyname ("localhost");
  if (!host) {
    log (strerror (errno));
    return;
  }
  // Get a socket.
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    log (strerror (errno));
    return;    
  }
  // Let the kernel reuse the socket address, allowing us to run twice in a row,
  // without waiting for the (ip, port) tuple to time out.  
  int i = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
  // Initialize IP address and port number.
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons (HTTPD_PORT);
  address.sin_addr.s_addr = INADDR_ANY;
  memset(&(address.sin_zero), 0, 8);    
  // Bind address to socket.
  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
    close(sock);
    log (strerror (errno));
    return;
  }
  // Set socket to non-blocking mode.
  fcntl (sock, F_SETFL, fcntl (sock, F_GETFL, 0) | O_NONBLOCK);
  // Listen on socket.
  if (listen(sock, 10) == -1) {
    log (strerror (errno));
    close(sock);
    return;
  }
  // Handle connections.
  // At first we had the problem of a hanging server. Bibledit would quit, but
  // the socket would still block waiting for a request to come, and the next 
  // time Bibledit would start, the address would be in use already.
  // This new system solves that. The socket is non-blocking, and if no connection
  // comes it, it runs all over again. This way nothing hangs.
  // Another stab at this problem is given in the object destructor: it closes
  // the two sockets by force, regardless of who's communicating or listening on
  // zit.
  thread_stop = false;
  socklen_t addr_length = sizeof (struct sockaddr_in);
  while (!thread_stop) {
    conn = accept (sock, (struct sockaddr *) &address, &addr_length);
    if (conn >= 0) {
      handle_request (conn);
    } else {
      g_usleep (50000);
    }
  }
  close (sock);
}


void Httpd::handle_request (int fd)
{
  // Read the request from the browser. Example of a typical request:
  // GET / HTTP/1.1
  // Host: localhost:51517
  // User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1b1) Gecko/20060708 Firefox/2.0b1
  // Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5
  // Accept-Language: en-us,en;q=0.5
  // Accept-Encoding: gzip,deflate
  // Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
  // Keep-Alive: 300
  // Connection: keep-alive
  // 
  char buf[1024];
  int amount = read (fd, buf, sizeof (buf));
  if (amount > 0) {
    // Put a 0 at the last character, to close the string.
    buf[amount] = '\0';
    // write (1, buf, amount); // For testing purposes.
    // Divide the request into its parts.
    Parse parse (buf, false);
    if (parse.words.size() >= 2) {
      // Get the filename requested, and the command: the part after the ?.
      ustring filename (parse.words[1]);
      if (filename == "/") filename = "index.html";
      ustring command;
      size_t question_pos;
      question_pos = filename.find ("?");
      if (question_pos != string::npos) {
        command = filename.substr (question_pos + 1, 1000);
        filename.erase (question_pos, filename.length() - question_pos);
        char * command2 = strdup (command.c_str());
        url_decode (command2);
        command = command2;
        free (command2);
      }
      // No intrusions: take filename, strip path off, Bibledit's data path.
      filename = gw_build_filename (directories_get_package_data (), gw_path_get_basename (filename));
      // Decide what action to take.
      if ((gw_path_get_basename (filename) == "search.html") && (!command.empty())) {
        send_search (fd, filename, command);
      } else if ((gw_path_get_basename (filename) == "bibledit_loads_references.html") && (!command.empty())) {
        // E.g.: http://localhost:51516/bibledit_loads_references.html?search-whole-word=word
        if (command.length() >= 18) {
          command.erase (0, 18);
          search_whole_word = command;
        }
        send_file (fd, filename);
      } else {
        send_file (fd, filename);
      }
    }
  }
  close(fd);
}


void Httpd::log (const ustring & message)
{
  gw_message ("Webserver: " + message);
}


void Httpd::sendline (int fd, const ustring& line)
{
  write (fd, line.c_str(), strlen (line.c_str()));
  write (fd, "\n", 1);
}


char * Httpd::getmimetype (char *name) {
  
struct {
  char *ext;
  char *type;
} mime_table[] = {

{ ".html", "text/html" },
{ ".htm", "text/html" },
{ ".txt", "text/plain" },
{ ".rtx", "text/richtext" },
{ ".etx", "text/x-setext" },
{ ".tsv", "text/tab-separated-values" },
{ ".css", "text/css" },
{ ".xml", "text/xml" },
{ ".dtd", "text/xml" },
{ ".gif", "image/gif" },
{ ".jpg", "image/jpeg" },
{ ".jpeg", "image/jpeg" },
{ ".jpe", "image/jpeg" },
{ ".jfif", "image/jpeg" },
{ ".tif", "image/tiff" },
{ ".tiff", "image/tiff" },
{ ".pbm", "image/x-portable-bitmap" },
{ ".pgm", "image/x-portable-graymap" },
{ ".ppm", "image/x-portable-pixmap" },
{ ".pnm", "image/x-portable-anymap" },
{ ".xbm", "image/x-xbitmap" },
{ ".xpm", "image/x-xpixmap" },
{ ".xwd", "image/x-xwindowdump" },
{ ".ief", "image/ief" },
{ ".png", "image/png" },
{ ".au", "audio/basic" },
{ ".snd", "audio/basic" },
{ ".aif", "audio/x-aiff" },
{ ".aiff", "audio/x-aiff" },
{ ".aifc", "audio/x-aiff" },
{ ".ra", "audio/x-pn-realaudio" },
{ ".ram", "audio/x-pn-realaudio" },
{ ".rm", "audio/x-pn-realaudio" },
{ ".rpm", "audio/x-pn-realaudio-plugin" },
{ ".wav", "audio/wav" },
{ ".mid", "audio/midi" },
{ ".midi", "audio/midi" },
{ ".kar", "audio/midi" },
{ ".mpga", "audio/mpeg" },
{ ".mp2", "audio/mpeg" },
{ ".mp3", "audio/mpeg" },
{ ".mpeg", "video/mpeg" },
{ ".mpg", "video/mpeg" },
{ ".mpe", "video/mpeg" },
{ ".qt", "video/quicktime" },
{ ".mov", "video/quicktime" },
{ ".avi", "video/x-msvideo" },
{ ".movie", "video/x-sgi-movie" },
{ ".mv", "video/x-sgi-movie" },
{ ".vx", "video/x-rad-screenplay" },
{ ".a", "application/octet-stream" },
{ ".bin", "application/octet-stream" },
{ ".exe", "application/octet-stream" },
{ ".dump", "application/octet-stream" },
{ ".o", "application/octet-stream" },
{ ".class", "application/java" },
{ ".js", "application/x-javascript" },
{ ".ai", "application/postscript" },
{ ".eps", "application/postscript" },
{ ".ps", "application/postscript" },
{ ".dir", "application/x-director" },
{ ".dcr", "application/x-director" },
{ ".dxr", "application/x-director" },
{ ".fgd", "application/x-director" },
{ ".aam", "application/x-authorware-map" },
{ ".aas", "application/x-authorware-seg" },
{ ".aab", "application/x-authorware-bin" },
{ ".fh4", "image/x-freehand" },
{ ".fh7", "image/x-freehand" },
{ ".fh5", "image/x-freehand" },
{ ".fhc", "image/x-freehand" },
{ ".fh", "image/x-freehand" },
{ ".spl", "application/futuresplash" },
{ ".swf", "application/x-shockwave-flash" },
{ ".dvi", "application/x-dvi" },
{ ".gtar", "application/x-gtar" },
{ ".hdf", "application/x-hdf" },
{ ".hqx", "application/mac-binhex40" },
{ ".iv", "application/x-inventor" },
{ ".latex", "application/x-latex" },
{ ".man", "application/x-troff-man" },
{ ".me", "application/x-troff-me" },
{ ".mif", "application/x-mif" },
{ ".ms", "application/x-troff-ms" },
{ ".oda", "application/oda" },
{ ".pdf", "application/pdf" },
{ ".rtf", "application/rtf" },
{ ".bcpio", "application/x-bcpio" },
{ ".cpio", "application/x-cpio" },
{ ".sv4cpio", "application/x-sv4cpio" },
{ ".sv4crc", "application/x-sv4crc" },
{ ".sh", "application/x-shar" },
{ ".shar", "application/x-shar" },
{ ".sit", "application/x-stuffit" },
{ ".tar", "application/x-tar" },
{ ".tex", "application/x-tex" },
{ ".texi", "application/x-texinfo" },
{ ".texinfo", "application/x-texinfo" },
{ ".tr", "application/x-troff" },
{ ".roff", "application/x-troff" },
{ ".man", "application/x-troff-man" },
{ ".me", "application/x-troff-me" },
{ ".ms", "application/x-troff-ms" },
{ ".zip", "application/x-zip-compressed" },
{ ".tsp", "application/dsptype" },
{ ".wsrc", "application/x-wais-source" },
{ ".ustar", "application/x-ustar" },
{ ".cdf", "application/x-netcdf" },
{ ".nc", "application/x-netcdf" },
{ ".doc", "application/msword" },
{ ".ppt", "application/powerpoint" },
{ ".wrl", "model/vrml" },
{ ".vrml", "model/vrml" },
{ ".mime", "message/rfc822" },
{ ".pac", "application/x-ns-proxy-autoconfig" },
{ ".wml", "text/vnd.wap.wml" },
{ ".wmlc", "application/vnd.wap.wmlc" },
{ ".wmls", "text/vnd.wap.wmlscript" },
{ ".wmlsc", "application/vnd.wap.wmlscriptc" },
{ ".wbmp", "image/vnd.wap.wbmp" },
{ ".tgz", "application/x-gzip" },
{ ".tar.gz", "application/x-gzip" },
{ ".bz2", "application/x-bzip2" },
{ ".zip", "application/zip" },
{ ".svg", "image/svg+xml" },
{ ".ico", "image/x-icon" }
};
  
  int namelen, extlen;
  unsigned int i;
  namelen = strlen(name);
  for (i = 0; i < sizeof (mime_table) / sizeof (*mime_table); i++) {
    extlen = strlen(mime_table[i].ext);
    if (extlen >= namelen) continue;
    if (strcasecmp(name+(namelen-extlen), mime_table[i].ext) == 0)
      return mime_table[i].type;
  }
  return "text/plain";
}


void Httpd::send_content_type (int fd, const ustring& filename)
{
  gchar * fn = g_strdup (filename.c_str());
  ustring content_type = getmimetype (fn);
  g_free (fn);
  content_type.insert (0, "Content-Type: ");
  sendline (fd, content_type);
}


void Httpd::send_file (int fd, const ustring& filename)
{
  if (g_file_test (filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sendline (fd, "HTTP/1.1 200 OK");
    send_content_type (fd, filename);
    sendline (fd, "");
    gchar *contents;
    gsize  length;
    g_file_get_contents (filename.c_str(), &contents, &length, NULL);
    // If it starts with olh_, then insert a link to the main index in the code. 
    ustring basename = gw_path_get_basename (filename);    
    if (g_str_has_prefix (basename.c_str(), "olh_")) {
      ustring s (contents);
      size_t pos = s.find ("</body>");
      s.insert (pos, "<p>See also the <a href=\"index.html\">general online help.</a></p>\n");
      write (fd, s.c_str(), strlen (s.c_str()));
    } else {
      write (fd, contents, length);
    }
    g_free (contents);
  } else {
    send_404 (fd);
  }
}


void Httpd::send_404 (int fd)
{
  sendline (fd, "HTTP/1.1 404");
  send_content_type (fd, "x.htm");
  sendline (fd, "");
  sendline (fd, "<HTML><BODY><TITLE></TITLE><H2>Not found</H2></BODY></HTML>");
}


void Httpd::send_search (int fd, const ustring& filename, const ustring& command)
// Searches for a word in Bibledit's helpfiles.
{
  // Get the word to search for. 
  ustring searchword (command);
  searchword.erase (0, 2);
  // If no search word given, just send the file as it is.
  if (searchword.empty()) {
    send_file (fd, filename);
    return;
  }
  // Read the text, and insert the search results at the appropriate place.
  ReadText rt (filename, true, false);
  sendline (fd, "HTTP/1.1 200 OK");
  send_content_type (fd, filename);
  sendline (fd, "");
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find ("</body>") != string::npos) {
      send_search_results (fd, searchword);
    }
    sendline (fd, rt.lines[i]);
  }
}


void Httpd::send_search_results (int fd, const ustring& searchword)
{
  // Assemble grep command.
  ustring command = "grep -i -l";
  command.append (shell_quote_space (searchword));
  command.append (gw_build_filename (directories_get_package_data (), "*.html")); // todo use a ReadFiles function, and add all arguments to spawn.arg(). Everywhere.
  // Run grep and output results.
  FILE *stream;
  stream = popen (command.c_str (), "r");
  char buf[1024];
  bool succesfull = false;
  while (fgets (buf, sizeof (buf), stream)) {
    // Read a filename from grep.
    ustring filename = buf;
    filename.erase (filename.length() - 1, 1);
    // Extract the heading from the file
    string title = "Untitled";
    ifstream in (filename.c_str());
    string s;
    while (getline (in, s)) {
      if (s.find ("<h2") != string::npos) {
        size_t pos2 = s.find (">");
        pos2++;
        title = s.substr (pos2, s.length() - pos2 - 5);
        break;
      }
    }
    // Get basename for html linking.
    filename = gw_path_get_basename (filename);
    // Output html code.
    if (filename != "allpages.html") {
      sendline (fd, "<h3><a href=\"" + filename + "\">" + title + "</a></h3>");
      succesfull = true;
    }
  }
  pclose (stream);
  // Indicate if nothing was found.
  if (!succesfull) {
    sendline (fd, "<p>No search results.</p>");
  }
}


int Httpd::hexit(char c)
{
  if ( c >= '0' && c <= '9' )
    return c - '0';
  if ( c >= 'a' && c <= 'f' )
    return c - 'a' + 10;
  if ( c >= 'A' && c <= 'F' )
    return c - 'A' + 10;
  return 0;
}


void Httpd::url_decode (char *buf)
// Decode string %xx -> char (in place)
{
  int v;
  char *p, *s, *w;

  w=p=buf;
  while (*p) {
    v=0;

    if (*p=='%') {
      s=p;
      s++;

      if (isxdigit((int) s[0]) && isxdigit((int) s[1]) ) {
        v=hexit(s[0])*16+hexit(s[1]);
        if (v) { /* do not decode %00 to null char */
          *w=(char)v;
          p=&s[1];
        }
      }

    }

    if (!v) *w=*p;
    p++; w++;
  }
  *w='\0';
}


#endif
