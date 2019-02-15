/*
 * Каждые A0MeasurePeriod миллисекунд измеряем значение A0 и сохраняем данные.
 */
void A0Mesuring() {
  // Измерение значения А0 раз в A0MeasurePeriod миллисекунд
  if (A0MeasureTimer <= millis()) {
    // Делаем новую заметку, когда надо сделать замер в следующий раз
    A0MeasureTimer += A0MeasurePeriod;

    // Надо ли в новом элементе писать легенду.
    bool writeLabel = false;
    int legendWriteEachElement = mesValuesSize/(labelsQuantity-1);

    // Вначале заполняем массив с нулевой позиции, когда массив будет заполнен, начнём сдвигать старые значения
    if (mesValuesMessedTimes == mesValuesSize) {
      // Массив уже был заполнен, кол-во сделанных замеров больше длины массива

      // Если в первом элементе, который будет удаляться, было что-то написано,
      // то в новом (последнем) элементе мы запишем новое значение.
      writeLabel = labels[mesValuesSize-legendWriteEachElement] != "";

      // Сдвигаем значения в массиваx
      for (int i = 0; i < mesValuesSize-1; i++) {
        // Результаты замеров
        mesValues[i] = mesValues[i+1];
        // Легенда
        labels[i] = labels[i+1];
      }
    } else {
      // Массив ещё не заполнен

      // Когда на графике показано слишком много замеров, то надписи легенды сливаются.
      // Следовательно мы сделаем 5 записей в легенде, остальное будет пустым.
      writeLabel = (mesValuesMessedTimes%legendWriteEachElement) == 0;

      // Сдвигаем верхнюю границы "заполнености"
      mesValuesMessedTimes++;
    }

    // В последнем блоке сохраняем новый результат измерения
    mesValues[mesValuesMessedTimes-1] = analogRead(A0);
    // И легенду
    time_t now = time(nullptr);
    struct tm * timeinfo = localtime(&now);
    String timeString = writeLabel ?
                          (String)timeinfo->tm_hour+":"+(String)timeinfo->tm_min :
                          "";
    labels[mesValuesMessedTimes-1] = timeString;


    // Сохраняем минимальное и максимальное значения замеров
    if (mesValuesMessedTimes == 1) {
      // При первом проходе первый результат замера - минимальный и максимальный.
      mesValuesMin = mesValues[0];
      mesValuesMax = mesValuesMin;
    } else {
      // Если это не первый замер, то есть уже с чем сравнивать.

      // Сравниваем минимальное значение с новым
      if (mesValues[mesValuesMessedTimes-1] < mesValuesMin) {
        mesValuesMin = mesValues[mesValuesMessedTimes-1];
      }

      // Сравниваем максимальное значение с новым
      if (mesValues[mesValuesMessedTimes-1] > mesValuesMax) {
        mesValuesMax = mesValues[mesValuesMessedTimes-1];
      }
    }
  }
}


/** Блок работы с файловой системой **/
/*
 * Загрузка настроек из файловой системы.
 * (Таким образом настройки должны не сбрасываться при выключении.)
 * При первом включении эти настройки сохраняются на диске.
 */
void FS_loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/"+(String)configFileName, "r");

  if (!configFile) {
    // Такого файла пока ещё нет.
    Serial.println("Не удалось открыть файл с настройками");
    // Сохраняем данные по умолчанию
    FS_saveDefaultConfig();
    return;
  }

/* Зачем?
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return;
  }
*/

  // загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  configFile.close();
  return;
}


/*
 * Загружает настройки по умолчанию и сохраняет их в конфигурационный файл.
 */
void FS_saveDefaultConfig() {
  // Создаём JSON-объект
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  // Настройки WiFi
  root["wifiName"] = default_wifiName;
  root["wifiPassword"] = default_wifiPassword;

  // Настройки WiFi в режиме AP
  root["APwifiName"] = default_APwifiName;
  root["APwifiPassword"] = default_APwifiPassword;
  
  // Настройка часов
  root["timezone"] = default_timezone;

  // Сохраняем результат
  jsonConfig = "";
  root.printTo(jsonConfig);

  // Сохраняем результат в файл
  FS_saveConfig(jsonConfig);
}


/*
 * Перезаписывает строчку configSetup в файл конфигурации.
 */
bool FS_saveConfig (String configSetup){
//  return true;
  return FS_writeFile(configFileName, configSetup); //  @TODO
}


/*
 * Сохраняет строчку strings в файле fileName.
 * Если его не было, то создаёт.
 * Если он был, то переписывает.
 */
bool FS_writeFile(String fileName, String strings ) {
  File openedFile = SPIFFS.open("/" + fileName, "w");
  if (!openedFile) {
    return false;
  }
  openedFile.print(strings);
  openedFile.close();
  return true;
}




/*
 * Берёт значение из поля name
 */
String jsonRead(String &json, String key) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  return root[key];
//  return root[name].as<String>();
}


/*
 * Берёт значение из поля name
 */
int jsonReadInt(String &json, String key) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  int i = root[key].as<int>();
  return i;
}

// ------------- Запись значения json String
String jsonWrite(String &json, String key, String value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[key] = value;
  json = "";
  root.printTo(json);
  return json;
}

// ------------- Запись значения json int
String jsonWrite(String &json, String key, int value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[key] = value;
  json = "";
  root.printTo(json);
  return json;
}

// ------------- Запись значения json float
String jsonWrite(String &json, String key, float value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  root[key] = value;
  json = "";
  root.printTo(json);
  return json;
}
