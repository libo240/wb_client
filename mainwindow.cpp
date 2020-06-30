#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "qmylistwidget.h"
#include "qmylabel.h"
#include "setipdialog.h"
#include "setmapdialog.h"
#include "syncmapdialog.h"
#include "datainitdialog.h"
#include "addobstaclesdialog.h"

#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QTreeWidget>
#include <QPoint>
#include <QList>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <fstream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui_le_status_bar_ = NULL;
    current_map_name_ = "";
    current_pos_name_ = "";
    current_path_name_ = "";
    curren_bag_name_ = "";
    is_update_ = false;
    is_set_path_ = false;
    is_verification_ = false;
    is_verification_point_ = false;
    output_msg_ = make_shared<navigation::NavigationMsg>();

    current_speed_.linearSpeed = 0;
    current_speed_.angularSpeed = 0;

    ui->setupUi(this);

    Init();
}

MainWindow::~MainWindow()
{
    DELETE(ui_le_status_bar_);
    //    DELECT(wb_client_thread_);
    delete ui;
}

void MainWindow::Init()
{    
    ui_le_status_bar_ = new QLabel;
    ui->statusbar->addWidget(ui_le_status_bar_);

    //    ui->pb_start_scan_map->setVisible(false);
    //    ui->pb_get_map_list->setVisible(false);
    //    ui->pb_nav_list->setVisible(false);

    //    ui->pb_bag_list->setVisible(false);
    //    ui->pb_real_display->setVisible(false);
    //    ui->pb_real_laser_phit->setVisible(false);
    //    ui->pb_cancel->setVisible(false);
    //    ui->pb_save->setVisible(false);
    //    ui->pb_charge->setVisible(false);
    //    ui->pb_pasue_nav->setVisible(false);
    //    ui->pb_cancel_nav->setVisible(false);

    //    ui->pb_up->setVisible(false);
    //    ui->pb_right->setVisible(false);
    //    ui->pb_down->setVisible(false);
    //    ui->pb_left->setVisible(false);

    //    ui->le_scan_status->setVisible(false);
    ui->le_scan_status->setStyleSheet("color:orange");

    qRegisterMetaType<std::string>("std::string");
    connect(ui->le_map, &QMyLabel::click ,this, &MainWindow::show_img_list);
    connect(ui->le_map, &QMyLabel::key_press, this, &MainWindow::label_key_press);
    connect(&comm_, &Communication::success_connect, this, &MainWindow::success_connect);
    connect(&comm_, &Communication::success_start_scan_map, this, &MainWindow::success_start_scan_map);
    connect(&comm_, &Communication::success_cancel_scan_map, this, &MainWindow::success_cancel_scan_map);
    connect(&comm_, &Communication::success_async_stop_scan_map, this, &MainWindow::success_async_stop_scan_map);
    connect(&comm_, &Communication::success_scan_status_push, this, &MainWindow::success_scan_status_push);
    connect(&comm_, &Communication::success_map_png, this, &MainWindow::success_map_png);
    connect(&comm_, &Communication::success_get_map_list, this, &MainWindow::success_get_map_list);
    connect(&comm_, &Communication::success_real_position, this, &MainWindow::success_real_position);
    connect(&comm_, &Communication::success_real_laser_phit, this, &MainWindow::success_real_laser_phit);
    connect(&comm_, &Communication::success_real_display, this, &MainWindow::success_real_display);
    connect(&comm_, &Communication::success_sync_progress_push, this, &MainWindow::success_sync_progress_push);
    connect(&comm_, &Communication::success_initialize_customized, this, &MainWindow::success_initialize_customized);
    connect(&comm_, &Communication::success_move, this, &MainWindow::success_move);
    connect(&comm_, &Communication::success_charge, this, &MainWindow::success_charge);
    connect(&comm_, &Communication::success_navigate, this, &MainWindow::success_navigate);
    connect(&comm_, &Communication::success_real_navigation_path, this, &MainWindow::success_real_navigation_path);
    connect(&comm_, &Communication::success_virtual_obstacles, this, &MainWindow::success_virtual_obstacles);
    connect(&comm_, &Communication::success_position_list, this, &MainWindow::success_position_list);
    connect(&comm_, &Communication::success_navigation_path_list, this, &MainWindow::success_navigation_path_list);
    connect(&comm_, &Communication::success_navigation_path_list_detail, this, &MainWindow::success_navigation_path_list_detail);
    connect(&comm_, &Communication::success_navigation_path_detail, this, &MainWindow::success_navigation_path_detail);
    connect(&comm_, &Communication::success_version, this, &MainWindow::success_version);
    connect(&comm_, &Communication::success_get_bag_file_list, this, &MainWindow::success_get_bag_file_list);
    connect(&comm_, &Communication::success_get_bag_file, this, &MainWindow::success_get_bag_file);

    string text_data = "{\"data\":[{\"angle\":-4.7219500215147585,\"createdAt\":\"2016-08-11 04:13:38\",\"gridX\":458,\"gridY\":238,\"id\":0,\"mapId\":0,\"mapName\":\"test\",\"name\":\"demo_position\",\"type\":0,\"worldPose\":{\"orientation\":{\"w\":0.9991511204590953,\"x\":0,\"y\":0,\"z\":-0.041195126961019124},\"position\":{\"x\":-1.873163616425341,\"y\":-12.877138903739242,\"z\":0}}},{\"angle\":4.566441294071982,\"createdAt\":\"2016-08-11 04:15:07\",\"gridX\":253,\"gridY\":315,\"id\":0,\"mapId\":0,\"mapName\":\"test\",\"name\":\"demo_position1\",\"type\":1,\"worldPose\":{\"orientation\":{\"w\":0.9992061050614292,\"x\":0,\"y\":0,\"z\":0.03983917177814183},\"position\":{\"x\":-12.136897140857005,\"y\":-9.005574429284978,\"z\":0}}}],\"successed\":true}";
    success_position_list(text_data);
}

void MainWindow::success_get_map_list(const string &text_data)
{
    map_list_msg_.clear();
    QMenu* ui_map_list = new QMenu(this);
    ui_map_list->setObjectName("map_list");

    connect(ui_map_list, &QMenu::triggered, this, &MainWindow::show_map_task_list);
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
        for(unsigned int i = 0; i < tree["data"].size(); i++)
        {
            string name = tree["data"][i]["name"].asString();
            ui_map_list->addAction(QString::fromStdString(name));
            map_list_msg_.insert(pair<string, string>(name, tree["data"][i].toStyledString()));
        }
    }
    catch(exception& e)
    {
        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    }
    QPoint pos = QCursor::pos();
    ui_map_list->setGeometry(pos.x(), pos.y(),120,200);
    ui_map_list->show();
}

void MainWindow::success_real_position(const string &text_data)
{
    control_data_.real_position = QPoint(-1,-1);
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

        string map_name = tree["data"]["mapName"].asString();
        ui->gb_map->setTitle(QString::fromStdString(map_name));
        int x = round(tree["data"]["gridPosition"]["x"].asDouble());
        int y = round(tree["data"]["gridPosition"]["y"].asDouble());
        control_data_.real_position.setX(x);
        control_data_.real_position.setY(y);
    }
    catch(exception& e)
    {
        cout << "on_message:Json -- parse sync_progress_push fail:" << e.what() << endl;
    }
    DrawMap();
}

void MainWindow::success_real_laser_phit(const string &text_data)
{
    control_data_.Clear();
    control_data_.real_laser_phit = text_data;
}

void MainWindow::success_real_display(const string &binary)
{
    control_data_.Clear();
    control_data_.binary_data = binary;
}

void MainWindow::success_sync_progress_push(const string &text_data)
{
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
        int i = tree["data"]["progress"].asInt();
        SyncMapDialog* temp = this->findChild<SyncMapDialog*>();
        if(NULL != temp)
        {
            temp->UpdateProgess(i);
        }
    }
    catch(exception& e)
    {
        cout << "on_message:Json -- parse sync_progress_push fail:" << e.what() << endl;
    }
}

void MainWindow::success_initialize_customized()
{
    ui->pb_up->setVisible(true);
    ui->pb_right->setVisible(true);
    ui->pb_down->setVisible(true);
    ui->pb_left->setVisible(true);
    ui->pb_charge->setVisible(true);
    ui->pb_nav_list->setVisible(true);
    ui->pb_pasue_nav->setVisible(true);
    ui->pb_cancel_nav->setVisible(true);
}

void MainWindow::success_move()
{
    QString temp = QString("current speed: linearSpeed:%1, angularSpeed:%2")\
            .arg(QString::number(current_speed_.linearSpeed)).arg(QString::number(current_speed_.angularSpeed));
    display_msg(temp);
}

void MainWindow::success_charge(const QString &text)
{
    ui->pb_charge->setText(text);
}

void MainWindow::success_navigate(const QString &text)
{
    ui->pb_pasue_nav->setText(text);
}

void MainWindow::success_real_navigation_path(const string &text_data)
{
    control_data_.real_navigation_path = text_data;
}

void MainWindow::success_virtual_obstacles(const string &text_data)
{
    control_data_.virtual_obstacles = text_data;
}

void MainWindow::success_position_list(const string &text_data)
{
    control_data_.position_list = text_data;
    //    {
    //        map_point_list_.clear();
    //        QMyListWidget* lw_nav_list = new QMyListWidget(this);

    //        connect(lw_nav_list, &QMyListWidget::left_clicked, this, &MainWindow::show_pos_message);
    //        connect(lw_nav_list, &QMyListWidget::right_clicked, this, &MainWindow::show_pos_task_list);

    //        try
    //        {
    //            Json::Value tree;
    //            Json::String err;
    //            Json::CharReaderBuilder reader;
    //            std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
    //            json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
    //            for(unsigned int i = 0; i < tree["data"].size(); i++)
    //            {
    //                string name = tree["data"][i]["name"].asString();
    //                lw_nav_list->addItem(QString::fromStdString(name));
    //                map_point_list_.insert(pair<string, string>(name, tree["data"][i].toStyledString()));
    //            }
    //        }
    //        catch(exception& e)
    //        {
    //            cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    //        }

    //        lw_nav_list->setGeometry(10,160,105,300);
    //        lw_nav_list->show();
    //    }
    map_point_list_.clear();

    QTreeWidget* treeWidget = ui->sa_nav->findChild<QTreeWidget*>("navigation_tree");
    if(NULL == treeWidget)
    {
        treeWidget = new QTreeWidget(ui->sa_nav);
        treeWidget->setObjectName("navigation_tree");
        treeWidget->setHeaderHidden(true);
    }
    treeWidget->clear();

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    disconnect(treeWidget,&QTreeWidget::itemDoubleClicked, this, &MainWindow::navigation_path_detail);
    disconnect(treeWidget,&QTreeWidget::customContextMenuRequested, this, &MainWindow::show_path_task_list);
    connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::show_pos_task_list);
    QTreeWidgetItem* points = new QTreeWidgetItem(treeWidget);
    points->setText(0,"points");
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

        for(unsigned int i = 0; i < tree["data"].size(); i++)
        {
            string name = tree["data"][i]["name"].asString();
            QTreeWidgetItem* point = new QTreeWidgetItem(points);
            point->setText(0,QString::fromStdString(name));
            map_point_list_.insert(pair<string, string>(name, tree["data"][i].toStyledString()));
        }
    }
    catch(exception& e)
    {
        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    }
    ui->sa_nav->setMinimumSize(300,300);
    treeWidget->setGeometry(0,0,300,300);
    treeWidget->show();
}

void MainWindow::success_navigation_path_list(const string &text_data)
{
    QTreeWidget* treeWidget = ui->sa_nav->findChild<QTreeWidget*>("navigation_tree");
    if(NULL == treeWidget)
    {
        treeWidget = new QTreeWidget(ui->sa_nav);
        treeWidget->setObjectName("navigation_tree");
        treeWidget->setHeaderHidden(true);
    }
    treeWidget->clear();
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    disconnect(treeWidget,&QTreeWidget::customContextMenuRequested, this, &MainWindow::show_pos_task_list);
    connect(treeWidget, &QTreeWidget::itemDoubleClicked , this, &MainWindow::navigation_path_list_detail);
    connect(treeWidget, &QTreeWidget::customContextMenuRequested,this,&MainWindow::show_path_task_list);
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

        for(unsigned int i = 0; i < tree["data"].size(); i++)
        {
            string name = tree["data"][i]["path_list_name"].asString();
            QTreeWidgetItem* list_name = new QTreeWidgetItem(treeWidget);
            list_name->setText(0,QString::fromStdString(name));
        }
    }
    catch(exception& e)
    {
        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    }
    ui->sa_nav->setMinimumSize(300,300);
    treeWidget->setGeometry(0,0,300,300);
    treeWidget->show();


//    map_path_list_.clear();
//    QMyListWidget* ui_path_list = new QMyListWidget(this);

//    connect(ui_path_list, &QMyListWidget::left_clicked, this, &MainWindow::show_path_message);
//    connect(ui_path_list, &QMyListWidget::right_clicked, this, &MainWindow::show_path_task_list);

//    try
//    {
//        Json::Value tree;
//        Json::String err;
//        Json::CharReaderBuilder reader;
//        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
//        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
//        for(unsigned int i = 0; i < tree["data"].size(); i++)
//        {
//            string name = tree["data"][i]["path_list_name"].asString();
//            ui_path_list->addItem(QString::fromStdString(name));
//            map_path_list_.insert(pair<string, string>(name, tree["data"][i].toStyledString()));
//        }
//    }
//    catch(exception& e)
//    {
//        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
//    }

//    ui_path_list->setGeometry(10,160,105,300);
    //    ui_path_list->show();
}

void MainWindow::success_navigation_path_list_detail(const string &text_data)
{
    QTreeWidget* treeWidget = ui->sa_nav->findChild<QTreeWidget*>("navigation_tree");
    if(NULL == treeWidget)
    {
        return;
    }

    QTreeWidgetItem* path_list = NULL;

    for(int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        if(current_path_list_name_ == treeWidget->topLevelItem(i)->text(0).toStdString())
        {
            path_list = treeWidget->topLevelItem(i);
        }
    }
    if(NULL == path_list)
    {
        return;
    }
    path_list->takeChildren();

    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);

        for(unsigned int i = 0; i < tree["data"]["path_list"].size(); i++)
        {
            string name = tree["data"]["path_list"][i]["path_name"].asString();
            QTreeWidgetItem* path_name = new QTreeWidgetItem(path_list);
            path_name->setText(0,QString::fromStdString(name));
        }
    }
    catch(exception& e)
    {
        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    }
    ui->sa_nav->setMinimumSize(300,300);
    treeWidget->setGeometry(0,0,300,300);
    treeWidget->show();

}

void MainWindow::success_navigation_path_detail(const string &text_data)
{
    control_data_.navigation_path_detail = text_data;
}

void MainWindow::success_verification_point(bool success)
{
    is_verification_ = true;
    is_verification_point_ = success;
}

void MainWindow::success_version(const string &text_data)
{
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
        QString str = QString::fromStdString(tree["data"].asString());
        QString version = QString("version:%1").arg(str);
        ui->le_version->setText(version);
    }
    catch(exception& e)
    {
        cout << "on_message: Json -- parse version fail:" << e.what() << endl;
    }
}

void MainWindow::success_get_bag_file_list(const string &text_data)
{
    map_bag_list_.clear();

    QMyListWidget* ui_bag_list = new QMyListWidget(this);
    ui_bag_list->setObjectName("ui_bag_list");
    connect(ui_bag_list, &QMyListWidget::right_clicked, this, &MainWindow::show_bag_task_list);
    try
    {
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(text_data.c_str(), text_data.c_str() + text_data.length(), &tree,&err);
        for(unsigned int i = 0; i < tree["data"].size(); i++)
        {
            string data_name = tree["data"][i]["file_name"].asString();
            ui_bag_list->addItem(QString::fromStdString(data_name));
            map_bag_list_.insert(pair<string, string>(data_name, tree["data"][i].toStyledString()));
        }
    }
    catch(exception& e)
    {
        cout<<"Json -- parse TextData failed :"<<e.what()<<endl;
    }

    ui_bag_list->setGeometry(QCursor::pos().x()-105,QCursor::pos().y(),105,300);
    ui_bag_list->show();
}

void MainWindow::DrawMap()
{
    if(control_data_.binary_data.empty() && control_data_.real_laser_phit.empty())
    {
        return;
    }

    //binarydata
    cv::Mat img;
    double zoom = ui->sp_zoom->text().toDouble();

    bool load_lar = true;
    std::vector<uchar> img_data(control_data_.binary_data.begin(), control_data_.binary_data.end());
    if(img_data.size())
    {
        img = cv::imdecode(img_data, CV_LOAD_IMAGE_COLOR);
        load_lar = false;
    }

    if(load_lar)
    {
        //激光数据
        try
        {
            string laser = control_data_.real_laser_phit;
            Json::Value tree;
            Json::String err;
            Json::CharReaderBuilder reader;
            std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
            json_read->parse(laser.c_str(), laser.c_str() + laser.length(), &tree,&err);

            int heigh = tree["data"]["mapInfo"]["gridHeight"].asInt();
            int width = tree["data"]["mapInfo"]["gridWidth"].asInt();
            img = cv::Mat::zeros(heigh, width, CV_8UC3);


            Json::Value gridPhits = tree["data"]["gridPhits"];
            for(unsigned int i = 0; i < gridPhits.size(); i++)
            {
                cv::Point pt;
                pt.x = gridPhits[i]["x"].asDouble();
                pt.y = heigh - gridPhits[i]["y"].asDouble();

                if(pt.x >= 0 && pt.x <= width && pt.y >= 0 && pt.y <= heigh)
                {
                    img.at<cv::Vec3b>(pt.x, pt.y) = cv::Vec3b(255,0,0);
                }
            }
        }
        catch(exception& e)
        {
            cout << "Json -- parse real_laser_phit failed:" << e.what() <<endl;
        }
    }


    int thickness = 2;
    if(zoom < 1)
    {
        thickness = round(thickness/zoom);
    }

    //虚拟墙
    try
    {
        string virtual_o = control_data_.virtual_obstacles;
        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(virtual_o.c_str(), virtual_o.c_str() + virtual_o.length(), &tree,&err);

        Json::Value::Members list = tree["data"]["obstacles"].getMemberNames();
        foreach (Json::String key_name, list)
        {
            //圆
            if(key_name == "circles")
            {
                Json::Value circles = tree["data"]["obstacles"]["circles"];
                for(unsigned int i = 0; i < circles.size(); i++)
                {
                    cv::Point pt;
                    pt.x = circles[i]["center"]["x"].asInt();
                    pt.y = img.rows - circles[i]["center"]["y"].asInt();
                    int radius = round(circles[i]["radius"].asDouble());
                    cv::circle(img, pt, radius, cv::Scalar(0, 0,255),thickness);
                }
            }//圆

            //线
            if(key_name == "lines")
            {
                Json::Value lines = tree["data"]["obstacles"]["lines"];
                for(unsigned int i = 0; i < lines.size(); i++)
                {
                    cv::Point pt1, pt2;
                    pt1.x = lines[i]["start"]["x"].asInt();
                    pt1.y = lines[i]["start"]["y"].asInt();
                    pt2.x = lines[i]["end"]["x"].asInt();
                    pt2.y = lines[i]["end"]["y"].asInt();
                    cv::line(img, pt1, pt2, cv::Scalar(0, 0,255), thickness);
                }
            }//线

            //多边形
            if(key_name == "polygons")
            {
                Json::Value polygons = tree["data"]["obstacles"]["polygons"];
                for(unsigned int i = 0; i < polygons.size(); i++)
                {
                    for(unsigned int j = 0; j < polygons[i].size(); j++)
                    {
                        cv::Point pt1, pt2;
                        int m = (j+1)%polygons.size();
                        pt1.x = polygons[i][j]["x"].asInt();
                        pt1.y = polygons[i][j]["y"].asInt();
                        pt2.x = polygons[i][m]["x"].asInt();
                        pt2.y = polygons[i][m]["y"].asInt();
                        cv::line(img, pt1, pt2, cv::Scalar(0, 0,255), thickness);
                    }

                }
            }//多边形

            //折线
            if(key_name == "polylines")
            {
                Json::Value polylines = tree["data"]["obstacles"]["polylines"];
                for(unsigned int i = 0; i < polylines.size(); i++)
                {
                    for(unsigned int j = 0; j < polylines[i].size() - 1; j++)
                    {
                        int m = j+1;
                        cv::Point pt1, pt2;
                        pt1.x = polylines[i][j]["x"].asInt();
                        pt1.y = polylines[i][j]["y"].asInt();
                        pt2.x = polylines[i][m]["x"].asInt();
                        pt2.y = polylines[i][m]["y"].asInt();
                        cv::line(img, pt1, pt2, cv::Scalar(0, 0,255), thickness);
                    }

                }
            }//折线

            //矩形
            if(key_name == "rectangles")
            {
                Json::Value rectangles = tree["data"]["obstacles"]["rectangles"];
                for(unsigned int i = 0; i < rectangles.size(); i++)
                {
                    cv::Point pt1, pt2;
                    pt1.x = rectangles[i]["start"]["x"].asInt();
                    pt1.y = rectangles[i]["start"]["y"].asInt();
                    pt2.x = rectangles[i]["end"]["x"].asInt();
                    pt2.y = rectangles[i]["end"]["y"].asInt();
                    cv::rectangle(img, pt1, pt2, cv::Scalar(0, 0,255), thickness);
                }
            } //矩形
        }
    }
    catch(exception& e)
    {
        cout << "Json -- parse obstacles failed:" << e.what() <<endl;
    }//虚拟墙


    //导航路径详情
    try
    {
        int focus = 3;
        string path_detail = control_data_.navigation_path_detail;

        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(path_detail.c_str(), path_detail.c_str() + path_detail.length(), &tree,&err);

        Json::Value points = tree["data"]["points"];

        for(unsigned int i = 1; i < points.size(); i++)
        {
            cv::Point pt1,pt2;
            pt1.x = points[i-1]["grid_x"].asInt();
            pt1.y = img.rows - points[i-1]["grid_y"].asInt();
            pt2.x = points[i]["grid_x"].asInt();
            pt2.y = img.rows - points[i]["grid_y"].asInt();
            cv::line(img, pt1, pt2, cv::Scalar(125,125,0),thickness*focus);
        }
    }
    catch(exception& e)
    {
        cout << "Json -- parse position failed:" << e.what() <<endl;
    }//导航路径详情


    if(is_update_ && set_obstacles_points.size())
    {
        if(1 == set_obstacles_points.size())
        {
            cv::Point pt2;
            pt2.x = set_obstacles_points[0].first;
            pt2.y = img.rows - set_obstacles_points[0].second;
            cv::circle(img, pt2, thickness*3, cv::Scalar(125,0,125),-1);
        }
        else if (set_obstacles_points.size() > 1)
        {
            if (shape_ == "circles")
            {
                cv::Point pt1, pt2;
                pt1.x = set_obstacles_points[0].first;
                pt1.y = img.rows - set_obstacles_points[0].second;
                pt2.x = set_obstacles_points[1].first;
                pt2.y = img.rows - set_obstacles_points[1].second;

                int x = abs(pt1.x - pt2.x);
                int y = abs(pt1.y - pt2.y);
                int radius = round(sqrt(x*x+y*y));
                cv::circle(img, pt1, radius, cv::Scalar(0, 125,255),thickness);
            }
            if(shape_ == "lines")
            {
                cv::Point pt1, pt2;
                pt1.x = set_obstacles_points[0].first;
                pt1.y = img.rows - set_obstacles_points[0].second;
                pt2.x = set_obstacles_points[1].first;
                pt2.y = img.rows - set_obstacles_points[1].second;
                cv::line(img, pt1, pt2, cv::Scalar(0, 125,255), thickness);
            }
            if(shape_ == "polygons")
            {
                for(unsigned int i = 0; i < set_obstacles_points.size(); i++)
                {
                    cv::Point pt1, pt2;
                    int j = (i+1)%set_obstacles_points.size();
                    pt1.x = set_obstacles_points[i].first;
                    pt1.y = img.rows - set_obstacles_points[i].second;
                    pt2.x = set_obstacles_points[j].first;
                    pt2.y = img.rows - set_obstacles_points[j].second;
                    cv::line(img, pt1, pt2, cv::Scalar(0, 125,255), thickness);
                }
            }
            if(shape_ == "polylines")
            {
                for(unsigned int i = 0; i < set_obstacles_points.size() - 1; i++)
                {
                    cv::Point pt1, pt2;
                    int j = i+1;
                    pt1.x = set_obstacles_points[i].first;
                    pt1.y = img.rows - set_obstacles_points[i].second;
                    pt2.x = set_obstacles_points[j].first;
                    pt2.y = img.rows - set_obstacles_points[j].second;
                    cv::line(img, pt1, pt2, cv::Scalar(0, 125,255), thickness);
                }
            }
            if(shape_ == "rectangles")
            {
                cv::Point pt1, pt2;
                pt1.x = set_obstacles_points[0].first;
                pt1.y = img.rows - set_obstacles_points[0].second;
                pt2.x = set_obstacles_points[1].first;
                pt2.y = img.rows - set_obstacles_points[1].second;
                cv::rectangle(img, pt1, pt2, cv::Scalar(0, 125,255), thickness);
            }
        }

    }//update Virtual

    //地图点
    try
    {
        int focus = 3;
        string pos_list = control_data_.position_list;

        Json::Value tree;
        Json::String err;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader>const json_read(reader.newCharReader());
        json_read->parse(pos_list.c_str(), pos_list.c_str() + pos_list.length(), &tree,&err);
        for(unsigned int i = 0; i < tree["data"].size(); i++)
        {
            string name = tree["data"][i]["name"].asString();
            if(name == current_pos_name_)
            {
                focus = 4;
            }
            cv::Point pt;
            pt.x = tree["data"][i]["gridX"].asInt();
            pt.y = img.rows - tree["data"][i]["gridY"].asInt();
            cv::circle(img, pt, thickness*focus, cv::Scalar(0,255,0),-1);
        }
    }
    catch(exception& e)
    {
        cout << "Json -- parse position failed:" << e.what() <<endl;
    }//地图点

    //实时位置
    {
        QPoint pt = control_data_.real_position;
        if(pt.x() != -1 && pt.y() != -1)
        {
            cv::Point pt2;
            pt2.x = pt.x();
            pt2.y = img.rows - pt.y();
            cv::circle(img, pt2, thickness*3, cv::Scalar(255,125,0),-1);
        }
    }
    //实时位置


    init_pos.heigh = img.rows;
    init_pos.width = img.cols;

    int width = round(img.cols*zoom);
    int heigh = round(img.rows*zoom);
    ui->scrollAreaWidgetContents->setMinimumSize(width,heigh);
    ui->scrollAreaWidgetContents->resize(width,heigh);
    ui->le_map->resize(width,heigh);
    ui->le_map->setPixmap(QPixmap::fromImage(QImage((uchar*)img.data, img.cols, img.rows, QImage::Format_RGB888).scaled(ui->le_map->width(), ui->le_map->height())));
}

void MainWindow::success_connect()
{
    ui->pb_start_scan_map->setVisible(true);
    ui->pb_get_map_list->setVisible(true);
    ui->pb_bag_list->setVisible(true);
    ui->pb_real_display->setVisible(true);
    ui->pb_real_laser_phit->setVisible(true);
    ui->pb_nav_list->setVisible(true);

    ui->pb_charge->setVisible(true);
    ui->pb_pasue_nav->setVisible(true);
    ui->pb_cancel_nav->setVisible(true);

    ui->pb_up->setVisible(true);
    ui->pb_right->setVisible(true);
    ui->pb_down->setVisible(true);
    ui->pb_left->setVisible(true);
}

bool MainWindow::success_get_bag_file(const string &binary_data)
{
    ofstream outFile(curren_bag_name_);
    while (outFile.is_open())
    {
        outFile.write(binary_data.c_str(), binary_data.size());
        outFile.close();
    }

    return true;
}

void MainWindow::show_img_list(QMouseEvent *ev)
{
    if(ev->button() == Qt::RightButton)
    {
        init_pos.x = round(ev->pos().x()/ui->sp_zoom->text().toDouble());
        init_pos.y = init_pos.heigh - round(ev->pos().y()/ui->sp_zoom->text().toDouble());
        QMenu* map_img_task = new QMenu(this);
        QAction* init = map_img_task->addAction("initialize_customized");
        QAction* virtual_obstacles = map_img_task->addAction("virtual_obstacles");
        QAction* update_virtual_obstacles = map_img_task->addAction("update_virtual_obstacles");
        QAction* record_position = map_img_task->addAction("record_position");
        QAction* navigate_position = map_img_task->addAction("navigate_position");
        QAction* set_navigation_path = map_img_task->addAction("set_navigation_path");

        connect(init, &QAction::triggered, this, &MainWindow::show_initialize_box);
        connect(virtual_obstacles, &QAction::triggered, this, &MainWindow::virtual_obstacles);
        connect(update_virtual_obstacles, &QAction::triggered, this, &MainWindow::show_update_obstacles_box);
        connect(record_position, &QAction::triggered, this, &MainWindow::show_record_box);
        connect(navigate_position, &QAction::triggered, this, &MainWindow::show_nav_position_box);
        connect(set_navigation_path, &QAction::triggered, this, &MainWindow::show_set_nav_path_box);

        map_img_task->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 200, 150);
        map_img_task->show();
    }
    else if (ev->button() == Qt::LeftButton)
    {
        if(is_update_)
        {
            init_pos.x = round(ev->pos().x()/ui->sp_zoom->text().toDouble());
            init_pos.y = init_pos.heigh - round(ev->pos().y()/ui->sp_zoom->text().toDouble());
            QString x = QString::number(init_pos.x);
            QString y = QString::number(init_pos.y);
            emit current_pos(x, y);
            set_obstacles_points.push_back(pair<int, int>(init_pos.x, init_pos.y));
        }

        if(is_set_path_)
        {
            init_pos.x = round(ev->pos().x()/(ui->sp_zoom->text().toDouble()));
            init_pos.y = init_pos.heigh - round(ev->pos().y()/(ui->sp_zoom->text().toDouble()));
            QString x = QString::number(init_pos.x);
            QString y = QString::number(init_pos.y);            

            DataInitDialog* temp = this->findChild<DataInitDialog*>("nav_path");
            if(NULL != temp)
            {
                verification_point(0, init_pos.x, init_pos.y);
                temp->SetValue("0", x, y);
            }
            struct timeval tv1, tv2;
            gettimeofday(&tv1, NULL);
            while (!is_verification_)
            {
                gettimeofday(&tv2, NULL);
                if(((tv2.tv_sec - tv1.tv_sec)*1000 + (tv2.tv_usec - tv1.tv_usec)/1000) > 5000)
                {
                    break;
                }
                usleep(100);
                break;
            }
            if(is_verification_point_)
            {
                set_nav_path_.push_back(pair<int, int>(init_pos.x, init_pos.y));
                is_verification_ = false;
                is_verification_point_ = false;
            }
        }
    }
}

void MainWindow::label_key_press(Qt::Key key)
{
    switch (key) {
    case Qt::Key_Left:
    {
        on_pb_left_clicked();
        break;
    }
    case Qt::Key_Up:
    {
        on_pb_up_clicked();
        break;
    }
    case Qt::Key_Right:
    {
        on_pb_right_clicked();
        break;
    }
    case Qt::Key_Down:
    {
        on_pb_down_clicked();
        break;
    }
    default:
        break;
    }
}

void MainWindow::Move()
{
    std::string key = "move";
    this->output_msg_->set_key(key);
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["speed"]["linearSpeed"] = current_speed_.linearSpeed;
    root["speed"]["angularSpeed"] = current_speed_.angularSpeed;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::set_ip_addr(const QString &control, const QString &message)
{
    comm_.SetIPAddr(control.toStdString(), message.toStdString());
}

void MainWindow::set_scan_map_name(const QString &msg, int type)
{
    output_msg_->Clear();
    output_msg_->set_key("start_scan_map");
    output_msg_->set_uuid("1");

    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    Json::Value root;
    root["map_name"] = msg.toStdString();
    root["type"] = type;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
    current_map_name_ = msg.toStdString();
}

void MainWindow::display_msg(const QString &msg)
{
    if(NULL != ui_le_status_bar_)
    {
        ui_le_status_bar_->setText(msg);
    }
}

//void MainWindow::show_map_message(const QModelIndex &index, bool check)
//{
//    if(check)
//    {
//        string str = index.data().toString().toStdString();
//        if(map_list_msg_.end() != map_list_msg_.find(str))
//        {
//            QTextEdit* ui_map_msg = this->findChild<QTextEdit*>("ui_map_msg");
//            if(NULL == ui_map_msg)
//            {
//                ui_map_msg = new QTextEdit(this);
//                ui_map_msg->setObjectName("ui_map_msg");
//                QPoint pos = QCursor::pos();
//                pos = mapFromGlobal(pos);
//                ui_map_msg->setGeometry(pos.x(), pos.y(), 100, 150);
//            }

//            ui_map_msg->setText(QString::fromStdString(map_list_msg_[str]));
//            ui_map_msg->show();
//        }
//    }
//    else
//    {
//        QTextEdit* ui_map_msg = this->findChild<QTextEdit*>("ui_map_msg");
//        if(NULL != ui_map_msg)
//        {
//            ui_map_msg->deleteLater();
//        }
//    }
//}

void MainWindow::show_map_task_list(const QAction *action)
{
    QMenu* map_list = this->findChild<QMenu*>("map_list");
    if(NULL == map_list)
    {
        return;
    }
    map_list->show();
    current_map_name_ = action->text().toStdString();
    QMenu* map_task = new QMenu(map_list);
    map_task->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 100);

    QAction* map_png = map_task->addAction("map_png");
    QAction* delete_map = map_task->addAction("delete_map");
    QAction* load_map = map_task->addAction("load_map");
    //    QAction* initialize_customized = map_task->addAction("initialize_customized");
    QAction* download = map_task->addAction("download");
    QAction* upload = map_task->addAction("upload");

    connect(map_png, &QAction::triggered, this, &MainWindow::map_png);
    connect(delete_map, &QAction::triggered, this, &MainWindow::delete_map);
    connect(load_map, &QAction::triggered, this, &MainWindow::load_map);
    //    connect(initialize_customized, &QAction::triggered, this, &MainWindow::show_initialize_box);
    connect(download, &QAction::triggered, this, &MainWindow::download);
    connect(upload, &QAction::triggered, this, &MainWindow::upload);

    map_task->show();
}

//void MainWindow::show_pos_message(const QModelIndex &index, bool check)
//{
//    if(check)
//    {
//        string str = index.data().toString().toStdString();
//        if(map_point_list_.end() != map_point_list_.find(str))
//        {
//            QTextEdit* ui_pos_msg = this->findChild<QTextEdit*>("ui_pos_msg");
//            if(NULL == ui_pos_msg)
//            {
//                ui_pos_msg = new QTextEdit(this);
//                ui_pos_msg->setObjectName("ui_pos_msg");
//                ui_pos_msg->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 150);
//            }

//            ui_pos_msg->setText(QString::fromStdString(map_point_list_[str]));
//            ui_pos_msg->show();
//        }
//    }
//    else
//    {
//        QTextEdit* ui_pos_msg = this->findChild<QTextEdit*>("ui_pos_msg");
//        if(NULL != ui_pos_msg)
//        {
//            ui_pos_msg->deleteLater();
//        }
//    }
//}

void MainWindow::show_pos_task_list()
{
    QMenu* pos_task = new QMenu(this);

    QTreeWidget* treewidget= ui->sa_nav->findChild<QTreeWidget*>("navigation_tree");
    if(NULL == treewidget)
    {
        return;
    }
    current_pos_name_ = treewidget->currentItem()->text(0).toStdString();

    pos_task->setObjectName("pos_task");
    pos_task->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 100);
    QAction* navigate_point = pos_task->addAction("navigate_point");
    QAction* delete_position = pos_task->addAction("delete_position");
    QAction* rename_position = pos_task->addAction("rename_position");

    connect(navigate_point, &QAction::triggered, this, &MainWindow::navigate_point);
    connect(delete_position, &QAction::triggered, this, &MainWindow::delete_position);
    connect(rename_position, &QAction::triggered, this, &MainWindow::show_rename_box);

    pos_task->show();
}

//void MainWindow::show_path_message(const QModelIndex &index, bool check)
//{
//    if(check)
//    {
//        string str = index.data().toString().toStdString();
//        if(map_path_list_.end() != map_path_list_.find(str))
//        {
//            QTextEdit* ui_path_msg = this->findChild<QTextEdit*>("ui_path_msg");
//            if(NULL == ui_path_msg)
//            {
//                ui_path_msg = new QTextEdit(this);
//                ui_path_msg->setObjectName("ui_path_msg");
//                ui_path_msg->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 150);
//            }

//            ui_path_msg->setText(QString::fromStdString(map_path_list_[str]));
//            ui_path_msg->show();
//        }
//    }
//    else
//    {
//        QTextEdit* ui_path_msg = this->findChild<QTextEdit*>("ui_path_msg");
//        if(NULL != ui_path_msg)
//        {
//            ui_path_msg->deleteLater();
//        }
//    }
//}

void MainWindow::show_path_task_list()
{
    QMenu* path_task = new QMenu(this);

    QTreeWidget* treewidget= ui->sa_nav->findChild<QTreeWidget*>("navigation_tree");
    if(NULL == treewidget)
    {
        return;
    }

    QTreeWidgetItem* curItem = treewidget->currentItem();
    if(NULL == curItem->parent())
    {
        return;
    }

    current_path_name_ = curItem->text(0).toStdString();
    current_path_list_name_ = curItem->parent()->text(0).toStdString();

    path_task->setObjectName("path_task");
    path_task->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 100);

    QAction* navigate_path = path_task->addAction("navigate_path");
    QAction* navigation_path_detail = path_task->addAction("navigation_path_detail");


    connect(navigate_path, &QAction::triggered, this, &MainWindow::navigate_path);
    connect(navigation_path_detail, &QAction::triggered, this, &MainWindow::navigation_path_detail);
    path_task->show();
}

//void MainWindow::show_bag_message(const QModelIndex &index, bool check)
//{
//    if(check)
//    {
//        string str = index.data().toString().toStdString();
//        if(map_bag_list_.end() != map_bag_list_.find(str))
//        {
//            QTextEdit* ui_bag_msg = this->findChild<QTextEdit*>("ui_bag_msg");
//            if(NULL == ui_bag_msg)
//            {
//                ui_bag_msg = new QTextEdit(this);
//                ui_bag_msg->setObjectName("ui_bag_msg");
//                ui_bag_msg->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 100);
//            }

//            ui_bag_msg->setText(QString::fromStdString(map_bag_list_[str]));
//            ui_bag_msg->show();
//        }
//    }
//    else
//    {
//        QTextEdit* ui_bag_msg = this->findChild<QTextEdit*>("ui_bag_msg");
//        if(NULL != ui_bag_msg)
//        {
//            ui_bag_msg->deleteLater();
//        }
//    }
//}

void MainWindow::show_bag_task_list(const QModelIndex &index)
{
    QMenu* ui_bag_task = new QMenu(this);
    curren_bag_name_ = index.data().toString().toStdString();
    ui_bag_task->setObjectName("ui_bag_task");
    ui_bag_task->setGeometry(QCursor::pos().x(), QCursor::pos().y(), 100, 100);
    QAction* get_bag_file = ui_bag_task->addAction("get_bag_file");
    QAction* delete_bag_file = ui_bag_task->addAction("delete_bag_file");

    connect(get_bag_file, &QAction::triggered, this, &MainWindow::get_bag_file);
    connect(delete_bag_file, &QAction::triggered, this, &MainWindow::delete_bag_file);

    ui_bag_task->show();
}

void MainWindow::show_initialize_box()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    DataInitDialog* ui_initialize = new DataInitDialog("0",QString::number(init_pos.x), QString::number(init_pos.y),this);
    connect(ui_initialize, &DataInitDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_initialize, &DataInitDialog::initialize, this, &MainWindow::initialize_customized);
    ui_initialize->exec();
}

void MainWindow::show_rename_box()
{
    DataInitDialog* ui_rename = new DataInitDialog(QString::fromStdString(current_map_name_),QString::fromStdString(current_pos_name_),"",this);
    ui_rename->SetTag("map","old","new");
    connect(ui_rename, &DataInitDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_rename, &DataInitDialog::initialize, this, &MainWindow::rename_position);
    ui_rename->exec();
}

void MainWindow::show_record_box()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    DataInitDialog* ui_record = new DataInitDialog(this);
    ui_record->SetTag("name:", "type:","zero");
    connect(ui_record, &DataInitDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_record, &DataInitDialog::initialize, this, &MainWindow::record_position);
    ui_record->exec();
}


void MainWindow::show_nav_position_box()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    DataInitDialog* ui_nav_position = new DataInitDialog("0",QString::number(init_pos.x), QString::number(init_pos.y),this);

    connect(ui_nav_position, &DataInitDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_nav_position, &DataInitDialog::initialize, this, &MainWindow::navigate_position);
    ui_nav_position->exec();
}

void MainWindow::show_update_obstacles_box()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }

    AddObstaclesDialog* ui_update = new AddObstaclesDialog(this);
    connect(ui_update,&AddObstaclesDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_update,&AddObstaclesDialog::set_shape, this, &MainWindow::set_shape);
    connect(ui_update,&AddObstaclesDialog::save_obstacles, this, &MainWindow::update_virtual_obstacles);
    connect(ui_update,&AddObstaclesDialog::save_shape, this, &MainWindow::save_shape);
    connect(ui_update,&AddObstaclesDialog::redraw, this, &MainWindow::redraw);
    connect(ui_update,&AddObstaclesDialog::set_status,this, &MainWindow::is_update);
    connect(this,&MainWindow::current_pos, ui_update, &AddObstaclesDialog::show_point);

    shape_ = "circles";
    is_update_ = true;
    set_obstacles_points.clear();
    set_obstacles_.clear();
    ui_update->show();
}

void MainWindow::show_set_nav_path_box()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    set_nav_path_.clear();
    DataInitDialog* ui_nav_path = new DataInitDialog(this);
    ui_nav_path->setObjectName("nav_path");
    connect(ui_nav_path, &DataInitDialog::display_msg, this, &MainWindow::display_msg);
    connect(ui_nav_path, &DataInitDialog::ok_click, this, &MainWindow::set_navigation_path);
    connect(ui_nav_path, &DataInitDialog::set_nav, this, &MainWindow::is_set_nav);
    is_set_path_ = true;

    ui_nav_path->show();
}

void MainWindow::delete_map()
{
    this->output_msg_->set_key("delete_map");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::load_map()
{
    output_msg_->Clear();
    this->output_msg_->set_key("load_map");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::initialize_customized(const QString& angle, const QString& x, const QString& y)
{
    std::string key = "initialize_customized";
    this->output_msg_->set_key(key);

    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["mapName"] = current_map_name_;
    root["point"]["angle"] = angle.toDouble();
    root["point"]["position"]["x"] = x.toDouble();
    root["point"]["position"]["y"] = y.toDouble();

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);

    ui->pb_nav_list->setVisible(true);
    ui->pb_charge->setVisible(true);
    ui->pb_pasue_nav->setVisible(true);
    ui->pb_cancel_nav->setVisible(true);

    ui->pb_up->setVisible(true);
    ui->pb_right->setVisible(true);
    ui->pb_down->setVisible(true);
    ui->pb_left->setVisible(true);

}

void MainWindow::download()
{
    SyncMapDialog* ui_sync_map = new SyncMapDialog("download",this);
    connect(ui_sync_map, SIGNAL(sync_map(const QString &,const QString &)), this, SLOT(sync_map(const QString &,const QString &)));
    connect(ui_sync_map, SIGNAL(display_msg(const QString &)), this, SLOT(display_msg(const QString &)));
    ui_sync_map->show();
}

void MainWindow::upload()
{
    SyncMapDialog* ui_sync_map = new SyncMapDialog("upload",this);
    connect(ui_sync_map, SIGNAL(sync_map(const QString &,const QString &)), this, SLOT(sync_map(const QString &,const QString &)));
    connect(ui_sync_map, SIGNAL(display_msg(const QString &)), this, SLOT(display_msg(const QString &)));
    ui_sync_map->show();
}

void MainWindow::save_shape()
{
    if(set_obstacles_points.size()>1)
    {
        std::string shape = shape_.toStdString();
        if("circles" == shape)
        {
            Json::Value circle;
            circle["center"]["x"] = set_obstacles_points[0].first;
            circle["center"]["y"] = set_obstacles_points[0].second;
            int x = abs(set_obstacles_points[0].first - set_obstacles_points[1].first);
            int y = abs(set_obstacles_points[0].second - set_obstacles_points[1].second);
            double radius = sqrt(x*x+y*y);
            circle["radius"] = radius;
            set_obstacles_["data"]["obstacles"]["circle"].append(circle);
        }
        else if ("lines" == shape || "rectangles" == shape)
        {
            Json::Value value;
            value["start"]["x"] = set_obstacles_points[0].first;
            value["start"]["y"] = set_obstacles_points[0].second;
            value["end"]["x"] = set_obstacles_points[1].first;
            value["end"]["y"] = set_obstacles_points[1].second;
            set_obstacles_["data"]["obstacles"][shape].append(value);
        }
        else if ("polylines" == shape || "polygons" == shape)
        {
            Json::Value object;
            for(unsigned int i = 0; i < set_obstacles_points.size(); i++)
            {
                Json::Value value;
                value["x"] = set_obstacles_points[i].first;
                value["y"] = set_obstacles_points[i].second;
                object.append(value);
            }
            set_obstacles_["data"]["obstacles"][shape].append(object);
        }
    }
    control_data_.virtual_obstacles = set_obstacles_.toStyledString();
    set_obstacles_points.clear();
}

void MainWindow::set_shape(const QString &obstacles)
{
    shape_ = obstacles;
}

void MainWindow::redraw()
{
    set_obstacles_points.clear();
}

void MainWindow::is_update(bool is_update)
{
    is_update_ = is_update;
}

void MainWindow::is_set_nav(bool statu)
{
    is_set_path_ = statu;
}

void MainWindow::map_png()
{
    output_msg_->Clear();
    this->output_msg_->set_key("map_png");

    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::nav_list(const QAction *test)
{
    if(NULL == test)
    {
        return;
    }
    output_msg_->Clear();
    QString type = test->text();
    std::string key = "position_list";
    Json::Value root;
    if("pos_0" == type)
    {
        key = "position_list";
        root["map_name"] = current_map_name_;
        root["type"] = 0;
    }
    else if("pos_1" == type)
    {
        key = "position_list";
        root["map_name"] = current_map_name_;
        root["type"] = 1;
    }
    else if("pos_2" == type)
    {
        key = "position_list";
        root["map_name"] = current_map_name_;
        root["type"] = 2;
    }
    else if("path" == type)
    {
        key = "navigation_path_list";
        root["map_name"] = current_map_name_;
    }
    else
    {
        return;
    }
    this->output_msg_->set_key(key);
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);

    //Todo
    string text_list = "{\"data\":[{\"map_name\":\"test\",\"path_list_name\":\"1\"},{\"map_name\":\"test\",\"path_list_name\":\"2\"}],\"successed\":true}";
    success_navigation_path_list(text_list);
}

void MainWindow::navigate_point()
{
    output_msg_->Clear();
    this->output_msg_->set_key("navigate_point");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["position_name"] = current_pos_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::navigate_position(const QString& angle, const QString& x, const QString& y)
{
    output_msg_->Clear();
    this->output_msg_->set_key("navigate_position");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["destination"]["angle"] = angle.toDouble();
    root["destination"]["position"]["x"] = x.toDouble();
    root["destination"]["position"]["y"] = y.toDouble();

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::record_position(const QString& t1, const QString& t2, const QString& t3)
{
    output_msg_->Clear();
    this->output_msg_->set_key("record_position");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["position_name"] = t1.toStdString();
    root["type"] = t2.toInt();

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::delete_position()
{
    output_msg_->Clear();
    this->output_msg_->set_key("delete_position");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["position_name"] = current_pos_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::rename_position(const QString& map_name, const QString& old_name, const QString &new_name)
{
    output_msg_->Clear();
    this->output_msg_->set_key("rename_position");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = map_name.toStdString();
    root["origin_name"] = old_name.toStdString();
    root["new_name"] = new_name.toStdString();

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::navigation_path_list_detail(const QTreeWidgetItem *index)
{
    //顶层节点才是list
    if(NULL != index->parent())
    {
        return;
    }
    current_path_list_name_ = index->text(0).toStdString();

    output_msg_->Clear();
    this->output_msg_->set_key("navigation_path_list_detail");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["path_list_name"] = current_path_list_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
    string text_path = "{\"data\":{\"map_name\":\"test\",\"path_list_name\":\"1\",\"path_list\":[{\"path_name\":\"path3\",\"points\":[{\"angle\":-4.7219500215147585,\"grid_x\":458,\"grid_y\":238},{\"name\":\"111\"}]},{\"path_name\":\"path2\",\"points\":[{\"angle\":-4.7219500215147585,\"grid_x\":458,\"grid_y\":238},{\"name\":\"111\"}]},{\"path_name\":\"path3\",\"points\":[{\"angle\":-4.7219500215147585,\"grid_x\":458,\"grid_y\":238},{\"name\":\"111\"}]}]},\"successed\":true}";
    success_navigation_path_list_detail(text_path);//Todo
}

void MainWindow::navigate_path()
{
    output_msg_->Clear();
    this->output_msg_->set_key("navigate_path");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["navigate_path_list_name"] = current_path_list_name_;
    root["navigate_path_name"] = current_path_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::navigation_path_detail()
{
    output_msg_->Clear();
    this->output_msg_->set_key("navigation_path_detail");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["path_list_name"] = current_path_list_name_;
    root["path_name"] = current_path_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::verification_point(const double angle, const int gridx, const int gridy)
{
    output_msg_->Clear();
    this->output_msg_->set_key("verification_point");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["angle"] = angle;
    root["grid_x"] = gridx;
    root["grid_y"] = gridy;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::set_navigation_path()
{
    output_msg_->Clear();
    this->output_msg_->set_key("set_navigation_path");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["path_name"] = "path123";
    for(unsigned int i = 0; i < set_nav_path_.size();i++)
    {
        Json::Value object;
        object["angle"] = 0;
        object["grid_x"] = set_nav_path_[i].first;
        object["grid_y"] = set_nav_path_[i].second;
        root["points"].append(object);
    }

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::show_version()
{
    std::string key = "version";
    this->output_msg_->set_key(key);
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::sync_map(const QString &conrtol_type_, const QString &data_type)
{
    output_msg_->Clear();
    this->output_msg_->set_key("sync_map");
    this->output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    Json::Value root;
    root["map_name"] = current_map_name_;
    root["control_type"] = conrtol_type_.toStdString();
    QStringList list_type = data_type.split(",");
    foreach (QString var, list_type)
    {
        root["data_type"].append(var.toStdString());
    }

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::get_bag_file()
{
    output_msg_->Clear();
    this->output_msg_->set_key("get_bag_file");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["file_name"] = curren_bag_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::delete_bag_file()
{
    output_msg_->Clear();
    this->output_msg_->set_key("delete_bag_file");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["file_name"] = curren_bag_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::success_start_scan_map()
{
    ui->pb_cancel->setVisible(true);
    ui->pb_save->setVisible(true);
    ui->le_scan_status->setVisible(true);
}

void MainWindow::success_cancel_scan_map()
{
    ui->pb_cancel->setVisible(false);
    ui->pb_save->setVisible(false);
    ui->le_scan_status->setText("status:no signal");
    ui->le_scan_status->setVisible(false);
}

void MainWindow::success_async_stop_scan_map()
{
    ui->pb_cancel->setVisible(false);
    ui->pb_save->setVisible(false);
    ui->le_scan_status->setText("status:no signal");
    ui->le_scan_status->setVisible(false);
}

void MainWindow::success_scan_status_push(const QString &status)
{
    ui->le_scan_status->setText(status);
}

void MainWindow::success_map_png(const string &binary)
{
    control_data_.Clear();
    control_data_.binary_data = binary;
}

void MainWindow::on_pb_setip_clicked()
{
    SetIPDialog* ui_set_ip_ = new SetIPDialog(this);
    connect(ui_set_ip_, SIGNAL(set_ip_addr(const QString&,const QString&)), this, SLOT(set_ip_addr(const QString&,const QString&)));
    connect(ui_set_ip_, SIGNAL(display_msg(const QString&)), this, SLOT(display_msg(const QString&)));
    ui_set_ip_->exec();
}

void MainWindow::on_pb_start_scan_map_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    SetMapDialog* ui_set_map_ = new SetMapDialog(this);
    connect(ui_set_map_, SIGNAL(set_scan_map_name(const QString&, int)), this, SLOT(set_scan_map_name(const QString&, int)));
    connect(ui_set_map_, SIGNAL(display_msg(const QString&)), this, SLOT(display_msg(const QString&)));
    ui_set_map_->exec();
}

void MainWindow::on_pb_get_map_list_clicked()
{
    string text_data = "{\"data\":[{\"createdAt\":\"2016-08-11 04:08:30\",\"dataFileName\":\"40dd8fcd-5e6d-4890-b620-88882d9d3977.data\",\"id\":0,\"mapInfo\":{\"gridHeight\":992,\"gridWidth\":992,\"originX\":-24.8,\"originY\":-24.8,\"resolution\":0.05000000074505806},\"name\":\"demo1\",\"status\":4},{\"createdAt\":\"2016-08-11 04:08:30\",\"dataFileName\":\"40dd8fcd-5e6d-4890-b620-88882d9d3977.data\",\"id\":0,\"mapInfo\":{\"gridHeight\":992,\"gridWidth\":992,\"originX\":-24.8,\"originY\":-24.8,\"resolution\":0.05000000074505806},\"name\":\"demo2\",\"status\":4},{\"createdAt\":\"2016-08-11 04:08:30\",\"dataFileName\":\"40dd8fcd-5e6d-4890-b620-88882d9d3977.data\",\"id\":0,\"mapInfo\":{\"gridHeight\":992,\"gridWidth\":992,\"originX\":-24.8,\"originY\":-24.8,\"resolution\":0.05000000074505806},\"name\":\"demo3\",\"status\":4}],\"successed\":true}";
    success_get_map_list(text_data);

    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }

    QMyListWidget* temp = this->findChild<QMyListWidget*>("lw_map_list");
    if(NULL != temp)
    {
        temp->deleteLater();
        return;
    }

    output_msg_->Clear();
    output_msg_->set_key("get_map_list");
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_cancel_clicked()
{
    output_msg_->Clear();
    output_msg_->set_key("cancel_scan_map");
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_save_clicked()
{
    output_msg_->Clear();
    output_msg_->set_key("async_stop_scan_map");
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_real_laser_phit_clicked()
{
    output_msg_->Clear();
    this->output_msg_->set_key("real_laser_phit");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_up_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    current_speed_.linearSpeed += 1;
    Move();
}

void MainWindow::on_pb_right_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    current_speed_.angularSpeed -= 1;
    Move();
}

void MainWindow::on_pb_down_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    current_speed_.linearSpeed -= 1;
    Move();
}

void MainWindow::on_pb_left_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    current_speed_.angularSpeed += 1;
    Move();
}

void MainWindow::on_pb_nav_list_clicked()
{
    QMenu* pos_menu = new QMenu(this);
    pos_menu->addAction("pos_0");
    pos_menu->addAction("pos_1");
    pos_menu->addAction("pos_2");
    pos_menu->addAction("path");
    connect(pos_menu, &QMenu::triggered, this, &MainWindow::nav_list);
    pos_menu->setGeometry(QCursor::pos().x(),QCursor::pos().y(),100,150);
    pos_menu->show();
}

void MainWindow::on_pb_charge_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }

    string key = "";
    if(ui->pb_charge->text() == "start_charge")
    {
        key = "start_charge";
    }
    else if(ui->pb_charge->text() == "stop_charge")
    {
        key = "stop_charge";
    }
    else
    {
        return;
    }
    output_msg_->Clear();
    output_msg_->set_key(key);
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_pasue_nav_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }

    string key = "";
    if(ui->pb_pasue_nav->text() == "pause_nav")
    {
        key = "pause_navigate";
    }
    else if(ui->pb_pasue_nav->text() == "resume_nav")
    {
        key = "resume_navigate";
    }
    else
    {
        return;
    }
    output_msg_->Clear();
    output_msg_->set_key(key);
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_cancel_nav_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }
    output_msg_->Clear();
    output_msg_->set_key("cancel_navigate");
    output_msg_->set_uuid("1");
    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_bag_list_clicked()
{
    if(!comm_.Connect())
    {
        display_msg("error:Remote connection is not established!");
        return;
    }

    QMyListWidget* temp = this->findChild<QMyListWidget*>("ui_bag_list");
    if(NULL != temp)
    {
        temp->deleteLater();
        return;
    }

    output_msg_->Clear();
    this->output_msg_->set_key("get_bag_file_list");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::on_pb_real_display_clicked()
{
    output_msg_->Clear();
    output_msg_->set_key("real_display");
    output_msg_->set_uuid("1");

    cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< endl;

    comm_.Send(output_msg_);
}

void MainWindow::virtual_obstacles()
{
    this->output_msg_->set_key("virtual_obstacles");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    Json::Value root;
    root["map_name"] = current_map_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(root, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;

    comm_.Send(output_msg_);
}

void MainWindow::update_virtual_obstacles()
{
    this->output_msg_->set_key("update_virtual_obstacles");
    this->output_msg_->set_uuid("1");
    std::cout << "key:"<<output_msg_->key()<<"\n\r"<<"uuid:"<<output_msg_->uuid()<< std::endl;

    set_obstacles_["data"]["ID"] = "";
    set_obstacles_["data"]["createDate"] = "";
    set_obstacles_["data"]["description"] = "";
    set_obstacles_["data"]["mapName"] = current_map_name_;

    Json::StreamWriterBuilder writerBuilder;
    std::unique_ptr<Json::StreamWriter> json_write(writerBuilder.newStreamWriter());
    std::ostringstream ss;
    json_write->write(set_obstacles_, &ss);
    std::string strContent = ss.str();
    this->output_msg_->set_textdata(strContent.c_str(),strContent.size());
    std::cout << strContent << std::endl;
    comm_.Send(output_msg_);

    set_obstacles_.clear();
}
