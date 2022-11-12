# Flower_Care_Sys
基于arduino的物联网植物养护系统

这是一个开源的；基于arduino；改自yltzdhbc https://github.com/yltzdhbc/FloewrCareSys 的flower care sys项目，基于blinker物联网平台；使用3D打印制作的一个浇花系统；

![dee8e35c2b8c08dbb9b8b0535c585de](\image\dee8e35c2b8c08dbb9b8b0535c585de.jpg)

## 零件清单

这个是在这个项目里你所需要的所有东西

* 1 x 7Pin0.96 SPI OLED 显示屏
* 1 x NodeMcu esp8266
* 1 x USB-A to micro-USB 数据线
* 杜邦线（公对公、母对母、公对母）若干
* 1 x 双路继电器
* 1 x 面包板电源模块
* 1 x USB转DC升压线
* 1 x PTC加热片
* 1 x 土壤湿度传感器
* 1 x 水泵
* 3D 打印的外壳（如果没有可以使用两块面包板代替）

**Tools:**

* 一些M3自攻螺丝

* 螺丝刀

* 电烙铁

## 软件

所有的代码都在 Automatic-sprout-cultivation-system.ino文件中，使用arduino打开 编译下载即可

值得注意的是 ，必须先安装esp8266扩展，下载blinker库，下载u8g2库（OLED驱动），才能够正常编译

在U8g2\src\clib里面找到u8g2_fonts.c文件将其替换为此项目上传的u8g2_fonts.c文件

接入流程 

开机 

**在Blinker APP 中 开发者>开发工具>EspTouch** 进行配网

输入自己的WIFI名称 密码 手机会自动将密码发送到设备上

在blinkerAPP中需要进行如下设置

APP界面先打开你接入的开发板界面，点击右上的圆里三个点的图标，点击界面配置，把APP界面文件里面的字符复制粘贴进去，点更新配置就行了

![5c4d4d6fc2615cfc313afc1930a4258](\image\5c4d4d6fc2615cfc313afc1930a4258.jpg)

## 硬件电路

OLED 

​	 D5      >     MOSI

​     D7      >     SCK

​     D2      >     DC

​     D8      >     CS 

​     D3      >     RES

​	VCC	>	5v

​	GND	>	GND

继电器

​	in1	>		d4

​	in2	>		d1

​	no1	>	水泵正

​	com1	>	3.3v

​	水泵负	>	GND

​	no2	>	ptc加热片正

​	com2	>	3.3v

​	加热片负	>	GND

土壤湿度传感器

​      VCC      >     5V

​      GND      >     GND

​      SIG>     A0

## 组装提示

准备好螺丝刀可以省很多的事儿

检查好正负极再打开电源为了安全