# お風呂が溜まりましたセンサー

## 要旨

お風呂のお湯が一定まで溜まったら、LINEを通じてお湯が溜まったことを通知します。

![お風呂](/doc/room_ofuro.png)

![通知](/doc/Notify.png)

## 必要な機器

- ESP-WROOM-32
- SEN0193(土壌水分センサー)
- 単三乾電池４本

## 配線図

以下のような配線となります。
- 信号線 pin36（下図、黄色）でSEN0193から入力

![配線図](/doc/配線図.png)

## ソースコードポイント

- 起動時に、WiFi接続に接続する。

``` C++ 
void setup(){
    :
  // WiFi接続
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }
}
```

- センサーが閾値以上の水没を検知したらLINEに通知

``` C++ 
void httpsPost(){
    :
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
    :
}
```

