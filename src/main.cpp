#if defined( ARDUINO )
#include <Arduino.h>
#include <SD.h>
#include <SPIFFS.h>
#endif

#include <M5UnitOLED.h>
#include <M5UnitLCD.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <AudioOutput.h>
#include <AudioFileSourceBuffer.h>
#include <AudioGeneratorMP3.h>
#include "AudioFileSourceHTTPSStream.h"
#include "AudioOutputM5Speaker.h"
#include "WebVoiceVoxTTS.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "rootCACertificate.h"
#include "rootCAgoogle.h"
#include <ArduinoJson.h>
#include <ESP32WebServer.h>
//#include <ESPmDNS.h>
#include <deque>
#include "AudioWhisper.h"
#include "Whisper.h"
#include "Audio.h"
#include "CloudSpeechClient.h"

// 保存する質問と回答の最大数
const int MAX_HISTORY = 5;

// 過去の質問と回答を保存するデータ構造
std::deque<String> chatHistory;

ESP32WebServer server(80);

//---------------------------------------------
String OPENAI_API_KEY = "";
String VOICEVOX_API_KEY = "";
String STT_API_KEY = "";
String TTS_SPEAKER_NO = "3";
String TTS_SPEAKER = "&speaker=";
String TTS_PARMS = TTS_SPEAKER + TTS_SPEAKER_NO;

//---------------------------------------------
// C++11 multiline string constants are neato...
static const char HEAD[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="UTF-8">
  <title>AIｽﾀｯｸﾁｬﾝ</title>
</head>)KEWL";

static const char APIKEY_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8">
    <title>APIキー設定</title>
  </head>
  <body>
    <h1>APIキー設定</h1>
    <form>
      <label for="role1">OpenAI API Key</label>
      <input type="text" id="openai" name="openai" oninput="adjustSize(this)"><br>
      <label for="role2">VoiceVox API Key</label>
      <input type="text" id="voicevox" name="voicevox" oninput="adjustSize(this)"><br>
      <label for="role3">Speech to Text API Key</label>
      <input type="text" id="sttapikey" name="sttapikey" oninput="adjustSize(this)"><br>
      <button type="button" onclick="sendData()">送信する</button>
    </form>
    <script>
      function adjustSize(input) {
        input.style.width = ((input.value.length + 1) * 8) + 'px';
      }
      function sendData() {
        // FormDataオブジェクトを作成
        const formData = new FormData();

        // 各ロールの値をFormDataオブジェクトに追加
        const openaiValue = document.getElementById("openai").value;
        if (openaiValue !== "") formData.append("openai", openaiValue);

        const voicevoxValue = document.getElementById("voicevox").value;
        if (voicevoxValue !== "") formData.append("voicevox", voicevoxValue);

        const sttapikeyValue = document.getElementById("sttapikey").value;
        if (sttapikeyValue !== "") formData.append("sttapikey", sttapikeyValue);

	    // POSTリクエストを送信
	    const xhr = new XMLHttpRequest();
	    xhr.open("POST", "/apikey_set");
	    xhr.onload = function() {
	      if (xhr.status === 200) {
	        alert("データを送信しました！");
	      } else {
	        alert("送信に失敗しました。");
	      }
	    };
	    xhr.send(formData);
	  }
	</script>
  </body>
</html>)KEWL";

static const char ROLE_HTML[] PROGMEM = R"KEWL(
<!DOCTYPE html>
<html>
<head>
	<title>ロール設定</title>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<style>
		textarea {
			width: 80%;
			height: 200px;
			resize: both;
		}
	</style>
</head>
<body>
	<h1>ロール設定</h1>
	<form onsubmit="postData(event)">
		<label for="textarea">ここにロールを記述してください。:</label><br>
		<textarea id="textarea" name="textarea"></textarea><br><br>
		<input type="submit" value="Submit">
	</form>
	<script>
		function postData(event) {
			event.preventDefault();
			const textAreaContent = document.getElementById("textarea").value.trim();
//			if (textAreaContent.length > 0) {
				const xhr = new XMLHttpRequest();
				xhr.open("POST", "/role_set", true);
				xhr.setRequestHeader("Content-Type", "text/plain;charset=UTF-8");
			// xhr.onload = () => {
			// 	location.reload(); // 送信後にページをリロード
			// };
			xhr.onload = () => {
				document.open();
				document.write(xhr.responseText);
				document.close();
			};
				xhr.send(textAreaContent);
//        document.getElementById("textarea").value = "";
				alert("Data sent successfully!");
//			} else {
//				alert("Please enter some text before submitting.");
//			}
		}
	</script>
</body>
</html>)KEWL";


#define SDU_APP_PATH "/M5Core2AvatarLite.bin" // title for SD-Updater UI
#define SDU_APP_NAME "Image Avater Lite" // title for SD-Updater UI
#include <M5StackUpdater.h> // https://github.com/tobozo/M5Stack-SD-Updater/

#include "M5ImageAvatarLite.h"
#include "ImageAvatarSystemConfig.h" 

// サーボを利用しない場合は下記の1行をコメントアウトしてください。
#define USE_SERVO

// M5GoBottomのLEDを使わない場合は下記の1行をコメントアウトしてください。
#define USE_LED

// デバッグしたいときは下記の１行コメントアウトしてください。
//#define DEBUG

M5GFX &gfx( M5.Lcd ); // aliasing is better than spawning two instances of LGFX

// JSONファイルとBMPファイルを置く場所を切り替え
// 開発時はSPIFFS上に置いてUploadするとSDカードを抜き差しする手間が省けます。
fs::FS json_fs = SD; // JSONファイルの収納場所(SPIFFS or SD)
fs::FS bmp_fs  = SD; // BMPファイルの収納場所(SPIFFS or SD)

using namespace m5imageavatar;

ImageAvatarSystemConfig system_config;
const char* avatar_system_json = "/json/M5AvatarLiteSystem.json"; // ファイル名は32バイトを超えると不具合が起きる場合あり
uint8_t avatar_count = 0;
ImageAvatarLite avatar(json_fs, bmp_fs);
#ifdef USE_SERVO
  #include "ImageAvatarServo.h"
  ImageAvatarServo servo;
  bool servo_enable = true; // サーボを動かすかどうか
  TaskHandle_t servoloopTaskHangle;
#endif

#ifdef USE_LED
  #include <FastLED.h>
  #define NUM_LEDS 10
#ifdef ARDUINO_M5STACK_FIRE
  #define LED_PIN 15
#else
  #define LED_PIN 25
#endif
  CRGB leds[NUM_LEDS];
  CRGB led_table[NUM_LEDS / 2] = {CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Orange, CRGB::Red };
  void turn_off_led() {
    // Now turn the LED off, then pause
    for(int i=0;i<NUM_LEDS;i++) leds[i] = CRGB::Black;
    FastLED.show();  
  }

  void clear_led_buff() {
    // Now turn the LED off, then pause
    for(int i=0;i<NUM_LEDS;i++) leds[i] =  CRGB::Black;
  }

  void level_led(int level1, int level2) {  
  if(level1 > 5) level1 = 5;
  if(level2 > 5) level2 = 5;
    
    clear_led_buff(); 
    for(int i=0;i<level1;i++){
      leds[NUM_LEDS/2-1-i] = led_table[i];
    }
    for(int i=0;i<level2;i++){
      leds[i+NUM_LEDS/2] = led_table[i];
    }
    FastLED.show();
  }
#endif

void status_led(CRGB color){
#ifdef USE_LED
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
#endif
}

#define LIPSYNC_LEVEL_MAX 10.0f
static float lipsync_level_max = LIPSYNC_LEVEL_MAX;
uint8_t expression = 0;
float mouth_ratio = 0.0f;
bool sing_happy = true;

// ----- あまり間隔を短くしすぎるとサーボが壊れやすくなるので注意(単位:msec)
static long interval_min      = 1000;        // 待機時インターバル最小
static long interval_max      = 10000;       // 待機時インターバル最大
static long interval_move_min = 500;         // 待機時のサーボ移動時間最小
static long interval_move_max = 1500;        // 待機時のサーボ移動時間最大
static long sing_interval_min = 500;         // 歌うモードのインターバル最小
static long sing_interval_max = 1500;        // 歌うモードのインターバル最大
static long sing_move_min     = 500;         // 歌うモードのサーボ移動時間最小
static long sing_move_max     = 1500;        // 歌うモードのサーボ移動時間最大
// サーボ関連の設定 end
// --------------------

/// set M5Speaker virtual channel (0-7)
static constexpr uint8_t m5spk_virtual_channel = 0;
AudioOutputM5Speaker out(&M5.Speaker, 0);
AudioGeneratorMP3 *mp3;
AudioFileSourceBuffer *buff = nullptr;
int preallocateBufferSize = 30*1024;
uint8_t *preallocateBuffer;
AudioFileSourceHTTPSStream *file = nullptr;

static fft_t fft;
static constexpr size_t WAVE_SIZE = 320;
static int16_t raw_data[WAVE_SIZE * 2];

// auto poweroff 
uint32_t auto_power_off_time = 0;  // USB給電が止まった後自動で電源OFFするまでの時間（msec）。0は電源OFFしない。
uint32_t last_discharge_time = 0;  // USB給電が止まったときの時間(msec)

// Multi Threads Settings
TaskHandle_t lipsyncTaskHandle;
SemaphoreHandle_t xMutex = NULL;

String speech_text = "";
String speech_text_buffer = "";
DynamicJsonDocument chat_doc(1024*10);
String json_ChatString = "{\"model\": \"gpt-3.5-turbo-0613\",\"messages\": [{\"role\": \"user\", \"content\": \"""\"}]}";
String Role_JSON = "";

bool init_chat_doc(const char *data)
{
  DeserializationError error = deserializeJson(chat_doc, data);
  if (error) {
    Serial.println("DeserializationError");
    return false;
  }
  String json_str; //= JSON.stringify(chat_doc);
  serializeJsonPretty(chat_doc, json_str);  // 文字列をシリアルポートに出力する
//  Serial.println(json_str);
    return true;
}

void handleRoot() {
  server.send(200, "text/plain", "hello from m5stack!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
//  server.send(404, "text/plain", message);
  server.send(404, "text/html", String(HEAD) + String("<body>") + message + String("</body>"));
}

void handle_speech() {
  String message = server.arg("say");
  String speaker = server.arg("voice");
  if(speaker != "") {
    TTS_PARMS = TTS_SPEAKER + speaker;
  }
  Serial.println(message);
  ////////////////////////////////////////
  // 音声の発声
  ////////////////////////////////////////
   //avatar.setExpression(Expression::Happy);
  Voicevox_tts((char*)message.c_str(), (char*)TTS_PARMS.c_str());
  server.send(200, "text/plain", String("OK"));
}

String https_post_json(const char* url, const char* json_string, const char* root_ca) {
  String payload = "";
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(root_ca);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      https.setTimeout( 65000 ); 
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, url)) {  // HTTPS
        Serial.print("[HTTPS] POST...\n");
        // start connection and send HTTP header
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Authorization", String("Bearer ") + OPENAI_API_KEY);
        int httpCode = https.POST((uint8_t *)json_string, strlen(json_string));
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
            Serial.println("//////////////");
            Serial.println(payload);
            Serial.println("//////////////");
          }
        } else {
          Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
      // End extra scoping block
    }  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return payload;
}

String chatGpt(String json_string) {
  String response = "";;
   //avatar.setExpression(Expression::Doubt);
  //avatar.setSpeechText("考え中…");
  String ret = https_post_json("https://api.openai.com/v1/chat/completions", json_string.c_str(), root_ca_openai);
   //avatar.setExpression(Expression::Neutral);
  //avatar.setSpeechText("");
  Serial.println(ret);
  if(ret != ""){
    DynamicJsonDocument doc(2000);
    DeserializationError error = deserializeJson(doc, ret.c_str());
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
       //avatar.setExpression(Expression::Sad);
      //avatar.setSpeechText("エラーです");
      response = "エラーです";
      delay(1000);
      //avatar.setSpeechText("");
       //avatar.setExpression(Expression::Neutral);
    }else{
      const char* data = doc["choices"][0]["message"]["content"];
      Serial.println(data);
      response = String(data);
      std::replace(response.begin(),response.end(),'\n',' ');
    }
  } else {
     //avatar.setExpression(Expression::Sad);
    //avatar.setSpeechText("わかりません");
    response = "わかりません";
    delay(1000);
    //avatar.setSpeechText("");
     //avatar.setExpression(Expression::Neutral);
  }
  return response;
}

String InitBuffer = "";

void handle_chat() {
  static String response = "";
  // tts_parms_no = 1;
  String text = server.arg("text");
  String speaker = server.arg("voice");
  if(speaker != "") {
    TTS_PARMS = TTS_SPEAKER + speaker;
  }
  Serial.println(InitBuffer);
  init_chat_doc(InitBuffer.c_str());
  // 質問をチャット履歴に追加
  chatHistory.push_back(text);
   // チャット履歴が最大数を超えた場合、古い質問と回答を削除
  if (chatHistory.size() > MAX_HISTORY * 2)
  {
    chatHistory.pop_front();
    chatHistory.pop_front();
  }

  for (int i = 0; i < chatHistory.size(); i++)
  {
    JsonArray messages = chat_doc["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    if(i % 2 == 0) {
      systemMessage1["role"] = "user";
    } else {
      systemMessage1["role"] = "assistant";
    }
    systemMessage1["content"] = chatHistory[i];
  }

  String json_string;
  serializeJson(chat_doc, json_string);
  if(speech_text=="" && speech_text_buffer == "") {
    response = chatGpt(json_string);
    speech_text = response;
    // 返答をチャット履歴に追加
    chatHistory.push_back(response);
  } else {
    response = "busy";
  }
  // Serial.printf("chatHistory.max_size %d \n",chatHistory.max_size());
  // Serial.printf("chatHistory.size %d \n",chatHistory.size());
  // for (int i = 0; i < chatHistory.size(); i++)
  // {
  //   Serial.print(i);
  //   Serial.println("= "+chatHistory[i]);
  // }
  serializeJsonPretty(chat_doc, json_string);
  Serial.println("====================");
  Serial.println(json_string);
  Serial.println("====================");
  server.send(200, "text/html", String(HEAD)+String("<body>")+response+String("</body>"));
}

void exec_chatGPT(String text) {
  static String response = "";
  Serial.println(InitBuffer);
  init_chat_doc(InitBuffer.c_str());
  // 質問をチャット履歴に追加
  chatHistory.push_back(text);
   // チャット履歴が最大数を超えた場合、古い質問と回答を削除
  if (chatHistory.size() > MAX_HISTORY * 2)
  {
    chatHistory.pop_front();
    chatHistory.pop_front();
  }

  for (int i = 0; i < chatHistory.size(); i++)
  {
    JsonArray messages = chat_doc["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    if(i % 2 == 0) {
      systemMessage1["role"] = "user";
    } else {
      systemMessage1["role"] = "assistant";
    }
    systemMessage1["content"] = chatHistory[i];
  }

  String json_string;
  serializeJson(chat_doc, json_string);
  if(speech_text=="" && speech_text_buffer == "") {
    response = chatGpt(json_string);
    speech_text = response;
    // 返答をチャット履歴に追加
    chatHistory.push_back(response);
  } else {
    response = "busy";
  }
  // Serial.printf("chatHistory.max_size %d \n",chatHistory.max_size());
  // Serial.printf("chatHistory.size %d \n",chatHistory.size());
  // for (int i = 0; i < chatHistory.size(); i++)
  // {
  //   Serial.print(i);
  //   Serial.println("= "+chatHistory[i]);
  // }
  serializeJsonPretty(chat_doc, json_string);
  Serial.println("====================");
  Serial.println(json_string);
  Serial.println("====================");

}

void handle_role() {
  // ファイルを読み込み、クライアントに送信する
  server.send(200, "text/html", ROLE_HTML);
}

bool save_json(){
  // SPIFFSをマウントする
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  // JSONファイルを作成または開く
  File file = SPIFFS.open("/data.json", "w");
  if(!file){
    Serial.println("Failed to open file for writing");
    return false;
  }

  // JSONデータをシリアル化して書き込む
  serializeJson(chat_doc, file);
  file.close();
  return true;
}

/**
 * アプリからテキスト(文字列)と共にRoll情報が配列でPOSTされてくることを想定してJSONを扱いやすい形に変更
 * 出力形式をJSONに変更
*/
void handle_role_set() {
  // POST以外は拒否
  if (server.method() != HTTP_POST) {
    return;
  }
  String role = server.arg("plain");
  if (role != "") {
//    init_chat_doc(InitBuffer.c_str());
    init_chat_doc(json_ChatString.c_str());
    JsonArray messages = chat_doc["messages"];
    JsonObject systemMessage1 = messages.createNestedObject();
    systemMessage1["role"] = "system";
    systemMessage1["content"] = role;
//    serializeJson(chat_doc, InitBuffer);
  } else {
    init_chat_doc(json_ChatString.c_str());
  }
  //会話履歴をクリア
  chatHistory.clear();

  InitBuffer="";
  serializeJson(chat_doc, InitBuffer);
  Serial.println("InitBuffer = " + InitBuffer);
  Role_JSON = InitBuffer;

  // JSONデータをspiffsへ出力する
  save_json();

  // 整形したJSONデータを出力するHTMLデータを作成する
  String html = "<html><body><pre>";
  serializeJsonPretty(chat_doc, html);
  html += "</pre></body></html>";

  // HTMLデータをシリアルに出力する
  Serial.println(html);
  server.send(200, "text/html", html);
//  server.send(200, "text/plain", String("OK"));
};

// 整形したJSONデータを出力するHTMLデータを作成する
void handle_role_get() {

  String html = "<html><body><pre>";
  serializeJsonPretty(chat_doc, html);
  html += "</pre></body></html>";

  // HTMLデータをシリアルに出力する
  Serial.println(html);
  server.send(200, "text/html", String(HEAD) + html);
};

void printDebug(const char *str) {
#ifdef DEBUG
  Serial.println(str);
#ifdef USE_WIFI
  uint8_t buf[BUFFER_LEN];
  memcpy(buf, str, BUFFER_LEN);
  peerClients();
  sendData(buf);
#endif
#endif
}

void printFreeHeap() {
    char buf[250];
    sprintf(buf, "Free Heap Size = %d\n", esp_get_free_heap_size());
    printDebug(buf);
    sprintf(buf, "Total PSRAM Size = %d\n", ESP.getPsramSize());
    printDebug(buf);
    sprintf(buf, "Free PSRAM Size = %d\n", ESP.getFreePsram());
    printDebug(buf);

}

// Start----- Task functions ----------
void lipsync(void *args) {
  // スレッド内でログを出そうとすると不具合が起きる場合があります。
  DriveContext * ctx = reinterpret_cast<DriveContext *>(args);
  ImageAvatarLite *avatar = ctx->getAvatar();
  for(;;) {
     uint64_t level = 0;
    auto buf = out.getBuffer();
    if (buf) {
#ifdef USE_LED
      if (mp3->isRunning()) {
            // buf[0]: LEFT
            // buf[1]: RIGHT
            switch(system_config.getLedLR()) {
              case 1: // Left Only
                level_led(abs(buf[0])*30/INT16_MAX,abs(buf[0])*30/INT16_MAX);
                break;
              case 2: // Right Only
                level_led(abs(buf[1])*30/INT16_MAX,abs(buf[1])*30/INT16_MAX);
                break;
              default: // Stereo
                level_led(abs(buf[1])*30/INT16_MAX,abs(buf[0])*30/INT16_MAX);
                break;
            }
      }
#endif
      memcpy(raw_data, buf, WAVE_SIZE * 2 * sizeof(int16_t));
      fft.exec(raw_data);
      // リップシンクで抽出する範囲はここで指定(低音)0〜64（高音）
      // 抽出範囲を広げるとパフォーマンスに影響します。
      for (size_t bx = 5; bx <= 32; ++bx) { 
        int32_t f = fft.get(bx);
        level += abs(f);
        //Serial.printf("bx:%d, f:%d\n", bx, f) ;
      }
      //Serial.printf("level:%d\n", level >> 16);
    }

    //Serial.printf("data=%d\n\r", level >> 16);
    mouth_ratio = (float)(level >> 16)/lipsync_level_max;
    if (mouth_ratio > 1.2f) {
      if (mouth_ratio > 1.5f) {
        lipsync_level_max += 10.0f; // リップシンク上限を大幅に超えるごとに上限を上げていく。
      }
      mouth_ratio = 1.2f;
    }
    avatar->setMouthOpen(mouth_ratio);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }   
}

#ifdef USE_SERVO
void servoloop(void *args) {
  long move_time = 0;
  long interval_time = 0;
  long move_x = 0;
  long move_y = 0;
  bool sing_mode = false;
  for (;;) {
    if (mouth_ratio == 0.0f) {
      // 待機時の動き
      interval_time = random(interval_min, interval_max);
      move_time = random(interval_move_min, interval_move_max);
      lipsync_level_max = LIPSYNC_LEVEL_MAX; // リップシンク上限の初期化
      sing_mode = false;

    } else {
      // 歌うモードの動き
      interval_time = random(sing_interval_min, sing_interval_max);
      move_time = random(sing_move_min, sing_move_max);
      sing_mode = true;
    } 
    
//    Serial.printf("x:%f:y:%f\n", gaze_x, gaze_y);
    // X軸は90°から+-でランダムに左右にスイング
    int random_move = random(15);  // ランダムに15°まで動かす
    int direction = random(2);
    if (direction == 0) {
      move_x = 90 - mouth_ratio * 15 - random_move;
    } else {
      move_x = 90 + mouth_ratio * 15 + random_move;
    }
    // Y軸は90°から上にスイング（最大35°）
    move_y = 90 - mouth_ratio * 10 - random_move;
    servo.moveXY(move_x, move_y, move_time, move_time);
    if (sing_mode) {
      // 歌っているときはうなずく
      servo.moveXY(move_x, move_y + 10, 400, 400);
    }
    vTaskDelay(interval_time/portTICK_PERIOD_MS);

  }
}
#endif

void speech(void *args);
void startThreads() {
#ifdef USE_SERVO
  //servo.check();
  delay(2000);
  xTaskCreateUniversal(servoloop,
                        "servoloop",
                        4096,
                        NULL,
                        3,
//                        3,
                        &servoloopTaskHangle,
                        APP_CPU_NUM);
  servo_enable = system_config.getServoRandomMode();
 // サーボの動きはservo_enableで管理
  if (servo_enable) {
    vTaskResume(servoloopTaskHangle);
  } else {
    vTaskSuspend(servoloopTaskHangle);
  }
#endif
  xTaskCreatePinnedToCore(speech,
                          "speech",
                           1024*20, 
                           NULL, 
                           2, 
                           NULL, 
                           1);
}

void playMP3(AudioFileSourceBuffer *buff){
  mp3->begin(buff, &out);
}

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}

// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}

String SpeechToText(bool isGoogle){
  Serial.println("\r\nRecord start!\r\n");

  String ret = "";
  if( isGoogle) {
    Audio* audio = new Audio();
    audio->Record();  
    Serial.println("Record end\r\n");
    Serial.println("音声認識開始");
    //avatar.setSpeechText("わかりました");  
    status_led(CRGB::Orange);
    CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(root_ca_google, STT_API_KEY.c_str());
    ret = cloudSpeechClient->Transcribe(audio);
    delete cloudSpeechClient;
    delete audio;
  } else {
    AudioWhisper* audio = new AudioWhisper();
    audio->Record();  
    Serial.println("Record end\r\n");
    Serial.println("音声認識開始");
    //avatar.setSpeechText("わかりました");  
    status_led(CRGB::Orange);
    Whisper* cloudSpeechClient = new Whisper(root_ca_openai, OPENAI_API_KEY.c_str());
    ret = cloudSpeechClient->Transcribe(audio);
    delete cloudSpeechClient;
    delete audio;
  }
  return ret;
}

void speech(void *args)
{
  while(1){
    if(speech_text != ""){
      //avatar.setExpression(Expression::Happy);
      speech_text_buffer = speech_text;
      speech_text = "";
      Voicevox_tts((char*)speech_text_buffer.c_str(), (char*)TTS_PARMS.c_str());
      delay(500);
      while(1){
        if (mp3->isRunning()) {
          if (!mp3->loop()) {
            mp3->stop();
            if(file != nullptr){delete file; file = nullptr;}
            Serial.println("mp3 stop");
            //avatar.setExpression(Expression::Neutral);
//            fill_solid(leds, NUM_LEDS, CRGB::Black);
            speech_text_buffer = "";
            break;
          }
        }
        delay(10);
      }
    } else {
      delay(100);
    }
  }
}

struct box_t
{
  int x;
  int y;
  int w;
  int h;
  int touch_id = -1;

  void setupBox(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
  }
  bool contain(int x, int y)
  {
    return this->x <= x && x < (this->x + this->w)
        && this->y <= y && y < (this->y + this->h);
  }
};
//static box_t box_servo;
static box_t box_stt;
static box_t box_BtnA;
static box_t box_BtnC;

void setup() {
  auto cfg = M5.config();
#ifdef ARDUINO_M5STACK_FIRE
  cfg.internal_imu = false; // サーボの誤動作防止(Fireは21,22を使うので干渉するため)
#endif
  M5.begin(cfg);
  preallocateBuffer = (uint8_t *)heap_caps_malloc(preallocateBufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
//  preallocateBuffer = (uint8_t *)malloc(preallocateBufferSize);
  if (!preallocateBuffer) {
    M5.Display.printf("FATAL ERROR:  Unable to preallocate %d bytes for app\n", preallocateBufferSize);
    for (;;) { delay(1000); }
  }

  { /// custom setting
    auto spk_cfg = M5.Speaker.config();
    /// Increasing the sample_rate will improve the sound quality instead of increasing the CPU load.
    spk_cfg.sample_rate = 96000; // default:64000 (64kHz)  e.g. 48000 , 50000 , 80000 , 96000 , 100000 , 128000 , 144000 , 192000 , 200000
    spk_cfg.task_pinned_core = PRO_CPU_NUM;//APP_CPU_NUM;
    spk_cfg.task_priority = 1;//configMAX_PRIORITIES - 2;
    spk_cfg.dma_buf_count = 8;
    //spk_cfg.stereo = true;
    spk_cfg.dma_buf_len = 512;
    M5.Speaker.config(spk_cfg);
  }
  //checkSDUpdater( SD, MENU_BIN, 2000, TFCARD_CS_PIN ); // Filesystem, Launcher bin path, Wait delay
  xMutex = xSemaphoreCreateMutex();
  SPIFFS.begin();
  SD.begin(GPIO_NUM_4, SPI, 25000000);
  
  system_config.loadConfig(json_fs, avatar_system_json);
  system_config.printAllParameters();
  M5.Speaker.setVolume(system_config.getVolume());
  Serial.printf("SystemVolume: %d\n", M5.Speaker.getVolume());
  M5.Speaker.setChannelVolume(m5spk_virtual_channel, system_config.getVolume());
  Serial.printf("ChannelVolume: %d\n", M5.Speaker.getChannelVolume(m5spk_virtual_channel));
  M5.Lcd.setBrightness(system_config.getLcdBrightness());
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillScreen(BLACK);
  
#ifdef USE_SERVO
  // 2022.4.26 ServoConfig.jsonを先に読まないと失敗する。（原因不明）
  
  servo.init(json_fs, system_config.getServoJsonFilename().c_str());
  servo.attachAll();
#endif

  delay(500);
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(system_config.getWiFiSSID().c_str(), system_config.getWiFiPass().c_str());
  // Try forever
  M5.Lcd.print("Connecting to WiFi");
  Serial.printf_P(PSTR("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    M5.Lcd.print(".");
    delay(250);
  }
  Serial.printf_P(PSTR("\nConnected\n"));
  M5.Lcd.println("");    Serial.println("");
  M5.Lcd.println("Connected");
  Serial.printf_P(PSTR("Go to http://"));
  M5.Lcd.print("Go to http://");
  Serial.println(WiFi.localIP());
  M5.Lcd.println(WiFi.localIP());
  M5.Speaker.tone(1000, 100);
  delay(1000);

  init_chat_doc(json_ChatString.c_str());
  // SPIFFSをマウントする
  if(SPIFFS.begin(true)){
    // JSONファイルを開く
    File file = SPIFFS.open("/data.json", "r");
    if(file){
      DeserializationError error = deserializeJson(chat_doc, file);
      if(error){
        Serial.println("Failed to deserialize JSON");
        init_chat_doc(json_ChatString.c_str());
      }
      serializeJson(chat_doc, InitBuffer);
      Role_JSON = InitBuffer;
      String json_str; 
      serializeJsonPretty(chat_doc, json_str);  // 文字列をシリアルポートに出力する
      Serial.println(json_str);
    } else {
      Serial.println("Failed to open file for reading");
      init_chat_doc(json_ChatString.c_str());
    }
  } else {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  // And as regular external functions:
  server.on("/speech", handle_speech);
  //server.on("/face", handle_face);
  server.on("/chat", handle_chat);
  server.on("/role", handle_role);
  server.on("/role_set", HTTP_POST, handle_role_set);
  server.on("/role_get", handle_role_get);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  M5.Lcd.println("HTTP server started");  
  
  Serial.printf_P(PSTR("/ to control the chatGpt Server.\n"));
  M5.Lcd.print("/ to control the chatGpt Server.\n");
  delay(3000);

  mp3 = new AudioGeneratorMP3();
//  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");

#ifdef USE_LED
  FastLED.addLeds<SK6812, LED_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
  FastLED.setBrightness(32);
  level_led(5, 5);
  delay(1000);
  turn_off_led();
#endif

  auto_power_off_time = system_config.getAutoPowerOffTime();
  String avatar_filename = system_config.getAvatarJsonFilename(avatar_count);
  int no = (int)system_config.getTtsSpeakerNo();
  if(no > 60) no = 3;
  TTS_SPEAKER_NO = String(no);
  TTS_PARMS = TTS_SPEAKER + TTS_SPEAKER_NO;
  avatar.init(&gfx, avatar_filename.c_str(), false, 0);
  avatar.start();
  // audioの再生より、リップシンクを優先するセッティングにしています。
  // 音のズレや途切れが気になるときは下記のlipsyncのtask_priorityを3にしてください。(口パクが遅くなります。)
  // I2Sのバッファ関連の設定を調整する必要がある場合もあり。
  avatar.addTask(lipsync, "lipsync", 2);
  //a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  //a2dp_sink.setHvtEventCallback(hvt_event_callback);
  //a2dp_sink.start(system_config.getBluetoothDeviceName().c_str(), system_config.getBluetoothReconnect());
  startThreads();

  OPENAI_API_KEY = system_config.getOpenaiApiKey();
  VOICEVOX_API_KEY = system_config.getVoicevoxApiKey();
  STT_API_KEY = system_config.getSttApiKey();

  box_stt.setupBox(0, 0, M5.Display.width(), 60);
  box_BtnA.setupBox(0, 100, 40, 60);
  box_BtnC.setupBox(280, 100, 40, 60);
}


String random_words[18] = {"あなたは誰","楽しい","怒った","可愛い","悲しい","眠い","ジョークを言って","泣きたい","怒ったぞ","こんにちは","お疲れ様","詩を書いて","疲れた","お腹空いた","嫌いだ","苦しい","俳句を作って","歌をうたって"};
int random_time = -1;
bool random_speak = true;
static int lastms1 = 0;

void report_batt_level(){
  char buff[100];
   int level = M5.Power.getBatteryLevel();
  if(M5.Power.isCharging())
    sprintf(buff,"充電中、バッテリーのレベルは%d％です。",level);
  else
    sprintf(buff,"バッテリーのレベルは%d％です。",level);
   //avatar.setExpression(Expression::Happy);
  speech_text = String(buff);
  //Voicevox_tts(buff, (char*)TTS_PARMS.c_str());
   //avatar.setExpression(Expression::Neutral);
  status_led(CRGB::Blue);
  Serial.println("mp3 begin");
}

void switch_monologue_mode(){
    String tmp;
    if(random_speak) {
      tmp = "独り言始めます。";
      lastms1 = millis();
      random_time = 40000 + 1000 * random(30);
    } else {
      tmp = "独り言やめます。";
      random_time = -1;
    }
    random_speak = !random_speak;
     //avatar.setExpression(Expression::Happy);
    speech_text = tmp;
    //Voicevox_tts((char*)tmp.c_str(), (char*)TTS_PARMS.c_str());
     //avatar.setExpression(Expression::Neutral);
    status_led(CRGB::Blue);
    Serial.println("mp3 begin");
}

void loop()
{
  static int lastms = 0;

  if (random_time >= 0 && millis() - lastms1 > random_time)
  {
    lastms1 = millis();
    random_time = 40000 + 1000 * random(30);
    if (!mp3->isRunning() && speech_text=="" && speech_text_buffer == "") {
      status_led(CRGB::Blue);
      exec_chatGPT(random_words[random(18)]);
    }
  }

  M5.update();
#if defined(ARDUINO_M5STACK_Core2) || defined( ARDUINO_M5STACK_CORES3 )
  auto count = M5.Touch.getCount();
  if (count)
  {
    auto t = M5.Touch.getDetail();
    if (t.wasPressed())
    {          
      if (box_stt.contain(t.x, t.y)&&(!mp3->isRunning()))
      {
        M5.Speaker.tone(1000, 100);
        delay(200);
        M5.Speaker.end();
        // /bool prev_servo_home = servo_home;
        random_speak = true;
        random_time = -1;
         //avatar.setExpression(Expression::Happy);
        //avatar.setSpeechText("御用でしょうか？");
        status_led(CRGB::Magenta);
        M5.Speaker.end();
        String ret;
        if(OPENAI_API_KEY != STT_API_KEY){
          Serial.println("Google STT");
          ret = SpeechToText(true);
        } else {
          Serial.println("Whisper STT");
          ret = SpeechToText(false);
        }
        Serial.println("音声認識終了");
        Serial.println("音声認識結果");
        if(ret != "") {
          status_led(CRGB::LightGreen);
          Serial.println(ret);
          if (!mp3->isRunning() && speech_text=="" && speech_text_buffer == "") {
            exec_chatGPT(ret);
            status_led(CRGB::Blue);
          }
        } else {
          Serial.println("音声認識失敗");
            status_led(CRGB::Red);
          //avatar.setExpression(Expression::Sad);
          //avatar.setSpeechText("聞き取れませんでした");
          delay(2000);
          status_led(CRGB::Black);
          //avatar.setSpeechText("");
           //avatar.setExpression(Expression::Neutral);
        } 
        M5.Speaker.begin();
      }
      if (box_BtnA.contain(t.x, t.y))
      {
        M5.Speaker.tone(1000, 100);
        switch_monologue_mode();
      }
      if (box_BtnC.contain(t.x, t.y))
      {
        M5.Speaker.tone(1000, 100);
        report_batt_level();
      }
    }
  }
#endif

 #ifdef USE_SERVO
  if (M5.BtnA.pressedFor(2000)) {
    M5.Speaker.tone(600, 500);
    delay(500);
    M5.Speaker.tone(800, 200);
    // サーボチェックをします。
    vTaskSuspend(servoloopTaskHangle); // ランダムな動きを止める。
    servo.check();
    vTaskResume(servoloopTaskHangle);  // ランダムな動きを再開
  }
#endif 
  if (M5.BtnA.wasClicked()) {
    // アバターを変更します。
    avatar_count++;
    if (avatar_count >= system_config.getAvatarMaxCount()) {
      avatar_count = 0;
    }
    Serial.printf("Avatar No:%d\n", avatar_count);
    M5.Speaker.tone(600, 100);
    avatar.changeAvatar(system_config.getAvatarJsonFilename(avatar_count).c_str());
  }

#ifdef USE_SERVO
  if (M5.BtnB.pressedFor(2000)) {
    // サーボを動かす＜＞止めるの切り替え
    servo_enable = !servo_enable;
    Serial.printf("BtnB was pressed servo_enable:%d", servo_enable);
    if (servo_enable) {
      M5.Speaker.tone(500, 200);
      delay(200);
      M5.Speaker.tone(700, 200);
      delay(200);
      M5.Speaker.tone(1000, 200);
      vTaskResume(servoloopTaskHangle);
    } else {
      M5.Speaker.tone(1000, 200);
      delay(200);
      M5.Speaker.tone(700, 200);
      delay(200);
      M5.Speaker.tone(500, 200);
      vTaskSuspend(servoloopTaskHangle);
    }
  }
#endif

  if (M5.BtnB.wasClicked()) {
    size_t v = M5.Speaker.getChannelVolume(m5spk_virtual_channel);
    v += 10;
    if (v <= 255) {
      M5.Speaker.setVolume(v);
      M5.Speaker.setChannelVolume(m5spk_virtual_channel, v);
      Serial.printf("Volume: %d\n", v);
      Serial.printf("SystemVolume: %d\n", M5.Speaker.getVolume());
      M5.Speaker.tone(1000, 100);
    }
  }

  if (M5.BtnC.pressedFor(2000)) {
    M5.Speaker.tone(1000, 100);
    delay(500);
    M5.Speaker.tone(600, 100);
    // 表情を切り替え
    expression++;
    Serial.printf("ExpressionMax:%d\n", avatar.getExpressionMax());
    if (expression >= avatar.getExpressionMax()) {
      expression = 0;
    }
    Serial.printf("----------Expression: %d----------\n", expression);
     // //avatar.setExpression(system_config.getAvatarJsonFilename(avatar_count).c_str(), expression);
    Serial.printf("Resume\n");
  }
  if (M5.BtnC.wasClicked()) {
    size_t v = M5.Speaker.getChannelVolume(m5spk_virtual_channel);
    v -= 10;
    if (v > 0) {
      M5.Speaker.setVolume(v);
      M5.Speaker.setChannelVolume(m5spk_virtual_channel, v);
      Serial.printf("Volume: %d\n", v);
      Serial.printf("SystemVolume: %d\n", M5.Speaker.getVolume());
      M5.Speaker.tone(800, 100);
    }
  }
#ifndef ARDUINO_M5STACK_FIRE // FireはAxp192ではないのとI2Cが使えないので制御できません。
//  if (M5.Power.Axp192.getACINVolatge() < 3.0f) {
  if (!M5.Power.isCharging() && M5.Power.getBatteryLevel()<80) {
    // USBからの給電が停止したとき
    // Serial.println("USBPowerUnPluged.");
    M5.Power.setLed(0);
    if ((auto_power_off_time > 0) and (last_discharge_time == 0)) {
      last_discharge_time = millis();
    } else if ((auto_power_off_time > 0) and ((millis() - last_discharge_time) > auto_power_off_time)) {
      M5.Power.powerOff();
    }
  } else {
    //Serial.println("USBPowerPluged.");
    M5.Power.setLed(80);
    if (last_discharge_time > 0) {
      last_discharge_time = 0;
    }
  }
#endif
  server.handleClient();
  delay(100);
}
 