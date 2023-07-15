#include <ESP8266WiFi.h> // library  untuk ESP8266
#include <WiFiClientSecure.h> // library untuk Wifi
#include <UniversalTelegramBot.h> // library unutk telegram
#include <OneWire.h> // library  untuk OneWire
#include <DallasTemperature.h> // library untuk sensor suhu
#include <BlynkSimpleEsp8266.h> // library untuk blynk

#define BLYNK_TEMPLATE_ID "TMPL6CEWENhJW" // id template blynk yang digunakan
#define BLYNK_TEMPLATE_NAME "Quickstart Template" // nama template blynk yg digunakan
#define BLYNK_AUTH_TOKEN "dgKCSw3fHCq9Uk1u-zAP8LQNIXYp6SFv" // token blynk

#define ssid "Amelia" // Nama wifi
#define pass "KosKos2dan4" // Password wifi
#define token "6030855794:AAEHcGoBBGf0cRzJZAttH7-cxHpSYzWaOvY" // Token BOT Telegram
String id = "1386652179"; // ID Telegram

#define buzzer D3 // Pin buzzer di D3 atau GPIO0
#define ONE_WIRE_BUS D4 // pin D4 untuk data sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int suhu = 0;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(token, secured_client);
unsigned long bot_lasttime;
unsigned long suhu_lasttime;
unsigned long update_interval = 1000; // Interval waktu pembaruan suhu (dalam milidetik)

void mengirimpesan(int pesanbaru)
{
  for (int i = 0; i < pesanbaru; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (text == "/suhu") // pesan perintah untuk bertanya kondisi suhu
    {
      String message = "Lapor suhu saat ini: " + String(suhu) + "°C"; //pesan balasan dengan
      bot.sendMessage(chat_id, message);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  sensors.begin();
  WiFi.begin(ssid, pass);
  secured_client.setInsecure();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); // Initialize Blynk with your authentication token and WiFi credentials
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  bot_lasttime = millis();
  suhu_lasttime = millis();
}

void loop()
{
  Blynk.run(); // Run Blynk

  if (millis() - bot_lasttime > 1000)
  {
    int pesanbaru = bot.getUpdates(bot.last_message_received + 1);

    while (pesanbaru)
    {
      mengirimpesan(pesanbaru);
      pesanbaru = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

  if (millis() - suhu_lasttime >= update_interval)
  {
    sensors.requestTemperatures();
    suhu = sensors.getTempCByIndex(0);
    // Menampilkan suhu di layar monitor
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.println("°C");

    if (suhu < 25 || suhu > 32)
    {
      // Mengirim pesan peringatan ke Telegram
      String message = "Lapor Suhu Saat ini sangat ";
      if (suhu < 25)
        message += "dingin";
      else
        message += "panas";
      message += ". Suhu mencapai " + String(suhu) + "°C";
      bot.sendMessage(id, message);

      // Mengaktifkan buzzer
      buzzerNyala();
    }
    else
    {
      // Menonaktifkan buzzer
      digitalWrite(buzzer, LOW);
    }

    // Update Blynk LCD widget with the temperature
    Blynk.virtualWrite(V4, "Suhu Air: " + String(suhu) + "°C");
    // Mengirim data suhu ke widget Super Chart
    Blynk.virtualWrite(V0, suhu);

    suhu_lasttime = millis(); // Memperbarui waktu terakhir pembacaan suhu
  }
}

void buzzerNyala()
{
  // Menyalakan buzzer
  digitalWrite(buzzer, HIGH);
  delay(500);
  digitalWrite(buzzer, LOW);
  delay(500);
}
