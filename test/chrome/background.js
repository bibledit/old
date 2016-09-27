// Called when the user clicks on the browser action.
chrome.browserAction.onClicked.addListener(function(tab) {
  // No tabs or host permissions needed!
  console.log('Turning ' + tab.url + ' red!');
  chrome.tabs.executeScript({
    code: 'document.body.style.backgroundColor="red"'
  });
  console.log ("Requesting a wake lock");
  chrome.tabs.executeScript({
    code: 'chrome.power.requestKeepAwake ("display")'
  });
  //chrome.power.requestKeepAwakes ("system");
  chrome.power.requestKeepAwakes ("display");
});
