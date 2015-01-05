/*
** Copyright (©) 2003-2013 Teus Benschop.
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
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/


#include "libraries.h"
#include "utilities.h"
#include "httpd.h"
#include <glib.h>
#include "bible.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "uname.h"
#include "books.h"
#include "settings.h"
#include <errno.h>
#include "directories.h"
#include "tiny_utilities.h"
#include "html.h"


Httpd::Httpd(bool dummy)
// This is a basic webserver tailored to Bibledit's specific needs.
{
  // New g_thread_new ("httpd", GThreadFunc (thread_start), gpointer(this));
  g_thread_create(GThreadFunc(thread_start), gpointer(this), false, NULL);
}

Httpd::~Httpd()
{
  // When the object is destroyed, a flag is set indicating this. The socket 
  // times out, and the webserver sees the flags, and shuts down.
  thread_stop = true;
  // Wait a bit longer than the server's timeout on the non blocking port.
  g_usleep(200000);
  // Just to be sure, and solve a problem of sockets not being closed, which has been 
  // seen randomly, we close the socket and connection here.
#ifdef WIN32
  closesocket(conn);
  closesocket(sock);
#else
  close(conn);
  close(sock);
#endif
}

void Httpd::thread_start(gpointer data)
{
  ((Httpd *) data)->thread_main();
}

void Httpd::thread_main()
{
  // Get the host.
  // Convert the host address to an IP number, possibly looking in DNS.
  struct hostent *host = gethostbyname("localhost");
  if (!host) {
    log(strerror(errno));
    return;
  }
  // Get a socket.
#ifdef WIN32
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == INVALID_SOCKET)
#else
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
#endif
  {
    log(strerror(errno));
    return;
  }
  // Let the kernel reuse the socket address, allowing us to run twice in a row,
  // without waiting for the (ip, port) tuple to time out.  
  const char i = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
  // Initialize IP address and port number.
  struct sockaddr_in address;
  address.sin_family = AF_INET;
#ifdef WIN32
  WSAHtons(sock, HTTPD_PORT, &address.sin_port);
  // Having this bound listening externally seems like a really bad idea.
  // Changed to use loopback for Windows. 
  // Yes, not everyone has a loopback, but they can fix this themselves.
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
  address.sin_port = htons(HTTPD_PORT);
  // On Linux even people from outside can consult the helpfile.
  address.sin_addr.s_addr = INADDR_ANY;
#endif
  memset(&(address.sin_zero), 0, 8);
  // Bind address to socket.
#ifdef WIN32
  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
    closesocket(sock);
#else
  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
    close(sock);
#endif
    log(strerror(errno));
    return;
  }
  // Set socket to non-blocking mode.
#ifdef WIN32
  u_long socketmode = 1; // Set non-blocking
  ioctlsocket(sock, FIONBIO, &socketmode);
#else
  fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
#endif
  // Listen on socket.
#ifdef WIN32
  if (listen(sock, 10) == SOCKET_ERROR) {
    closesocket(sock);
#else
  if (listen(sock, 10) == -1) {
    close(sock);
#endif
    log(strerror(errno));
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
  // it.
  thread_stop = false;
  socklen_t addr_length = sizeof(struct sockaddr_in);
  while (!thread_stop) {
    conn = accept(sock, (struct sockaddr *)&address, &addr_length);
#ifdef WIN32
    if (conn == INVALID_SOCKET) {
      closesocket(conn);
#else
    if (conn < 0) {
      close(conn);
#endif
      g_usleep(50000);
    } else { // accept() worked
      handle_request(conn);
    }
  } // end accept loop
#ifdef WIN32
  closesocket(sock);
#else 
  close(sock);
#endif
}

void Httpd::handle_request(int fd)
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
#ifdef WIN32
  int amount = recv(fd, buf, sizeof(buf), 0);
#else
  int amount = read(fd, buf, sizeof(buf));
#endif
  if (amount > 0) {
    // Put a 0 at the last character, to close the string.
    buf[amount] = '\0';
    // Divide the request into its parts.
    Parse parse(buf, false);
    if (parse.words.size() >= 2) {
      // Get the filename requested, and the command: the part after the ?.
      // Also whether this url should open a capable browser.
      ustring filename(parse.words[1]);
      if (filename.find("_difficult_url_") != string::npos) {
        difficult_url = filename;
        difficult_url.erase(0, difficult_url.find("_difficult_url_") + 15);
        filename = "/";
      }
      if (filename == "/")
        filename = "index.html";
      ustring command;
      size_t question_pos;
      question_pos = filename.find("?");
      if (question_pos != string::npos) {
        command = filename.substr(question_pos + 1, 1000);
        filename.erase(question_pos, filename.length() - question_pos);
        char *command2 = strdup(command.c_str());
        html_url_decode(command2);
        command = command2;
        free(command2);
      }
      {
        // No intrusions: take filename, strip path off, Bibledit's data path.
        filename = gw_build_filename(Directories->get_package_data(), gw_path_get_basename(filename));
        // Decide what action to take.
        if ((gw_path_get_basename(filename) == "search.html") && (!command.empty())) {
          send_search(fd, filename, command);
        } else if ((gw_path_get_basename(filename) == "bibledit_loads_references.html") && (!command.empty())) {
          // E.g.: http://localhost:51516/bibledit_loads_references.html?search-whole-word=word
          if (command.length() >= 18) {
            command.erase(0, 18);
            search_whole_word = command;
          }
          send_file(fd, filename);
        } else {
          send_file(fd, filename);
        }
      }
    }
  }
#ifdef WIN32
  closesocket(fd);
#else
  close(fd);
#endif
}

void Httpd::log(const ustring & message)
{
  gw_message("Webserver: " + message);
}

void Httpd::sendline(int fd, const ustring & line)
{
#ifdef WIN32
  if (send(fd, line.c_str(), strlen(line.c_str()), 0)) ;
  if (send(fd, "\n", 1, 0)) ;
#else
  if (write(fd, line.c_str(), strlen(line.c_str()))) ;
  if (write(fd, "\n", 1)) ;
#endif
}

const char *Httpd::getmimetype(char *name)
{
  // DD: Shouldn't we read this from a system mime.types ?
  struct {
    const char *ext;
    const char *type;
  } mime_table[] = {

    {
    ".html", "text/html"}, {
    ".htm", "text/html"}, {
    ".txt", "text/plain"}, {
    ".rtx", "text/richtext"}, {
    ".etx", "text/x-setext"}, {
    ".tsv", "text/tab-separated-values"}, {
    ".css", "text/css"}, {
    ".xml", "text/xml"}, {
    ".dtd", "text/xml"}, {
    ".gif", "image/gif"}, {
    ".jpg", "image/jpeg"}, {
    ".jpeg", "image/jpeg"}, {
    ".jpe", "image/jpeg"}, {
    ".jfif", "image/jpeg"}, {
    ".tif", "image/tiff"}, {
    ".tiff", "image/tiff"}, {
    ".pbm", "image/x-portable-bitmap"}, {
    ".pgm", "image/x-portable-graymap"}, {
    ".ppm", "image/x-portable-pixmap"}, {
    ".pnm", "image/x-portable-anymap"}, {
    ".xbm", "image/x-xbitmap"}, {
    ".xpm", "image/x-xpixmap"}, {
    ".xwd", "image/x-xwindowdump"}, {
    ".ief", "image/ief"}, {
    ".png", "image/png"}, {
    ".au", "audio/basic"}, {
    ".snd", "audio/basic"}, {
    ".aif", "audio/x-aiff"}, {
    ".aiff", "audio/x-aiff"}, {
    ".aifc", "audio/x-aiff"}, {
    ".ra", "audio/x-pn-realaudio"}, {
    ".ram", "audio/x-pn-realaudio"}, {
    ".rm", "audio/x-pn-realaudio"}, {
    ".rpm", "audio/x-pn-realaudio-plugin"}, {
    ".wav", "audio/wav"}, {
    ".mid", "audio/midi"}, {
    ".midi", "audio/midi"}, {
    ".kar", "audio/midi"}, {
    ".mpga", "audio/mpeg"}, {
    ".mp2", "audio/mpeg"}, {
    ".mp3", "audio/mpeg"}, {
    ".mpeg", "video/mpeg"}, {
    ".mpg", "video/mpeg"}, {
    ".mpe", "video/mpeg"}, {
    ".qt", "video/quicktime"}, {
    ".mov", "video/quicktime"}, {
    ".avi", "video/x-msvideo"}, {
    ".movie", "video/x-sgi-movie"}, {
    ".mv", "video/x-sgi-movie"}, {
    ".vx", "video/x-rad-screenplay"}, {
    ".a", "application/octet-stream"}, {
    ".bin", "application/octet-stream"}, {
    ".exe", "application/octet-stream"}, {
    ".dump", "application/octet-stream"}, {
    ".o", "application/octet-stream"}, {
    ".class", "application/java"}, {
    ".js", "application/x-javascript"}, {
    ".ai", "application/postscript"}, {
    ".eps", "application/postscript"}, {
    ".ps", "application/postscript"}, {
    ".dir", "application/x-director"}, {
    ".dcr", "application/x-director"}, {
    ".dxr", "application/x-director"}, {
    ".fgd", "application/x-director"}, {
    ".aam", "application/x-authorware-map"}, {
    ".aas", "application/x-authorware-seg"}, {
    ".aab", "application/x-authorware-bin"}, {
    ".fh4", "image/x-freehand"}, {
    ".fh7", "image/x-freehand"}, {
    ".fh5", "image/x-freehand"}, {
    ".fhc", "image/x-freehand"}, {
    ".fh", "image/x-freehand"}, {
    ".spl", "application/futuresplash"}, {
    ".swf", "application/x-shockwave-flash"}, {
    ".dvi", "application/x-dvi"}, {
    ".gtar", "application/x-gtar"}, {
    ".hdf", "application/x-hdf"}, {
    ".hqx", "application/mac-binhex40"}, {
    ".iv", "application/x-inventor"}, {
    ".latex", "application/x-latex"}, {
    ".man", "application/x-troff-man"}, {
    ".me", "application/x-troff-me"}, {
    ".mif", "application/x-mif"}, {
    ".ms", "application/x-troff-ms"}, {
    ".oda", "application/oda"}, {
    ".pdf", "application/pdf"}, {
    ".rtf", "application/rtf"}, {
    ".bcpio", "application/x-bcpio"}, {
    ".cpio", "application/x-cpio"}, {
    ".sv4cpio", "application/x-sv4cpio"}, {
    ".sv4crc", "application/x-sv4crc"}, {
    ".sh", "application/x-shar"}, {
    ".shar", "application/x-shar"}, {
    ".sit", "application/x-stuffit"}, {
    ".tar", "application/x-tar"}, {
    ".tex", "application/x-tex"}, {
    ".texi", "application/x-texinfo"}, {
    ".texinfo", "application/x-texinfo"}, {
    ".tr", "application/x-troff"}, {
    ".roff", "application/x-troff"}, {
    ".man", "application/x-troff-man"}, {
    ".me", "application/x-troff-me"}, {
    ".ms", "application/x-troff-ms"}, {
    ".zip", "application/x-zip-compressed"}, {
    ".tsp", "application/dsptype"}, {
    ".wsrc", "application/x-wais-source"}, {
    ".ustar", "application/x-ustar"}, {
    ".cdf", "application/x-netcdf"}, {
    ".nc", "application/x-netcdf"}, {
    ".doc", "application/msword"}, {
    ".ppt", "application/powerpoint"}, {
    ".wrl", "model/vrml"}, {
    ".vrml", "model/vrml"}, {
    ".mime", "message/rfc822"}, {
    ".pac", "application/x-ns-proxy-autoconfig"}, {
    ".wml", "text/vnd.wap.wml"}, {
    ".wmlc", "application/vnd.wap.wmlc"}, {
    ".wmls", "text/vnd.wap.wmlscript"}, {
    ".wmlsc", "application/vnd.wap.wmlscriptc"}, {
    ".wbmp", "image/vnd.wap.wbmp"}, {
    ".tgz", "application/x-gzip"}, {
    ".tar.gz", "application/x-gzip"}, {
    ".bz2", "application/x-bzip2"}, {
    ".zip", "application/zip"}, {
    ".svg", "image/svg+xml"}, {
    ".ico", "image/x-icon"}
  };

  int namelen, extlen;
  unsigned int i;
  namelen = strlen(name);
  for (i = 0; i < sizeof(mime_table) / sizeof(*mime_table); i++) {
    extlen = strlen(mime_table[i].ext);
    if (extlen >= namelen)
      continue;
    if (strcasecmp(name + (namelen - extlen), mime_table[i].ext) == 0)
      return mime_table[i].type;
  }
  return "text/plain";
}

void Httpd::send_content_type(int fd, const ustring & filename)
{
  gchar *fn = g_strdup(filename.c_str());
  ustring content_type = getmimetype(fn);
  g_free(fn);
  content_type.insert(0, "Content-Type: ");
  sendline(fd, content_type);
}

void Httpd::send_file(int fd, const ustring & filename)
{
  if (g_file_test(filename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    sendline(fd, "HTTP/1.1 200 OK");
    send_content_type(fd, filename);
    sendline(fd, "");
    gchar *contents;
    gsize length;
    g_file_get_contents(filename.c_str(), &contents, &length, NULL);
    // If it starts with olh_, then insert a link to the main index in the code. 
    ustring basename = gw_path_get_basename(filename);
    if (g_str_has_prefix(basename.c_str(), "olh_")) {
      ustring s(contents);
      size_t pos = s.find("</body>");
      s.insert(pos, "<p>See also the <a href=\"index.html\">general online help.</a></p>\n");
#ifdef WIN32
      if (send(fd, s.c_str(), strlen(s.c_str()), 0)) ;
    } else {
      if (send(fd, contents, length, 0)) ;
#else
      if (write(fd, s.c_str(), strlen(s.c_str()))) ;
    } else {
      if (write(fd, contents, length)) ;
#endif
    }
    g_free(contents);
  } else {
    send_404(fd);
  }
}

void Httpd::send_404(int fd)
{
  sendline(fd, "HTTP/1.1 404");
  send_content_type(fd, "x.htm");
  sendline(fd, "");
  sendline(fd, "<HTML><BODY><TITLE></TITLE><H2>Not found</H2></BODY></HTML>");
}


void Httpd::send_lines (int fd, const vector <ustring>& lines)
{
  sendline(fd, "HTTP/1.1 200 OK");
  send_content_type(fd, "x.htm");
  sendline(fd, "");
  for (unsigned int i = 0; i < lines.size(); i++) {
    sendline (fd, lines[i]);
  }
}


void Httpd::send_search(int fd, const ustring & filename, const ustring & command)
// Searches for a word in Bibledit's helpfiles.
{
  // Get the word to search for. 
  ustring searchword(command);
  searchword.erase(0, 2);
  // If no search word given, just send the file as it is.
  if (searchword.empty()) {
    send_file(fd, filename);
    return;
  }
  // Read the text, and insert the search results at the appropriate place.
  ReadText rt(filename, true, false);
  sendline(fd, "HTTP/1.1 200 OK");
  send_content_type(fd, filename);
  sendline(fd, "");
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (rt.lines[i].find("</body>") != string::npos) {
      send_search_results(fd, searchword);
    }
    sendline(fd, rt.lines[i]);
  }
}

void Httpd::send_search_results(int fd, ustring searchword)
{
  // Case insensitive search.
  searchword = lowerCase (searchword);
  // Variable whether anything turned up.
  bool succesful = false;
  // Go through all documentation.
  ReadFiles rf (Directories->get_package_data(), "", ".html");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    ustring filename = gw_build_filename (Directories->get_package_data(), rf.files[i]);
    ReadText rt (filename, true, false);
    // Search for the text.
    bool hit = false;
    for (unsigned int i2 = 0; i2 < rt.lines.size(); i2++) {
      ustring line = lowerCase (rt.lines[i2]);
      size_t pos = line.find (searchword);
      if (pos != string::npos) {
        hit = true;
        break;
      }
    }
    if (hit) {
      // Extract the heading from the file
      string title;
      ReadText rt(filename, true, true);
      for (unsigned int i = 0; i < rt.lines.size(); i++) {
        if (title.empty()) {
          if (rt.lines[i].find("<h2") != string::npos) {
            title = rt.lines[i];
            size_t pos = title.find_last_of("<");
            if (pos != string::npos) {
              title.erase(pos, 1000);
            }
            pos = title.find_last_of(">");
            if (pos != string::npos) {
              title.erase(0, ++pos);
            }
          }
        }
      }
      if (title.empty())
        title = "Untitled";
      // Get basename for html linking.
      filename = gw_path_get_basename(filename);
      // Output html code.
      if (filename != "allpages.html") {
        sendline(fd, "<h3><a href=\"" + filename + "\">" + title + "</a></h3>");
        succesful = true;
      }
    }
  }
  // Indicate if nothing was found.
  if (!succesful) {
    sendline(fd, "<p>No search results.</p>");
  }
}

