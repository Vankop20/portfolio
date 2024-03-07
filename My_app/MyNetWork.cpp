#include "MyNetWork.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

std::string InetIPv4(unsigned long ip) {  // нужно написать нормально
  std::string res;
  int tmp = ip / (1 << 24);
  ip = ip % (1 << 24);
  if (tmp >= 100) {
    res.push_back(('0' + (tmp / 100)));
    tmp %= 100;
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else if (tmp >= 10) {
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else {
    res.push_back(('0' + tmp));
  }
  res.push_back('.');
  tmp = ip / (1 << 16);
  ip = ip % (1 << 16);
  if (tmp >= 100) {
    res.push_back(('0' + (tmp / 100)));
    tmp %= 100;
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else if (tmp >= 10) {
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else {
    res.push_back(('0' + tmp));
  }
  res.push_back('.');
  tmp = ip / (1 << 8);
  ip = ip % (1 << 8);
  if (tmp >= 100) {
    res.push_back(('0' + (tmp / 100)));
    tmp %= 100;
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else if (tmp >= 10) {
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else {
    res.push_back(('0' + tmp));
  }
  res.push_back('.');
  tmp = ip;
  if (tmp >= 100) {
    res.push_back(('0' + (tmp / 100)));
    tmp %= 100;
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else if (tmp >= 10) {
    res.push_back(('0' + (tmp / 10)));
    tmp %= 10;
    res.push_back(('0' + tmp));
  } else {
    res.push_back(('0' + tmp));
  }
  return res;
}

int Socket() {
  try {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      throw 0;
    }
    return sock;
  } catch (...) {
    std::cout << "error create socket" << std::endl;
    throw 0;
  }
}

void Bind(int sockfd, sockaddr_in* addr) {
  try {
    if ((bind(sockfd, reinterpret_cast<sockaddr*>(addr), sizeof(*addr))) ==
        -1) {
      throw 1;
    }
  } catch (...) {
    std::cout << "error create bind" << std::endl;
    throw 1;
  }
}

void Listen(int sockfd) {
  try {
    if ((listen(sockfd, 5)) != 0) {
      throw 2;
    }
  } catch (...) {
    std::cout << "error listen" << std::endl;
    throw 2;
  }
}

int Accept(int sockfd, sockaddr_in* addr) {
  try {
    int new_socket;
    socklen_t len = static_cast<socklen_t>(sizeof(*addr));
    if ((new_socket =
             accept(sockfd, reinterpret_cast<sockaddr*>(addr), &len)) ==
        -1) {  // возможен баг из-за ссылки на локальную переменную
      throw 1;
    }
    return new_socket;
  } catch (...) {
    std::cout << "error create bind" << std::endl;
    throw 1;
  }
}

int Close(int sockfd) {
  try {
    if ((close(sockfd)) == -1) {
      throw 1;
    }
    return 0;
  } catch (...) {
    std::cout << "error close" << std::endl;
    throw 1;
  }
}

int Connect(int sockfd, struct sockaddr_in* serv_addr) {
  try {
    int len = static_cast<int>(sizeof(*serv_addr));
    if ((connect(sockfd, reinterpret_cast<sockaddr*>(serv_addr), len)) ==
        -1) {  // возможен баг из-за ссылки на локальную переменную
      throw 1;
    }
    return 0;
  } catch (...) {
    std::cout << "error create connect" << std::endl;
    throw 1;
  }
}

int Send(int sockfd, int* buf, int size) {
  try {
    int len;
    if ((len = send(sockfd, buf, size, 0)) == -1) {
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error send" << std::endl;
    throw 1;
  }
}

int Send(int sockfd, char* buf, int size) {
  try {
    int len;
    if ((len = send(sockfd, buf, size, 0)) == -1) {
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error send" << std::endl;
    throw 1;
  }
}

int Send(int sockfd, unsigned char* buf, int size) {
  try {
    int len;
    if ((len = send(sockfd, buf, size, 0)) == -1) {
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error send" << std::endl;
    throw 1;
  }
}

int Recv(int sockfd, int* buf, int size) {
  try {
    int len;
    if ((len = recv(sockfd, buf, size, 0)) == -1) {
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error recv" << std::endl;
    throw 1;
  }
}

int Recv(int sockfd, char* buf, int size) {
  try {
    int len;
    if ((len = recv(sockfd, buf, size, 0)) ==
        -1) {  // возможен баг из-за ссылки на локальную переменную
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error recv" << std::endl;
    throw 1;
  }
}

int Recv(int sockfd, unsigned char* buf, int size) {
  try {
    int len;

    if ((len = recv(sockfd, buf, size, 0)) ==
        -1) {  // возможен баг из-за ссылки на локальную переменную
      throw 1;
    }
    return len;
  } catch (...) {
    std::cout << "error recv" << std::endl;
    throw 1;
  }
}

void Inet_pton(int af, const char* src, void* dst) {
  try {
    int res = inet_pton(af, src, dst);
    if (res == 0 || res == -1) {
      throw 1;
    }
  } catch (...) {
    std::cout << "error pton" << std::endl;
    throw 1;
  }
}
