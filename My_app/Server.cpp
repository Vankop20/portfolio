#include "MyLib.hpp"

int main(){
    Server my_server;
    my_server.Start();
}

// команда компиляции g++ MyNetWork.cpp MyLib.cpp -o client Client.cpp `pkg-config --cflags --libs opencv4 Qt5Widgets` -lasound
