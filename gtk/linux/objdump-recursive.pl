#!/usr/bin/perl

###############################################################################
#
# Modified by Tomonobu Saito (Tomonobu.Saito@jp.sony.com)
# 
# Version 1.0 on Nov 10, 2015
#
# Modified by Matt Postiff (postiffm@umich.edu)
# Version 1.1 on March 28, 2016 for bibledit
# Original: https://gist.github.com/Tomonobu3110/963cfc63d8a3b94b55cf
# Usage example: linux/objdump-recursive.pl --uniq "/c/Program Files (x86)/Bibledit-4.9.3/editor/bin/bibledit-gtk.exe" | sort
#
# This program performs recursive objdump checks for binaries and libraries
# It recurses through entire objdump tree for every listed binary and library
# It completes when no matches found in the specified branch (see $root).
# Same limitations to standard objdump apply
# objdump cannot check libraries with no permissions
#
###############################################################################

# Original script :

###############################################################################
#
# Written by Igor Ljubuncic (igor.ljubuncic@intel.com)
#            Yuval Nissan (yuval.nissan@intel.com)
#
# Version 1.0 on Mar 29, 2011
#
# This program performs recursive ldd checks for binaries and libraries
# It recurses through entire ldd tree for every listed binary and library
# It completes when no matches found in the current branch
# Same limitations to standard ldd apply
# ldd cannot check libraries with no permissions
#
###############################################################################

# /*
#
#   This file is provided under a dual BSD/GPLv2 license.  When using or
#   redistributing this file, you may do so under either license.
#
#   GPL LICENSE SUMMARY
#
#   Copyright(c) 2011 Intel Corporation. All rights reserved.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of version 2 of the GNU General Public License as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#   The full GNU General Public License is included in this distribution
#   in the file called LICENSE.GPL.
#
#   Contact Information:
#   Igor Ljubuncic, igor.ljubuncic@intel.com
#   P.O.B 1659, MATAM, 31015 Haifa, Israel
#
#   BSD LICENSE
#
#   Copyright(c) 2011 Intel Corporation. All rights reserved.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     * Neither the name of Intel Corporation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#  */


use strict;
use Data::Dumper;
use Getopt::Long;
use File::Basename;

# global variables
# ----------------

my $result;
my @inputs = ();
my $helpoption=0;
my $debug=0;
my $verbose=0;
my $sep;
my $print_vars;
my $uniq;
my %uniq;

### (saito) Root directory for finding xxx.so
my $root = "/c/Program Files (x86)/Bibledit-4.9.3/editor/bin";

#############################################
#############################################
###                                       ###
###             FUNCTION MAIN             ###
###                                       ###
#############################################
#############################################


GetOptions('h|help+'    => \$helpoption,
           'debug+'     => \$verbose,
           't=s'        => \$sep,
           'l'          => \$print_vars,
           'uniq'       => \$uniq
);

if ($verbose==1) {
        # Print messages for debug purposes
        $debug = 1;
}

if (($helpoption==1) || ($ARGV[0] eq "")) {
    print "\nRecursive ldd v1.00\n";
    print "Written by Igor Ljubuncic (igor.ljubuncic\@intel.com)\n";
    print "Written by Yuval Nissan (yuval.nissan\@intel.com)\n\n";
    print "Usage mode:\n\n";
    print "-d\t\tverbose output\n";
    print "-h|help\t\tprint help\n";
        print "-t\t\tdelimiter\n";
        print "-l\t\tprint env. variables\n";
        print "-uniq\t\tprint unique values only\n\n";
        exit 0;
}

$sep ||= "\t";

push @inputs, @ARGV;

if($print_vars) {
        print "ldd output can be affected by:\n";
        print "\$LD_LIBRARY_PATH = '".$ENV{LD_LIBRARY_PATH}."'\n";
        print "\$LD_PRELOAD = '".$ENV{LD_PRELOAD}."'\n\n";
}

&recurseLibs($inputs[0], 0);
delete $uniq{$inputs[0]};
my $k;
foreach $k (keys %uniq) {
     if ($uniq{$k} == -1) { print "Not found $k\n"; }
  elsif ($uniq{$k} == 1 ) { print "Found     $k\n"; }
  else                    { print "????????? $k\n"; }
}
# This is a more concise way to print, but doesn't take into account
# the coding I added 
#print join("\n", keys(%uniq))."\n" if $uniq;

print "\n";

exit 0;

##############################
##############################
##                          ##
##        FUNCTIONS         ##
##                          ##
##############################
##############################

sub recurseLibs
{
        my $filename=shift;
        my $depth = shift;
        print ">>>>> Working on file: $filename\n" if $debug;
        print "$sep"x$depth if not $uniq;
        ++$depth;
        return if $uniq{$filename} and $uniq;
        $uniq{$filename} = 1;
        print basename($filename) . "\n" if not $uniq;
        chomp(my @libraries = `objdump -x "$filename" | grep "DLL Name"`);
        print "Libraries:\n@libraries\n" if $debug;

        foreach my $line (@libraries) {
                next if not $line;
                $line =~ s/^\s+//g;
                $line =~ s/\s+$//g;
				print $line if $debug;
                # If static or else
                if (($line =~ /statically linked/) or ($line =~ /not a dynamic executable/)) {
                        return;
                }
                elsif($line =~ /not found/) {
                        print "$sep"x$depth if not $uniq;
                        print "$line\n" if not $uniq;
                        $uniq{$line} = 1;
                        next;
                }

                # Split and recurse on libraries (third value is the lib path):
                my @newlibs = split(/\s+/,$line);
                print Dumper(\@newlibs) if $debug;

                # find lib file from $root. $root may have spaces in Windows land, so 
				# needs to be quoted carefully.
                chomp(my @realfile = `/usr/bin/find \"$root\" -name $newlibs[2]`);

                # Skip if no mapped or directly linked
                # Sane output comes with four elements
                if (scalar(@realfile) < 1) {
                        print "$sep"x$depth if not $uniq;
                        print $newlibs[2] . " (not found)\n" if not $uniq;
                        $uniq{$newlibs[2]} = -1; #-1 indicates not found
                        next;
                }

                print "\nRecursive library search on $realfile[0].\n\n" if $debug;
                &recurseLibs($realfile[0], $depth);

        }

        print "<<<<< Working on file: $filename\n" if $debug;
        return;
}
