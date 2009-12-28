/*
 ** Copyright (©) 2003-2009 Teus Benschop.
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


#include "ipc.h"


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message,
                                                InterprocessCommunicationChannelType channel, 
                                                InterprocessCommunicationSubjectType subject, 
                                                const ustring& payload)
{
  ustring url = "http://localhost/bibledit/ipc/";
  switch (message) {
    case icmtClearMessages: url.append ("clearmessages.php"); break;
    case icmtStoreMessage:  url.append ("storemessage.php");  break;
    case icmtListen:        url.append ("getmessage.php");    break;
  }
  if (channel != icctNone) {
    url.append ("?channel=");
  }
  switch (channel) {
    case icctNone:                                break;
    case icctXiphos:    url.append ("xiphos");    break;
    case icctBibleTime: url.append ("bibletime"); break;
    case icctBibledit:  url.append ("bibledit");  break;
  }
  if (subject != icstNone) {
    url.append ("&subject=");
  }
  switch (subject) {
    case icstNone:                          break;
    case icstGoto:   url.append ("goto");   break;
    case icstQuit:   url.append ("quit");   break;
    case icstReload: url.append ("reload"); break;
    case icstGetref: url.append ("getref"); break;
  }
  if (!payload.empty()) {
    url.append ("&message=");
    url.append (payload);
  }
  return url;  
}


void interprocess_communication_message_url_add_to_message (ustring& url, const ustring& addition)
// This one adds the "addition" to the "url" as part of the message.
// It ensures that the result is understood by the interprocess communications system.
{
  // Does the url already have the "?".
  bool question_added = false;
  if (url.find ("?") == string::npos) {
    url.append ("?");
    question_added = true;
  }
  // Does the url already have the &message field?
  if (url.find ("&message=") == string::npos) {
    if (!question_added) {
      url.append ("&");
    }
    url.append ("message=");
  }
  // Append the addition to the url.
  url.append (addition);
}

