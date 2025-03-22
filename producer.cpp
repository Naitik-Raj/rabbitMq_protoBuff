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
        opts.auth = Channel::OpenOpts::BasicAuth("admin", "your_password");

        auto channel = Channel::Open(opts);
    
        // string exchange_name = "amq.direct";
        string exchange_name = "amq.fanout";
        string routing_key = "user.signup";
        // channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_DIRECT, false, true, false); //direct exchange
        
        channel->DeclareExchange(exchange_name, Channel::EXCHANGE_TYPE_FANOUT, false, true, false); //fanout exchange
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <user_data_file>" << endl;
        return 1;
    }

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Read user data from file
    User naitik;
    fstream input(argv[1], ios::in | ios::binary);
    if (!naitik.ParseFromIstream(&input)) {
        cerr << "Failed to parse user data." << endl;
        return 1;
    }

    publish_user(naitik);
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}