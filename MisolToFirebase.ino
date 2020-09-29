#include <WiFiClientSecure.h>
#define SSID                  "greenio"
#define PASSWORD              "green7650"
#define FIREBASE_HOST         "https://environment-e3963.firebaseio.com/"
#define FIREBASE_AUTH         "NVp26j3hrk8xlWQZTqboJlSI874e8Dm1stXIgNiq"



 


#include <HardwareSerial.h>
const long interval = 16000;  //millisecond
unsigned long previousMillis = 0;
HardwareSerial mySerial(1);
uint8_t cc;
String hexString = "";



struct misoWeather {
  uint8_t idType;
  uint8_t securityCode;
  float winDir, temp;
  float hum, winSpeed, gustSpeed;
  float accRainfall, uv, light, baroPress;
  String  CRC;
  String checksum;
  float pm2_5;
};

struct pms7003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm01_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms7003data dataPMS;
struct misoWeather dataMiso;
HardwareSerial hwSerial(2);
#define SERIAL1_RXPIN 26
#define SERIAL1_TXPIN 25
unsigned long currentMillis;



//#include "EEPROM.h"

#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value


String  TD32_Get_Firebase(String path );               // รับค่า path จาก Firebase
int     TD32_Set_Firebase(String path, String value, bool push = false ); // ส่งค่าขึ้น Firebase  (ทับข้อมูลเดิมใน path เดิม)
int     TD32_Push_Firebase(String path, String value); // ส่งค่าขึ้น Firebase แบบ Pushing data  (เพิ่มเข้าไปใหม่เรื่อยๆใน path เดิม)
//--------------------------------------------------------
bool CHECK = true;

void setup() {
  Serial.begin(115200);
 

  Serial.println("Connecting..");
  WiFi.begin(SSID, PASSWORD);                       // ทำการเชื่อมต่อ WiFi
  while (WiFi.status() != WL_CONNECTED ) delay(400); // รอจนกว่าจะเชื่อมต่อ WiFi ได้
  Serial.println();
  Serial.println(WiFi.localIP());                   // แสดงค่า IP หลังเชื่อมต่อ WiFi สำเร็จ

  TD32_Set_Firebase("name", "\"Powered By Nipat Academy\"");   // กำหนด ค่า "Trident_ESP32" ให้กับ ตัวแปร name บน Firebase
  Serial.println( TD32_Get_Firebase("name"));       // ดึงค่าของ name บน Firebase กลับลงมาแสดงบน ESP32

  pinMode(33, OUTPUT); // turn on PMS7003 board thingcontrol v1.0
  digitalWrite(33, HIGH); // turn on PMS7003
  delay(400);
  hwSerial.begin(9600, SERIAL_8N1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  previousMillis = millis();
  Serial.println("Start");


}


uint32_t timer;
void loop() {
  if ( millis() - timer > 60000) {     // ทำการอ่านค่าและส่งทุกๆ 60 วินาที
    timer = millis();
    hexString = "";
    Serial.println("recv (HEX): ");
    while (mySerial.available())
    {
      cc = mySerial.read();

      if (cc < 0x10) {
        Serial.print("0");
        hexString +=  String(cc, HEX);
      }
      Serial.print(cc, HEX);
      hexString +=  String(cc, HEX);
    }
    Serial.println("");
    parseHex(hexString);
    readPMSdata(&hwSerial);
    previousMillis = currentMillis;


    TD32_Set_Firebase("Set/winDir", String(dataMiso.winDir));
    TD32_Set_Firebase("Set/temp", String(dataMiso.temp));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/hum", String(dataMiso.hum));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/winSpeed", String(dataMiso.winSpeed));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/gustSpeed", String(dataMiso.gustSpeed));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/gustSpeed", String(dataMiso.gustSpeed));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/accRainfall", String(dataMiso.accRainfall));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/uv", String(dataMiso.uv));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/light", String(dataMiso.light));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/baroPress", String(dataMiso.baroPress));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/pm2.5", String(dataPMS.pm25_env));// ตั้งค่า  Firebase
    TD32_Set_Firebase("Set/pm10", String(dataPMS.pm100_env));// ตั้งค่า  Firebase

    TD32_Push_Firebase("Push/winDir", String(dataMiso.winDir));
    TD32_Push_Firebase("Push/temp", String(dataMiso.temp));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/hum", String(dataMiso.hum));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/winSpeed", String(dataMiso.winSpeed));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/gustSpeed", String(dataMiso.gustSpeed));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/gustSpeed", String(dataMiso.gustSpeed));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/accRainfall", String(dataMiso.accRainfall));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/uv", String(dataMiso.uv));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/light", String(dataMiso.light));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/baroPress", String(dataMiso.baroPress));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/pm2.5", String(dataPMS.pm25_env));// ตั้งค่า  Firebase
    TD32_Push_Firebase("Push/pm10", String(dataPMS.pm100_env));// ตั้งค่า  Firebase

  }
}


long unsigned int hex2Dec(String hex) {
  String tobeDec = "0x" + hex;
  char str[16];
  tobeDec.toCharArray(str, 16);

  long unsigned int val = strtol(str, NULL, 16);
  //  Serial.println(val);
  return val;
}

int parseWindDir(String hexWind) {

  String hexString = "";
  hexString.concat("0");  hexString.concat(hexWind.charAt(0));
  hexString.concat("0");  hexString.concat(hexWind.charAt(1));
  hexString.concat("0");  hexString.concat(hexWind.charAt(2));
  Serial.println(hexString);
  int number = (int) strtol( &hexString[0], NULL, 16);  //separate BINARAY bit
  // Split them up into r, g, b values
  int r = number >> 16;
  int g = number >> 8 & 0xFF;
  int b = number & 0xFF;
  String st = String(r, BIN);
  String nd = String(g, BIN);
  String rd = String(b, BIN);
  // padding zero
  st = pad0Left(st, "0");
  nd = pad0Left(nd, "0");
  rd = pad0Left(rd, "0");
  char st_array[5];
  char nd_array[5];
  char rd_array[5];
  st.toCharArray(st_array, 5);
  nd.toCharArray(nd_array, 5);
  rd.toCharArray(rd_array, 5);
  char windChars[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //+1
  // follow by the Weather Station  Misol manual 9bits
  windChars[0] = rd_array[0];
  windChars[1] = st_array[0];
  windChars[2] = st_array[1];
  windChars[3] = st_array[2];
  windChars[4] = st_array[3];
  windChars[5] = nd_array[0];
  windChars[6] = nd_array[1];
  windChars[7] = nd_array[2];
  windChars[8] = nd_array[3];

  Serial.print(rd_array[0]);
  Serial.print(st_array[0]); Serial.print(st_array[1]); Serial.print(st_array[2]); Serial.print(st_array[3]);
  Serial.print(nd_array[0]); Serial.print(nd_array[1]); Serial.print(nd_array[2]); Serial.println(nd_array[3]);

  int windDirDec = strtol(windChars, (char**) NULL, 2);

  Serial.println(windDirDec);
  return windDirDec;
}

void parseHex (String hex) {
  String idType = hex.substring(0, 2);
  String securityCode = hex.substring(2, 4);
  String _winDir  = hex.substring(4, 7);
  String _temp  = hex.substring(7, 10);
  String _hum = hex.substring(10, 12);
  String _winSpeed = hex.substring(12, 14);
  String _gustSpeed = hex.substring(14, 16);
  String _accRainfall = hex.substring(16, 20);
  String _uv = hex.substring(20, 24);
  String _light = hex.substring(24, 30);
  String _baroPress = hex.substring(35, 40);
  dataMiso.winDir = parseWindDir(_winDir);
  dataMiso.temp =  (hex2Dec(_temp) - 400)  / 10.0;
  dataMiso.hum = hex2Dec(_hum);
  dataMiso.winSpeed = (hex2Dec(_winSpeed) / 8) * 1.12;
  dataMiso.gustSpeed = hex2Dec(_gustSpeed);
  dataMiso.accRainfall = hex2Dec(_accRainfall);
  dataMiso.uv = hex2Dec(_uv);
  dataMiso.light = hex2Dec(_light) / 10.0;
  dataMiso.baroPress = hex2Dec(_baroPress) / 100.0;
  Serial.println("Debug:");
  //  Serial.print("  idType:");  Serial.println(dataMiso.idType);
  //  Serial.print("  securityCode:");  Serial.println(dataMiso.securityCode);
  Serial.print("  winDir:"); Serial.println(dataMiso.winDir);
  Serial.print("  temp:");  Serial.println(dataMiso.temp);
  Serial.print("  hum:");   Serial.println(dataMiso.hum);
  Serial.print("  winSpeed:");   Serial.println(dataMiso.winSpeed);
  Serial.print("  gustSpeed:");   Serial.println(dataMiso.gustSpeed);
  Serial.print("  accRainfall:");   Serial.println(dataMiso.accRainfall);
  Serial.print("  uv:");   Serial.println(dataMiso.uv);
  Serial.print("  light:");   Serial.println(dataMiso.light);
  Serial.print("  baroPress:");   Serial.println(dataMiso.baroPress);
  Serial.print("  pm2.5:");  Serial.println(dataPMS.pm25_env);
  Serial.print("  pm10:");  Serial.println(dataPMS.pm100_env);
  Serial.print("  pm0.3:");  Serial.println(dataPMS.particles_03um);
  
  Serial.println("end");
 


}
String pad0Left(String str, String padStr) {

  while (str.length() < 4) {
    str = padStr + str;
  }
  return str;
}

boolean readPMSdata(Stream *s) {
  Serial.println("readPMSdata");
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  memcpy((void *)&dataPMS, (void *)buffer_u16, 30);
  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }
  if (sum != dataPMS.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}

const char* FIREBASE_ROOT_CA =  "-----BEGIN CERTIFICATE-----\n"
                                "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n"
                                "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n"
                                "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n"
                                "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n"
                                "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n"
                                "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n"
                                "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n"
                                "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n"
                                "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n"
                                "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n"
                                "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n"
                                "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n"
                                "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n"
                                "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n"
                                "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n"
                                "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n"
                                "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n"
                                "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n"
                                "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n"
                                "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n"
                                "-----END CERTIFICATE-----\n";

int TD32_Set_Firebase(String path, String value, bool push ) {
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST); host.replace("https://", "");
  if (host[host.length() - 1] == '/' ) host = host.substring(0, host.length() - 1);
  String resp = "";
  int httpCode = 404; // Not Found

  String firebase_method = (push) ? "POST " : "PUT ";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if ( ssl_client.connect( host.c_str(), 443)) {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request +=  firebase_method + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
    request += "Content-Length: " + String( value.length()) + "\r\n\r\n";
    request += value;

    ssl_client.print(request);
    while ( ssl_client.connected() && !ssl_client.available()) delay(10);
    if ( ssl_client.connected() && ssl_client.available() ) {
      resp      = ssl_client.readStringUntil('\n');
      httpCode  = resp.substring(resp.indexOf(" ") + 1, resp.indexOf(" ", resp.indexOf(" ") + 1)).toInt();
    }
    ssl_client.stop();
  }
  else {
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return httpCode;
}

int TD32_Push_Firebase(String path, String value) {
  return TD32_Set_Firebase(path, value, true);
}

String TD32_Get_Firebase(String path ) {
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST); host.replace("https://", "");
  if (host[host.length() - 1] == '/' ) host = host.substring(0, host.length() - 1);
  String resp = "";
  String value = "";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if ( ssl_client.connect( host.c_str(), 443)) {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request += "GET " + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n\r\n";

    ssl_client.print( request);
    while ( ssl_client.connected() && !ssl_client.available()) delay(10);
    if ( ssl_client.connected() && ssl_client.available() ) {
      while ( ssl_client.available()) resp += (char)ssl_client.read();
      value = resp.substring( resp.lastIndexOf('\n') + 1, resp.length() - 1);
    }
    ssl_client.stop();
  } else {
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return value;
}
