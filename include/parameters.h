#pragma once //prevent file from being included more than once
#include<string>
using namespace std;

class userParameters {
 private:
    String ssid;
    String password;

 public:
 void setSsid(String sid) { ssid = sid; }
  const char* getSsid() { return ssid.c_str(); }

  void setPassword(String pwd) { password = pwd; }
  const char* getPassword() { return password.c_str(); }
};