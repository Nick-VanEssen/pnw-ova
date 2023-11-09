#include <Arduino.h>
#include <ESP_Mail_Client.h>
#include "settings.h"
#include "email.h"

// Declare the global used SMTPSession object for SMTP transport
SMTPSession smtp;

// Callback function to get the Email sending status
void smtpCallback(SMTP_Status status);

void MAILRESULTS::send(){

  //Set the network reconnection option
  MailClient.networkReconnect(true);

  // Enable the debug via Serial port
  // 0 for no debugging
  // 1 for basic level debugging
  // Debug port can be changed via ESP_MAIL_DEFAULT_DEBUG_PORT in ESP_Mail_FS.h
  smtp.debug(1);

  // Set the callback function to get the sending results
  smtp.callback(smtpCallback);

  // Declare the Session_Config for user defined session credentials
  Session_Config config;

  // Set the session config
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  
  // Set the NTP config time
  // For times east of the Prime Meridian use 0-12
  // For times west of the Prime Meridian add 12 to the offset.
  // Ex. American/Denver GMT would be -6. 6 + 12 = 18
  // See https://en.wikipedia.org/wiki/Time_zone for a list of the GMT/UTC timezone offsets
  
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 18; //cst time zone
  config.time.day_light_offset = 0;

  // Declare the message class
  SMTP_Message message;

  // Set the message headers
  message.sender.name = F("OVA Test");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("Test Email");
  message.addRecipient(F("Matt"), RECIPIENT_EMAIL);

  // Send HTML message
  /*String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;*/

  // Send raw text message
  String textMsg = "Hello World! - Sent from the OVA ESP32";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  // Connect to the server
  if (!smtp.connect(&config))
  {
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn())
  {
    Serial.println("\nNot yet logged in.");
  }
  else
  {
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  // Start sending Email and close the session
  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
}

/********** Does this belong in or after void loop()? Not sure, putting it before it for now **********/

// Callback function to get the Email sending status
void smtpCallback(SMTP_Status status)
{
  // Print the current status
  Serial.println(status.info());

  // Print the sending result
  if (status.success())
  {
    // ESP_MAIL_PRINTF used in the examples is for format printing via debug Serial port
    // that works for all supported Arduino platform SDKs e.g. AVR, SAMD, ESP32 and ESP8266.
    // In ESP8266 and ESP32, you can use Serial.printf directly.

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      // Get the result item
      SMTP_Result result = smtp.sendingResult.getItem(i);

      // In case, ESP32, ESP8266 and SAMD device, the timestamp get from result.timestamp should be valid if
      // your device time was synched with NTP server.
      // Other devices may show invalid timestamp as the device time was not set i.e. it will show Jan 1, 1970.
      // You can call smtp.setSystemTime(xxx) to set device time manually. Where xxx is timestamp (seconds since Jan 1, 1970)

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    // You need to clear sending result as the memory usage will grow.
    smtp.sendingResult.clear();
  }
}