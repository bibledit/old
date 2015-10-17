var page = require('webpage').create();
page.open('http://localhost:8080/workbench/index', function(status) {
  console.log("Status: " + status);
  page.render('example.png');
  phantom.exit();
});
