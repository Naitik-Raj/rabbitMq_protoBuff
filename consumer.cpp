#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <csignal>
#include "message.pb.h"

using namespace std;
using namespace AmqpClient;

void signalHandler(int signal) {
    // Handle signal for graceful shutdown
    if (signal == SIGINT) {
        cout << "Received SIGINT. Stopping consumer..." << endl;
        exit(0);
    }
}

int main() {
    try {
        signal(SIGINT, signalHandler);
        cout << "Attempting to connect to RabbitMQ..." << endl;

        AmqpClient::Channel::OpenOpts opts;
        opts.host = "localhost";
        opts.port = 5672;
        opts.vhost = "/";
        opts.auth = AmqpClient::Channel::OpenOpts::BasicAuth("admin", "N@!t!k854305");

        auto channel = AmqpClient::Channel::Open(opts);
        cout << "Connected to RabbitMQ!" << channel << endl;

        string exchange_name = "amq.direct";
        string routing_key = "user.signup";
        string queue_name = "user_queue";
        channel->DeclareQueue(queue_name, false, true, false, false);
        channel->BindQueue(queue_name, exchange_name, routing_key);

        string consumer_tag = channel->BasicConsume(queue_name, "", true, false, false);
        cout << "Consumer tag: " << consumer_tag << endl;

        while (true) {
            // AmqpClient::Envelope::ptr_t envelope;
            AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
            AmqpClient::BasicMessage::ptr_t msg_in = envelope->Message();
            string body = msg_in->Body();

            User user;
            if (user.ParseFromString(body)) {
                cout << "Received User: " << endl;
                cout << "ID: " << user.id() << endl;
                cout << "Name: " << user.name() << endl;
                cout << "Email: " << user.email() << endl;
            } else {
                cerr << "Failed to parse received message!" << endl;
            }
            
            channel->BasicAck(envelope);
        }

    } catch (const exception &ex) {
        cerr << "Exception in main: " << ex.what() << endl;
    }
    return 0;
}