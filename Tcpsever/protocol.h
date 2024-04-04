#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <unistd.h>
#include <stdlib.h>
typedef unsigned int uint;

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

#define DEL_FRIEND_OK "delect friend ok!"   //删除好友

#define DIR_NO_EXIT "cur dir not exit!"     //创建文件夹
#define FILE_NAME_EXITED "file name exited!"
#define CREATE_DIR_OK "create dir ok!"

#define DEL_DIR_OK "delete dir ok!"
#define DEL_DIR_FAILURED "delete dir failured!"

#define RENAME_FILE_OK "rename file ok!"
#define RENAME_FILE_FAILURED "rename file failured"

#define ENTER_DIR_OK "enter dir ok"
#define ENTER_DIR_FAILURED "enter dir failured:is not a dir"

#endif // PROTOCOL_H

struct fileinfo
{
    char filename[32];  //文件名字
    int filetype;       //文件类型
};

struct pdu{
    uint uiPDUlen;      //总的协议数据单元大小
    uint uiMsgType;     //消息类型
    char caData[64];
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //实际消息
};
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

    msg_group_chat_request,  //群聊
    msg_group_chat_respon,

    msg_creat_dir_request,  //创建文件夹
    msg_creat_dir_respon,

    msg_flushfile_request,  //刷新文件
    msg_flushfile_respon,

    msg_deldir_request,    //删除文件夹
    msg_deldir_respon,

    msg_rename_dir_request,    //重命名文件
    msg_rename_dir_respon,

    msg_enter_dir_request,    //进入文件夹
    msg_enter_dir_respon,

    msg_upload_request,    //上传文件
    msg_uploadr_respon,

    msg_type_max = 0x00ffffff
};

pdu* mkpud(uint uimsgLen);
