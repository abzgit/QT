#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef unsigned int uint;
#endif // PROTOCOL_H

#define REGIST_OK "regist ok"                            // 注册
#define REGIST_FAILED "regist failed"

#define LOGIN_OK "login ok"                              // 登录
#define LOGIN_FAILED "login failed"

// 好友操作
#define SEARCH_USER_OK "search user ok"                  // 查找
#define SEARCH_USER_OFFLINE "user offline"
#define SEARCH_USER_EMPTY "no such people"

#define UNKNOW_ERROR "unknow erroe"     //搜索
#define EXEITED_FRIEND "friend exit"
#define ADD_FRIEND_OFFLINE "use offline"
#define ADD_FRIEND_NO_EXIT "use not exist"

#define DEL_FRIEND_OK "delect friend ok!"

enum msg_type{
    msg_type_min = 0,
    msg_type_regist_request,    //注册请求
    msg_type_regist_respond,

    msg_type_login_request,     //登录请求
    msg_type_login_respond,

    msg_type_cancle_request,    //注销请求
    msg_type_cancle_respond,

    msg_type_online_request,    //查看在线好友请求
    msg_type_online_respond,

    msg_type_search_request,    //查找好友请求
    msg_type_search_respond,

    msg_type_addfri_request,    //添加好友请求
    msg_type_addfri_respond,

    msg_type_agree_add_friend,  //同意添加
    msg_typr_disagree_add,

    msg_flush_request,  //刷新好友列表
    msg_flush_respon,

    msg_delfriend_request,  //删除好友
    msg_delfriend_respon,

    msg_privatechat_request,  //私聊
    msg_privatechat_respon,

    msg_type_max = 0x00ffffff
};

struct pdu{
    uint uiPDUlen;      //总的协议数据单元大小
    uint uiMsgType;     //消息类型
    char caData[64];
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //实际消息
};
pdu* mkpud(uint uimsgLen);
