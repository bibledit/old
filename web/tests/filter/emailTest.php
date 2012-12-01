<?php

 
class filterEmailTest extends PHPUnit_Framework_TestCase
{

  public function testFilterEmailExtractEmail()
  {
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("Foo Bar <foo@bar.eu>"));
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("<foo@bar.eu>"));
    $this->assertEquals("foo@bar.eu", Filter_Email::extractEmail ("foo@bar.eu"));
  }

  public function testFilterEmailExtractBody()
  {
$body = <<<EOD
body
EOD;
    $this->assertEquals("body", Filter_Email::extractBody ($body));
$body = <<<EOD

test

On Wed, 2011-03-02 at 08:26 +0100, Bibledit-Web wrote:

> test notes three


> test

On Wed, 2011-03-02 at 08:26 +0100, Bibledit-Web wrote:

>    test notes three 
EOD;
    $this->assertEquals("test", Filter_Email::extractBody ($body, "2011", "Bibledit-Web"));

  }


  
}
?>