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


// Basic C headers.
#include <cstdlib>
#ifdef WIN32
#include <io.h>
#include <sys/utime.h>
#include <../dirent/include/dirent.h>
#else
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


// C headers in sub folders.
#include <sys/stat.h>
#ifdef WIN32
#include <ws2tcpip.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#include <sys/types.h>


// C++ headers.
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <algorithm>
#include <set>
#include <chrono>
#include <iomanip>
#include <stdexcept>
#include <thread>


// Headers dependencies.
#ifdef WIN32
#else
#endif
#include <sqlite3.h>


// Bibledit headers.
// Include config.h only where needed in the .cpp files as that speeds up recompilation after increasing the version number.
// #include <config.h> 


using namespace std;


