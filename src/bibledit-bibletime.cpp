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

#include "bibledit-bibletime.h"
#include <glib.h>
#include "ipc.h"
#include "user.h"

#define STAGE_ZERO 0
#define STAGE_COMMUNICATE 1
#define STAGE_WAIT_RETRY 1000
#define STAGE_RETRY 1100
#define BIBLETIME "bibletime"

int stage = STAGE_ZERO;
ustring previous_message;
ustring bibledit_dcop_name;
bool connected = false;
ustring reply;
InterprocessCommunication *ipc;

void main_loop()
{
  switch (stage) {
  case STAGE_ZERO:
    {
      // Check if BibleTime is running.
      if (!program_is_running(BIBLETIME)) {
        log("BibleTime is not running");
        stage = STAGE_WAIT_RETRY;
        break;
      }
      // Check if dcop protocol is working, and get bibletime's name.
      bibledit_dcop_name.clear();
      // Run dcop. We could have used GwSpawn for that, but wish to not link 
      // to that so as to make the binary smaller. Linking to GwSpawn involves
      // linking to so many other functions that come in its trail.
      FILE *stream = popen(dcop(), "r");
      char buf[1024];
      while (fgets(buf, sizeof(buf), stream)) {
        ustring output = buf;
        output = trimstring(output);
        if (output.find(BIBLETIME) != string::npos) {
          bibledit_dcop_name = output;
        }
      }
      pclose(stream);
      // Was bibledit's dcop name found?
      if (bibledit_dcop_name.empty()) {
        log("No DCOP connection possible");
        stage = STAGE_WAIT_RETRY;
        break;
      }
      // Ok, connection is possible.
      log("Connected");
      set_connected(true);
      stage = STAGE_COMMUNICATE;

    }
  case STAGE_COMMUNICATE:
    {
      getreference();
      setreference();
      getmodules();
      search();
      reloadmodules();
      break;
    }
  case STAGE_WAIT_RETRY:
    {
      stage++;
      break;
    }
  case STAGE_RETRY:
    {
      stage = STAGE_ZERO;
      break;
    }
  default:
    {
      stage++;
    }
  }
  // Delay a bit.
  g_usleep(300000);
}

void log(const ustring & message)
{
  if (message != previous_message) {
    if (write(1, message.c_str(), strlen(message.c_str()))) ;
    if (write(1, "\n", 1)) ;
    previous_message = message;
  }
}

const gchar *dcop()
{
  return "dcop";
}

void getreference()
// Get the reference from BibleTime. Send it.
{
  // Only communicate when BibleTime is connected.
  if (connected) {
    // Store BibleTime's reply.
    ustring result;
    // Execute command to elicit a reply.
    ustring command(dcop());
    command.append(dcop_arguments());
    command.append("getCurrentReference");
    FILE *stream = popen(command.c_str(), "r");
    char buf[1024];
    if (fgets(buf, sizeof(buf), stream)) {
      result = trimstring(buf);
    }
    check_exit(pclose(stream));
    // Only proceed if we have a different value than before.
    if (result != reply) {
      reply = result;
      if (!result.empty())
        log("Received " + result);
      // Clean up the reply and see whether we have either a Bible or a Commentary.
      result = trimstring(result);
#define BIBLETIME_BIBLE "[BIBLE]"
#define BIBLETIME_COMMENTARY "[COMMENTARY]"
      size_t offset;
      offset = result.find(BIBLETIME_BIBLE);
      if (offset != string::npos) {
        offset += strlen(BIBLETIME_BIBLE);
      } else {
        offset = result.find(BIBLETIME_COMMENTARY);
        if (offset != string::npos) {
          offset += strlen(BIBLETIME_COMMENTARY);
        }
      }
#undef BIBLETIME_BIBLE
#undef BIBLETIME_COMMENTARY
      // In case of Bible/commentary, transform the reference to our format.
      // Send the result over the IPC, if we got something.
      if (offset != string::npos) {
        result.erase(0, offset);
        result = trimstring(result);
        vector < ustring > payload;
        payload.push_back(result);
        ipc->send(ipcstBibleditBin, ipcctBibleTimeReference, payload);
      }
    }
  }
}

void setreference()
// Send the reference to BibleTime.
{
  // Get possible reference to be sent.
  vector < ustring > reference;
  reference = ipc->get_payload(ipcctBibleTimeReference);
  ipc->erase_payload(ipcctBibleTimeReference);

  // Bail out if there is no valid reference to be sent.
  if (reference.empty())
    return;

  // Bail out if disconnected.
  if (!connected)
    return;

  // Send the reference to BibleTime.
  // Handle case that project is closed, so the book is 0. Solves a crash bug.
  ustring osisreference(reference[0]);
  if (osisreference.find(".") == 0)
    return;
  ustring command(dcop());
  command.append(dcop_arguments());
  command.append("syncAllVerseBasedModules");
  command.append(" " + osisreference);
  check_exit(system(command.c_str()));
  log("Sent: " + osisreference);
}

ustring dcop_arguments()
{
  ustring arguments(" ");
  arguments.append(bibledit_dcop_name);
  arguments.append(" BibleTimeInterface ");
  return arguments;
}

void check_exit(int exitstatus)
{
  if (exitstatus != 0) {
    set_connected(false);
    stage = STAGE_ZERO;
    log("Error while communicating");
  }
}

void getmodules()
{
  // Bail out if disconnected.
  if (!connected)
    return;

  // Look whether the modules are to be fetched. If not, bail out.
  vector < ustring > flag;
  flag = ipc->get_payload(ipcctBibleTimeGetModules);
  ipc->erase_payload(ipcctBibleTimeGetModules);
  if (flag.empty())
    return;

  // Get the Bibles from BibleTime. Send them.
  {
    vector < ustring > bibles;
    ustring message = "Bibles:";
    ustring command(dcop());
    command.append(dcop_arguments());
    command.append("getModulesOfType BIBLES");
    FILE *stream = popen(command.c_str(), "r");
    char buf[1024];
    unsigned int i = 0;
    while (fgets(buf, sizeof(buf), stream)) {
      ustring result = trimstring(buf);
      if (!result.empty()) {
        bibles.push_back(result);
        if (i)
          message.append(",");
        message.append(" " + result);
        i++;
      }
    }
    pclose(stream);
    log(message);
    ipc->send(ipcstBibleditBin, ipcctBibleTimeBibles, bibles);
  }

  // Get the Commentaries from BibleTime. Send them.
  {
    vector < ustring > commentaries;
    ustring message = "Commentaries:";
    ustring command(dcop());
    command.append(dcop_arguments());
    command.append("getModulesOfType COMMENTARIES");
    FILE *stream = popen(command.c_str(), "r");
    char buf[1024];
    unsigned int i = 0;
    while (fgets(buf, sizeof(buf), stream)) {
      ustring result = trimstring(buf);
      if (!result.empty()) {
        commentaries.push_back(result);
        if (i)
          message.append(",");
        message.append(" " + result);
        i++;
      }
    }
    pclose(stream);
    log(message);
    ipc->send(ipcstBibleditBin, ipcctBibleTimeCommentaries, commentaries);
  }
}

void reloadmodules()
// Sends a signal to BibleTime to reload all the modules.
{
  // Bail out if disconnected.
  if (!connected)
    return;

  // Look whether the modules are to be reloaded. If not, bail out.
  vector < ustring > flag;
  flag = ipc->get_payload(ipcctBibleTimeReloadModules);
  ipc->erase_payload(ipcctBibleTimeGetModules);
  if (flag.empty())
    return;

  // Get the Bibles from BibleTime.
  log("Reloading modules");
  ustring command = (dcop());
  command.append(dcop_arguments());
  command.append("reloadModules");
  if (system(command.c_str())) ;
}

void set_connected(bool connect)
// Writes whether BibleTime is connected.
{
  connected = connect;
  vector < ustring > payload;
  payload.push_back(convert_unsigned_int_to_string(connect));
  ipc->send(ipcstBibleditBin, ipcctBibleTimeConnected, payload);
}

void search()
{
  // Bail out if disconnected.
  if (!connected)
    return;

  // Look in the database whether a search has to be made. 
  // If not, bail out.
  vector < ustring > searchcommand;
  searchcommand = ipc->get_payload(ipcctBibleTimeSearchCommand);
  if (searchcommand.empty())
    return;

  // Delete the search commands.
  ipc->erase_payload(ipcctBibleTimeSearchCommand);

  // Assemble the search variables.
  ustring module = searchcommand[0];
  ustring text = searchcommand[1];
  unsigned int selector = convert_string_to_int(searchcommand[2]);

  // Create the command to send to BibleTime in order to search.
  vector < ustring > search_command;
  switch (selector) {
  case 0:
    search_command.push_back("searchInDefaultBible");
    break;
  case 1:
    search_command.push_back("searchInOpenModules");
    break;
  case 2:
    search_command.push_back("searchInModule");
    search_command.push_back(module);
    break;
  }

  // Add searchtext to the command.  
  // Escape certain characters in the text, because it goes through the shell.
  gchar *escaped_text = g_strescape(text.c_str(), "");
  ustring searchtext = escaped_text;
  g_free(escaped_text);
  search_command.push_back(searchtext);

  // Get the search results from BibleTime.
  vector < ustring > search_results;
  ustring command(dcop());
  command.append(dcop_arguments());
  for (unsigned int i = 0; i < search_command.size(); i++) {
    command.append(search_command[i] + " ");
  }
  FILE *stream = popen(command.c_str(), "r");
  char buf[1024];
  if (fgets(buf, sizeof(buf), stream)) {
    ustring result = trimstring(buf);
    if (!result.empty()) {
      search_results.push_back(result);
    }
  }
  pclose(stream);

  // Write these results to the database.
  ipc->send(ipcstBibleditBin, ipcctBibleTimeSearchResults, search_results);

  // Set the flag that the search has been done.
  vector < ustring > searchdone;
  searchdone.push_back(" ");
  ipc->send(ipcstBibleditBin, ipcctBibleTimeSearchDone, searchdone);

  // Log  
  log("Search for " + text + ": " + convert_unsigned_int_to_string(search_results.size()) + " results");
}

bool program_is_running(const gchar * program)
{
  ustring command = "ps -C ";
  command.append(program);
  command.append(" > /dev/null");
  int exitcode = system(command.c_str());
  return (exitcode == 0);
}

ustring trimstring(const ustring s)
{
  if (s.length() == 0)
    return s;
  // Strip spaces, tabs, new lines, carriage returns.
  size_t beg = s.find_first_not_of(" \t\n\r");
  size_t end = s.find_last_not_of(" \t\n\r");
  // No non-spaces  
  if (beg == string::npos)
    return "";
  return ustring(s, beg, end - beg + 1);
}

unsigned int convert_string_to_int(const ustring & str)
{
  unsigned int i = 0;
  istringstream r(str);
  r >> i;
  return i;
}

ustring convert_unsigned_int_to_string(unsigned int i)
{
  ostringstream r;
  r << i;
  return r.str();
}

int main(int argc, char *argv[])
{
  // Do not allow to run as root.
  if (getuid() == 0)
    return 1;

  // Thread support.
  g_thread_init(NULL);

  // IPC system.
  InterprocessCommunication myipc(ipcstBibleditBibletime);
  ipc = &myipc;

  // Delay a bit to allow the main binary to settle.
  g_usleep(1000000);

  // Keep going if we ought to run.
  while (ipc->get_payload(ipcctBibleTimeShutdown).empty()) {
    main_loop();
  }

  // The job's been done.
  return 0;
}
