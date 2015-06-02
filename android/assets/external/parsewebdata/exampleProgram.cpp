/*
 ============================================================================
 Name        : exampleProgram.c
 Author      : Cyril Margorin
 Version     :
 Copyright   : Your copyright notice
 Description : Uses shared library to print greeting
 To run the resulting executable the LD_LIBRARY_PATH must be
 set to ${project_loc}/ParseWebData/.libs
 Alternatively, libtool creates a wrapper shell script in the
 build directory of this program which can be used to run it.
 Here the script will be called exampleProgram.
 ============================================================================
 */

#include <ParseWebData.h>

#include <string.h>
#include <iostream>
#include <iomanip>


using namespace std;


int main(int argc, char *argv[]) {
	struct ContentDataPair {
		const char* ct;
		const char* data;
	};
	ContentDataPair testData[] = {
		{ "multipart/form-data; boundary=----WebKitFormBoundaryoSPfJ4Xa1po4HxQA",
				"------WebKitFormBoundaryoSPfJ4Xa1po4HxQA\r\n"
				"Content-Disposition: form-data; name=\"file\"; filename=\"file.txt\"\r\n"
				"Content-Type: text/plain\r\n"
				"\r\n"
				"file-content\r\n"
				"\r\n"
				"------WebKitFormBoundaryoSPfJ4Xa1po4HxQA\r\n"
				"Content-Disposition: form-data; name=\"command\"\r\n"
				"\r\n"
				"cmd\r\n"
				"------WebKitFormBoundaryoSPfJ4Xa1po4HxQA--\r\n" 
		},
		{ "application/x-www-form-urlencoded",
				"file=file-content&command=cmd" 
		}, 
		{ "text/plain",
				"file=file-content\r\n"
				"command=cmd\r\n" 
		}, 
		{ NULL, NULL } 
	};

	for (ContentDataPair *data = testData; data->ct != NULL; ++data) {
		ParseWebData::WebDataMap dataMap;
		ParseWebData::parse_post_data(data->data, data->ct, dataMap);
		cout << "Data with type " << data->ct << endl;
		for (ParseWebData::WebDataMap::const_iterator iter = dataMap.begin();
				iter != dataMap.end(); ++iter) {
			cout << setw(20) << setfill('-') << '-' << endl;
			cout << (*iter).first << " : " << (*iter).second.value << endl;
		}
		cout << setw(40) << setfill('=') << '=' << endl;
	}
	return 0;
}
