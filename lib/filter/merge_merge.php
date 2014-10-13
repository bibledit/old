<?php
/**
 * <pre>
 * Invision Power Services
 * IP.Board v3.1.2
 * Three Way Merge Class
 * Last Updated: $Date: 2009-02-04 20:03:59 +0000 (Wed, 04 Feb 2009) $ BY $author$
 * </pre>
 *
 * @author 		Matt Mecham
 * @copyright	(c) 2001 - 2010 Invision Power Services, Inc.
 * @license		http://www.invisionpower.com/community/board/license.html
 * @package		IP.Board
 * @link		http://www.invisionpower.com
 * @version		$Rev: 3887 $
 *
 */


/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

// The code was downloaded from http://ipb.googlecode.com where it was said to be licensed under the GPLv3.
// The code was subsequently updated.


class ThreeWayMerge
{
	private $_ORIG = array();
	private $_NEW = array();
	private $_CUSTOM = array();
	
	/**
	 * @param	string		Original Text
	 * @param	string		New Text
	 * @param	string		Custom Text
	 */
	public function __construct( $ORIG, $NEW, $CUSTOM)
	{
		$this->_ORIG   = ( ! is_array( $ORIG ) )   ? explode( "\n", str_replace( "\r\n", "\n",  trim( $ORIG   ) ) ) : $ORIG;
		$this->_NEW    = ( ! is_array( $NEW ) )    ? explode( "\n", str_replace( "\r\n", "\n" , trim( $NEW    ) ) ) : $NEW;
		$this->_CUSTOM = ( ! is_array( $CUSTOM ) ) ? explode( "\n", str_replace( "\r\n", "\n" , trim( $CUSTOM ) ) ) : $CUSTOM;
		
    require_once 'merge_diff3.php';
    require_once 'merge_renderer.php';
    require_once 'merge_inline.php';
	}
	
	/**
	 * Merge
	 * @return	string		Merged string
	 */
	 
	public function merge()
	{
    /* Create the Diff object. */
    $diff = new Text_Diff3( $this->_ORIG, $this->_NEW, $this->_CUSTOM );
    
    /* Output the diff in unified format. */
    return $this->_postProcessQuick( implode( "\n", $diff->mergedOutput( 'XXXNEWXXX', 'XXXCUSTOMXXX' ) ) );
	}
	
	/**
	 * Post process merge (quick)
	 *
	 * @access	private
	 * @param	string		Merge text
	 * @return	string		processed text
	 */
	private function _postProcessQuick( $text )
	{
		$count = 0;

		preg_match_all( "#<<<<<<< XXXNEWXXX(.+?)=======(.+?)>>>>>>> XXXCUSTOMXXX#si", $text, $matches );
		
		if ( is_array($matches) AND count($matches) )
		{
			foreach( $matches[1] as $index => $m )
			{	
				$_all	 = $matches[0][$index];
				$_new    = trim( $matches[1][$index], "\n" );
				$_custom = trim( $matches[2][$index], "\n" );
				
				$_new    = "<be:conflict id=\"{$count}\">\n<be:cblock type=\"original\">\n" . $_custom . "\n</be:cblock>\n<ips:cblock type=\"new\">\n" . $_new . "\n</ips:cblock>\n</ips:conflict>";
					
				$text = str_replace( $_all,  $_new, $text );
				
				$count++;
			}
		}
		
		return $text;
	}
	

}
