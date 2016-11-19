#ifndef _CURL_FUNC_H_
#define _CURL_FUNC_H_


#include <string>
#include <io.h>

using namespace std;


int downloadfile(const char *url, char *down_path, int timeout, char *fileName);

#endif