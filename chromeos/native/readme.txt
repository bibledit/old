Bibledit as a native Chrome app or extension.

There is a Chrome "extension" and a Chrome "app". They are different.

A Chrome extension runs within the Chrome browser. A Chrome app runs on Chrome OS outside of the Chrome browser.

A Chrome extension or app can access the network through defined and limited APIs:
* https://developer.chrome.com/apps/app_network
* https://developer.chrome.com/native-client/pepper_stable/c/struct_p_p_b___t_c_p_socket__1__2
* https://developer.chrome.com/apps/sockets_tcpServer
* https://developer.chrome.com/apps/app_network
This means that for secure communications tlsmbed would need to be updated to use these APIs.

For just now it probably is not worth the effort to create a "native" Bibledit for Chrome or Chrome OS. Google will soon support Android apps on Chrome OS. This is quicker and easier to get Bibledit to Chrome OS natively.

The "extension" folder gives a very basic demo of how Bibledit could look on the surface in Chrome.
