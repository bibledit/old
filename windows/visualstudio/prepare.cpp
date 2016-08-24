#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

string file_get_contents (string filename)
{
    ifstream ifs (filename.c_str(), ios::in | ios::binary | ios::ate);
    streamoff filesize = ifs.tellg();
    ifs.seekg (0, ios::beg);
    vector <char> bytes ((int)filesize);
    ifs.read (&bytes[0], (int)filesize);
    return string (&bytes[0], (int)filesize);
}

void file_put_contents (string filename, string contents)
{
    ofstream file;
    file.open (filename, ios::binary | ios::trunc);
    file << contents;
    file.close ();
}

vector <string> explode (string value, char delimiter)
{
    vector <string> result;
    istringstream iss (value);
    for (string token; getline (iss, token, delimiter); )
    {
        result.push_back (move (token));
    }
    return result;
}

string implode (vector <string>& values, string delimiter)
{
    string full;
    for (vector<string>::iterator it = values.begin (); it != values.end (); ++it)
    {
        full += (*it);
        if (it != values.end ()-1) full += delimiter;
    }
    return full;
}

int main ()
{
    // Load Makefile.am.
    string makefile_am = file_get_contents ("Makefile.am");
    
    // Keep the bit from the Makefile.am with the C and C++ source files.
    string libbibledit_a_sources = "libbibledit_a_SOURCES = \\\n";
    size_t pos = makefile_am.find (libbibledit_a_sources);
    makefile_am.erase (0, pos + libbibledit_a_sources.size ());
    string bin_programs = "bin_PROGRAMS";
    pos = makefile_am.find (bin_programs);
    makefile_am.erase (pos);

    // Remove all white space from the source files.
    while ((pos = makefile_am.find_first_of (" \t\r")) != string::npos) {
        makefile_am.erase (pos, 1);
    }
    while (makefile_am.substr (makefile_am.size () - 1) == "\n") {
        makefile_am.erase (makefile_am.size () - 1, 1);
    }
    
    // Remove backslashes from the source files.
    while ((pos = makefile_am.find_first_of ("\\")) != string::npos) {
        makefile_am.erase (pos, 1);
    }

    // Load server.vcxproj line by line.
    string server_vcxproj = file_get_contents ("server.vcxproj");
    vector <string> server_vcxproj_lines = explode (server_vcxproj, '\n');
    
    // Insert the source files at the correct location.
    for (auto & server_vcxproj_line : server_vcxproj_lines) {
        if (server_vcxproj_line.find ("dummy.cpp") != string::npos) {
            vector <string> sources = explode (makefile_am, '\n');
            for (auto source : sources) {
                server_vcxproj_line.append ("\n");
                server_vcxproj_line.append ("    <ClInclude Include=\"");
                server_vcxproj_line.append (source);
                server_vcxproj_line.append ("\" />");
            }
        }
    }
    
    server_vcxproj = implode (server_vcxproj_lines, "\n");
    file_put_contents ("server.vcxproj", server_vcxproj);
    
    return 0;
}
