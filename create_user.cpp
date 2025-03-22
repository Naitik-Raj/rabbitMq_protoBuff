#include <iostream>
#include <fstream>
#include "message.pb.h"

using namespace std;

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    User naitik;
    naitik.set_id(101);
    naitik.set_name("Naitik");
    naitik.set_email("naitik@example.com");

    fstream output("user_data.bin", ios::out | ios::binary);
    if (!naitik.SerializeToOstream(&output)) {
        cerr << "Failed to write user data." << endl;
        return 1;
    }

    cout << "User data written to user_data.bin" << endl;
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
