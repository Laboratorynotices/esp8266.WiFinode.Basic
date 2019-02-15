void HTTP_init(void) {
  HTTP.on("/restart", handle_Restart); // Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok

  // Генерируем данные с датчика А0
  HTTP.on("/data.json", handle_LiveData);

  // Генерируем данные для графика(А0)
  HTTP.on("/char.json", handle_CharData);

  // Генерируем данные с нынешними настройками
  HTTP.on("/liveConfig.json", handle_LiveConfigData);

  // Сохраняет данные конфигурации
  HTTP.on("/updateCongigData", handle_UpdateConfigData);

  // Запускаем HTTP сервер
  HTTP.begin();

}

// Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
void handle_Restart() {
  String restart = HTTP.arg("device");
  if (restart == "ok") ESP.restart();
  HTTP.send(200, "text/plain", "OK");
}


/**
 * Генерация ответа в формате JSON о параметрах датчиков.
 */
void handle_LiveData() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  // Перебираем параметры, что были отправлены вместе с GET-запросом
  for (uint8_t i=0; i<HTTP.args(); i++){
    if (HTTP.argName(i) == "A0") {
      root[HTTP.arg(i)] = mesValues[mesValuesMessedTimes-1];
    } else if (HTTP.argName(i) == "A0min") {
      root[HTTP.arg(i)] = mesValuesMin;
    } else if (HTTP.argName(i) == "A0max") {
      root[HTTP.arg(i)] = mesValuesMax;
    }
  }

  // Сохраняем результат в переменной message
  String message = "";
  root.printTo(message);

  HTTP.send(200, "application/json", message);
}


/**
 * Генерация ответа в формате JSON для графика.
 */
void handle_CharData() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  // Легенда графика
  //String labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sa"};
  JsonArray& labelsData = root.createNestedArray("labels");
  labelsData.copyFrom(labels);

  // Значения
  JsonArray& data1 = root.createNestedArray("series");
  JsonArray& data2 = data1.createNestedArray();
  data2.copyFrom(mesValues);

  // Максимальное и минимальное значения
  root["low"] = mesValuesMin;
  root["high"] = mesValuesMax;

  // Сохраняем результат в переменной message
  String message = "";
  root.printTo(message);

  HTTP.send(200, "application/json", message);
/*
{
  "labels": ["Mon", "Tue", "Wed", "Thu", "Fri", "Sa"],
  "series": [[3, 2, 3, 2, 3, 2]]
}
*/
}


/*
 * Выдаёт данные конфигурации в формате JSON
 */
void handle_LiveConfigData() {
  HTTP.send(200, "application/json", jsonConfig);
}


/*
 * Сохраняет данные настроек.
 * Данные приходят AJAX-запросом со страницы настроек.
 */
void handle_UpdateConfigData() {
  // Перебираем параметры, что были отправлены вместе с POST-запросом
  for (uint8_t i=0; i<HTTP.args(); i++){
    if (HTTP.argName(i) == "timezone") {
      int value = HTTP.arg(i).toInt();
      jsonWrite(jsonConfig, HTTP.argName(i), value);
    } else {
      jsonWrite(jsonConfig, HTTP.argName(i), HTTP.arg(i));
    }
  }

  // Сохраняем результат в файл
  FS_saveConfig(jsonConfig);

  HTTP.send(200, "text/plain", "");
//  HTTP.sendHeader("Location", String("index.htm?site=config"), true);
//  HTTP.send (308, "text/plain", "");
}
