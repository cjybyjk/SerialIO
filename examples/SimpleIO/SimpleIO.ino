/*
    printf/scanf for Arduino Serial
    Author：Chen Jingyi(cjybyjk@zjnu.edu.cn)
    License: MIT
    Date: 2019-11-10
*/

#include <SerialIO.h>

int a = 0;
char b[255];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  serialprintf("A=%d, B=%s\n", a, b);
  waitserial(); // Wait for serial available
  serialscanf("%d%s", &a, &b);
}
