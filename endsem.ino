#include <WiFi.h>
#include <HTTPClient.h>
#include <UniversalTelegramBot.h>
#define BOT_TOKEN "5865323482:AAFrIpHgHKVJ0k-TxJ8LqrTt3Opc28zj7Z0"
// id = 5865323482
#include <WiFiClientSecure.h>

WiFiClientSecure secure_client;
UniversalTelegramBot bot(BOT_TOKEN, secure_client);

const unsigned long BOT_interval = 1000; 

unsigned long bot_last;
int balance = 15000;
const char* ssid = "Harsh_sharma";
const char* password = "hulahoop";
String input_id;
int user_withdraw;
int user_deposit;
String msg_prev;
int login_status;
String GOOGLE_SCRIPT_ID1 = "AKfycbzcIGv-vg0UK5VUUTM4g4SoYJ07PrY60mIsR3rsNCz9ZcrZRwBqC9fFdOCZoZ6QZVyQ-Q";
String GOOGLE_SCRIPT_ID2 = "AKfycbxpTSakCgobU83Zqz2_lbEYvHpa4kOgs0GEbmcRnGYCRLg7Xn4Jr3JbZPgaPbq0OpOx1A";

void update_spreadsheet(String action, String value)
{
  if (WiFi.status() == WL_CONNECTED) {
    String url_Final = "https://script.google.com/macros/s/"+ GOOGLE_SCRIPT_ID2 +"/exec?"+"Action=" + String(action) + "&Amount=" + String(value) +"&Balance=" + String(balance);
    Serial.print("POST data to spreadsheet:");
    Serial.println(url_Final);
    HTTPClient http;
    http.begin(url_Final.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS) ;
    int httpCode = http.GET() ;
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    String out = "Your"+ action+ " transaction was successful. The avalaible balance is"+balance+".";
    Serial.println(out);
    http.end();
  }
  delay(1000);
}

String login_user()
{
  String output;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID1 + "/exec?read";
    Serial.println("Making a request");
    http.begin(url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
      output = payload;
      }
    else {
      Serial.println("Error on sending request");
    }
    http.end();
  }
  delay(1000);
  Serial.println(output);
  return output;
}

void withdraw(int amt)
{
    balance = balance - amt;
}

void deposit(int amt)
{
    balance = balance + amt;
}
void messagehandler(int msgs)
{
  Serial.print("Currently Handling Messages: ");
  Serial.println(msgs);
  int i;
  String answer;
  for (i = 0; i < msgs; i++)
  {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    if (msg.text == "/help")
      {
        answer = "Welcome to this ATM Please use these commands to get started\n";
        answer +="\login \n";
        answer +="\withdraw \n";
        answer +="\deposit \n";
      }
    else if (msg.text == "/login"){
      msg_prev = msg.text;
      if (login_status==0){
      answer = "Welcome to our Custom ATM! Nice to meet you *" + msg.from_name + "* Enter your username and password";
      }
      else
      answer = "You are already logged in";
    }
    else if (msg.text == "/logout"){
      msg_prev = msg.text;
      if (login_status==1){
      answer = "Thank you for visting";
      login_status = 0;
      }
      else
      answer = "You are already logged out from session";
    }
    else if(msg_prev == "/login" && (msg.text == login_user()) ){
      msg_prev = msg.text;
      login_status = 1;
      answer = "Welcome *" + msg.from_name + "* What operation would you like to perform? \n Select \balance or \deposit" ;
    }
    else if (msg.text == "/status"){
      msg_prev = msg.text;
      answer = "Currently I am a ATM.";
    }
    else if (msg.text == "/balance")
    {
      msg_prev = msg.text;
      answer = "Your Current Balance is";
      bot.sendMessage(msg.chat_id, answer, "Markdown");
      bot.sendMessage(msg.chat_id, String(balance), "Markdown");
      continue;
    }
    else if (msg.text == "/withdraw")
    {
      msg_prev = msg.text;
      answer = "Enter the amount for withdrawl \n Note: The amount  withdrawn should be in multiple of hundred.";
      bot.sendMessage(msg.chat_id, answer, "Markdown");
      continue;
    }
    else if (msg_prev== "/withdraw")
    {
      msg_prev = msg.text;
      user_withdraw = msg.text.toInt();
      if (user_withdraw < balance)
        withdraw(user_withdraw);
      else
      {
        answer = "Insufficient Balance. You will be logged out from the session.";
        login_status = 0;
        bot.sendMessage(msg.chat_id, answer, "Markdown");
        continue;
      }
      answer = "Withdrawal Successful Please collect your cash!";
      bot.sendMessage(msg.chat_id, answer, "Markdown");
      update_spreadsheet("Debit",msg.text);
      continue;
    }
    else if (msg.text == "/deposit")
    {
     msg_prev = msg.text;
     answer = "Enter the amount to deposit in your account \n Note: The amount to deposit should be in multiple of hundred.";
    }
    else if (msg_prev== "/deposit")
    {
      msg_prev = msg.text;
      user_deposit = msg.text.toInt();
      if (user_deposit<10000)
        deposit(user_deposit);
      else
      {
        answer = "Limit Exceeded. You will be logged out from the session.";
        login_status = 0;
        bot.sendMessage(msg.chat_id, answer, "Markdown");
        continue;
      }
      answer = "Deposited Successfully!Your Current Balance is";
      bot.sendMessage(msg.chat_id, answer, "Markdown");
      bot.sendMessage(msg.chat_id, String(balance), "Markdown");
      update_spreadsheet("Credit",msg.text);
      continue;
    }
    else
      answer = "I am sorry I didn't catch that";

    bot.sendMessage(msg.chat_id, answer, "Markdown");
  }
}


void bot_setup()
{
  const String commands = F("["
"{\"command\":\"help\",  \"description\":\"Get bot usage help\"},"
"{\"command\":\"start\", \"description\":\"Message sent when you open a chat with a bot\"},"
"{\"command\":\"status\",\"description\":\"Answer device current status\"},"
"{\"command\":\"login\",\"description\":\"\login for new user\"},"
"{\"command\":\"balance\",\"description\":\"Check current balance\"},"
"{\"command\":\"deposit\",\"description\":\"Deposit amount\"},"
"{\"command\":\"logout\",\"description\":\"Log out from session\"},"
"{\"command\":\"withdraw\",\"description\":\"Withraw amount\"}"
"]");
  bot.setMyCommands(commands);
}

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial.print("Connecting to Wifi ");
Serial.print(ssid);
WiFi.begin(ssid, password);
secure_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
while (WiFi.status() != WL_CONNECTED)
{
  Serial.print(".");
  delay(500);
}
Serial.print("\n Your IP address is: ");
Serial.println(WiFi.localIP());

bot_setup();

}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - bot_last > BOT_interval)
  {
    int new_msgs = bot.getUpdates(bot.last_message_received + 1);

    while (new_msgs)
    {
      Serial.println("Received response");
      messagehandler(new_msgs);
      new_msgs = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_last = millis();
  }
}
