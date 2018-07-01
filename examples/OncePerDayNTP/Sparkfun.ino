
WiFiClient client;


bool sendSparkfun(byte sparkfunType, String youtubename,int year,  int month, int day, int hour, int minute) {

  // Use Sparkfun testing stream
  const char* host = "data.sparkfun.com";
  const char* streamId   = "0lgRlppWnWu7GyRD7brV";
  const char* privateKey = "D6GZ6jj7k7cMGJRzM1n6";

  Serial.print("Connecting to "); Serial.print(host);

  int retries = 5;
  while (!client.connect(host, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if (!client.connected()) {
    Serial.println("Failed to connect, going back to sleep");
    return false;
  }

  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&youtubename=";
  url += youtubename;
  url += "&year=";
  url += year; 
  url += "&month=";
  url += month; 
  url += "&day=";
  url += day;
  url += "&hour=";
  url += hour; 
  url += "&minute=";
  url += minute;  

  Serial.println();
  Serial.print("sparkfun: ");


  Serial.print("Request URL: "); Serial.println(url);

  client.print(String("GET ") + url +
               " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  int timeout = 5 * 10; // 5 seconds
  while (!client.available() && (timeout-- > 0)) {
    delay(100);
  }

  if (!client.available()) {
    Serial.println("No response, going back to sleep");
    return false;
  }
  Serial.println(F("disconnected"));
  return true;
}
