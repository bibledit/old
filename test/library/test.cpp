/*
 Copyright (©) 2003-2015 Teus Benschop.
 
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


#include <libraries.h>
#include <config.h>


vector <string> filter_url_scandir (string folder)
{
    vector <string> files;
    DIR * dir = opendir (folder.c_str());
    if (dir) {
        struct dirent * direntry;
        while ((direntry = readdir (dir)) != NULL) {
            string name = direntry->d_name;
            if (name.substr (0, 1) == ".") continue;
            if (name == "gitflag") continue;
            files.push_back (name);
        }
        closedir (dir);
    }
    sort (files.begin(), files.end());
    return files;
}




int main (int argc, char **argv)
{
    if (argc) {};
    if (argv[0]) {};
    
    string consultations_in = "/home/2000/consultations";
    string consultations_out = "/home/2000/2000/consultations";
    
    vector <int> identifiers;
    
    vector <string> bits1 = filter_url_scandir (consultations_in);
    for (auto & bit1 : bits1) {
        if (bit1.length () != 3) continue;
        string directory_in = consultations_in + "/" + bit1;
        string directory_out = consultations_out + "/" + bit1;
        mkdir (directory_out.c_str(), 0777);
        vector <string> bits2 = filter_url_scandir (directory_in);
        for (auto & bit2 : bits2) {
            if (bit2.length () != 3) continue;
            string directory_in = consultations_in + "/" + bit1 + "/" + bit2;
            string directory_out = consultations_out + "/" + bit1 + "/" + bit2;
            mkdir (directory_out.c_str(), 0777);
            vector <string> bits3 = filter_url_scandir (directory_in);
            for (auto & bit3 : bits3) {
                if (bit3.length () != 3) continue;
                if (bit3[0] != '0') continue;
                string directory_in = consultations_in + "/" + bit1 + "/" + bit2 + "/" + bit3;
                string directory_out = consultations_out + "/" + bit1 + "/" + bit2 + "/" + bit3;
                mkdir (directory_out.c_str(), 0777);
                int identifier = stoi (bit1 + bit2 + bit3);
                identifiers.push_back (identifier);
                string command = "cp " + directory_in + "/* " + directory_out;
                system (command.c_str ());
                cout << command << endl; // Todo
            }
        }
    }

    cout << identifiers.size () << endl;
    
    
    return EXIT_SUCCESS;
}


