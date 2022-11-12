#define DHTPIN 12 //对应D6针脚接入DHT11
#define DHTTYPE DHT11 //选择传感器类型 DHT11
#define BLINKER_PRINT Serial//串口输出
#define U8X8_USE_PINS
#define BLINKER_WIFI
#define BLINKER_MIOT_SENSOR //作为温度传感器接入小爱
#include <Blinker.h>//Blinker官方库
#include <SPI.h>    //显示屏SPI传输库
#include <U8g2lib.h>//显示屏的库
#include <DHT.h>   //温度传感器运行库 自用DHT11

DHT dht(DHTPIN, DHTTYPE);

uint32_t read_time = 0;
float humi_read, temp_read; //获取DHT11的数据
int sensorPin = A0;  //土壤传感器对应的针脚
int sensorValue = 0; //存放土壤模拟信号的变量
float precentsensorValue = 0; //设置初始湿度变量为0
int waterPin = D4;       //湿度 继电器 水泵控制
int temperaturePin = D1; //温度 继电器 PTC加热片控制
int zdjhPin = D9;
int zdjrPin = D10;
int sliderValue = 40;    //湿度阈值 通过土壤传感器
int sliderValue1 = 20;   //温度阈值 通过气温传感器

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, D5 /*clock*/, D7 /*data*/, D8 /*cs*/, D2 /*dc*/, D3 /*reset*/);

BlinkerNumber NumSoil("num-soil");//Blinke上自定义显示数据的土壤湿度模块
BlinkerNumber Numtem("num-tem");  //Blinke上自定义显示数据的气温温度模块
BlinkerNumber Numhum("num-hum");  //Blinke上自定义显示数据的气温湿度模块

BlinkerButton BtnJiaohua("btn-Jiaohua"); //手动浇水按键
BlinkerButton BtnJiare("btn-Jiare");     //手动加热按键
BlinkerButton BtnJiaohuazidong("btn-jhzd");//自动浇水控制
BlinkerButton BtnJiarezidong("btn-jrzd");//自动加热控制

BlinkerSlider Slider1("ran-1"); //设置滑块控制浇水湿度阈值
BlinkerSlider Slider2("ran-2"); //设置滑块控制加热温度阈值

BlinkerText Tex1("tex-1"); //文本1 手动浇水状态
BlinkerText Tex2("tex-2"); //文本2 自动浇水状态
BlinkerText Tex3("tex-3"); //文本3 手动加热状态
BlinkerText Tex4("tex-4"); //文本4 自动加热状态
BlinkerText Tex5("tex-5");//文本5 时间信息
BlinkerText Tex6("tex-6");//文本6 自动浇水开关状态
BlinkerText Tex7("tex-7");//文本6 自动加热开关状态
char auth[] = "这里填平台获取的KEY";
char ssid[] = "这里填你的WIFI名字（ESP8266指支持2.4Ghz的wifi）"; 
char pswd[] = "这里填你的WIFI密码"; 

void slider1_callback(int32_t value)
{
  sliderValue = value;
  BLINKER_LOG("get slider value: ", value);
}

void slider2_callback(int32_t value)
{
  sliderValue1 = value;
  BLINKER_LOG("get slider2 value: ", value);
}

void heartbeat() //心跳包回调函数
{
  if (digitalRead(waterPin) == HIGH)
    Tex2.print("没有浇水");
  else if (digitalRead(waterPin) == LOW)
    Tex2.print("正在浇水");
  if (digitalRead(temperaturePin) == HIGH)
    Tex4.print("没有加热");
  else if (digitalRead(temperaturePin) == LOW)
    Tex4.print("正在加热");
  Tex5.print(rts());
  Slider1.print(sliderValue);
  Slider2.print(sliderValue1);
  NumSoil.print(precentsensorValue);
  Numtem.print(temp_read);
  Numhum.print(humi_read);
}

void BtnJiaohuazidong_callback(const String &state) //自动浇水按键回调函数
{
  BLINKER_LOG("get button state: ", state);
  if (state == "on")
  {
    digitalWrite(zdjhPin, LOW); //给空针脚写入低电平
    Tex6.print("自动浇水状态 开");
    BtnJiaohuazidong.print("on");
  }
  if (state == "off")
  {
    digitalWrite(zdjhPin, HIGH);  //给空针脚写入高电平
    digitalWrite(waterPin, HIGH); //我也不知道为什么关闭自动开关的时候不会停止工作 于是就直接关了
    Tex6.print("自动浇水状态 关");
    BtnJiaohuazidong.print("off");
  }
  loop();
}

void BtnJiarezidong_callback(const String &state) //自动加热按键回调函数
{
  BLINKER_LOG("get button state: ", state);
  if (state == "on")
  {
    digitalWrite(zdjrPin, LOW); //给空针脚写入低电平
    Tex7.print("自动加热状态 开");
    BtnJiarezidong.print("on");
  }
  if (state == "off")
  {
    digitalWrite(zdjrPin, HIGH); //给空针脚写入高电平
    digitalWrite(temperaturePin, HIGH); //我也不知道为什么关闭自动开关的时候不会停止工作 于是就直接关了
    Tex7.print("自动加热状态 关");
    BtnJiarezidong.print("off");
  }
  loop();
}

void BtnJiaohua_callback(const String &state) //手动浇水按键回调函数
{
  BLINKER_LOG("get button state: ", state);
  if (state == "on")
  {
    digitalWrite(waterPin, LOW); //开启水泵继电器
    Tex1.print("浇水中");
    BtnJiaohua.print("on");
  }
  if (state == "off")
  {
    digitalWrite(waterPin, HIGH); //关掉水泵继电器
    Tex1.print("浇水暂停");
    BtnJiaohua.print("off");
  }
}

void BtnJiare_callback(const String &state) //手动加热回调函数
{
  BLINKER_LOG("get button state: ", state);
  if (state == "on")
  {
    digitalWrite(temperaturePin, LOW); //开启PTC加热板的继电器
    Tex3.print("加热中");
    BtnJiare.print("on");
  }
  if (state == "off")
  {
    digitalWrite(temperaturePin, HIGH); //关掉PTC加热板的继电器
    Tex3.print("加热暂停");
    BtnJiare.print("off");
  }
}

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
    
}

void dataStorage()
{
    Blinker.dataStorage("cha-temp", temp_read);
    Blinker.dataStorage("cha-humi", humi_read);
    Blinker.dataStorage("cha-hum",precentsensorValue);
}
 
String rts()//计算运行时间
    {
    int rt = Blinker.runTime();
    int r,e,f,s;
    String fh;
    Blinker.delay(100);
    if(rt >= 86400)//天数
    {r = rt / 86400;
     e = rt / 3600 - r*24;
     f = rt / 60 - r*1440 - e*60;
     s = rt - r*86400 - e*3600 - f*60;}
    else if(rt >= 3600)
    {r = 0;
     e = rt / 3600;
     f = rt / 60 - e*60;
     s = rt - e*3600 - f*60;}
    else
    {r = 0;
     e = 0;
     f = rt / 60;
     s = rt - f*60;}
     
    //BLINKER_LOG(r," ",e," ",f," ",s);//输出数据测试
    
    if(f==0 & e==0 & r==0)
    {fh = String("")+ s +"秒";}
    else if(r == 0 & e == 0 )
    {fh = String("")+ f + "分" + s +"秒";}
    else if(r == 0)
    {fh = String("")+ e + "时" + f + "分" + s +"秒"; }
    else
    {fh = String("")+ r + "天" + e + "时" + f + "分" + s +"秒";}

    return(fh);
    }

void miotQuery(int32_t queryCode) //接入小爱模块
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_HUMI_NUMBER : //给小爱同学返回湿度数据
            BLINKER_LOG("MIOT Query HUMI");
            BlinkerMIOT.humi(precentsensorValue); //湿度值
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_TEMP_NUMBER : //给小爱同学返回温度数据
            BLINKER_LOG("MIOT Query TEMP");
            BlinkerMIOT.temp(temp_read);  //温度值
            BlinkerMIOT.print();
            break;
        default :                     //默认在传感器没有返回数值的时候播报的数值
            BlinkerMIOT.temp(0);
            BlinkerMIOT.humi(0);
            BlinkerMIOT.print();
            break;
    }
}

void oledDisplay()
{
  u8g2.firstPage();
  do
  {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
        if (isnan(h) || isnan(t))
    {
        BLINKER_LOG("Failed to read from DHT sensor!");
    }
    else
    {
       // BLINKER_LOG("Humidity: ", h, " %");
       //BLINKER_LOG("Temperature: ", t, " *C");
        humi_read = h;
        temp_read = t;
        float hic = dht.computeHeatIndex(t, h, false);
    }
    u8g2.setFont(u8g2_font_helvR10_tf);
    u8g2.setCursor(0, 12);
    u8g2.print("ASCS BY Akagi_cv"); //你可以改成你想要的文字 仅限英文 因为我中文字库就只有 温度湿度和没有在浇水加热1234567890%.℃ 这些且加载的字体是英文的
    u8g2.setCursor(0, 28);
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    u8g2.print("湿度: " + String(precentsensorValue) + "％"); //输出土壤湿度
    u8g2.setCursor(0, 44);
    u8g2.setFont(u8g2_font_unifont_t_chinese1); 
    u8g2.print("温度: " + String(t) + "℃"); //输出空气温度
    u8g2.setCursor(0, 60);
    u8g2.setFont(u8g2_font_unifont_t_chinese1);
    int water = digitalRead(waterPin);
    int temper = digitalRead(temperaturePin);
    if (water == temper && water == 0x1)
      u8g2.print("没有浇水和加热");
    else if (water > temper)
      u8g2.print("在加热没有浇水");
    else if (water < temper)
      u8g2.print("在浇水没有加热");
    else if (water == temper && water == 0x0)
      u8g2.print("在浇水和加热");
  } while (u8g2.nextPage());
}

void u8g2Init()
{
  u8g2.begin();
  u8g2.setFlipMode(0);
  u8g2.clearBuffer();
  u8g2.enableUTF8Print();
}

void setup()
{
  Serial.begin(115200); //初始化串口
  BLINKER_DEBUG.stream(BLINKER_PRINT);
  pinMode(LED_BUILTIN, OUTPUT); //默认把所有东西全部关完     
  digitalWrite(LED_BUILTIN, HIGH);  
  pinMode(waterPin, OUTPUT);        
  digitalWrite(waterPin, HIGH);       
  pinMode(temperaturePin, OUTPUT);    
  digitalWrite(temperaturePin, HIGH);
  pinMode(zdjhPin, OUTPUT);
  digitalWrite(zdjhPin, HIGH);
  pinMode(zdjrPin, OUTPUT);
  digitalWrite(zdjrPin, HIGH);
  Blinker.attachData(dataRead);
  Blinker.attachDataStorage(dataStorage);//附加数据存储

  u8g2Init(); //初始化OLED
  dht.begin();

  Blinker.begin(auth, ssid, pswd); //联网初始化

  BlinkerMIOT.attachQuery(miotQuery);//初始化接入小爱查询端口

  Blinker.attachHeartbeat(heartbeat); //心跳包初始化
  
  BtnJiaohuazidong.attach(BtnJiaohuazidong_callback);//初始化自动浇水按键
  BtnJiarezidong.attach(BtnJiarezidong_callback);//初始化自动加热按键
  BtnJiaohua.attach(BtnJiaohua_callback); //初始化浇水按键
  BtnJiare.attach(BtnJiare_callback);     //初始化加热按键
  Slider1.attach(slider1_callback);       //初始化浇水阈值滑块
  Slider2.attach(slider2_callback);       //初始化加热阈值滑块
  
}

void loop()
{
  Blinker.run();
  sensorValue = analogRead(sensorPin); //读取土壤湿度数据
  precentsensorValue = float((-(sensorValue - 866)) / 234.0 * 50); //我也不知道怎么算的，反正看着数值还行

  float t = dht.readTemperature();
  int water = digitalRead(waterPin);
  int temper = digitalRead(temperaturePin);
  int zdjh = digitalRead(zdjhPin);
  int zdjr = digitalRead(zdjrPin);
  if (zdjh == 0) //自动浇花的空针脚低电平的时候才会开启自动功能
  {
    if (precentsensorValue < sliderValue) //比较土壤湿度与阈值的大小
      digitalWrite(waterPin, LOW);    //返回为真开启继电器
    else                              //比阈值大或相等的情况下执行的操作
      digitalWrite(waterPin, HIGH);   //关掉继电器
  }

  if (zdjr == 0) //自动加热的空针脚低电平的时候才会开启自动功能
  {
    if (t < sliderValue1) //比较空气温度与阈值的大小
      digitalWrite(waterPin, LOW);  //返回为真开启继电器
    else                            //比阈值大或相等的情况下执行的操作
      digitalWrite(waterPin, HIGH); //关掉继电器
  }

  oledDisplay();
  Blinker.delay(1500); //1.5s刷新一次
}
