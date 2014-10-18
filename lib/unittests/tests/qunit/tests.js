
test ("Checksum Receive 1", function () {
  var json = '{"data":"\\\\v Verse 1","checksum":3}';
  var data = checksum_receive (json);
  ok (data == "\\v Verse 1", data);
});

test ("Checksum Receive 2", function () {
  var json = '{"data":"\\\\v Verse 1","checksum":2}';
  var data = checksum_receive (json);
  ok (data == false, data);
});

test ("Checksum Get 1", function () {
  var checksum = checksum_get ("1");
  ok (checksum == 1, checksum);
});

test ("Checksum Get 2", function () {
  var checksum = checksum_get ("1 2 3 DDD");
  ok (checksum == 4, checksum);
});

