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


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message)
{
  return interprocess_communication_message_url (message, InterprocessCommunicationRecipientType (0), InterprocessCommunicationSubjectType (0), "");
}


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message, 
                                                InterprocessCommunicationRecipientType recipient, 
                                                InterprocessCommunicationSubjectType subject, const ustring& payload)
{
  ustring url = "http://localhost/bibledit/ipc/";
  switch (message) {
    case icmtClearMessages: 
    {
      url.append ("clearmessages.php");
      return url;
    }
    case icmtStoreMessage: url.append ("storemessage.php");
  }
  url.append ("?recipient=");
  switch (recipient) {
    case icrtXiphos: url.append ("xiphos"); break;
    case icrtBibleTime: url.append ("bibletime"); break;
  }
  url.append ("&subject=");
  switch (subject) {
    case icstGoto: url.append ("goto"); break;
    case icstQuit: url.append ("quit"); break;
  }
  if (!payload.empty()) {
    url.append ("&message=");
    url.append (payload);
  }
  return url;  
}



