#include "communication.h"
#include "jsoncpp/json/json.h"

using namespace std;
Communication::Communication()
{
    connect_ = false;
    ip_set_ = false;
    control_client_thread_ = new std::thread(bind(&Communication::loop,this));;
    message_client_thread_ = new std::thread(bind(&Communication::loop2, this));;


    conrtol_addr_ = "";
    message_addr_ = "";
}

Communication::~Communication()
{
    DELETE(control_client_thread_);
    DELETE(message_client_thread_)
}

void Communication::Send(std::shared_ptr<navigation::NavigationMsg> msg)
{
    std::vector<uint8_t> buf(msg->ByteSize());
    msg->SerializeToArray(&buf[0], msg->ByteSize());
    on_send(hdl1_,buf);
}

bool Communication::Connect()
{
    return connect_;
}

void Communication::SetIPAddr(const string &conrtol, const string &message)
{
    conrtol_addr_ = conrtol;
    message_addr_ = message;
    ip_set_ = true;
}

void Communication::loop()
{
    while(1)
    {
        if(!ip_set_)
        {
            usleep(3000);
            continue;
        }        


        std::cout << conrtol_addr_ << std::endl;

        try
        {
            client_control_.set_access_channels(websocketpp::log::alevel::all);
            client_control_.clear_access_channels(websocketpp::log::alevel::frame_payload);
            client_control_.set_error_channels(websocketpp::log::elevel::all);


            // Initialize ASIO
            client_control_.init_asio();

            // Register our message handler

            client_control_.set_message_handler(bind(&Communication::on_message,this,&client_control_,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
            client_control_.set_close_handler(bind(&Communication::on_close,this,&client_control_,websocketpp::lib::placeholders::_1));
            client_control_.set_open_handler(bind(&Communication::on_open,this,&client_control_,websocketpp::lib::placeholders::_1));

            websocketpp::lib::error_code ec;
            client::connection_ptr con1 = client_control_.get_connection(conrtol_addr_, ec);
            if (ec)
            {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                exit(0);
            }
            hdl1_ = con1->get_handle();


            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            client_control_.connect(con1);
            /*c.connect(con2);*/
            // Start the ASIO io_service run loop
            // this will cause a single connection to be made to the server. c.run()
            // will exit when this connection is closed.
            client_control_.run();
        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;

        }
    }
}

void Communication::loop2()
{
    while(1)
    {
        if(!ip_set_)
        {
            usleep(3000);
            continue;
        }
        usleep(100);

        std::cout << message_addr_ << std::endl;

        try
        {
            client_message_.set_access_channels(websocketpp::log::alevel::all);
            client_message_.clear_access_channels(websocketpp::log::alevel::frame_payload);
            client_message_.set_error_channels(websocketpp::log::elevel::all);


            // Initialize ASIO
            client_message_.init_asio();

            // Register our message handler

            client_message_.set_message_handler(bind(&Communication::on_message,this,&client_message_,websocketpp::lib::placeholders::_1,websocketpp::lib::placeholders::_2));
            client_message_.set_close_handler(bind(&Communication::on_close,this,&client_message_,websocketpp::lib::placeholders::_1));
            client_message_.set_open_handler(bind(&Communication::on_open,this,&client_message_,websocketpp::lib::placeholders::_1));

            websocketpp::lib::error_code ec;
            client::connection_ptr con1 = client_message_.get_connection(message_addr_, ec);
            if (ec)
            {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                exit(0);
            }
            hdl2_ = con1->get_handle();


            // Note that connect here only requests a connection. No network messages are
            // exchanged until the event loop starts running in the next line.
            client_message_.connect(con1);
            /*c.connect(con2);*/
            // Start the ASIO io_service run loop
            // this will cause a single connection to be made to the server. c.run()
            // will exit when this connection is closed.
            client_message_.run();
        }
        catch (websocketpp::exception const & e)
        {
            std::cout << e.what() << std::endl;

        }
    }
}


void Communication::on_send(websocketpp::connection_hdl _hdl, vector<uint8_t> &msg)
{
    //Todo
//    client_control_.send(_hdl, static_cast<void const*>(&msg[0]), msg.size(), websocketpp::frame::opcode::binary);
}

void Communication::on_message(client *c, websocketpp::connection_hdl _hdl, message_ptr msg)
{
    std::vector<uint8_t> buf(msg->get_payload().begin(), msg->get_payload().end());

    navigation::NavigationMsg input_msg;

    input_msg.ParseFromArray(&buf[0],buf.size());
    std::string key = input_msg.key();
    std::string uuid = input_msg.uuid();

    std::string text_data = input_msg.textdata();

    if(key != "real_position" && key != "scan_status_push" && key != "task_status")
    {
        std::cout << "get\tkey:"<<key<<"\t"<<"uuid:"<<uuid<< std::endl;
        cout << text_data << endl;

    }

    bool success = false;
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

        success = tree["successed"].asBool();


        if(!success)
        {
            QString error_msg = "error:";
            emit display_msg(error_msg);
        }
    }
    catch(exception& e)
    {
        cout << "on_message: Json -- parse textData failed:"<< e.what() <<endl;
    }

    emKEYTYPE key_flag = ParsingMsgType(key);

    switch (key_flag)
    {
    case START_SCAN_MAP:
    {
        emit success_start_scan_map();
        break;
    }
    case CANCEL_SCAN_MAP:
    {
        emit success_cancel_scan_map();
        break;
    }
    case ASYNC_STOP_SCAN_MAP:
    {
        emit success_async_stop_scan_map();
        break;
    }
    case SCAN_STATUS_PUSH:
    {
        try
        {
            Json::Value tree;
            Json::String err;
            Json::CharReaderBuilder reader;
            std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
            json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

            int i = tree["data"]["status"].asInt();

            QString status = QString("status:%1").arg(i);

            emit success_scan_status_push(status);
        }
        catch(exception& e)
        {
            cout << "on_message: Json -- parse scan_status_push fail:" << e.what() << endl;
        }
        break;
    }
    case MAP_PNG:
    {
        emit success_map_png(input_msg.binarydata());
        break;
    }
    case GET_MAP_LIST:
    {
        emit success_get_map_list(text_data);
        break;
    }
    case DELETE_MAP:
    {
        emit display_msg("success:Map has been deleted.");
        break;
    }
    case REAL_POSITION:
    {
        emit success_real_position(text_data);
        break;
    }
    case REAL_LASER_PHIT:
    {
        emit success_real_laser_phit(text_data);
        break;
    }
    case REAL_DISPLAY:
    {
        emit success_real_display(input_msg.binarydata());
        break;
    }
    case SYNC_MAP:
    {
        emit display_msg("success:sync_map.");
        break;
    }
    case SYNC_PROGRESS_PUSH:
    {
        emit success_sync_progress_push(text_data);
        break;
    }
    case LOAD_MAP:
    {
        emit display_msg("success:Map has been loaded.");
        break;
    }
    case INITIALIZE_CUSTOMIZED:
    {
        emit success_initialize_customized();
        emit display_msg("success:Map has been initialized.");
        break;
    }
    case MOVE:
    {
        emit success_move();
        break;
    }
    case START_CHARGE:
    {
        emit success_charge("stop_charge");
        break;
    }
    case STOP_CHARGE:
    {
        emit success_charge("start_charge");
        break;
    }
    case NAVIGATE_POINT:
    {
        emit display_msg("success:Navigate to point.");
        break;
    }
    case NAVIGATE_POSITION:
    {
        emit display_msg("success:Navigate to position.");
        break;
    }
    case PAUSE_NAVIGATE:
    {

        emit success_navigate("resume_nav");
        emit display_msg("success:Navigation has been paused.");
        break;
    }
    case RESUME_NAVIGATE:
    {
        emit display_msg("success:Navigation has been resumed.");
        emit success_navigate("pause_nav");
        break;
    }
    case CANCEL_NAVIGATE:
    {
        display_msg("success:Navigation has been cancelled.");
        break;
    }
    case TASK_STATUS:
    {
        //Todo
        break;
    }
    case REAL_NAVIGATION_PATH:
    {
        emit success_real_navigation_path(text_data);
        break;
    }
    case NAVIGATE_PATH:
    {
        emit display_msg("success:navigate_path.");
        break;
    }
    case VIRTUAL_OBSTACLES:
    {
        emit success_virtual_obstacles(text_data);
        break;
    }
    case UPDATE_VIRTUAL_OBSTACLES:
    {
        emit display_msg("success:The virtual obstacles is added.");
        //Todo
        break;
    }
    case POSITION_LIST:
    {
        emit success_position_list(text_data);
        break;
    }
    case RECORD_POSITION:
    {
        emit display_msg("success:The position is added.");
        break;
    }
    case DELETE_POSITION:
    {
        emit display_msg("success:The position has been deleted.");
        break;
    }
    case RENAME_POSITION:
    {
        emit display_msg("success:The position has been renamed.");
        break;
    }
    case NAVIGATION_PATH_LIST:
    {
        emit success_navigation_path_list(text_data);
        break;
    }
    case NAVIGATION_PATH_LIST_DETAIL:
    {
        emit success_navigation_path_list_detail(text_data);
        break;
    }
    case NAVIGATION_PATH_DETAIL:
    {
        emit success_navigation_path_detail(text_data);
        break;
    }
    case VERIFICATION_POINT:
    {
        emit success_verification_point(success);
        break;
    }
    case SET_NAVIGATION_PATH:
    {
        emit display_msg("success:set_navigation_path.");
        break;
    }
    case VERSION:
    {
        emit success_version(text_data);
        break;
    }
    case GET_BAG_FILE_LIST:
    {
        emit success_get_bag_file_list(text_data);
        break;
    }
    case GET_BAG_FILE:
    {
        emit success_get_bag_file(input_msg.binarydata());
        break;
    }
    case DELETE_BAG_FILE:
    {
        emit display_msg("success:Bag has been deleted.");
        break;
    }
    default:
        break;
    }
}

void Communication::on_open(client *c, websocketpp::connection_hdl _hdl)
{
    string msg = "hello";
    c->send(_hdl,msg,websocketpp::frame::opcode::text);
    c->get_alog().write(websocketpp::log::alevel::app,"Tx:"+msg);
    connect_ = true;

    emit success_connect();
    show_version();//获取版本
}

void Communication::on_close(client *c, websocketpp::connection_hdl _hdl)
{
    connect_ = false;
    c->close(_hdl,websocketpp::close::status::normal,"");
}

emKEYTYPE Communication::ParsingMsgType(const string &key)
{
    if("start_scan_map" == key)
    {
        return START_SCAN_MAP;
    }
    if("cancel_scan_map" == key)
    {
        return CANCEL_SCAN_MAP;
    }
    if("async_stop_scan_map" == key)
    {
        return ASYNC_STOP_SCAN_MAP;
    }
    if("scan_status_push" == key)
    {
        return SCAN_STATUS_PUSH;
    }
    if("map_png" == key)
    {
        return MAP_PNG;
    }
    if("get_map_list" == key)
    {
        return GET_MAP_LIST;
    }
    if("delete_map" == key)
    {
        return DELETE_MAP;
    }
    if("real_position" == key)
    {
        return REAL_POSITION;
    }
    if("real_laser_phit" == key)
    {
        return REAL_LASER_PHIT;
    }
    if("real_display" == key)
    {
        return REAL_DISPLAY;
    }
    if("sync_map" == key)
    {
        return SYNC_MAP;
    }
    if("sync_progress_push" == key)
    {
        return SYNC_PROGRESS_PUSH;
    }
    if("load_map" == key)
    {
        return LOAD_MAP;
    }
    if("initialize_customized" == key)
    {
        return INITIALIZE_CUSTOMIZED;
    }
    if("move" == key)
    {
        return MOVE;
    }
    if("start_charge" == key)
    {
        return START_CHARGE;
    }
    if("stop_charge" == key)
    {
        return STOP_CHARGE;
    }
    if("navigate_point" == key)
    {
        return NAVIGATE_POINT;
    }
    if("navigate_position" == key)
    {
        return NAVIGATE_POSITION;
    }
    if("pause_navigate" == key)
    {
        return PAUSE_NAVIGATE;
    }
    if("resume_navigate" == key)
    {
        return RESUME_NAVIGATE;
    }
    if("cancel_navigate" == key)
    {
        return CANCEL_NAVIGATE;
    }
    if("task_status" == key)
    {
        return TASK_STATUS;
    }
    if("real_navigation_path" == key)
    {
        return REAL_NAVIGATION_PATH;
    }
    if("navigate_path" == key)
    {
        return NAVIGATE_PATH;
    }
    if("virtual_obstacles" == key)
    {
        return VIRTUAL_OBSTACLES;
    }
    if("update_virtual_obstacles" == key)
    {
        return UPDATE_VIRTUAL_OBSTACLES;
    }
    if("position_list" == key)
    {
        return POSITION_LIST;
    }
    if("record_position" == key)
    {
        return RECORD_POSITION;
    }
    if("delete_position" == key)
    {
        return DELETE_POSITION;
    }
    if("rename_position" == key)
    {
        return RENAME_POSITION;
    }
    if("navigation_path_list" == key)
    {
        return NAVIGATION_PATH_LIST;
    }
    if("navigation_path_list_detail" == key)
    {
        return NAVIGATION_PATH_LIST_DETAIL;
    }
    if("navigation_path_detail" == key)
    {
        return NAVIGATION_PATH_DETAIL;
    }
    if("verification_point" == key)
    {
        return VERIFICATION_POINT;
    }
    if("set_navigation_path" == key)
    {
        return SET_NAVIGATION_PATH;
    }
    if("version" == key)
    {
        return VERSION;
    }
    if("get_bag_file_list" == key)
    {
        return GET_BAG_FILE_LIST;
    }
    if("get_bag_file" == key)
    {
        return GET_BAG_FILE;
    }
    if("delete_bag_file" == key)
    {
        return DELETE_BAG_FILE;
    }

    return DEFAULT;
}

void Communication::show_version()
{
    navigation::NavigationMsg output_msg;
    std::string key = "version";
    output_msg.set_key(key);
    output_msg.set_uuid("1");
    std::cout << "key:"<<output_msg.key()<<"\n\r"<<"uuid:"<<output_msg.uuid()<< std::endl;

    std::vector<uint8_t> buf(output_msg.ByteSize());
    output_msg.SerializeToArray(&buf[0], output_msg.ByteSize());
    on_send(hdl1_,buf);
}
