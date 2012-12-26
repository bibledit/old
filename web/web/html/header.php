<?php
/**
* @package bibledit
*/
/*
 ** Copyright (©) 2003-2012 Teus Benschop.
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */


/**
* Class for creating a html Bible header with breadcrumbs and search box.
*/
class Html_Header
{


  private $htmlText;
 
 
  public function __construct ($htmlText)
  {
    $this->htmlText = $htmlText;
  }
    

  public function create ($breadcrumbs) // Todo
  {
    if (!is_array ($breadcrumbs)) return;
    $tableElement = $this->htmlText->newTable ();
    $tableRowElement = $this->htmlText->newTableRow ($tableElement);
    $tableDataElement = $this->htmlText->newTableData ($tableRowElement);


    $text = "";
    $spanElement = $this->htmlText->newElement ("span");
    $spanElement->nodeValue = htmlspecialchars ($text, ENT_QUOTES, "UTF-8");
    $tableDataElement->appendChild ($spanElement);


    
    $this->htmlText->newParagraph ("breadcrumbs");
    $crumbAdded = false;
    foreach ($breadcrumbs as $breadcrumb) {
      if ($crumbAdded) {
        $this->htmlText->addText ("»");
      }
      $this->htmlText->addLink ($this->htmlText->currentPDomElement, $breadcrumb [1], "", $breadcrumb [0], "", ' ' . $breadcrumb [0] . ' ');
      $crumbAdded = true;
    }
  }

  
}


?>
