/*
 * Соединение с сервером времени
 */
void time_init() {
  configTime(jsonReadInt(jsonConfig, "timezone") * 3600, dst * 0, NTPServer1, NTPServer2);
  Serial.println("\nПодключение к серверу времени.");
  delay(1000);
  for (int i=0; !time(nullptr) && i<10; i++) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");

/*
  time_t now = time(nullptr);
  timeinfo = localtime(&now);
  Serial.println(timeinfo->tm_hour);
  Serial.println(timeinfo->tm_year+1900);
*/
}
