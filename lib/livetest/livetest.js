var page = require('webpage').create();
page.open('http://localhost:8080/workspace/index', function(status) {
  console.log("Status: " + status);
  page.render('example.png');
  phantom.exit();
});
