## 嵌入式系统通用驱动程序接口及其实现-Usart操作

* 此接口用于具体项目中: 对Usart操作时，提供统一的存取操作函数，实现了调用层与的Usart硬件/存储空间的分离。

### 主要子模块说明：
#### 实例化统一接口
  Usart： 为其UsartDev的实例模块接口

#### UsartDev通讯接口，在不同MCU的实现：
  * UsartDev_ASM32: ASM32内部USART上的实现
  * UsartDev_ATMEGA: AVR ATMEGA系更内部USART上的实现
  * UsartDev_HC32: HC32低端MCU,内部USART上的实现
  * UsartDev_HC32F4: HC32F4,内部USART上的实现
  * UsartDev_LPC_ARM: NXP的ARM芯片,内部USART上的实现
  * UsartDev_QextserialPort: 电脑上，使用QextserialPort模块时的驱动实现
  * UsartDev_STM32: STM32低端MCU,内部USART上的实现
  * UsartDev_Io: 在使用IO口模拟时的实现，需"UsartHwIo"子模块配合

#### Usart永久参数的管理
  * UsartCfg: 为USART的配置子模块，可实现波特率，校验位等的永久保存与修改。
    + UsartCfg_SMenu: 为其在SMenu菜单中的实现，可在数码管上显示与修改相关参数
    + UsartCfg_TMenu: 为其在TMenu菜单中的实现，可在彩屏，点阵屏上显示与修改相关参数
  * UsartCfgUser0: UsartCfg支持扩展方式0的参数
    + 此扩展支持主机，从机等工作模式，主机时支持发送间隔，接收等持时间，从机时支持从机地址，帧间隔等参数。
    + UsartCfgUser0_TMenu:  UsartCfg支持扩展参数保存(如从机时通讯地址等)时，在TMenu菜单中的实现

#### UsartDev的多态支持：
  UsartDevPt: 即UsartDev的多态支持，如：一个MCU内，用IO口及USART硬件同时驱动，用此可实现统一管理。

#### UsartHW 硬件配置接口，在不同MCU的实现：
 UsertDev仅负责双向通讯，对USART硬件的配置，使用UsartHw实现，且硬件相关，实现的硬件有：
  * UsartHw_ASM32: ASM32内部USART上的实现接口，含ASM32F00系列内部实现
  * UsartHw_ATMEGA: AVR ATMEGA系更内部USART上的实现
  * UsartHw_HC32: HC32低端MCU,内部USART上的实现接口，含HC32F00,F0,F4系列的内部实现。
  * UsartHW_LPC_ARM: NXP的ARM芯片,内部USART上的实现
  * UsartDev_STM32: STM32低端MCU,内部USART上的实现

-------------------------------------------------------------------------------

#### 软件结构说明:
  + **Usart通用操作接口:**  即对外接口调用文件:**Usart.h** ，其它软件部分操作Usart时,**包含此文件即可**。
  + **Usart操作接口的各种实现:** 以*Usart(下横线_)Usart硬件名称(或载体)_专用编译环境(可选,严重不建议代码与编译环境相关连)*命名, 与具体使用的Usart硬件有关，即有各种不同的实现。但一种硬件仅实现一次，项目中需要那个加入那个即可，具有通用性(*实现时需尽量排除编译器影响，使一个芯片的实现能在各种编译器里运行*)。

  +UsartHw专用于对用struct _UsartDevCfg结构对Usart进行配置，故每个UsartDev的具体实现，对应有UsartHw的实现及接口
  + UsartDevCfg.为struct _UsartDevCfg结构的成员操作函数的宏实现

#### 使用说明：
 + 1.根据项目嵌入式硬件不同，将Usart.h和**与项目对应的**的Usart具体实现文件。增加到开发环境中。

#### 目录结构组织：
* **小型项目时**: 即不区分组件层，放在“项目源文件目录\Usart”下，内部不再有子目录
* **大中型项目时**: 区分组件层，放在“项目源文件目录\components\Usart”下，若项目很多，且同一项目也有较多实现时，可将具体实现文件放在此目录“项目名称”目录下，以实现分类存放。

## 开源项目说明
* 为各类单片机提供模板支持,**欢迎大家增加对各类嵌入式硬件的操作模板**,以让更多人使用
* 版权声明: ...ch这世道，说了也等于白说，总之以下点：
 + **源代码部分：** 可以自由使用，源代码中，也不需做任何版权声明。
 + **分享时：** 为防止碎化片，请注明出处，以利于开源项目的推广。
 + **关于fork：**  这个欢迎(但为防止碎化片化，请不要分支或单独推广)。更欢迎为此开源项目直接贡献代码。 

## 此开源项目对应的教程
* 视频在分享平台：http://thtfcccj.56.com
* 与视频同步输入的文字,在http://blog.csdn.net/thtfcccj
* 同步的开源项目，则在代码托管平台：https://github.com/thtfcccj

## 此开源项目对应的两个件教学视频：(暂无)










