// libbibledit.h

#include <stdio.h>
#include <string>

using namespace std;

extern "C"
{
  __declspec(dllexport) void bibledit_root_win(const char *directory);
	__declspec(dllexport) void bibledit_start_win();
  __declspec(dllexport) bool bibledit_running_win();
  __declspec(dllexport) void bibledit_stop_win();
}