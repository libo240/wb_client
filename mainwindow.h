#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QLabel>
#include <QModelIndex>
//#include <QTextEdit>
//#include <QMouseEvent>
#include <qmylabel.h>
#include <QTreeWidgetItem>

#include <iostream>
#include <thread>
#include <jsoncpp/json/json.h>

#include "qmylistwidget.h"
#include "communication.h"
#include "NavigationMsg.pb.h"


#define DELETE(p) if(NULL != p){\
    delete p;\
    p = NULL;}

namespace Ui {
class MainWindow;
}


struct stSPEED
{
    double linearSpeed;
    double angularSpeed;
};

struct stControl
{
    QPoint real_position;
    std::string binary_data;
    std::string real_laser_phit;
    std::string virtual_obstacles;
    std::string position_list;
    std::string navigation_path_detail;
    std::string real_navigation_path;
    
    void Clear()
    {
        real_position = QPoint(-1,-1);
        binary_data.clear();
        real_laser_phit.clear();
        virtual_obstacles.clear();
        position_list.clear();
        navigation_path_detail.clear();
        real_navigation_path.clear();
    }
};


struct stPos
{
    int width;
    int heigh;
    double x;
    double y;
};

using namespace std;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:    
    void Init();
    void DrawMap();
    
private:
    Ui::MainWindow *ui;
    QLabel* ui_le_status_bar_;

    bool is_update_;
    bool is_set_path_;
    bool is_verification_;
    bool is_verification_point_;

    string current_map_name_;
    string current_pos_name_;
    string current_path_list_name_;
    string current_path_name_;
    string curren_bag_name_;
    map<string, string> map_list_msg_;
    map<string, string> map_point_list_;
    map<string, string> map_path_list_;
    map<string, string> map_bag_list_;


    stSPEED current_speed_;
    stControl control_data_;
    
    shared_ptr<navigation::NavigationMsg> output_msg_;

    stPos init_pos;
    Communication comm_;

    std::vector<std::pair<int,int>> set_nav_path_;
    std::vector<std::pair<int,int>> set_obstacles_points;
    QString shape_;
    Json::Value set_obstacles_;

signals:
    void current_pos(const QString &x, QString & y);

private slots:
    void set_ip_addr(const QString& control, const QString& message);
    void set_scan_map_name(const QString& msg, int type);
    //on_pb_cancel_clicked()
    //on_pb_save_clicked()
    void map_png();
    //on_pb_get_map_list_clicked()
    void delete_map();
    //on_pb_real_laser_phit_clicked()
    //on_pb_real_display_clicked()
    void sync_map(const QString& conrtol_type_, const QString& data_type);
    void load_map();
    void initialize_customized(const QString &angle, const QString &x, const QString &y);
    void Move();
    //on_pb_charge_clicked()
    void navigate_point();
    void navigate_position(const QString &angle, const QString &x, const QString &y);
    //on_pb_pasue_nav_clicked()
    //on_pb_cancel_nav_clicked()
    //Todo::real_navigation_path
    void navigate_path();
    void virtual_obstacles();
    void update_virtual_obstacles();
    void nav_list(const QAction* test);
    void record_position(const QString& t1, const QString& t2, const QString& t3);
    void delete_position();
    void rename_position(const QString& map_name, const QString& old_name, const QString& new_name);
    void navigation_path_list_detail(const QTreeWidgetItem *index);
    void navigation_path_detail();
    void verification_point(const double angle, const int gridx, const int gridy);
    void set_navigation_path();
    void show_version();
    //on_pb_bag_list_clicked()
    void get_bag_file();
    void delete_bag_file();



    void display_msg(const QString& msg);
    void label_key_press(Qt::Key key);
    void show_img_list(QMouseEvent *ev);
//    void show_map_message(const QModelIndex& index,bool check);
    void show_map_task_list(const QAction *action);
//    void show_pos_message(const QModelIndex& index,bool check);
    void show_pos_task_list();
//    void show_path_message(const QModelIndex& index,bool check);
    void show_path_task_list();
//    void show_bag_message(const QModelIndex& index,bool check);
    void show_bag_task_list(const QModelIndex &index);
    void show_initialize_box();
    void show_rename_box();
    void show_record_box();
    void show_nav_position_box();
    void show_update_obstacles_box();
    void show_set_nav_path_box();
    void download();
    void upload();

    void save_shape();
    void set_shape(const QString &obstacles);
    void redraw();
    void is_update(bool is_update);
    void is_set_nav(bool statu);


    void success_connect();
    void success_start_scan_map();
    void success_cancel_scan_map();
    void success_async_stop_scan_map();
    void success_scan_status_push(const QString& status);
    void success_map_png(const std::string& binary);
    void success_get_map_list(const string &text_data);
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
    void success_navigation_path_list(const string& text_data);
    void success_navigation_path_list_detail(const string &text_data);
    void success_navigation_path_detail(const std::string& text_data);
    void success_verification_point(bool success);
    void success_version(const std::string& text_data);
    void success_get_bag_file_list(const string& text_data);
    bool success_get_bag_file(const string& binary_data);
    
private slots:
    void on_pb_setip_clicked();
    void on_pb_start_scan_map_clicked();
    void on_pb_get_map_list_clicked();
    void on_pb_cancel_clicked();
    void on_pb_save_clicked();
    void on_pb_real_laser_phit_clicked();
    void on_pb_up_clicked();
    void on_pb_right_clicked();
    void on_pb_down_clicked();
    void on_pb_left_clicked();
    void on_pb_nav_list_clicked();
    void on_pb_charge_clicked();
    void on_pb_pasue_nav_clicked();
    void on_pb_cancel_nav_clicked();
    void on_pb_bag_list_clicked();
    void on_pb_real_display_clicked();
};

#endif // MAINWINDOW_H
