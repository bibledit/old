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


#ifndef INCLUDED_IPC_H
#define INCLUDED_IPC_H


#include "libraries.h"


enum InterprocessCommunicationMessageType {icmtClearMessages, icmtStoreMessage, icmtListen, icmtSetMessage};
enum InterprocessCommunicationChannelType {icctNone, icctXiphos, icctBibleTime, icctBibledit, icctVcsWorker, icctVcsControl};
enum InterprocessCommunicationSubjectType {icstNone, icstGoto, icstQuit, icstReload, icstGetref, icstPause, icstContinue, icstFocus};


ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message, 
                                                InterprocessCommunicationChannelType channel, 
                                                InterprocessCommunicationSubjectType subject, 
                                                const ustring& payload);
ustring interprocess_communication_message_url (InterprocessCommunicationMessageType message, 
                                                InterprocessCommunicationChannelType channel, 
                                                const ustring& subject, 
                                                const ustring& payload);
void interprocess_communication_message_url_add_to_message (ustring& url, const ustring& addition);


#endif
