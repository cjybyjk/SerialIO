/*
    printf/scanf for Arduino Serial
    Author：Chen Jingyi(cjybyjk@zjnu.edu.cn)
    License: MIT
    Date: 2019-11-10
*/

#include <SerialIO.h>

int vsscanf(char *__buf, char *__fmt, va_list __ap)
{
    char *pbuf = __buf;
    char *pfmt = __fmt;

    int conv = 0; // 返回值
    bool flagFmt = false; //正在读取格式化字符的标志

	unsigned char width = 0; // 场宽
	unsigned char numType = 1; //0: short, 1: normal, 2: long
    unsigned char base = 10; //进制
    char nums[39] = {0}; // 处理字符串转数值的中间变量
    char *str = NULL; // 读入字符串

	unsigned char twidth = 0; // 当前场宽
    bool ignorevar = false; // 读入但不存储

    while (*pfmt) {
        flagFmt = *pfmt == '%';
        ignorevar = false;
        if (!flagFmt) {
            if (*pfmt != *pbuf) { //输入和格式不一样就退出
                break;
            } else {
                pbuf++;
                pfmt++;
            }
        } else {
            while (flagFmt) {
                /* 预处理 */
                while (isspace(*pbuf)) {
                    pbuf++;
                }
                while (isspace(*pfmt)) {
                    pfmt++;
                }
                
                switch (*pfmt) {
                    case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
                        /* 场宽控制 */
                        width *= 10;
                        width += *pfmt - 48;
                        break;
                    case 'c':
                        /* 读入字符 */
                        if (!width) {
                            width = 1;
                        }
                        for (twidth = 0; !width || twidth < width; twidth++) {
                            if (!ignorevar) {
                                *va_arg(__ap, char*) = *pbuf;
                                conv++;
                            }
                            pbuf++;
                        }
                        flagFmt = false;
                        break;
                    case 's':
                        /* 读入字符串 */
                        twidth = 0;
                        str = NULL;
                        if (!ignorevar) {
                            str = va_arg(__ap, char*);
                        }
                        while (*pbuf && *pbuf != ' ' && *pbuf != '\t' && *pbuf != '\n') {
                            if (!ignorevar) {
                                str[twidth] = *pbuf;
                            }
                            pbuf++;
                            twidth++;
                            if (width && twidth >= width) {
                                break;
                            }
                        }
                        if (!ignorevar) {
                            str[twidth] = '\0';
                            conv++;
                        }
                        flagFmt = false;
                        break;
                    case 'l': case 'L':
                        numType = 2;
                        break;
                    case 'h':
                        /* short 格式控制字符 */
                        numType = 0;
                        break;
                    case 'b': case 'd': case 'i': case 'o': case 'u': case 'x':
                        /* 读入整数 */
                        memset(nums, 0, sizeof nums);
                        twidth = 0;
                        switch (*pfmt) {
                            case 'o':
                                base = 8;
                                break;
                            case 'x':
                                base = 16;
                                break;
                            default:
                                base = 10;
                                break;
                        }
                        while (*pbuf == '-' || *pbuf == '+' || 
                            (base == 10 && (*pbuf >= '0' && *pbuf <= '9')) ||
                            (base == 8 && (*pbuf >= '0' && *pbuf <= '7')) ||
                            (base == 16 && ((*pbuf >= '0' && *pbuf <= '9') || 
                                (*pbuf >= 'A' && *pbuf <= 'F') || (*pbuf >= 'a' && *pbuf <= 'f')))) {
                            if (!width || twidth < width) {
                                nums[twidth] = *pbuf;
                                pbuf++;
                            }
                            twidth++;
                        }
                        if (!ignorevar) {
                            switch (numType) 
                            {
                                case 0:
                                    *va_arg(__ap, unsigned short*) = (short)strtoul(nums, NULL, base);
                                    break;
                                case 2:
                                    *va_arg(__ap, unsigned long*) = strtoul(nums, NULL, base);
                                    break;
                                default:
                                    *va_arg(__ap, unsigned int*) = (int)strtoul(nums, NULL, base);
                                    break;
                            }
                            conv++;
                        }
                        numType = 1;
                        flagFmt = false;
                        break;
                    case 'f': case 'e': case 'E': case 'g': case 'G':
                        /* 读入实数 */
                        memset(nums, 0, sizeof nums);
                        twidth = 0;
                        while (*pbuf == '-' || *pbuf == '+' || *pbuf == '.' || *pbuf == 'e' || (*pbuf >= '0' && *pbuf <= '9')) {
                            if (!width || twidth < width) {
                                nums[twidth] = *pbuf;
                                pbuf++;
                            }
                            twidth++;
                        }
                        if (!ignorevar) {
                            switch (numType) // 判断数据大小
                            {
                                case 2:
                                    *va_arg(__ap, double*) = (double)atof(nums);
                                    break;
                                default:
                                    *va_arg(__ap, float*) = (float)atof(nums);
                                    break;
                            }
                            conv++;
                        }
                        numType = 1;
                        flagFmt = false;
                        break;
                    case '*':
                        ignorevar = true;
                        break;
                    default:
                        break;
                }
                pfmt++;
            }
        }
    }
    return conv;
}

int mserialprintf(HardwareSerial *serial, const char *__fmt, ...)
{
    char res[1024] = {0};
    // 通过 vsprintf 得到格式化后的字符串
    va_list ap;
    va_start(ap, __fmt);
    vsprintf(res, __fmt, ap);
    va_end(ap);
    // 输出到串口
    return serial->print(res);
}

void mwaitserial(HardwareSerial *serial)
{
    while(serial->available() == 0) {
        delay(2);
    }
}

void mreadserial(HardwareSerial *serial, char *__buf)
{
    while(Serial.available() > 0) {
        *__buf = serial->read();
        __buf++;
        delay(2);
    }
}

int mserialscanf(HardwareSerial *serial, char *__fmt, ...)
{
    char vbuf[1024] = {0};
    mreadserial(serial, vbuf);
    // 通过 vsscanf 处理输入
    va_list argp;
    va_start(argp, __fmt);
    int res = vsscanf(vbuf, __fmt, argp);
    va_end(argp);
    return res;
}
