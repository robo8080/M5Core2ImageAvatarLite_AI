# M5Core2ImageAvatarLite_AI
 AI版ImageAvatarLite for M5Stack Core2

AI版ImageAvatarLiteの特徴<br>

* 音声合成にWeb版 VOICEVOXを使います。
* 音声認識に"Google Cloud STT"か"OpenAI Whisper"のどちらかを選択できます。
<br>

Google Cloud STTは、”MhageGH”さんの [esp32_CloudSpeech](https://github.com/MhageGH/esp32_CloudSpeech/ "Title") を参考にさせて頂きました。ありがとうございました。<br>
"OpenAI Whisper"が使えるようにするにあたって、多大なご助言を頂いた”イナバ”さん、”kobatan”さんに感謝致します。<br>

---
---

### 設定方法 ###

1. dataフォルダ内にあるファイル及びフォルダをSDカードのルートにコピーしてください。
<br>
2. "/json/M5AvatarLiteSystem.json"の以下の設定を書き換えます。
    "wifi_ssid"             : "YOUR_WIFI_SSID",<br>
    "wifi_pass"             : "YOUR_WIFI_PASSWORD",<br>
    "openai_api_key"        : "SET YOUR OPENAI APIKEY",<br>
    "voicevox_api_key"      : "SET YOUR VOICEVOX APIKEY",<br>
    "stt_api_key"           : "SET YOUR STT APIKEY",<br>
    "tts_speaker_no"        : 3,    <br>
<br>
"M5AvatarLiteSystem.json"の内容

```
{
    "volume" : 180,
    "lcd_brightness" : 100,
    "avatar_json": [
        "/json/M5AvatarLite00slime.json",
        "/json/M5AvatarLite01puipui.json",
        "/json/M5AvatarLite02jacko.json",
        "/json/M5AvatarLite03girl.json",
        "/json/M5AvatarLite04robot.json"
    ],
    "wifi_ssid"             : "YOUR_WIFI_SSID",
    "wifi_pass"             : "YOUR_WIFI_PASSWORD",
    "openai_api_key"        : "SET YOUR OPENAI APIKEY",
    "voicevox_api_key"      : "SET YOUR VOICEVOX APIKEY",
    "stt_api_key"           : "SET YOUR STT APIKEY",
    "tts_speaker_no"        : 3,    
    "servo_json" : "/json/M5AvatarLiteServo.json",
    "servo_random_mode" : true,
    "auto_power_off_time" : 0,
    "led_lr" : 0
}
```

<br><br>
【注意】<br>"YOUR_STT_APIKEY"には"Google Cloud STTのAPIキー" または、"YOUR_OPENAI_APIKEY"と同じものを設定します。<br>
"YOUR_STT_APIKEY"に"YOUR_OPENAI_APIKEY"と同じものを設定した場合は音声認識にOpenAI Whisperが使われます。<br>

3. "/json/M5AvatarLiteServo.json"の設定を書き換えます。(※サーボを使う場合) <br>

### AIスタックチャンの操作方法 ###

* ｽﾀｯｸﾁｬﾝの額付近にタッチするとマイクからの録音が始まり音声認識で会話できるようになります。<br>
録音時間は7秒程度です。<br>

* ブラウザから会話できます。<br>
一時的な声の変更に、voiceパラメータを指定できます。<br>
値の一覧は一番下の話者番号一覧に有ります。<br>
例えば、次のように指定します。<br><br>
http://xxxx.xxxx.xxxx.xxxx/chat?voice=4&text=こんにちは<br>
<br>
(xxxx.xxxx.xxxx.xxxxはAIスタックチャンの起動時に表示されるIPアドレスです。)<br>

* ブラウザで"http://xxxx.xxxx.xxxx.xxxx/role"にアクセスすると、ロールを設定できます。<br>
テキストエリアに何も入力せずに送信すると、以前に設定されたロールが削除されます。<br><br>
ロール情報は自動的にspiffsに保存されます。<br>

* ブラウザで"http://xxxx.xxxx.xxxx.xxxx/role_get"にアクセスすると、現在設定しているロールを取得できます。<br>

* M5Stack Core2の画面左端中央付近にタッチすると、独り言モードをON/OFFできます。ランダムな時間間隔で、ランダムに喋ります。<br>
独り言モードでも従来通りスマホから会話できます。<br>
<br>

* M5Stack Core2の画面中央付近にタッチするとｽﾀｯｸﾁｬﾝの首振りを止められます。<br>

* M5Stack Core2の画面右端中央付近にタッチすると、音声合成のテストが出来ます。<br>

* ボタンAをクリックするとアバターを切り替えられます。。<br>

以上が、AIスタックチャンの使い方になります。<br>
これ以外の使い方はこのREADME下方の「ImageAvatarLiteの概要」を参照してください。<br>

---

### ChatGPTのAPIキー取得の参考リンク ###

* [ChatGPT API利用方法の簡単解説](https://qiita.com/mikito/items/b69f38c54b362c20e9e6/ "Title")<br>

### Web版 VOICEVOX のAPIキーの取得 ###

* Web版 VOICEVOX のAPIキーの取得方法は、このページ（[ttsQuestV3Voicevox](https://github.com/ts-klassen/ttsQuestV3Voicevox/ "Title")）の一番下の方を参照してください。)<br>

### Google Cloud Speech to TextのAPIキー取得の参考リンク ###

* [Speech-to-Text APIキーの取得／登録方法について](https://nicecamera.kidsplates.jp/help/feature/transcription/apikey/ "Title")<br>

### ChatGPTのキャラクター設定の参考リンク ###

* [ChatGPTのAPIでキャラクター設定を試してみた](https://note.com/it_navi/n/nf5f702b36a75#8e42f887-fb07-4367-9f3f-ab7f119eb064/ "Title")<br>
<br>

### VoiceVoxの話者番号 ###

* VoiceVox話者番号一覧<br>
 0:四国めたん（あまあま）<br>
 1:ずんだもん（あまあま）<br>
 2:四国めたん（ノーマル）<br>
 3:ずんだもん（ノーマル）<br>
 4:四国めたん（セクシー）<br>
 5:ずんだもん（セクシー）<br>
 6:四国めたん（ツンツン）<br>
 7:ずんだもん（ツンツン）<br>
 8:春日部つむぎ（ノーマル）<br>
 9:波音リツ（ノーマル）<br>
10:雨晴はう（ノーマル）<br>
11:玄野武宏（ノーマル）<br>
12:白上虎太郎（ふつう）<br>
13:青山龍星（ノーマル）<br>
14:冥鳴ひまり（ノーマル）<br>
15:九州そら（あまあま）<br>
16:九州そら（ノーマル）<br>
17:九州そら（セクシー）<br>
18:九州そら（ツンツン）<br>
19:九州そら（ささやき）<br>
20:もち子(cv 明日葉よもぎ)<br>
21:剣崎雌雄（ノーマル）<br>
22:ずんだもん（ささやき）<br>
23:WhiteCUL（ノーマル）<br>
24:WhiteCUL（たのしい）<br>
25:WhiteCUL（かなしい）<br>
26:WhiteCUL（びえーん）<br>
27:後鬼（人間ver.）<br>
28:後鬼（ぬいぐるみver.）<br>
29:No.7（ノーマル）<br>
30:No.7（アナウンス）<br>
31:No.7（読み聞かせ）<br>
32:白上虎太郎（わーい）<br>
33:白上虎太郎（びくびく）<br>
34:白上虎太郎（おこ）<br>
35:白上虎太郎（びえーん）<br>
36:四国めたん（ささやき）<br>
37:四国めたん（ヒソヒソ）<br>
38:ずんだもん（ヒソヒソ）<br>
39:玄野武宏（喜び）<br>
40:玄野武宏（ツンギレ）<br>
41:玄野武宏（悲しみ）<br>
42:ちび式じい（ノーマル）<br>
43:櫻歌ミコ（ノーマル）<br>
44:櫻歌ミコ（第二形態）<br>
45:櫻歌ミコ（ロリ）<br>
46:小夜/SAYO（ノーマル）<br>
47:ナースロボ＿タイプＴ（ノーマル）<br>
48:ナースロボ＿タイプＴ（楽々）<br>
49:ナースロボ＿タイプＴ（恐怖）<br>
50:ナースロボ＿タイプＴ（内緒話）<br>
51:†聖騎士 紅桜†（ノーマル）<br>
52:雀松朱司（ノーマル）<br>
53:麒ヶ島宗麟（ノーマル）<br>
54:春歌ナナ（ノーマル）<br>
55:猫使アル（ノーマル）<br>
56:猫使アル（おちつき）<br>
57:猫使アル（うきうき）<br>
58:猫使ビィ（ノーマル）<br>
59:猫使ビィ（おちつき）<br>
60:猫使ビィ（人見知り）<br>
<br><br>

---

# ImageAvatarLiteの概要

　あらかじめ用意した画像ファイル（BMP)とJSONファイルの設定を組み合わせてAvatarを作成できるアプリです。
# 開発環境
- VSCode(Ver.1.55.1)
- PlatformIO

## ArduinoIDEで使用する場合
srcフォルダとmain.cppの名前を揃えて変更してください。

### 変更例
src -> M5Core2ImageAvatarLite_AI<br>
main.cpp -> M5Core2ImageAvatarLite_AI.ino

# 必要なライブラリ
<b>Arduino-esp32はVer.2.0.3で動作確認しています。</b>

バージョンについては[platformio.ini](platformio.ini)を見てください。
- [M5Unified](https://github.com/m5stack/M5Unified)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)

## サーボを使う場合
- [ServoEasing](https://github.com/arminjo/ServoEasing)
- [ESPServo](https://github.com/madhephaestus/ESP32Servo)

## LEDを使う場合
- [FastLED](https://github.com/FastLED/FastLED)

# 対応機種
 メモリの都合上PSRAMが必要なのでM5Stack Core2~~とM5Stack Fireのみ~~を対象にしています。
 ~~4bitBMPを使用し、カラーパレットを使用することにより他の機種でも動きますが手順が複雑なのでCore2及びFireのみとします。~~

# 使い方
1. SDカードのルートにdataにあるフォルダ(bmp,json)をコピー
1. jsonフォルダの下記の2ファイルを設定します。
    - M5AvatarLiteSystem.json
    - M5AvatarLiteServo.json
1. プログラムを書き込むとAvatarが起動します。
~~1. Bluetoothスピーカーとして機能します。「ESP32」というデバイスをペアリングすると音を再生可能です。~~

## ボタン操作

- ボタンA
    - クリック<br>アバターの切り替え
    - 長押し<br>サーボテスト
- ボタンB
    - クリック<br>ボリュームアップ
    - 長押し<br>サーボ駆動のON/OFF切り替え
- ボタンC
    - クリック<br>ボリュームダウン
    - 長押し<br>アバターの表情切り替え

## SDカード上に必要なファイル

dataフォルダ内にあるファイル及びフォルダをSDカードのルートにコピーしてください。
 
 1. - /bmp_slime/<br>BMPファイル(サンプルのbmp_slimeでは全部で11ファイル)
    - /bmp_puipui/<br>
    - /bmp_jacko/<br>
 1. /json/<br>
    - M5AvatarLiteSystem.json<br>一番最初に読み込まれる設定ファイル
    - M5AvatarLite00.json<br>slimeの設定ファイル
    - M5AvatarLite01.json<br>puipuiの設定ファイル
    - M5AvatarLite02.json<br>jackolanternの設定ファイル
    - M5AvatarLiteServo.json(※サーボを使う場合)

# JSONファイルとBMPファイルの置き場所について
 main.cppの下記の行を変更するとJSONファイルとBMPファイルの収納場所をSDかSPIFFSか指定できます。SPIFFSに置くと開発するときにVSCodeからUploadできるようになり、SDカードを抜き差しして書き換える手間が省けます。
```
fs::FS json_fs = SD; // JSONファイルの収納場所(SPIFFS or SD)
fs::FS bmp_fs  = SD; // BMPファイルの収納場所(SPIFFS or SD)
```
 ## VSCodeからのデータUpload方法（英語）
 [ESP32 with VS Code and PlatformIO: Upload Files to Filesystem (SPIFFS)](https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/)

# M5AvatarLiteSystem.jsonの内容
一番最初に読み込まれる設定ファイルです。サンプルでは3つまで定義してあります。（最大8つ）
```
{
    "volume" : 100,                                // 起動時のボリューム
    "lcd_brightness" : 50,                         // LCDの明るさ
    "avatar_json": {
        "filename" : [
            "/json/M5AvatarLite00.json",           // countで設定した数に対応するAvatar定義を作成
            "/json/M5AvatarLite01.json",
            "/json/M5AvatarLite02.json"            // 3つ以上増やすときは末尾に「,」を追加
            // .... ,
            // .
            // "/json/M5AvatarLite07.json"         // 最大8まで
        ]
    },
    "wifi_ssid"             : "YOUR_WIFI_SSID",
    "wifi_pass"             : "YOUR_WIFI_PASSWORD",
    "openai_api_key"        : "SET YOUR OPENAI APIKEY",
    "voicevox_api_key"      : "SET YOUR VOICEVOX APIKEY",
    "stt_api_key"           : "SET YOUR STT APIKEY",
    "tts_speaker_no"        : 3,    
    "servo_json" : "/json/M5AvatarLiteServo.json",       // サーボの設定ファイル
    "servo_random_mode" : true,                          // 起動時にサーボを動かすかどうか
    "auto_power_off_time" : 0,                           // USBからの電源供給が止まった後にCore2の電源を切るまでの時間（0は電源OFFしません。）
     "led_lr"                                             // GoBottomのLEDBarをステレオで表示するかの切り替え（0:Stereo, 1:LeftOnly, 2:RightOnly）
}
```

 # カスタマイズ方法
 自分で24bitか16bitのBMPファイルを用意すると好きな画像をAvatar化することが可能です。

 ## 用意する画像ファイル
 サンプルの画像ファイルは全て24bitBMPファイルでWindows標準のペイントソフトを利用しています。
 ### 固定パーツ（頭の画像ファイル）
 1. 頭用BMP<br>背景となる画像ファイル320x240もしくは傾けるのであれば少し余裕が必要です。
 ![image](data/bmp_slime/head.bmp)
 ### 表情で変わるパーツ(右目と口)
 開いた状態と閉じた状態の<b>２種類×表情の数</b>だけ必要です。(同じパーツを流用も可能)
 1. 開いた目と閉じた「右目」のパーツ（左目は右目を反転させて表示します。）<br>サンプルではnormal,sad,angryの3種類用意してあります。<br>
 ![image](data/bmp_slime/eye_op_normal.bmp) ![image](data/bmp_slime/eye_cl_normal.bmp)
 1. 開いた口と閉じた口のパーツ<br>サンプルでは開いた口normal,sad,angryの3種類と閉じた口は共通パーツとして用意してあります。<br>
 ![image](data/bmp_slime/mouth_op_normal.bmp) ![image](data/bmp_slime/mouth_cl_normal.bmp)

目と口の透明化したい部分は透明色(M5AvatarLiteConfig.json)で塗りつぶします。サンプルでは緑（0x00FF00）になっています。

## JSONファイルの編集
下記を参考にして、JSONファイルを書き換えてください。
``` 
{
    "expression": [  // 表情（デフォルトは最大8パターンまで）
        "normal",
        "sad",
        "angry"
    ],

    "sprite_info": {
        "psram": "true",      // PSRAMの仕様有無（8bit以上ではtrueのまま）
        "color_depth": 16,    // 使用するBMPのカラー(4,8,16)
        "swap_bytes": 0
    },
    "color_info": {
        "skin"  : "0xFF5B00",             // 未使用
        "eye_white" : "0xFFFFFF",         // 未使用
        "transparent"    : "0x00FF00",    // 透明色
    },
    "fixed_parts": [
        {
            "parts": "head",
            "x": -10,                             // 頭部パーツの開始X座標
            "y": -10,                             // 頭部パーツの開始y座標
            "w": 340,                             // 頭部パーツの幅
            "h": 260,                             // 頭部パーツの高さ
            "filename": "/bmp_slime/head.bmp"        // 頭部パーツのファイル名
        }
    ],
    "mouth": [                                    // 口のパーツ設定
        {
            "normal": {     // 表情"normal"の口設定
                "x": 160,   // 口パーツの開始x座標
                "y": 200,   // 口パーツの開始y座標
                "w": 60,    // 口パーツの幅
                "h": 60,    // 口パーツの高さ
                "filename": {
                    "open": "/bmp_slime/mouth_op_normal.bmp",  // 開いた口
                    "close": "/bmp_slime/mouth_cl_normal.bmp"  // 閉じた口
                },
                "minScaleX": 1.0,     // 未使用
                "maxScaleX": 1.0,     // 未使用
                "minScaleY": 0.3,     // Y軸の拡大率がこれより小さくなると閉じる
                "maxScaleY": 1.0      // 未使用
            }
        },
        {
            "sad": {                              // 表情"sad"の口設定
                "x": 160,
                "y": 200,
                "w": 60,
                "h": 60,
                "filename": {
                    "open": "/bmp_slime/mouth_op_sad.bmp",
                    "close": "/bmp_slime/mouth_cl_normal.bmp"
                },
                "minScaleX": 1.0,
                "maxScaleX": 1.0,
                "minScaleY": 0.3,
                "maxScaleY": 1.0
            }
        },
        {
            "angry": {                            // 表情"angry"の口設定
                "x": 160,
                "y": 200,
                "w": 60,
                "h": 60,
                "filename": {
                    "open": "/bmp_slime/mouth_op_angry.bmp",
                    "close": "/bmp_slime/mouth_cl_normal.bmp"
                },
                "minScaleX": 1.0,
                "maxScaleX": 1.0,
                "minScaleY": 0.3,
                "maxScaleY": 1.0
            }
        }
    ],
    "eye": [ // -------------------------------- 目の設定
        {
            "normal": {        // 表情"normal"の目設定
                "rx": 120,     //       右目開始X座標
                "ry": 100,     //       右目開始Y座標
                "lx": 200,     //       左目開始X座標
                "ly": 100,     //       左目開始Y座標
                "w": 40,       //       目の幅
                "h": 60,       //       目の高さ      
                "filename": {
                    "open": "/bmp_slime/eye_op_normal.bmp", // 開いた目のファイル名
                    "close": "/bmp_slime/eye_cl_normal.bmp" // 閉じた目のファイル名
                },
                "minScaleX": 1.0,     // 未使用
                "maxScaleX": 1.0,     // 未使用
                "minScaleY": 0.3,     // 拡大倍率がこれより小さくなると閉じる
                "maxScaleY": 1.0      // 未使用
            }
        },
        {
            "sad": {           // 表情"sad"の目設定
                "rx": 120,
                "ry": 100,
                "lx": 200,
                "ly": 100,
                "w": 40,
                "h": 60,
                "filename": {
                    "open": "/bmp_slime/eye_op_sad.bmp",
                    "close": "/bmp_slime/eye_cl_sad.bmp"
                },
                "minScaleX": 1.0,
                "maxScaleX": 1.0,
                "minScaleY": 0.3,
                "maxScaleY": 1.0
            }
        },
        {
            "angry": {         // 表情"angry"の目設定
                "rx": 120,
                "ry": 100,
                "lx": 200,
                "ly": 100,
                "w": 40,
                "h": 60,
                "filename": {
                    "open": "/bmp_slime/eye_op_angry.bmp",
                    "close": "/bmp_slime/eye_cl_angry.bmp"
                },
                "minScaleX": 1.0,
                "maxScaleX": 1.0,
                "minScaleY": 0.3,
                "maxScaleY": 1.0
            }
        }
    ],
    "init_param": [                   // 初期設定（特に変更は必要ないですが表情の数必要です。）
        {
            "normal": {
                "eye_l_ratio": 0.0,  // 左目を開く倍率
                "eye_r_ratio": 0.0,  // 右目を開く倍率
                "mouth_ratio": 0.0,  // 口を開く倍率
                "angle": 0.0,        // Avatarの角度
                "breath": 0          // Avatarが息使いする高さ
            }
        },
        {
            "sad": {
                "eye_l_ratio": 0.0,
                "eye_r_ratio": 0.0,
                "mouth_ratio": 0.0,
                "angle": 0.0,
                "breath": 0
            }
        },
        {
            "angry": {
                "eye_l_ratio": 0.0,
                "eye_r_ratio": 0.0,
                "mouth_ratio": 0.0,
                "angle": 0.0,
                "breath": 0
            }
        }
    ]
}
```



# サーボについて
main.cpp冒頭の#define USE_SERVOのコメントを外すとサーボを利用できます。2021/11現在では[ｽﾀｯｸﾁｬﾝ](https://github.com/meganetaaan/stack-chan)での利用を想定しています。

## サーボの初期設定
X軸とY軸の2軸（ｽﾀｯｸﾁｬﾝのパン(x)とチルト(y)）で利用できます。/json/フォルダにM5AvatarLiteServo.jsonを置いてください。

[![M5Core2ImageAvatarLite](https://img.youtube.com/vi/07fEke_r3Xc/0.jpg)](https://www.youtube.com/watch?v=07fEke_r3Xc)


## M5AvatarLiteServoConfig.jsonの内容
```
{
    "initial_settings": {
        "x_axis": {            // パン
            "pin"    : 13,     // 水平方向のサーボピン番号
            "center" : 85,     // サーボの中心（初期位置）
            "lower"  : 0,      // サーボの下限角度
            "uppder"  : 180,   // サーボの上限角度
            "offset" : 0       // サーボのオフセット
        },
        "y_axis": {            // チルト
            "pin"    : 14,     // 垂直方向のサーボ品番号
            "center" : 60,     // サーボの中心(初期位置)
            "lower"  : 30,     // サーボの下限角度
            "upper"  : 90,     // サーボの上限角度
            "offset" : 0       // サーボのオフセット
        }

    },
    "servo_enable" : "false"   // 起動時にサーボを動かすかどうかのフラグ（未使用）
}
```

# 参考にしたリポジトリ
- [m5stack-avatar](https://github.com/meganetaaan/m5stack-avatar)
- [M5Stack_WebRadio_Avator](https://github.com/robo8080/M5Stack_WebRadio_Avator)

# 謝辞
このソフトを作成するにあたり、動きや構造の元となった[M5Stack-Avatar](https://github.com/meganetaaan/m5stack-avatar)を作成・公開してくださった[meganetaaan](https://github.com/meganetaaan)氏に感謝いたします。

ImageAvatarを実現するにあたり優れたパフォーマンス、機能を持ったLovyanGFX,M5Unifiedの作者[lovyan03](https://github.com/lovyan03)氏に感謝いたします。

ImageAvatar作成するにあたり、 初期の頃からたくさんのアドバイスを頂き、参考にさせていただいた[M5Stack_WebRadio_Avatar](https://github.com/robo8080/M5Stack_WebRadio_Avator)の作者[robo8080](https://github.com/robo8080)氏に感謝いたします。

色々なアドバイスを頂いた[tobozo](https;//github.com/tobozo)氏に感謝いたします。

# Credit
- [meganetaaan](https://github.com/meganetaaan)
- [lovyan03](https://github.com/lovyan03/LovyanGFX)
- [robo8080](https://github.com/robo8080)
- [tobozo](https://github.com/tobozo)

# LICENSE
[MIT](LICENSE)

# Author
[Takao Akaki](https://github.com/mongonta0716)
