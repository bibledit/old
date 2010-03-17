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
#include "settings.h"


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message,
                                                InterprocessCommunicationChannelType channel, 
                                                InterprocessCommunicationSubjectType subject, 
                                                const ustring& payload)
{
  ustring subject_string;
  switch (subject) {
    case icstNone:                                         break;
    case icstGoto:     subject_string.append ("goto");     break;
    case icstQuit:     subject_string.append ("quit");     break;
    case icstReload:   subject_string.append ("reload");   break;
    case icstGetref:   subject_string.append ("getref");   break;
    case icstPause:    subject_string.append ("pause");    break;
    case icstContinue: subject_string.append ("continue"); break;
    case icstFocus:    subject_string.append ("focus");    break;
  }
  return interprocess_communication_message_url (message, channel, subject_string, payload);
}


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message, // Todo
                                                InterprocessCommunicationChannelType channel, 
                                                const ustring& subject, 
                                                const ustring& payload)
{
  extern Settings * settings;
  ustring url = settings->genconfig.bibledit_web_url_get();
  url.append ("/ipc/");
  switch (message) {
    case icmtClearMessages: url.append ("clearmessages.php"); break;
    case icmtStoreMessage:  url.append ("storemessage.php");  break;
    case icmtListen:        url.append ("getmessage.php");    break;
    case icmtSetMessage:    url.append ("setmessage.php");    break;
  }
  url.append ("?channel=");
  switch (channel) {
    case icctNone:                                  break;
    case icctXiphos:     url.append ("xiphos");     break;
    case icctBibleTime:  url.append ("bibletime");  break;
    case icctBibledit:   url.append ("bibledit");   break;
    case icctVcsWorker:  url.append ("vcsworker");  break;
    case icctVcsControl: url.append ("vcscontrol"); break;
  }
  url.append ("&subject=");
  url.append (subject);
  url.append ("&message=");
  url.append (payload);
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

