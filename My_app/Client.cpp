#include "MyLib.hpp"

int main(int argc, char *argv[]){
    Client my_client;
    my_client.Start(argc, argv);
}

// команда компиляции g++ MyNetWork.cpp MyLib.cpp -o server Server.cpp `pkg-config --cflags --libs opencv4 Qt5Widgets` -lasound
