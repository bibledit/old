<h1>{t}Importing Bible data{/t}</h1>
<p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
<br>
<p>{t}This page assists you with importing USFM data into the Bible.{/t}</p>
<br>
<p>{t}The configuration of the server on which Bibledit-Web runs imposes some limits on the size of the file to be uploaded. The size of the file should not exceed either of the two limits given below:{/t}</p>
<p>{t}Maximum size of a file to be posted:{/t} {$post_max_size}</p>
<p>{t}Maximum size of a file to be uploaded:{/t} {$upload_max_filesize}</p>
<p>{t}The file to be uploaded can be a USFM file, or a compressed archive with USFM files.{/t}</p>
<br>
<form enctype="multipart/form-data" action="import2.php?bible={$bible}" method="POST">
  <p>{t}Choose the USFM file to be uploaded.{/t}</p>
  <input name="data" type="file" />
  <p>{t}Then press the Upload button.{/t}</p>
  <input type="submit" value="{t}Upload{/t}" />
</form>
<p>{t}Uploading may take some time. It depends on the speed of your network, and on the size of the file.{/t}</p>
<br>
<p>{t}If uploading fails, it may be because the file is too large, or the network too slow.{/t}</p>
