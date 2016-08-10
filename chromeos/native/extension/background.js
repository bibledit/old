chrome.browserAction.onClicked.addListener(function(activeTab){
  var newURL = "http://bibledit.org";
  chrome.tabs.create({ url: newURL });
});