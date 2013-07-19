USE `BIBLEDITDATABASE`;

CREATE TABLE IF NOT EXISTS resources (
  id int auto_increment primary key,
  name varchar (256),
  command varchar (256),
  code text
) engine = MyISAM;

DELETE FROM resources WHERE name = 'default (bibledit)';
INSERT INTO resources VALUES (
NULL, 
'default (bibledit)',
'php script',
'<?php
echo "This is the default resource.";
echo "It just outputs a fixed string.";
?>');

DELETE FROM resources WHERE name = 'resource (bibledit)';
INSERT INTO resources VALUES (
NULL, 
'resource (bibledit)',
'php script',
'<?php
echo "This is resource (bibledit).";
echo "The output does not change.";
?>');
