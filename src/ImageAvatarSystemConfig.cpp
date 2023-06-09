#include "ImageAvatarSystemConfig.h"


ImageAvatarSystemConfig::ImageAvatarSystemConfig() {
    _volume = 180;
    _lcd_brightness = 50;
    _avatar_count = 0;
    _servo_random_mode = true;
    _auto_power_off_time = 0;
    _led_lr = 0;
    _tts_speaker_no = 3;
    _wifi_ssid="YOUR_WIFI_SSID";
    _wifi_pass="YOUR_WIFI_PASSWORD";
    _openai_api_key="SET YOUR OPENAI APIKEY";
    _voicevox_api_key="SET YOUR VOICEVOX APIKEY";
    _stt_api_key="SET YOUR STT APIKEY";
    _role="";
};

ImageAvatarSystemConfig::~ImageAvatarSystemConfig() {};

void ImageAvatarSystemConfig::loadConfig(fs::FS& fs, const char *json_filename) {
    Serial.printf("----- ImageAvatarSystemConfig::loadConfig:%s\n", json_filename);
    File file = fs.open(json_filename);
    int res = file.available();
    Serial.printf("file:available:%d\n", res);
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.printf("json file read error: %s\n", json_filename);
        Serial.printf("error%s\n", error.c_str());
    }
    setSystemConfig(doc);

    file.close();
#ifdef DEBUG
     printAllParameters();
#endif
}

void ImageAvatarSystemConfig::setSystemConfig(DynamicJsonDocument doc) {

    _volume = doc["volume"];
    _lcd_brightness = doc["lcd_brightness"];
    JsonArray avatar_json = doc["avatar_json"];
    _avatar_count = avatar_json.size();

    for (int i=0; i<_avatar_count; i++) {
        _avatar_jsonfiles[i] = avatar_json[i].as<String>();
    }
    // _bluetooth_device_name = doc["bluetooth_device_name"].as<String>();
    // _bluetooth_reconnect = doc["bluetooth_reconnect"];
    _wifi_ssid = doc["wifi_ssid"].as<String>();
    _wifi_pass = doc["wifi_pass"].as<String>();
    _openai_api_key = doc["openai_api_key"].as<String>();
    _voicevox_api_key = doc["voicevox_api_key"].as<String>();
    _stt_api_key = doc["stt_api_key"].as<String>();
    _tts_speaker_no = doc["tts_speaker_no"];
    _role = doc["role"].as<String>();
    _servo_jsonfile = doc["servo_json"].as<String>(); 
    _servo_random_mode = doc["servo_random_mode"];
    _auto_power_off_time = doc["auto_power_off_time"];
    _led_lr = doc["led_lr"];
}

void ImageAvatarSystemConfig::printAllParameters() {
    Serial.printf("Volume: %d\n", _volume);
    Serial.printf("Brightness: %d\n", _lcd_brightness);
    Serial.printf("Avatar Max Count: %d\n", _avatar_count);
    for (int i=0; i<_avatar_count; i++) {
        Serial.printf("Avatar Json FileName:%d :%s\n", i, (const char *)_avatar_jsonfiles[i].c_str());
    }
    // Serial.printf("Bluetooth Device Name: %s\n", _bluetooth_device_name);
    // Serial.printf("Bluetooth Reconnect :%s\n", _bluetooth_reconnect ? "true" : "false");
    Serial.printf("Servo Json FileName: %s\n", (const char *)_servo_jsonfile.c_str());
    Serial.printf("Servo Random Mode:%s\n", _servo_random_mode ? "true" : "false");
    Serial.printf("AutoPowerOffTime: %d\n", _auto_power_off_time);
    Serial.printf("LED LR: %d\n", _led_lr);
    Serial.printf("Wi-Fi SSID: %s\n", (const char *)_wifi_ssid.c_str());
    Serial.printf("Wi-Fi PASS: %s\n", (const char *)_wifi_pass.c_str());
    Serial.printf("OpenAI API Key: %s\n", (const char *)_openai_api_key.c_str());
    Serial.printf("VOICEVOX API Key: %s\n", (const char *)_voicevox_api_key.c_str());
    Serial.printf("STT API Key: %s\n", (const char *)_stt_api_key.c_str());
    Serial.printf("TTS Speaker No.: %d\n", _tts_speaker_no);
    Serial.printf("Role: %s\n", (const char *)_role.c_str());
}