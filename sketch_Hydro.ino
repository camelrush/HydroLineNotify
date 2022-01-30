#include <WiFiClientSecure.h>

const char* WIFI_SSID="";      // change for your WiFi-ssid
const char* WIFI_PASSWORD="";  // change for your WiFi-password
const char* LINE_HOST = "notify-api.line.me";
const char* LINE_TOKEN = "";   // change for your Line-Token
const char* MESSAGE = "お風呂が溜まりました";
const int THRESHOLD = 1500;    // センサーの水没検知を示す閾値
const int SENSOR_PIN = 36;
const int SENSOR_STS_INIT = 0; // センサー状態 初期
const int SENSOR_STS_DROP = 1; // センサー状態 水没

int now_status;
int pre_status;

WiFiClientSecure client;

void setup() {

  // 土壌水分センサー SEN0193 準備
  Serial.begin(115200);
  while (!Serial);
  delay(2000);

  // WiFi接続
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);

  // LINEサービスへの接続
  Serial.println("\nStarting connection to server...");
  if (!client.connect(LINE_HOST, 443)){
    Serial.println("Connection failed!");
    return;
  }
  
  // センサー状態を初期化
  pre_status = SENSOR_STS_INIT;
}
   
void loop() {

  // センサー値取得
  int sensor_output= analogRead(SENSOR_PIN);
  if (sensor_output < THRESHOLD) {
    now_status = SENSOR_STS_DROP; 
  } else {
    now_status = SENSOR_STS_INIT; 
  }

  // 閾値を下回った直後、LINEへ通知   
  if (pre_status == SENSOR_STS_INIT && now_status == SENSOR_STS_DROP) {
    httpsPost();
  }

  // 今回のセンサー値を退避
  pre_status = now_status;

  delay(2000);
}

void httpsPost(){

  // HTTPSへアクセス（SSL通信）するためのライブラリ
  WiFiClientSecure client;

  // サーバー証明書の検証を行わずに接続する場合に必要
  client.setInsecure();
  
  Serial.println("Try");
  
  //LineのAPIサーバにSSL接続（ポート443:https）
  if (!client.connect(LINE_HOST, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");

  // リクエスト送信
  String query = String("message=") + String(MESSAGE);
  String request = String("") +
               "POST /api/notify HTTP/1.1\r\n" +
               "Host: " + LINE_HOST + "\r\n" +
               "Authorization: Bearer " + LINE_TOKEN + "\r\n" +
               "Content-Length: " + String(query.length()) +  "\r\n" + 
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
                query + "\r\n";
  client.print(request);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println("line:" + line);
    if (line == "\r") {
      break;
    }
  }

  String line = client.readStringUntil('\n');
  Serial.println(line);  
}
