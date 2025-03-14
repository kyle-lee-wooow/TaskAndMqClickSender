#pragma once
#include <iostream>
#include <mqtt/async_client.h>
#include <functional>  // 引入 std::function

const std::string TOPIC = "wow/key_sync";                    // MQTT 主题

namespace RemoteCtrl{
    
    class Callback : public virtual mqtt::callback
    {
    private:
        mqtt::async_client& client;
        mqtt::connect_options connOpts;
        std::function<void(const std::string&)> messageHandler;  // 自定义回调函数
        
    public:
        Callback(mqtt::async_client& cli, mqtt::connect_options opts)
            : client(cli), connOpts(std::move(opts)) {

        }

        void setCallFunc(std::function<void(const std::string&)> func) {
            messageHandler = func;
        }

        // 断线回调：触发自动重连
        void connection_lost(const std::string& cause) override {
            std::cerr << "\n 连接已断开，原因: " << cause << std::endl;
            std::cout << "尝试重新连接..." << std::endl;

            // 自动重连机制
            while (true) {
                try {
                    client.reconnect()->wait();
                    std::cout << "重连成功！" << std::endl;

                    // 恢复订阅
                    client.subscribe(TOPIC, 1)->wait();
                    std::cout << "已重新订阅主题：" << TOPIC << std::endl;
                    break;
                }
                catch (const mqtt::exception& exc) {
                    std::cerr << "重连失败: " << exc.what() << "，3秒后重试..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }
            }
        }

        // 消息回调
        void message_arrived(mqtt::const_message_ptr msg) override {
            // 执行传入的处理函数
            if (messageHandler) {
                messageHandler(msg->to_string());
            }
        }
    };

    class Conntection
    {

    public:
        std::string SERVER_ADDRESS ; // MQTT 服务器地址
        std::string MQ_NAME;                    // MQTT 主题
        std::string MQ_PWD;

        // 提供全局访问点
        static Conntection& getInstance() {
            static Conntection instance;  // C++11 局部静态变量，线程安全
            
            return instance;
        }

         void setMQTTParam(std::string& add, std::string& na, std::string& pwd) {
            // 提供全局访问点
            SERVER_ADDRESS = add;
            MQ_NAME = na;
            MQ_PWD = pwd;
        }
         // 发布消息
         void publishMessage(const std::string& message) { 

             try {
                 mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, message);
                 pubmsg->set_qos(1);
                 client->publish(pubmsg);
                 std::cout << "[MQTT] 发送消息: " << message << std::endl;
             }
             catch (const mqtt::exception& exc) {
                 std::cerr << "[MQTT] 发布失败: " << exc.what() << std::endl;
             }
         }

        std::string GenerateTimeBasedID() {
            // 获取当前时间的时间点
            auto now = std::chrono::system_clock::now();

            // 获取自 epoch（1970-01-01 00:00:00 UTC）以来的毫秒数
            auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

            std::cout << "Current timestamp (in milliseconds): " << timestamp_ms << std::endl;
            return  std::to_string(timestamp_ms);
        }
        int loading(std::function<void(const std::string&)> func) {

            if (lisTaskRunning) { return 0; }; 
                        
            if (!client) {
                std::string clientID = GenerateTimeBasedID();
                client = std::make_unique<mqtt::async_client>(SERVER_ADDRESS, clientID);
 
            }
                        // 设置连接参数
                        auto connOpts = mqtt::connect_options_builder()
                            .user_name(MQ_NAME)
                            .password(MQ_PWD)
                            .clean_session()
                            .automatic_reconnect(std::chrono::seconds(3), std::chrono::seconds(30)) // 自动重连机制
                            .finalize();

                   /*     Callback callback(client, connOpts);
                        callback.setCallFunc(func);
                        client.set_callback(callback);*/

                        connOpts.set_keep_alive_interval(20); // 保持连接的心跳时间（20秒）
                        connOpts.set_clean_session(true);     // 保证每次连接时清除旧的会话数据

                        try {
                            std::cout << "正在连接至 MQTT 服务器..." << std::endl;
                            client->connect(connOpts)->wait();
                            std::cout << " 已成功连接！" << std::endl;

                          
 
                        }
                        catch (const mqtt::exception& exc) {
                            std::cerr << "错误: " << exc.what() << std::endl;
                            return 1;
                        }
         
         
            lisTaskRunning = true;
            // 等待条件变量的信号来阻止主线程退出
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock);

            lisTaskRunning = false;
            return 0;
        }

        // 添加信号触发来停止客户端
        void stopMqtt() {
            cv.notify_all();
        }
    private:
        bool lisTaskRunning = false;  // 任务是否正在执行
 
        Conntection() {};
        ~Conntection() {};
        std::condition_variable cv;
        std::mutex mtx;
        std::unique_ptr<mqtt::async_client> client;
       

    };

   



}