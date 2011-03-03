<h1>{t}Consultations{/t}</h1>
<p>{t}The Notes are an important part of Consultations.{/t}</p>
<p>{t}The notes help to record the discussions with consultants and other people.{/t}</p>
<br>
<p>{t}Normally, when going to the Consultation Notes, you will see a list of notes. It would say, for example, "Displaying notes 1 - 5 / 5". This means that it would display notes 1 to 5, out of 5 notes total.{/t}</p>
<p>{t}You can edit the display. That means that you can edit the number of notes that will display in that list.{/t}</p>
<br>
<p>{t}Clicking "edit" takes you to a new page, called Edit Notes Display. This enables you to "edit" how the notes will be displayed, and how many.{/t}</p>
<br>
<p>{t}The first thing that you can edit is how many notes will be selected.{/t}</p>
<p>{t}If you select notes that refer to any passage, then notes will be selected that refer to any passage, from Genesis to Revelation. But when you select notes that refer to the current verse, then only those notes will be selected that refer to the currently opened verse. In case you have Genesis 2.2 open in the navigator, then only notes that refer to that particular verse are displayed.{/t}</p>
<p>{t}If you select notes that have been edited at any time, then all notes would be selected. But when you select notes that have been edited on "today", then only today's notes are displayed.{/t}</p>
<p>{t}The note selection mechanism combines all selection methods that have been set. If you select notes that refer to the current verse, for example, and that have been edited today, and only those notes that both refer to the current verse, and that also have been edited today, are displayed.{/t}</p>
<p>{t}There are other notes selection mechanisms as well. See the relevant page for all selection mechanisms. Things should be plain and there should be no need for any further information.{/t}</p>
<br>
<p>{t}One thing that would need explanation would be the mechanism that selects notes that contain certain text. This is intended to be used to search notes for text.{/t}</p>
<p>{t}If you select notes that contain "any" text, then searching for text in the note is not done.{/t}</p>
<p>{t}Searching for text can be done by selecting notes that contain specific words or text. When you select this option, you will see that an entry becomes visible. Search text should be entered there. Then the Save button should be pressed. This is to save that search text. Or just pressing Enter while the cursor is in that entry will do also.{/t}</p>
<p>{t}When you search for, for example, "house" (without the quotes), then notes that contain the word "house" will be selected. Notes that contain the word "houses", notice the plural, would not be selected. To select this plural form, you can use wildcards. Instead of entering "house", enter "house*". Notice the asterisk. This will select notes that contains the word "house" or "houses".{/t}</p>
<p>{t}You can also enter two or more words to search for. The search routine will then do its best to select these notes that contain some or all of these words. Notes that contain all words are given first, and notes that contains only one or a few of the words will be given later.{/t}</p>
<p>{t}If you select notes that refer to the current verse, and then also search for some text, then only the notes that refer to the current verse would be searched. It is important to be aware of this. At times one may search for words or phrases, and get zero results, yet he is sure that there should be some results. This would happen because the user does not search the whole Bible, but only the current verse. The same applies to other selection mechanisms. Therefore, when searching notes for text, you may want to set all other selection mechanisms to "any".{/t}</p>
<br>
<p>{t}There is more that can be said about searching notes. The wildcard operator already has been mentioned. Here is a list of possible operators:{/t}</p>

<table>
<tr><td></td><td>{t}By default, when no opeerator specified, the word is optional, but the result that contain it will be rated higher.{/t}</td></tr>
<tr><td>+</td><td>{t}A leading plus sign indicates that this word must be present in every result returned.{/t}</td></tr>
<tr><td>-</td><td>{t}A leading minus sign indicates that this word must not be present in any result returned.{/t}</td></tr>
<tr><td>~</td><td>{t}A leading tilde acts as a negation operator, causing the word's contribution to the result relevance to be negative. It's useful for marking noise words. A result that contains such a word will be rated lower than others, but will not be excluded altogether, as it would be with the minus operator.{/t}</td></tr>
<tr><td>*</td><td>{t}An asterisk is the wildcard operator. Unlike the other operators, it is appended to the word, or fragment, not prepended. This means that if there is a note with the word "house", and you search for "*ouse", this note won't be selected. Yes, this is unusual, but this is how the database search works. To minimize the pain of this, measures have been taken that if you type any part of the word, the expected result will still be found.{/t}</td></tr>
<tr><td>"</td><td>{t}Double quotes at the beginning and end of a phrase, gives only the notes that contain the complete phrase, as it was typed.{/t}</td></tr>
</table>

<br>
<p>{t}Each note can be assigned to somebody. The person to whom the note is assigned then knows that action should be taken on this note. Or the person can be automatically emailed if the note gets changed.{/t}</p>

<br>
<p>{t}The Consultation Notes can partly be operated by email.{/t}</p>
<p>{t}When you receive an email from Bibledit-Web, and this email contains a Consultation Note, then you can reply to this email. Your reply will be added as a comment to this particular Consultation Note.{/t}</p>
<p>{t}The reply should come from the email address which you registered with Bibledit-Web when you created your  account. The reply should also have (CSID123456789) in the subject line. The number 123456789 is a Consultation Note Identifier. It is different for every Note. Just replying to a Consultation Note you received is usually all that is needed.{/t}</p>
<p>{t}You can also submit a new Consultation Note by email.{/t}</p>
<p>{t}This is how to do it:{/t}</p>
<p>{t}Create a new email to the site's email address. The subject line contains "New note", then the passage, and finally the summary.{/t}</p>
<p>{t}For example:{/t}</p>
<p>New note Matthew 1.2 Revisit the word used.</p>
<p>{t}The body of the email will be the first comment on that note.{/t}</p>
