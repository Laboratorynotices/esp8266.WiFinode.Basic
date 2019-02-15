/*
 * Минимальный проект для ESP8266 с веб-оболочкой.
 * Модуль подключается к WiFi сети или создаёт свою.
 * В процессе работы модуль наблюдает за аналоговым входом на А0,
 * за которым можно наблюдать через веб-оболочку.
 */

#include "set.h"

void setup() {
  Serial.begin(115200);
  Serial.println("");
  //Запускаем файловую систему 
  Serial.println("Подключаемся к файловой системе");
  FS_init();

  Serial.println("Загрузка настройки модуля");
  FS_loadConfig();

  Serial.println("Подключаемся к WIFI-сети");
  //Запускаем WIFI
  WIFIinit();

  // Соединение с серверами времени
  time_init();

  //Настраиваем и запускаем SSDP интерфейс
  Serial.println("Start 3-SSDP");
  SSDP_init();

  //Настраиваем и запускаем HTTP интерфейс
  Serial.println("Start 2-WebServer");
  HTTP_init();

  pinMode(A0, INPUT);
}

void loop() {
  HTTP.handleClient();

  // Замеряем данные с A0
  A0Mesuring();

  delay(1);
}
