//
//  SendSMSForm.java
//  GoBible
//
//	Go Bible is a Free Bible viewer application for Java mobile phones (J2ME MIDP 1.0 and MIDP 2.0).
//	Copyright © 2003-2008 Jolon Faichney.
//	Copyright © 2008-2009 CrossWire Bible Society.
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either version 2
//	of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

import java.io.IOException;
import javax.microedition.io.*;
import javax.microedition.lcdui.*;
import javax.wireless.messaging.*;

/**

	Displays a form that allows the user to specify the verse to send.

**/
public class SendSMSForm extends Form implements CommandListener
{
	public final static int SMS = 0;
	public final static int MMS = 1;

	/** Type is SMS or MMS. **/
	private int type;
	
	private GoBible goBible;
	
	private TextField startVerseTextField;
	private TextField endVerseTextField;
	
	private Command insertCommand = new Command(GoBible.getString("UI-Send-Verses"), Command.OK, 0);
	private Command cancelCommand = new Command(GoBible.getString("UI-Cancel"), Command.CANCEL, 0);
	
	// TextBox used for entering the SMS message
	TextBox sendSMSTextBox;

	// Commands for the TextBox
	private Command okCommand = new Command(GoBible.getString("UI-Send"), Command.OK, 0);
	private Command backCommand = new Command(GoBible.getString("UI-Back"), Command.BACK, 0);

	// TextBox used for entering the phone number
	TextBox phoneNumberTextBox;

	// Commands for the phoneNumberTextBox
	private Command sendCommand = new Command(GoBible.getString("UI-Send"), Command.OK, 0);
	
	private String smsText;
	private	String verseRange;

	
	public SendSMSForm(GoBible goBible, int type)
	{
		super(GoBible.getString(type == SMS ? "UI-Send-SMS" : "UI-Send-MMS"));
	
		this.type = type;
		this.goBible = goBible;
		
		startVerseTextField = new TextField(goBible.bibleSource.getBookName(goBible.currentBookIndex) + " " + (goBible.currentChapterIndex + goBible.bibleSource.getStartChapter(goBible.currentBookIndex)) + ":", "" + (goBible.currentVerseIndex + 1), 3, TextField.NUMERIC);
		
		endVerseTextField = new TextField(GoBible.getString("UI-To") + " ", "" + (goBible.currentVerseIndex + 1), 3, TextField.NUMERIC);
		
		//startVerseTextField.setLayout(Item.LAYOUT_2 | Item.LAYOUT_SHRINK | Item.LAYOUT_VSHRINK);
		//endVerseTextField.setLayout(Item.LAYOUT_2 | Item.LAYOUT_SHRINK | Item.LAYOUT_VSHRINK);
		
		append(new StringItem(null, GoBible.getString("UI-Select-Verses-To-Send") + ":"));
		append(startVerseTextField);
		append(endVerseTextField);
		
		addCommand(insertCommand);
		addCommand(cancelCommand);
		
		setCommandListener(this);
	}
	
	public void commandAction(Command command, Displayable display)
	{
		switch (command.getCommandType())
		{
			case Command.OK:
			{
				if (command == insertCommand)
				{
					int startVerse = Integer.parseInt(startVerseTextField.getString());
					int endVerse = Integer.parseInt(endVerseTextField.getString());
					
					int numberOfVerses = goBible.bibleSource.getNumberOfVerses(goBible.currentBookIndex, goBible.currentChapterIndex);

					// Make sure the verses are in range
					if (startVerse >= 1 && endVerse >= 1 && startVerse <= numberOfVerses && endVerse <= numberOfVerses && endVerse >= startVerse)
					{
						// Create the text string containing the verse data
						
						String smsText = "\"";
						
						for (int i = startVerse; i <= endVerse; i++)
						{
							// Copy all but the control characters into the string
							for (int j = goBible.verseIndex[(i - 1) << 1]; j < goBible.verseIndex[((i - 1) << 1) + 1]; j++)
							{
								if (goBible.verseData[j] > 10)
								{
									smsText += goBible.verseData[j];
								}
							}
							
							smsText += (i != endVerse ? " " : "\"");
						}
						
						//smsText += " (";// + goBible.bibleSource.getBookName(goBible.currentBookIndex) + " " + (goBible.currentChapterIndex + goBible.bibleSource.getStartChapter(goBible.currentBookIndex)) + ":" + startVerse;
						verseRange = goBible.bibleSource.getBookName(goBible.currentBookIndex) + " " + (goBible.currentChapterIndex + goBible.bibleSource.getStartChapter(goBible.currentBookIndex)) + ":" + startVerse;
						
						if (startVerse != endVerse)
						{
							verseRange += "-" + endVerse;
						}
						
						smsText += " (" + verseRange + ").";
						
						try
						{
							sendSMSTextBox = new TextBox(GoBible.getString(type == SMS ? "UI-Send-SMS" : "UI-Send-MMS"), smsText, type == SMS ? 320 : 1024, TextField.ANY);
							
							sendSMSTextBox.addCommand(okCommand);
							sendSMSTextBox.addCommand(backCommand);
							sendSMSTextBox.addCommand(cancelCommand);
							
							sendSMSTextBox.setCommandListener(this);
							
							goBible.display.setCurrent(sendSMSTextBox);
						}
						catch (IllegalArgumentException e)
						{
							goBible.display.setCurrent(new Alert(GoBible.getString(type == SMS ? "UI-Send-SMS" : "UI-Send-MMS"), GoBible.getString("UI-Message-Too-Large-To-Be-Sent") + ".\n" + e.toString(), null, AlertType.CONFIRMATION), this);
						}
					}
					else
					{
						// Display an alert indicating the verses are out of range
						goBible.display.setCurrent(new Alert(GoBible.getString(type == SMS ? "UI-Send-SMS" : "UI-Send-MMS"), GoBible.getString("UI-Verse-Does-Not-Exist") + ".", null, AlertType.CONFIRMATION), this);
					}
				}
				else if (command == okCommand)
				{
					// The sendSMSTextBox is currently displayed so grab the contents and ask for the phone number
					smsText = sendSMSTextBox.getString();
					
					phoneNumberTextBox = new TextBox(GoBible.getString("UI-Phone-Number"), null, 20, TextField.PHONENUMBER);
					
					phoneNumberTextBox.addCommand(sendCommand);
					phoneNumberTextBox.addCommand(backCommand);
					phoneNumberTextBox.addCommand(cancelCommand);
					
					phoneNumberTextBox.setCommandListener(this);
					
					goBible.display.setCurrent(phoneNumberTextBox);
				}
				else if (command == sendCommand)
				{
					// Grab the phone number and send the SMS
					String phoneNumber = phoneNumberTextBox.getString();
					
					try
					{
						if (type == SMS)
						{
							// Create an SMS connection
							MessageConnection connection = (MessageConnection) Connector.open((type == SMS ? "sms" : "mms") + "://" + phoneNumber, Connector.WRITE);

							// Create the SMS message
							TextMessage message = (TextMessage) connection.newMessage(MessageConnection.TEXT_MESSAGE);

							message.setPayloadText(smsText);

							// Determine how many messages will be sent
							int segments = connection.numberOfSegments(message);

							// Send the SMS message
							connection.send(message);

							connection.close();

							// Go back the the main screen
							goBible.showMainScreen();
						}
						else	// MMS message type is created and sent here
						{
							/// Future example of Multipart Message sample to replace existing code
							/*
							 *
							 /**
								*  Sends a multi-part message on the specified connection
								*
								*  @param mc the MessageConnection
								*  @param msgParts the array of message parts to send
								*  @param startContentID is the ID of the start multimedia
								*  content part (SMIL)
								*  @param to is the message's TO list
								*  @param cc is the message's CC list
								*  @param bcc is the message's BCC list
								*  @param subject is the message's subject
								*  @param priority is the message's priority
								*  @param url is the destination URL, typically used in server mode
								*
								final public void sendMultipartMessage(
									MessageConnection mc,
									MessagePart[] msgParts,
									String startContentID,
									String[] to,
									String[] cc,
									String[] bcc,
									String subject,
									String priority,
									String url) {
								try {
									int i=0;
									MultipartMessage multipartMessage;
									multipartMessage = (MultipartMessage)
										mc.newMessage(MessageConnection.MULTIPART_MESSAGE);
									if (to != null) {
										for (i=0; i<to.length; i++) {
											multipartMessage.addAddress("to", to[i]);
										}
									}
									if (cc != null) {
										for (i=0; i<cc.length; i++) {
											multipartMessage.addAddress("cc", cc[i]);
										}
									}
									if (bcc != null) {
										for (i=0; i<bcc.length; i++) {
											multipartMessage.addAddress("bcc", bcc[i]);
										}
									}
									multipartMessage.setSubject(subject);
									if ((priority.equals("high")) ||
										(priority.equals("normal")) ||
										(priority.equals("low"))) {
										multipartMessage.setHeader("X-Mms-Priority", priority);
									}
									for (i=0; i<msgParts.length; i++) {
										multipartMessage.addMessagePart(msgParts[i]);
									}
									multipartMessage.setStartContentId(startContentID);
									sendMessage(mc, multipartMessage, url);
								} catch(Exception e) {
									//  Handle the exception...
								}
							}
							 *
							 * */
							MessageConnection connection = null;
							MultipartMessage mpmsg = null;
							String connUrl = "mms://" + phoneNumber;
							try
							{
								connection = (MessageConnection)Connector.open(connUrl);
							}
							catch (IOException e){}
							catch (SecurityException se) {}

							if (connection == null)
								break;

							try
							{
								MessagePart msgPart = null;
								mpmsg = (MultipartMessage)connection.newMessage(MessageConnection.MULTIPART_MESSAGE);
								String time = String.valueOf(System.currentTimeMillis());
								mpmsg.setHeader("X-Mms-Delivery-Time", time);
								mpmsg.setHeader("X-Mms-Priority", "normal");

								mpmsg.setSubject(verseRange);
								String encoding = "UTF-8";
								byte[] textMsgBytes = smsText.getBytes(encoding);
								msgPart = new MessagePart(textMsgBytes, 0, textMsgBytes.length, "text/plain", "id0", null, encoding);
								mpmsg.addMessagePart(msgPart);
//								connection.send(mpmsg);
							}
							catch(Exception ex){}
							finally
							{
								try
								{
									if (connection != null)
										connection.close();
								}
								catch(Exception ex){}
							}
						}
					}
					catch (java.io.IOException e)
					{
					}
					catch (SecurityException e)
					{
					}
					catch (IllegalArgumentException e)
					{
						goBible.display.setCurrent(new Alert(GoBible.getString(type == SMS ? "UI-Send-SMS" : "UI-Send-MMS"), GoBible.getString("UI-Message-Too-Large-To-Be-Sent") + ".\n" + e.toString(), null, AlertType.CONFIRMATION), this);
					}
					catch (Throwable t)
					{
						// Go back the the main screen
						goBible.showMainScreen();
					}

				}
				break;
			}
			
			case Command.BACK:
			case Command.CANCEL:
			{
				if (command == cancelCommand)
				{
					goBible.showMainScreen();
				}
				else if (command == backCommand)
				{
					if (goBible.display.getCurrent() == sendSMSTextBox)
					{
						// We were in the sendSMSTextBox and we want to go back to this form
						goBible.display.setCurrent(this);
					}
					else if (goBible.display.getCurrent() == phoneNumberTextBox)
					{
						// We were in the phoneNumberTextBox and we want to go back to the sendSMSTextBox
						goBible.display.setCurrent(sendSMSTextBox);
					}
				}
				break;
			}
		}
	}
}
