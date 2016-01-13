#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>

using namespace std;


struct cstring {
  char *ptr;
  size_t len;
};


void init_string (struct cstring *s) {
  s->len = 0;
  s->ptr = (char *) malloc(s->len+1);
  s->ptr[0] = '\0';
}


size_t writefunc(void *ptr, size_t size, size_t nmemb, struct cstring *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = (char *) realloc (s->ptr, new_len+1);
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;
  return size*nmemb;
}


int main (void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;

  struct cstring s;
  init_string(&s);

  curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_USERNAME, "username@gmail.com");
  curl_easy_setopt(curl, CURLOPT_PASSWORD, "password");

  curl_easy_setopt(curl, CURLOPT_URL, "pop3s://pop.gmail.com:995");

  curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); 
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0); 

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);

  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  //some servers needs this validation
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  } else {
  }

  cout << (char *) s.ptr << endl; // Todo
  if (s.ptr) free (s.ptr);

  curl_easy_cleanup(curl);

  return (int)res;
}

