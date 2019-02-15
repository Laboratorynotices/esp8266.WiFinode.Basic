#include <ESP8266WiFi.h>      //Работа с WiFi-сетью (стандартная библиотека)
#include <ESP8266WebServer.h> //Работа с Веб-сервером (стандартная библиотека)
#include <ESP8266SSDP.h>      //      Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step3-ssdp
#include <FS.h>               //Работа с файловой системой (стандартная библиотека)
#include <ArduinoJson.h>      //Работа с JSON-структурами (загружается через менеджер библиотек)
#include <time.h>             //Работа со временем (стандартная библиотека)

/** Настройки по умолчанию, на основе которых будет создан файл конфигурации **/
// Настройки WIFI
// Название WIFI сети по умолчанию
#define default_wifiName ""
// Пароль WIFI сети по умолчанию
#define default_wifiPassword ""
// Название WIFI сети по умолчанию, если модуль сам создаст сеть (АР)
#define default_APwifiName "WiFi"
//String _ssidAP = "WiFi";   // SSID AP точки доступа @TODO зачистить
// Пароль WIFI сети по умолчанию, если модуль сам создаст сеть (АР)
#define default_APwifiPassword ""
//String _passwordAP = ""; // пароль точки доступа @TODO зачистить

// Часовой пояс по умолчанию
#define default_timezone 1




// Переменная для хранения настроек в формате JSON
String jsonConfig = "{}";


// dst параметр для времени (зачем не знаю)
#define dst 0
//struct tm * timeinfo;
/*
struct tm
{
int    tm_sec;   //   Seconds [0,60]. 
int    tm_min;   //   Minutes [0,59]. 
int    tm_hour;  //   Hour [0,23]. 
int    tm_mday;  //   Day of month [1,31]. 
int    tm_mon;   //   Month of year [0,11]. 
int    tm_year;  //   Years since 1900. 
int    tm_wday;  //   Day of week [0,6] (Sunday =0). 
int    tm_yday;  //   Day of year [0,365]. 
int    tm_isdst; //   Daylight Savings flag. 
}
*/




// Данные для замеров
// Сколько измерений будет храниться (сохранено)
#define mesValuesSize 10
// Сколько измерений уже было сделано
byte mesValuesMessedTimes = 0;
// Место для хранения измерений
int mesValues[mesValuesSize] = {};
// Место для хранения легенды
String labels[mesValuesSize] = {};
// Количество отметок в легенде графика
#define labelsQuantity 5
// Минимальное измеренное значение
int mesValuesMin;
// Максимальное измеренное значение
int mesValuesMax;



// Время, когда должен сработать следующий таймер для измерений
unsigned long A0MeasureTimer = 0;
// Время как часто должен замеряться А0, 5с*1000
#define A0MeasurePeriod 5000;

IPAddress apIP(192, 168, 4, 1);

// Web интерфейс для устройства
ESP8266WebServer HTTP(80);
// Для файловой системы
File fsUploadFile;




String SSDP_Name="FSWebServer"; // Имя SSDP



/** Служебные данные, в коде не должно быть ни одного числа, ни одной строчки **/
// Название файла с настройками
#define configFileName "config.json"
// NTP Сервера
#define NTPServer1 "pool.ntp.org"
#define NTPServer2 "time.nist.gov"
