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

    int ret = 0; // 返回值
    bool flagFmt = false; //正在读取格式化字符的标志

	int width = 0; // 场宽
	char numType = 1; //0: short, 1: normal, 2: long, 3: long long
	long long num = 0; // 用于读入有符号整型
	unsigned long long unum = 0; // 用于读入无符号整型
	long double fnum = 0; // 用于读入实数
    char *str = NULL; // 读入字符串

	int twidth = 0; // 当前场宽
	char isLowerNum = 0; //小数点
	char sign = 1; // 符号
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
                while (*pbuf == ' ')
                {
                    pbuf++;
                }
                switch (*pfmt)
                {
                    case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
                    /* 场宽控制 */
                    {
                        width *= 10;
                        width += *pfmt - 48;
                        break;
                    }
                    case 'c':
                        /* 读入字符 */
                        if (!width) {
                            width = 1;
                        }
                        for (twidth = 0; !width || twidth < width; twidth++) {
                            if (!ignorevar) {
                                *va_arg(__ap, char*) = *pbuf;
                                ret++;
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
                            ret++;
                        }
                        flagFmt = false;
                        break;
                    case 'l':
                        /* long / long long 格式控制字符 */
                        if (numType == 2) {
                            numType = 3;
                        } else {
                            numType = 2;
                        }
                        break;
                    case 'L':
                        numType = 3;
                        break;
                    case 'h':
                        /* short 格式控制字符 */
                        numType = 0;
                        break;
                    case 'd': case 'i':
                        /* 读入有符号十进制整数 */
                        sign = 1;
                        num = 0;
                        twidth = 0;
                        if (*pbuf == '-') {
                            sign = -1;
                            pbuf++;
                        } else if (*pbuf == '+') {
                            pbuf++;
                        }
                        
                        while (*pbuf >= 48 && *pbuf <= 57) {
                            if (!width || twidth < width) {
                                num *= 10;
                                num += *pbuf - 48;
                                pbuf++;
                            }
                            twidth++;
                        }
                        num *= sign;
                        if (!ignorevar) {
                            switch (numType) // 判断数据大小
                            {
                                case 0:
                                    *va_arg(__ap, short*) = (short)num;
                                    break;
                                case 2:
                                    *va_arg(__ap, long*) = (long)num;
                                    break;
                                case 3:
                                    *va_arg(__ap, long long*) = num;
                                    break;
                                default:
                                    *va_arg(__ap, int*) = (int)num;
                                    break;
                            }
                            ret++;
                        }
                        numType = 1;
                        flagFmt = false;
                        break;
                    case 'u':
                        /* 读入无符号十进制整数 */
                        unum = 0;
                        twidth = 0;
                        isLowerNum = 0;
                        while (*pbuf >= 48 && *pbuf <= 57) {
                            if (!width || twidth < width) {
                                unum *= 10;
                                unum += *pbuf - 48;
                                pbuf++;
                            }
                            twidth++;
                        }
                        if (!ignorevar) {
                            switch (numType) // 判断数据大小
                            {
                                case 0:
                                    *va_arg(__ap, unsigned short*) = (unsigned short)unum;
                                    break;
                                case 2:
                                    *va_arg(__ap, unsigned long*) = (unsigned long)unum;
                                    break;
                                case 3:
                                    *va_arg(__ap, unsigned long long*) = unum;
                                    break;
                                default:
                                    *va_arg(__ap, unsigned int*) = (unsigned int)unum;
                                    break;
                            }
                            ret++;
                        }
                        numType = 1;
                        flagFmt = false;
                        break;
                    case 'f': case 'e': case 'E': case 'g': case 'G':
                        /* 读入有符号十进制实数（仅支持小数形式） */
                        fnum = 0;
                        sign = 1;
                        if (*pbuf == '-') {
                            sign = -1;
                            pbuf++;
                        } else if (*pbuf == '+') {
                            pbuf++;
                        }
                        
                        while ((*pbuf >= 48 && *pbuf <= 57) || (*pbuf == '.' && isLowerNum == 0)) {
                            if (*pbuf == '.') {
                                isLowerNum = 1;
                                pbuf++;
                            }
                            if (!width || twidth < width) {
                                if (isLowerNum) {
                                    fnum += (*pbuf - 48) * pow(10,(-1 * isLowerNum));
                                    isLowerNum++;
                                } else {
                                    fnum *= 10;
                                    fnum += *pbuf - 48;
                                }
                                pbuf++;
                            }
                            twidth++;
                        }
                        fnum *= sign;
                        if (!ignorevar) {
                            switch (numType) // 判断数据大小
                            {
                                case 2:
                                    *va_arg(__ap, double*) = (double)fnum;
                                    break;
                                case 3:
                                    *va_arg(__ap, long double*) = fnum;
                                    break;
                                default:
                                    *va_arg(__ap, float*) = (float)fnum;
                                    break;
                            }
                            ret++;
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
    return ret;
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
