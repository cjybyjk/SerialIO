/*
    printf/scanf for Arduino Serial
    Author：Chen Jingyi(cjybyjk@zjnu.edu.cn)
    License: MIT
    Date: 2019-11-10
*/

#ifndef _serialio_h_
#define _serialio_h_

#include <math.h>
#include <Arduino.h>

// scanf 实现
// 使用 __fmt 处理 __buf 中的值，将其赋给变量
// 目前仅实现了基础格式字符的处理
int vsscanf(char *__buf, char *__fmt, va_list __ap);

// 调用以下函数之前必须Serial.begin

// 格式化内容，然后输出到串口
// 用法和printf一样
int serialprintf(HardwareSerial *serial, const char *__fmt, ...);
int serialprintf(const char *__fmt, ...);

// 等待串口输入
void waitserial(HardwareSerial *serial);
void waitserial();

// 从串口读数据
void readserial(HardwareSerial *serial, char *__buf);

// 格式化Serial输入的内容
// 用法和scanf一样
int serialscanf(HardwareSerial *serial, char *__fmt, ...);
int serialscanf(char *__fmt, ...);

#endif
