#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <thread>
#include <QString>
#include <QObject>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include "NavigationMsg.pb.h"

#define DELETE(p) if(NULL != p){\
    delete p;\
    p = NULL;}

typedef websocketpp::client<websocketpp::config::asio_client> client;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

enum emKEYTYPE
{
    START_SCAN_MAP = 0,
    CANCEL_SCAN_MAP,
    ASYNC_STOP_SCAN_MAP,
    SCAN_STATUS_PUSH,
    MAP_PNG,
    GET_MAP_LIST,
    DELETE_MAP,
    REAL_POSITION,
    REAL_LASER_PHIT,
    REAL_DISPLAY,
    SYNC_MAP,
    SYNC_PROGRESS_PUSH,
    LOAD_MAP,
    INITIALIZE_CUSTOMIZED,
    MOVE,
    START_CHARGE,
    STOP_CHARGE,
    NAVIGATE_POINT,
    NAVIGATE_POSITION,
    PAUSE_NAVIGATE,
    RESUME_NAVIGATE,
    CANCEL_NAVIGATE,
    TASK_STATUS,
    REAL_NAVIGATION_PATH,
    NAVIGATE_PATH,
    VIRTUAL_OBSTACLES,
    UPDATE_VIRTUAL_OBSTACLES,
    POSITION_LIST,
    RECORD_POSITION,
    DELETE_POSITION,
    RENAME_POSITION,
    NAVIGATION_PATH_LIST,
    NAVIGATION_PATH_LIST_DETAIL,
    NAVIGATION_PATH_DETAIL,
    VERIFICATION_POINT,
    SET_NAVIGATION_PATH,
    VERSION,
    GET_BAG_FILE_LIST,
    GET_BAG_FILE,
    DELETE_BAG_FILE,
    DEFAULT = 99,
};

class Communication:public QObject
{
    Q_OBJECT
public:
    Communication();
    virtual ~Communication();

    void Send(std::shared_ptr<navigation::NavigationMsg> msg);
    bool Connect();
    void SetIPAddr(const std::string& conrtol, const std::string& message);


private:
    void loop();
    void loop2();

    void on_send(websocketpp::connection_hdl _hdl,std::vector<uint8_t> &msg);
    void on_message(client *c, websocketpp::connection_hdl _hdl,message_ptr msg);
    void on_open(client* c, websocketpp::connection_hdl _hdl);
    void on_close(client* c, websocketpp::connection_hdl _hdl);
    emKEYTYPE ParsingMsgType(const std::string& key);

    void show_version();

private:

    bool ip_set_;
    bool connect_;

    std::string conrtol_addr_;
    std::string message_addr_;

    std::thread *control_client_thread_;
    std::thread *message_client_thread_;

    client client_control_;
    client client_message_;

    websocketpp::connection_hdl hdl1_,hdl2_;

signals:
    void display_msg(const QString& msg);
    void success_connect();
    void success_start_scan_map();
    void success_cancel_scan_map();
    void success_async_stop_scan_map();
    void success_scan_status_push(const QString& status);
    void success_map_png(const std::string& binary);
    void success_get_map_list(const std::string& text_data);
    void success_real_position(const std::string& text_data);
    void success_real_laser_phit(const std::string& text_data);
    void success_real_display(const std::string& binary);
    void success_sync_progress_push(const std::string& text_data);
    void success_initialize_customized();
    void success_move();
    void success_charge(const QString& text);
    void success_navigate(const QString& text);
    void success_real_navigation_path(const std::string& text_data);
    void success_virtual_obstacles(const std::string& text_data);
    void success_position_list(const std::string& text_data);
    void success_navigation_path_list(const std::string& text_data);
    void success_navigation_path_list_detail(const std::string& text_data);
    void success_navigation_path_detail(const std::string& text_data);
    void success_verification_point(bool success);
    void success_version(const std::string& text_data);
    void success_get_bag_file_list(const std::string& text_data);
    void success_get_bag_file(const std::string& binary);
};

#endif // COMMUNICATION_H
