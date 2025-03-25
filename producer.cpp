#include <iostream>
#include "message.pb.h"
#include <fstream>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

using namespace std;
using namespace AmqpClient;

void publish_user(const User& user) {
    //connect to rabbitmq
    try{
        Channel::OpenOpts opts;
        opts.host = "localhost";
        opts.port = 5672;
        opts.vhost = "/";
        opts.auth = Channel::OpenOpts::BasicAuth("admin", "N@!t!k854305");

        auto channel = Channel::Open(opts);
    
        string exchange_name = "amq.direct";
        // string exchange_name = "amq.fanout";
        string routing_key = "user.signup";
        channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_DIRECT, false, true, false); //direct exchange
        
        // channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_FANOUT, false, true, false); //fanout exchange
        string serialized_user;
        user.SerializeToString(&serialized_user);

        while(true){
            // Serialize to string
            auto amqp_message = BasicMessage::Create(serialized_user);
            // channel->BasicPublish(exchange_name, routing_key, amqp_message); //routing key needed for direct exchange

            channel->BasicPublish(exchange_name,"", amqp_message);// Routing key is ignored in fanout exchanges
            cout << "Message published to RabbitMQ: " << amqp_message << endl;
            sleep(10);
        }
    }catch(const exception &ex) {
        cerr << "Exception in publish_user: " << ex.what() << endl;
    }
}

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Create user data
    User naitik;
    naitik.set_id(101);
    naitik.set_name("Naitik");
    naitik.set_email("naitik@example.com");

    publish_user(naitik);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}