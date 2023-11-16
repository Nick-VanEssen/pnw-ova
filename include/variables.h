#pragma once // prevent file from being included more than once
#include <Arduino.h>
using namespace std;

class variables
{
private:
public:
    String username;
    String hardware;
    String email;

    String getUser();
    String setUser(String user);
    String getHardware();
    String setHardware(String Hardware);
    String getEmail();
    String setEmail(String Email);
};
