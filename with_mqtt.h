#pragma once
#include <iostream>
#include <mqtt/async_client.h>
#include <functional>  // ���� std::function

const std::string TOPIC = "wow/key_sync";                    // MQTT ����

namespace RemoteCtrl{
    
    class Callback : public virtual mqtt::callback
    {
    private:
        mqtt::async_client& client;
        mqtt::connect_options connOpts;
        std::function<void(const std::string&)> messageHandler;  // �Զ���ص�����
        
    public:
        Callback(mqtt::async_client& cli, mqtt::connect_options opts)
            : client(cli), connOpts(std::move(opts)) {

        }

        void setCallFunc(std::function<void(const std::string&)> func) {
            messageHandler = func;
        }

        // ���߻ص��������Զ�����
        void connection_lost(const std::string& cause) override {
            std::cerr << "\n �����ѶϿ���ԭ��: " << cause << std::endl;
            std::cout << "������������..." << std::endl;

            // �Զ���������
            while (true) {
                try {
                    client.reconnect()->wait();
                    std::cout << "�����ɹ���" << std::endl;

                    // �ָ�����
                    client.subscribe(TOPIC, 1)->wait();
                    std::cout << "�����¶������⣺" << TOPIC << std::endl;
                    break;
                }
                catch (const mqtt::exception& exc) {
                    std::cerr << "����ʧ��: " << exc.what() << "��3�������..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }
            }
        }

        // ��Ϣ�ص�
        void message_arrived(mqtt::const_message_ptr msg) override {
            // ִ�д���Ĵ�����
            if (messageHandler) {
                messageHandler(msg->to_string());
            }
        }
    };

    class Conntection
    {

    public:
        std::string SERVER_ADDRESS ; // MQTT ��������ַ
        std::string MQ_NAME;                    // MQTT ����
        std::string MQ_PWD;

        // �ṩȫ�ַ��ʵ�
        static Conntection& getInstance() {
            static Conntection instance;  // C++11 �ֲ���̬�������̰߳�ȫ
            
            return instance;
        }

         void setMQTTParam(std::string& add, std::string& na, std::string& pwd) {
            // �ṩȫ�ַ��ʵ�
            SERVER_ADDRESS = add;
            MQ_NAME = na;
            MQ_PWD = pwd;
        }
         // ������Ϣ
         void publishMessage(const std::string& message) { 

             try {
                 mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, message);
                 pubmsg->set_qos(1);
                 client->publish(pubmsg);
                 std::cout << "[MQTT] ������Ϣ: " << message << std::endl;
             }
             catch (const mqtt::exception& exc) {
                 std::cerr << "[MQTT] ����ʧ��: " << exc.what() << std::endl;
             }
         }

        std::string GenerateTimeBasedID() {
            // ��ȡ��ǰʱ���ʱ���
            auto now = std::chrono::system_clock::now();

            // ��ȡ�� epoch��1970-01-01 00:00:00 UTC�������ĺ�����
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
                        // �������Ӳ���
                        auto connOpts = mqtt::connect_options_builder()
                            .user_name(MQ_NAME)
                            .password(MQ_PWD)
                            .clean_session()
                            .automatic_reconnect(std::chrono::seconds(3), std::chrono::seconds(30)) // �Զ���������
                            .finalize();

                   /*     Callback callback(client, connOpts);
                        callback.setCallFunc(func);
                        client.set_callback(callback);*/

                        connOpts.set_keep_alive_interval(20); // �������ӵ�����ʱ�䣨20�룩
                        connOpts.set_clean_session(true);     // ��֤ÿ������ʱ����ɵĻỰ����

                        try {
                            std::cout << "���������� MQTT ������..." << std::endl;
                            client->connect(connOpts)->wait();
                            std::cout << " �ѳɹ����ӣ�" << std::endl;

                          
 
                        }
                        catch (const mqtt::exception& exc) {
                            std::cerr << "����: " << exc.what() << std::endl;
                            return 1;
                        }
         
         
            lisTaskRunning = true;
            // �ȴ������������ź�����ֹ���߳��˳�
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock);

            lisTaskRunning = false;
            return 0;
        }

        // ����źŴ�����ֹͣ�ͻ���
        void stopMqtt() {
            cv.notify_all();
        }
    private:
        bool lisTaskRunning = false;  // �����Ƿ�����ִ��
 
        Conntection() {};
        ~Conntection() {};
        std::condition_variable cv;
        std::mutex mtx;
        std::unique_ptr<mqtt::async_client> client;
       

    };

   



}