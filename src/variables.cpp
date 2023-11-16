#include <Arduino.h>
#include <variables.h>
using namespace std;

variables var;

String getUser()
{
    return var.username;
}
String setUser(String user)
{
    var.username = user;
}

String getHardware()
{
    return var.hardware;
}
String setHardware(String Hardware)
{
    var.hardware = Hardware;
}

String getEmail()
{
    return var.email;
}
String setHardware(String Email)
{
    var.email = Email;
}
