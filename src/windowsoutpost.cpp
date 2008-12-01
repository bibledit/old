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
#include "windowsoutpost.h"
#include <glib/gthread.h>
#include "bible.h"
#include "gwrappers.h"
#include "shell.h"
#include "unixwrappers.h"
#include "uname.h"
#include "books.h"
#include "settings.h"
#include "tiny_utilities.h"

#define STAGE_ZERO 0
#define STAGE_START 1
#define STAGE_CONNECT 2
#define STAGE_COMMUNICATE 3
#define STAGE_WAIT_RETRY 1000
#define STAGE_RETRY 1600

WindowsOutpost::WindowsOutpost(bool dummy)
/*
 This controls a Bibledit Windows Outpost program.
 This is done through connecting to a port on that program, 
 and communicating with it.
 */
{
#ifndef WIN32
  sock = 0;
  clear();
  thread_running = false;
#endif
}

WindowsOutpost::~WindowsOutpost() {
#ifndef WIN32
  // Disconnect.
  disconnect();
  // Indicate to the thread we want to stop.
  thread_run = false;
  // Wait until thread has exited.
  while (thread_running)
    g_usleep(10000);
#endif
}

void WindowsOutpost::Start()
// This effectually starts the whole system.
{
#ifndef WIN32
  thread_run = true;
  g_thread_create (GThreadFunc (thread_start), gpointer (this), false, NULL);
#endif
}

void WindowsOutpost::BibleWorksReferenceSet(const Reference& reference) 
// Schedules a reference to be sent to BibleWorks.
{
#ifndef WIN32
  if (!reference.book)
    return;
  // As it does not respond properly to chapter/verses that are 0, care for that.
  unsigned int goto_ch = reference.chapter;
  if (goto_ch == 0)
    goto_ch = 1;
  ustring goto_vs = reference.verse;
  if (goto_vs == "0")
    goto_vs = "1";
  ustring bw_book = books_id_to_bibleworks(reference.book);
  if (bw_book.empty())
    return;
  bibleworks_reference_set_value = "BibleWorksReferenceSet " + bw_book + " " + convert_to_string(goto_ch) + ":" + goto_vs;
#endif
}

void WindowsOutpost::SantaFeFocusReferenceSet(const Reference& reference)
// Schedules a reference to be sent to the santa fe focus system.
{
#ifndef WIN32
  if (!reference.book)
    return;
  ustring bk = books_id_to_paratext(reference.book);
  if (bk.empty())
    return;
  santafefocus_reference_set_value = "SantaFeFocusReferenceSet " + bk + " " + convert_to_string(reference.chapter) + ":" + reference.verse;
  cout << "SantaFeFocusReferenceSet " << santafefocus_reference_set_value << endl;
#endif
}

void WindowsOutpost::SantaFeFocusWordSet(const ustring& word)
// Schedules a word to be sent to the santa fe focus system.
{
#ifndef WIN32
  santafefocus_word_set_value = "SantaFeFocusWordSet " + word;
#endif
}

#ifndef WIN32

void WindowsOutpost::clear() {
  disconnect();
  stage = STAGE_ZERO;
  connected = false;
}

void WindowsOutpost::thread_start(gpointer data) {
  ((WindowsOutpost*) data)->thread_main();
}

void WindowsOutpost::thread_main() {
  extern Settings * settings;
  thread_running = true;
  while (thread_run) {
    switch (stage)
    {
      case STAGE_ZERO:
      {
        // Next stage.
        stage = STAGE_START;
        break;
      }
      case STAGE_START:
      {
        // If Outpost is networked, skip this step.
        if (settings->genconfig.outpost_networked_get()) {
          stage = STAGE_CONNECT;
          break;
        }
        // Start Outpost if it does not already run.
        if (!program_is_running(BIBLEDIT_WINDOWS_OUTPOST)) {
          // Windows? Or Unix?
          if (uname_get() == untWindows) {
            // We run in Windows: start Outpost and wait a few seconds till started. 
            GwSpawn spawn("bwoutpost.exe");
            spawn.async();
            spawn.run();
            g_usleep(3000000);
          } else {
            // We run on Unix.
            // Is wine there?
            if (gw_find_program_in_path(settings->genconfig.wine_path_get())) {
              ustring command = settings->genconfig.outpost_command_get() + " &";
              system(command.c_str());
              // Wait few seconds to give it a change to start.
              g_usleep(3000000);
            } else {
              log("Wine not installed");
            }
          }
        }
        // Decide on next stage.
        if (program_is_running(BIBLEDIT_WINDOWS_OUTPOST))
          stage = STAGE_CONNECT;
        else
          stage = STAGE_WAIT_RETRY;
        break;
      }
      case STAGE_CONNECT:
      {
        ustring hostname = "localhost";
        if (settings->genconfig.outpost_networked_get())
          hostname = settings->genconfig.outpost_host_get();
        telnet(hostname);
        if (connected) {
          stage = STAGE_COMMUNICATE;
        } else {
          stage = STAGE_WAIT_RETRY;
        }
        break;
      }
      case STAGE_COMMUNICATE:
      {
        // Carry out the scheduled tasks.
        // If BibleWorks does not run, and Outpost tries to contact it, it 
        // crashes in Wine and becomes unusable. Therefore always check whether
        // BibleWorks runs.
        if (!bibleworks_reference_set_value.empty()) {
          if (program_is_running(settings->genconfig.bibleworks_executable_get()) || settings->genconfig.outpost_networked_get()) {
            ustring line (bibleworks_reference_set_value);
            bibleworks_reference_set_value.clear();
            send_line(line);
          }
        }
        if (!santafefocus_reference_set_value.empty()) {
          ustring line (santafefocus_reference_set_value);
          santafefocus_reference_set_value.clear();
          send_line (line);
        }
        if (!santafefocus_word_set_value.empty()) {
          ustring line (santafefocus_word_set_value);
          santafefocus_word_set_value.clear();
          send_line (line);
        }
        /*
         Later: 
         The status of the other programs needs to be collected regularly, e.g. 
         every second, so that the current reference is immediately available on 
         request.
         */
        break;
      }
      case STAGE_WAIT_RETRY:
      {
        stage++;
        break;
      }
      case STAGE_RETRY:
      {
        clear();
        break;
      }
      default:
      {
        stage++;
      }
    }
    g_usleep(300000);
  }
  thread_running = false;
}

void WindowsOutpost::disconnect() {
  if (sock) {
    // Send 'exit' command. This closes the socket and quits the program.
    // In case the Outpost runs on another host, do not quit it, but instead
    // make it visible again.
    extern Settings * settings;
    if (settings->genconfig.outpost_networked_get())
      send_line("show");
    else
      send_line("exit");
    close(sock);
  }
  sock = 0;
}

void WindowsOutpost::telnet(const ustring& hostname) {
  // If the computer address can be converted to an IP, do so. 
  // If not, try to look it up in DNS.
  host = gethostbyname(hostname.c_str());
  if (host) {
    // We got the host, now get a socket.
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) >= 0) {
      // We've got the socket, now set some variables.
      address.sin_family = AF_INET;
      address.sin_port = htons (51515);
      // Take the first IP address associated with this hostname.
      memcpy(&address.sin_addr, host->h_addr_list[0], sizeof (address.sin_addr));
      // Now connect.
      if (connect(sock, (struct sockaddr *) &address, sizeof (address))) {
        log("No connection possible");
        close(sock);
      } else {
        // We've a connection. Set the socket to non-blocking mode.
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
        log("Connected");
        connected = true;
        // Hide GUI.
        send_line("hide");
        // Check for the right version number.
        send_line("version");
        ustring s;
        s = Readln();
        if (connected) {
          if (s.length() > 5) {
            s = s.substr(3, 100);
            if (s != BIBLEDIT_WINDOWS_OUTPOST_VERSION) {
              log("Version is " + s + ", but " + BIBLEDIT_WINDOWS_OUTPOST_VERSION + " is needed");
              log("Running with reduced fuctionality");
            }
          }
        }
      }
    } else {
      log("No socket available");
    }
  } else {
    /* We can't find an IP number */
    log("Error looking up hostname");
  }
}

void WindowsOutpost::log(const ustring & message)
// Logs messages. 
// It does not output the same message repeatedly.
{
  if (message != last_message) {
    gw_message("Windows Outpost: " + message);
    last_message = message;
  }
}

void WindowsOutpost::send_line(const ustring & command)
// Sends "command" to the windows outpost.
{
  int result = 0;
  if (connected) {
    // Discard any previous reply.
    char buf[1024];
    read(sock, buf, sizeof (buf));
    write(sock, command.c_str(), command.length());
    result = write(sock, "\n", 1);
    // Give some time to allow the reply to come back.
    g_usleep(1000000);
  }
  if (result < 0) {
    connected = false;
    perror(NULL);
    log("Error sending data");
    clear();
  }
}

ustring WindowsOutpost::Readln()
/*
 Receives a line from the windows outpost.
 If no data is available, or no complete line is available yet, it returns 
 immediately with a null string.
 */
{

  if (sock) {
    char buf[1024];
    int len = 0;
    len = read(sock, buf, sizeof (buf));
    if (len > 0) {
      buf[len] = '\0';
      return trim(ustring(buf));
    }
    if (len < 0) {
      connected = false;
      perror(NULL);
      log("Error receiving data");
      close(sock);
      clear();
    }
  }
  return "";
}

#endif

void windowsoutpost_open_url(const ustring& url)
// This function supposes that the Bibledit Windows Outpost already runs.
// It commands the outpost to open "url", a .pdf file or .html, etc.
{
#ifndef WIN32
  struct sockaddr_in address;
  struct hostent *host;
  int sock;

  host = gethostbyname("localhost");
  if (host) {
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) >= 0) {
      address.sin_family = AF_INET;
      address.sin_port = htons (51515);
      memcpy(&address.sin_addr, host->h_addr_list[0], sizeof (address.sin_addr));
      if (connect(sock, (struct sockaddr *) &address, sizeof (address))) {
        cout << "Cannot connect" << endl;
        close(sock);
      } else {
        fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
        ustring command = "open " + url + "\n";
        write(sock, command.c_str(), command.length());
        cout << "Outpost opens " << url << endl;
      }
    } else {
      cout << "No socket available" << endl;
    }
  } else {
    cout << "Error looking up hostname" << endl;
  }
#endif
}

bool windowsoutpost_telnet(const ustring& hostname)
// This makes a connection with the Outpost on "host" and then disconnects again.
// It returns true if this worked out.
{
  bool success = false;
#ifndef WIN32
  struct sockaddr_in address;
  struct hostent *host;
  int sock;
  host = gethostbyname(hostname.c_str());
  if (host) {
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) >= 0) {
      address.sin_family = AF_INET;
      address.sin_port = htons (51515);
      memcpy(&address.sin_addr, host->h_addr_list[0], sizeof (address.sin_addr));
      if (connect(sock, (struct sockaddr *) &address, sizeof (address))) {
        close(sock);
      } else {
        success = true;
        close(sock);
      }
    }
  }
#endif
  return success;
}

// Below are some files that show how the SantaFeFocus system works. They were sent
// to me by Nathan Miles, Paratext programmer.


// File SFFocus.ctl
/*
 VERSION 5.00
 Begin VB.UserControl SFFocus 
 ClientHeight    =   1092
 ClientLeft      =   0
 ClientTop       =   0
 ClientWidth     =   1320
 InvisibleAtRuntime=   -1  'True
 ScaleHeight     =   1092
 ScaleWidth      =   1320
 Tag             =   "Test"
 ToolboxBitmap   =   "SFFocus.ctx":0000
 Begin VB.Image Image1 
 Height          =   384
 Left            =   0
 Picture         =   "SFFocus.ctx":0312
 Top             =   0
 Width           =   384
 End
 End
 Attribute VB_Name = "SFFocus"
 Attribute VB_GlobalNameSpace = False
 Attribute VB_Creatable = True
 Attribute VB_PredeclaredId = False
 Attribute VB_Exposed = False
 Option Explicit
 'VBLM OFF
 ' This control can only be used once per project. It will handle sending and
 ' receiving all SantaFeFocus windows messages

 ' Feb 26, 2002 - Created control - Ron Rother

 Public Enum FocusTypes
 ScriptureReferenceFocus = 1      ' A specific scripture reference, optionally including a phrase
 ScriptureReferenceListFocus = 2  ' A list of scripture references
 TopicFocus = 3                   ' A topic category
 WordFocus = 4                    ' A word from a scripture text
 DesktopFocus = 5                 ' A named configuration of windows setup for accomplishing a specific task
 CreateNoteFocus = 6              ' A command to TNE to create a new note for "text, reference"
 DebugFocus = 7
 ShowAnalusisFocus = 8
 ShowVocabulaFocus = 9
 ShowListFileFocus = 10
 CombinedRefListAndRefFocus = 11
 End Enum

 Public Event FocusReceived(ByVal FocusType As FocusTypes, ByVal FocusData As String)
 Public Event F1Pressed()

 'hwnd = 0 terminates and hwnd <> 0 initializes receive focus
 Public Sub InitReceiveFocus(ByVal hWnd As Long)
 Dim OwnerHwnd As Long
 
 If hWnd = 0 Then
 'terminate receive focus
 Call UserControl_Terminate
 Else
 'initialize receive focus
 If g_Hwnd <> 0 Then
 MsgBox "Only one SFFocus control can be used per project"
 Exit Sub
 End If
 'Hook new window (if any)
 If hWnd <> 0 Then
 HookWindow hWnd, Me
 End If
 End If
 End Sub

 'Private Function shiftAndMask(cc As Long, flag As Integer, shift As Integer) As String
 '    Dim result As Long
 '
 '    result = Fix(cc / 2 ^ shift)
 '    result = result And &H3F
 '    result = result + flag
 '
 '    ' nlm
 '    ' The following is a kludge which will not work correctly on Chineese system.
 '    ' It convert and 8 bit value to a unicode value under the assumption
 '    ' that the reverse conversion will be done when VB does RegSetValue.
 '    ' For chinese the reverse conversion will not work.
 '    ' In order to fix this I suppose we will have to add a fnction to some
 '    ' existing COM object to write the UTF value directly into the registry.
 '    shiftAndMask = Chr(result)
 'End Function
 '
 'Private Function ToUTF8(inp As String) As String
 '    Dim outp As String
 '    Dim i As Integer
 '    Dim cc As Long
 '
 '    outp = ""
 '    For i = 1 To Len(inp)
 '        cc = AscW(Mid(inp, i, 1))
 '        If cc <= 127 Then
 '            outp = outp + ChrW(cc)
 '        ElseIf cc <= 2047 Then
 '            outp = outp + shiftAndMask(cc, &HC0, 6) + shiftAndMask(cc, &H80, 0)
 '        Else
 '            outp = outp + shiftAndMask(cc, &HE0, 12) + shiftAndMask(cc, &H80, 6) + shiftAndMask(cc, &H80, 0)
 '        End If
 '    Next
 '
 '    ToUTF8 = outp
 'End Function

 Public Sub SendFocus(ByVal fType As FocusTypes, ByVal FocusData As String)
 Dim obj As New SCTextFile
 
 If fType = WordFocus Then
 ' This is a KLUDGE but I am out of time NLM
 obj.WordInFocus = FocusData
 'Call SFSetFocus(fType, ToUTF8(FocusData))
 Else
 Call SFSetFocus(fType, FocusData)
 If fType = ScriptureReferenceFocus And g_SendContextLibronix Then
 Call SetLibronixFocus(FocusData$)
 End If
 End If
 
 Call SFSignalFocusChange(fType)
 End Sub

 Public Function GetFocus(ByVal fType As FocusTypes) As String
 Dim tfType As Integer
 tfType = CInt(fType)
 GetFocus = SFGetFocus(tfType)
 End Function

 'Invoke WndProc event (called from BAS-module WndProc)
 Friend Function RaiseWndProc(ByVal fType As Long, ByVal FocusData As String) As Long
 RaiseEvent FocusReceived(fType, FocusData)
 End Function

 Friend Sub RaiseF1()
 RaiseEvent F1Pressed
 End Sub

 Private Sub UserControl_Resize()
 Size Image1.Width - 60, Image1.Height - 70
 End Sub

 'Unhook window if still hooked
 Private Sub UserControl_Terminate()
 If g_Hwnd <> 0 Then
 UnhookWindow g_Hwnd
 End If
 End Sub
 'VBLM ON

 'input = GEN 1:1
 Private Sub SetLibronixFocus(FocusData As String)
 Dim lbx As Object
 Dim lbxwndw As Object
 Dim i As Integer
 Dim LibRefString As String
 Dim windnum As Long
 Dim dolibronix As Long
 
 'convert to Libronix format "bible.mat.1.1"
 LibRefString = FormatLibronixRef(FocusData$)
 If LibRefString = "" Then Exit Sub
 
 'libronix may not be running or even installed
 On Error GoTo LibronixError
 
 Set lbx = GetObject(, "LibronixDLS.LbxApplication")
 Set lbxwndw = lbx.Application.windows
 windnum = lbxwndw.count
 For i = 0 To windnum - 1
 'for all the windows open in libronix, check to see if they are
 'resource windows, and more specifically bible type resources. The user
 'can set other reference works to scroll with the bible windows in
 'libronix
 If lbxwndw.Item(i).Type = "resource" Then
 If lbxwndw.Item(i).Info.ActiveDataType = "bible" Then
 lbxwndw.Item(i).Info.GoToReference LibRefString$
 End If
 End If
 Next
 LibronixError:
 'just get out
 End Sub

 'input = "GEN 1:1"
 'return = "bible.1.1.1"
 'return null if invalid reference
 Private Function FormatLibronixRef(FocusData As String) As String
 Dim ref As New SCReference
 Dim booknum As Integer
 
 FormatLibronixRef = ""
 ref.Versification = scEnglish
 ref.Parse FocusData
 If Not ref.Valid Then Exit Function
 
 If ref.Book > 87 Then
 Exit Function
 ElseIf ref.Book < 40 Then
 booknum = ref.Book ' OT is OK
 ElseIf ref.Book > 39 And ref.Book < 67 Then
 booknum = ref.Book + 21 'NT shifted up 21
 ElseIf ref.Book = 67 Then
 booknum = 40 ' Tobit
 ElseIf ref.Book = 68 Then
 booknum = 41 ' Judith
 ElseIf ref.Book = 69 Then
 booknum = 42 ' Additions to Esther
 ElseIf ref.Book = 70 Then
 booknum = 43 ' Wisdom of Solomon
 ElseIf ref.Book = 71 Then
 booknum = 44 ' Sirach
 ElseIf ref.Book = 72 Then
 booknum = 45 ' Baruch
 ElseIf ref.Book = 73 Then
 booknum = 46 ' Letter of Jeremiah
 ElseIf ref.Book = 74 Then
 booknum = 47 ' Song of Three Youths
 ElseIf ref.Book = 75 Then
 booknum = 48 ' Susanna
 ElseIf ref.Book = 76 Then
 booknum = 49 ' Bel and the Dragon
 ElseIf ref.Book = 77 Then
 booknum = 50 ' 1 Maccabees
 ElseIf ref.Book = 78 Then
 booknum = 51 ' 2 Maccabees
 ElseIf ref.Book = 79 Then
 booknum = 55 ' 3 Maccabees
 ElseIf ref.Book = 80 Then
 booknum = 57 ' 4 Maccabees
 ElseIf ref.Book = 81 Then
 booknum = 52 ' 1 Esdras
 ElseIf ref.Book = 82 Then
 booknum = 56 ' 2 Esdras
 ElseIf ref.Book = 83 Then
 booknum = 53 ' Prayer of Manasseh
 ElseIf ref.Book = 84 Then
 booknum = 54 ' Additional Psalm
 ElseIf ref.Book = 85 Then
 booknum = 58 ' Ode
 ElseIf ref.Book = 86 Then
 booknum = 59 ' Psalms of Solomon
 End If
 
 
 FormatLibronixRef = "bible." & CStr(booknum) & "." + CStr(ref.chapter) & "." + CStr(ref.Verse)
 End Function
 */

// File modSFFocus.bas
/*
 Attribute VB_Name = "modSFFocus"
 Option Explicit
 ' The format of the focus reference varies depending on the focus type as follows:
 '
 '     ScriptureReference [this is the format currently used by ESCES]
 '          * A scripture reference may optionally have a scrolling group.
 '            This is a single letter a..z.  Value is case insensitive.
 '            Default is scrolling group A.
 '
 '          * A scripture reference may optionally have a versification
 '            specification with the following form.  Value is case insensitive.
 '                         original (Greek/Hebrew)
 '                         vulgate
 '                         english (e.g. RSV -- this is default if no versification entry present)
 '                         lxx     (septuagint)
 '                         custom  locally defined custom versification
 '
 '          * book chapter:verse [n]phrase
 '                  book = GEN, EXO
 '                  chapter, verse = 1..
 '                     --- from [ on is optional and only present if a specific phrase is specified
 '                         n = 1.. occurence of phrase in verse
 '                         The phrase is terminated by a new line.
 '              "GEN 5:11"
 '              "A vulgate EXO 4:11 [2]latta"
 '
 '          * white space may be freely inserted for readability
 '          * applications may chose to ignore the referenced phrase information
 '            if it is not applicable to their processing.
 ''
 '     ScruptureReferenceList
 '         same as ScriptureReference except that reference can be repeated
 '
 '     Topic
 '         category \n subcategory  (subcategory is optional)
 '              "L\nL0010"
 '     Word
 '         word form \n surface|lemma \n language encoding
 '              "akka\nsurface\nchulupi"
 '
 '     Desktop
 '         name of desktop
 '              "Consultant Check"

 Public Type OSVERSIONINFO
 dwOSVersionInfoSize As Long
 dwMajorVersion As Long
 dwMinorVersion As Long
 dwBuildNumber As Long
 dwPlatformId As Long
 szCSDVersion As String * 128      '  Maintenance string for PSS usage
 End Type
 Public Declare Function GetVersionEx Lib "kernel32" Alias "GetVersionExA" (lpVersionInformation As OSVERSIONINFO) As Long

 Declare Function SetWindowLong Lib "USER32" Alias "SetWindowLongA" _
 (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
 
 Declare Function GetWindowLong Lib "USER32" Alias "GetWindowLongA" _
 (ByVal hWnd As Long, ByVal nIndex As Long) As Long
 
 Declare Function CallWindowProc Lib "USER32" Alias "CallWindowProcA" _
 (ByVal lpPrevWndFunc As Long, ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
 
 Declare Function RegisterWindowMessage& Lib "USER32" Alias "RegisterWindowMessageA" ( _
 ByVal lpstring As String)

 Public Declare Function PostMessage& Lib "USER32" Alias "PostMessageA" ( _
 ByVal hWnd As Long, _
 ByVal wMsg As Long, _
 ByVal wParam As Long, _
 ByVal lParam As Long)

 Public Declare Function RegSetValueA Lib "advapi32.dll" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal dwType As Long, ByVal lpData As String, ByVal cbData As Long) As Long
 Public Declare Function RegQueryValueA Lib "advapi32.dll" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal lpValue As String, lpcbValue As Long) As Long
 Declare Function RegQueryValueW Lib "advapi32.dll" ( _
 ByVal hKey As Long, ByVal lpSubKey As Long, _
 ByVal lpValue As Long, pcbData As Long) As Long

 Declare Function RegSetValueW Lib "advapi32.dll" ( _
 ByVal hKey As Long, ByVal lpSubKey As Long, _
 ByVal dwType As Long, ByVal lpData As Long, _
 ByVal cbData As Long) As Long

 Declare Function FindWindow Lib "USER32" Alias "FindWindowA" ( _
 ByVal classname As String, _
 ByVal winname As String) As Long
 
 Private Const HKEY_CURRENT_USER = &H80000001
 Private Const HWND_BROADCAST = -1
 Private Const GWL_WNDPROC = (-4)
 Public Const WM_NCDESTROY = &H82
 Public Const WM_HELP = &H53

 ' This variable contains the registered message number used to
 ' signal the focus change.
 Private m_FocusMessage As Long
 Private m_OldWndProc As Long
 Private m_ctrl As SFFocus
 Private m_IgnoreNextSFMessage As Boolean

 Public g_Hwnd As Long
 Public Const SFFOCUSMESSAGE = "SantaFeFocus"

 Global g_SendContextLibronix As Boolean

 'Hooks the specified window/control
 Public Function HookWindow(hWnd As Long, Ctrl As SFFocus) As Boolean
 Dim i As Integer
 g_Hwnd = hWnd
 Set m_ctrl = Ctrl
 m_OldWndProc = GetWindowLong(hWnd, GWL_WNDPROC)
 'Install custom window procedure for this window
 SetWindowLong hWnd, GWL_WNDPROC, AddressOf WndProc
 Call RegisterSFMessageIfNecessary
 m_IgnoreNextSFMessage = False
 End Function

 'Unhook the window
 Public Sub UnhookWindow(ByVal hWnd As Long)
 'Reset window hook for this window
 SetWindowLong hWnd, GWL_WNDPROC, m_OldWndProc
 g_Hwnd = 0
 End Sub

 'Call the original window procedure
 Public Function CallWndProc(hWnd As Long, Msg As Long, wParam As Long, lParam As Long) As Long
 CallWndProc = CallWindowProc(m_OldWndProc, hWnd, Msg, wParam, lParam)
 End Function

 ' Set the current focus information of the specified type as specified by
 ' focus argument.
 Public Sub SFSetFocus(ByVal fType As Integer, ByVal focus As String)
 Call RegSetValue(HKEY_CURRENT_USER, SFRegistryKeyName(fType), 1, focus$, Len(focus$))
 End Sub

 ' Signal that the specified focus type has been changed.
 '    Use PostMessage(HWND_BROADCAST, iRegisteredMessage, fType, 0)
 '    to notify all windows (including the hidden window created by this
 '    object) that a focus change of the specified type has occured.
 '    The RegisterWindowMessage(SFFocusMessage) system call will be used to create
 '    a publicly unique but sharable message number to be broadcast when a focus
 '    change occurs.
 Public Sub SFSignalFocusChange(ByVal fType As Integer)
 Call RegisterSFMessageIfNecessary
 m_IgnoreNextSFMessage = True  'don't respond to own postmessage
 Call PostMessage(HWND_BROADCAST, m_FocusMessage, fType, 0)
 End Sub

 ' Return the current focus information of the specified type.
 ' Current this limited to 16000 characters for the reference list focus and 512
 ' characters for all other focus types.
 Public Function SFGetFocus(ByVal fType As Integer) As String
 Dim focus As String
 Dim focuslen As Long
 
 If fType = ScriptureReferenceListFocus Or fType = CreateNoteFocus Or fType = CombinedRefListAndRefFocus Then
 focuslen = 16000
 Else
 focuslen = 512
 End If
 
 focus$ = String(focuslen, " ") 'VBLM SKIP=1
 
 Call RegQueryValue(HKEY_CURRENT_USER, SFRegistryKeyName(fType), focus$, focuslen)
 SFGetFocus = Left$(focus$, focuslen)
 If InStr(SFGetFocus, Chr(0)) Then
 SFGetFocus = Left(SFGetFocus, InStr(SFGetFocus, Chr(0)) - 1)
 End If
 End Function

 'Replacement window procedure--Invokes control handler
 Private Function WndProc(ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
 If Msg = WM_HELP Then
 m_ctrl.RaiseF1
 ElseIf m_FocusMessage = Msg Then
 If m_IgnoreNextSFMessage Then 'don't respond to own postmessage
 m_IgnoreNextSFMessage = False
 Else
 'Suppress unhandled run-time errors
 On Error Resume Next
 'Send message to control
 WndProc = m_ctrl.RaiseWndProc(wParam, SFGetFocus(CInt(wParam)))
 End If
 Else
 'Otherwise, just call default window handler
 WndProc = CallWindowProc(m_OldWndProc, hWnd, Msg, wParam, lParam)
 End If
 'Unhook this window if it is being destroyed
 If Msg = WM_NCDESTROY Then
 Call UnhookWindow(g_Hwnd)
 End If
 End Function

 ' Return the name of the registry key for the specified focus type
 Private Function SFRegistryKeyName(ByVal fType As Integer) As String
 'VBLM OFF
 Select Case fType
 Case ScriptureReferenceFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\ScriptureReference"
 Case ScriptureReferenceListFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\ScriptureReferenceList"
 Case TopicFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\Topic"
 Case WordFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\Word"
 Case DesktopFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\Desktop"
 Case CreateNoteFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\CreateNoteFocus"
 Case DebugFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\DebugFocus"
 Case ShowAnalusisFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\ShowAnalusisFocus"
 Case ShowVocabulaFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\ShowVocabulaFocus"
 Case ShowListFileFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\ShowListFileFocus"
 Case CombinedRefListAndRefFocus
 SFRegistryKeyName$ = "Software\SantaFe\Focus\CombinedRefListAndRefFocus"
 End Select
 'VBLM ON
 End Function

 Private Sub RegisterSFMessageIfNecessary()
 If m_FocusMessage = 0 Then
 m_FocusMessage = RegisterWindowMessage(SFFOCUSMESSAGE)
 End If
 End Sub
 Public Function RegQueryValue(hKey As Long, SubKey As String, _
 Value As String, ValueLength As Long) As Long
 
 If IsW2KorNewer() Then
 RegQueryValue = RegQueryValueW(hKey, StrPtr(SubKey), StrPtr(Value), ValueLength)
 If ValueLength > 1 Then
 ValueLength = ValueLength / 2
 End If
 Else
 RegQueryValue = RegQueryValueA(hKey, SubKey, Value, ValueLength)
 End If
 End Function

 Public Function RegSetValue(hKey As Long, SubKey As String, _
 dataType As Long, data As String, _
 DataLength As Long) As Long

 If IsW2KorNewer() Then
 RegSetValue = RegSetValueW(hKey, StrPtr(SubKey), dataType, StrPtr(data), DataLength)
 If DataLength > 1 Then
 DataLength = DataLength / 2
 End If
 Else
 RegSetValue = RegSetValueA(hKey, SubKey, dataType, data, DataLength)
 End If
 End Function

 ' Determine if this is Win95 or Win97
 Function IsW2KorNewer() As Boolean
 Dim OS As OSVERSIONINFO
 
 OS.dwOSVersionInfoSize = Len(OS)
 Call GetVersionEx(OS)
 IsW2KorNewer = OS.dwMajorVersion > 4
 End Function
 */

// todo start bwoutpost.exe from current directory in windows.
// Make the dialog smaller: no outpost selection, no wine selection, in win32
// Default is off, and on Windows don't use the outpost to start pdf and html, but use the "start" 
// function from mingw.
