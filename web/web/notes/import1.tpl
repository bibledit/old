<h1>{t}Importing notes{/t}</h1>
<p>{t}This page assists you with importing Consultation Notes.{/t}</p>
<p>{t}Bibledit-Gtk, part of the Bibledit Suite, has what it calls Project Notes. These can be imported into Bibledit-Web, and will appear there as Consultation Notes.{/t}</p>
<p>{t}When doing this import, it is important to know that this is a one-way operation. It means that, at the time of writing this, notes can be moved from Bibledit-Gtk to Bibledit-Web, but not the other way round.{/t}</p>
<p>{t}Importing the notes into Bibledit-Web, and using these there, has some advantages. For example, users can be notified of changes in the notes.{/t}</p>
<br>
<p>{t}Have you decided that you'd like to move your notes from Bibledit-Gtk to Bibledit-Web? Let's give it a go, then.{/t}</p>
<br>
<p>{t}The first thing to do is to get your notes out of Bibledit-Gtk and to save these to file.{/t}</p>
<p>{t}Start Bibledit-Gtk, and display the project notes window.{/t}</p>
<p>{t}In the menu, choose File, and then Backup. Note that at the time of writing this tutorial, the Export function does not work for Project Notes. Only Backup works.{/t}</p>
<p>{t}Go through the Backup assistant step by step, backup the "Notes", and save them as a file called "notes". Bibledit-Gtk will change this into a file called "notes.tar.gz". Complete the Backup procedure. It may take a while depending on the number of notes you have.{/t}</p>
<p>{t}Locate the file on disk, and see if it looks fine.{/t}</p>
<br>
<p>{t}The next thing is to prepare this notes.tar.gz file for uploading.{/t}</p>
<p>{t}The configuration of the server on which Bibledit-Web runs imposes some limits on the size of the file to be uploaded. The size of the file should not exceed either of the two limits given below:{/t}</p>
<p>{t}Maximum size of a file to be posted:{/t} {$post_max_size}</p>
<p>{t}Maximum size of a file to be uploaded:{/t} {$upload_max_filesize}</p>
<p>{t}The file to be uploaded is a compressed archive. If it is too large, you can remove parts from it. Files that can be removed are "index.sql", and the whole ".git" folder that resides in the "data" folder.{/t}</p>
<p>{t}If it is still too large, you can remove some of the notes data files from it, keep these, and import these through a second upload. The note data files are the files that have numeric names, e.g. "44166759".{/t}</p>
<br>
<p>{t}The next thing is to upload this file to Bibledit-Web.{/t}</p>
<form enctype="multipart/form-data" action="import2.php" method="POST">
  <p>{t}Choose the notes file to be uploaded. This tutorial assumes that the file is "notes.tar.gz".{/t}</p>
  <input name="notes" type="file" />
  <p>{t}Then press the Upload button.{/t}</p>
  <input type="submit" value="{t}Upload{/t}" />
</form>
<p>{t}Uploading may take some time. It depends on the speed of your network, and on the size of the notes file.{/t}</p>
<br>
<p>{t}If uploading fails, it may be because the file is too large, or the network too slow.{/t}</p>
