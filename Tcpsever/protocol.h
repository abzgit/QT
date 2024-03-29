#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef unsigned int uint;
#endif // PROTOCOL_H
struct pdu{
    uint uiPDUlen;      //总的协议数据单元大小
    uint uiMsgType;     //消息类型
    char caData[64];
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //实际消息
};
enum msg_type{
    msg_type_min = 0,
    msg_type_regist_request,
    msg_type_regist_respond,
    msg_type_login_request,
    msg_type_login_respond,
    msg_type_cancle_request,
    msg_type_cancle_respond,
    // msg_type_regist_request,
    // msg_type_regist_respond,
    msg_type_max = 0x00ffffff
};

pdu* mkpud(uint uimsgLen);
