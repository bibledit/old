Porting Bibledit to Chrome OS.

Bibledit is supported on Chrome OS.

Bibledit for Android runs on Chrome OS through ARC Welder.

More information is here:

https://developer.chrome.com/apps/getstarted_arc
https://chrome.google.com/webstore/detail/arc-welder/emfinbmielocnlhgmfkkmkngdoccbadn/

Basically it works like this: 
* Install and start ARC Welder. 
* Open the Bibledit release APK file. 
* Test it. 
* Download a ZIP file within ARC Welder.
* Unpack the ZIP file.
* Open manifest.json
* Add a copy of field “name” and name it "short_name"
* Update field “version”.
* ZIP the directory.
* Upload this ZIP file to the Chrome Web Store via the developer dashboard.

