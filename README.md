# Twilio Super SIM と M5Stack で SMS を送信する

M5Stack に M5Stack 3G ボードを組み合わせ、SIM には Twilio SuperSIM を使います。  
M5Stack の開発環境は、Visual Studio Code と、PlatformIO で構築してください。

動作としては、M5Stack のボタン C（一番右側）を押すと、Twilio Functions を呼び出し、Twilio Functions から SMS を送信します。  
Twilio Functions 側のプログラムは、こちらのレポジトリにあります。

メインのプログラムは、src の中にある、main.cpp です。
また、個別の設定情報を、src/twilio-config.h に記述する必要があります。  
サンプルとして、twilio-config.example.h を用意してありますので、名前を変更して内容を更新してください。

| 変数名     | 設定値                                       |
| :--------- | :------------------------------------------- |
| SERVER_URL | Twilio 側で準備した SMS 送信プログラムの URL |
| SIGNATURE  | 送信時に設定する X-Twilio-Signature の値     |

X-Twilio-Signature の値は、サーバー用レポジトリ内にある`calcSign.js`を使って計算します。  
詳しくはサーバー用レポジトリの`README.md`をご覧ください。
