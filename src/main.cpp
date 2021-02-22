#include <M5Stack.h>
#include <Avatar.h>
#include <faces/DogFace.h>

#define TINY_GSM_MODEM_UBLOX
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include "twilio-config.h"

// Server details
const char server[] = SERVER_URL;
const char resource[] = "/send-sms";
const int port = 443;
const char signature[] = SIGNATURE;

TinyGsm modem(Serial2);             // 3G board modem
TinyGsmClientSecure ctx(modem);     // SSLクライアント
HttpClient http(ctx, server, port); // Arduino Http Client

using namespace m5avatar;

Avatar avatar;

Face *faces[2];
const int facesSize = sizeof(faces) / sizeof(Face *);
int faceIdx = 0;

ColorPalette *cps[4];
const int cpsSize = sizeof(cps) / sizeof(ColorPalette *);
int cpsIdx = 0;

bool isShowingQR = false;

void setup()
{
  Serial.begin(115200);
  M5.begin();

  // 3G board modem setup
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println(F("M5Stack + 3G Module"));

  M5.Lcd.print(F("modem.restart()"));
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  modem.restart();
  M5.Lcd.println(F("done"));

  M5.Lcd.print(F("getModemInfo:"));
  Serial.print(F("getModemInfo:"));
  String modemInfo = modem.getModemInfo();
  M5.Lcd.println(modemInfo);
  Serial.println(modemInfo);

  M5.Lcd.print(F("waitForNetwork()"));
  while (!modem.waitForNetwork())
    M5.Lcd.print(".");
  M5.Lcd.println(F("Ok"));

  M5.Lcd.print(F("gprsConnect(SuperSIM)"));
  modem.gprsConnect("super", "", "");
  M5.Lcd.println(F("done"));

  M5.Lcd.print(F("isNetworkConnected()"));
  while (!modem.isNetworkConnected())
    M5.Lcd.print(".");
  M5.Lcd.println(F("Ok"));

  M5.Lcd.print(F("My IP addr: "));
  IPAddress ipaddr = modem.localIP();
  M5.Lcd.print(ipaddr);
  if (ipaddr.toString() == "0.0.0.0")
  {
    Serial.println(F("Reset..."));
    setup();
  }
  delay(2000);

  /* Avatar setup */
  M5.Lcd.setBrightness(30);
  M5.Lcd.clear();

  faces[0] = avatar.getFace();
  faces[1] = new DogFace();

  cps[0] = new ColorPalette();
  cps[1] = new ColorPalette();
  cps[2] = new ColorPalette();
  cps[3] = new ColorPalette();
  cps[1]->set(COLOR_PRIMARY, TFT_YELLOW);
  cps[1]->set(COLOR_BACKGROUND, TFT_DARKCYAN);
  cps[2]->set(COLOR_PRIMARY, TFT_DARKGREY);
  cps[2]->set(COLOR_BACKGROUND, TFT_WHITE);
  cps[3]->set(COLOR_PRIMARY, TFT_RED);
  cps[3]->set(COLOR_BACKGROUND, TFT_PINK);

  avatar.init();
  avatar.setColorPalette(*cps[0]);
}

void loop()
{
  M5.update();
  if (M5.BtnA.wasPressed())
  {
    avatar.setFace(faces[faceIdx]);
    faceIdx = (faceIdx + 1) % facesSize;
  }
  if (M5.BtnB.wasPressed())
  {
    avatar.setColorPalette(*cps[cpsIdx]);
    cpsIdx = (cpsIdx + 1) % cpsSize;
  }
  if (M5.BtnC.wasPressed())
  {
    avatar.setSpeechText("Sending...");

    // Send SMS via Twilio Function
    http.beginRequest();
    int err = http.get(resource);
    if (err != 0)
    {
      Serial.println(F("Connect failed."));
      Serial.print(F("err: "));
      Serial.print(err);
      return;
    }
    http.connectionKeepAlive();
    http.sendHeader("X-Twilio-Signature", signature); // Twilio Signature
    http.endRequest();
    Serial.println(F("connected."));

    int status = http.responseStatusCode();
    Serial.print(F("Response status code: "));
    Serial.println(status);
    if (!status)
    {
      delay(10000);
      return;
    }

    avatar.setExpression(Expression::Happy);
    avatar.setSpeechText("Sent.");

    http.stop();
    Serial.println(F("Disconnected."));

    delay(1000);

    avatar.setExpression(Expression::Neutral);
    avatar.setSpeechText("");
  }
}
