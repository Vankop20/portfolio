#pragma once

#include <alsa/asoundlib.h>
#include <netinet/ip.h>

#include <QtWidgets>
#include <list>
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>

#include "MyNetWork.hpp"

#define MyPort 10091;

static pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

void* Receiving(void* argv);
void* chating(void* argv);

class Massage {
 private:
  std::string massage_;
  std::vector<unsigned char> v_m_;
  int type_;

 public:
  Massage();
  Massage(int type, int size_n, const std::string& nick,
          const std::string massage = "");
  Massage(int type, int size_n, const std::string& nick,
          const std::vector<unsigned char>& massage = {});
  Massage(const Massage& other);
  Massage(const std::string& massage);
  explicit Massage(const QString& massage);
  Massage(const std::vector<unsigned char>& massage);
  int GetType() const;
  std::string GetNick() const;
  std::string GetCaller();
  char* GetRefMassege();
  unsigned char* GetRefVideo();
  std::string GetMassege() const;
  std::vector<unsigned char> GetVideo();
  int Size() const;
  friend std::istream& operator>>(std::istream& in, Massage& mas);
  ~Massage() = default;
};

class Server {
 private:
  uint16_t port_;
  pthread_t clients_[100] = {};
  std::list<int> clients_socketfd_;
  int sockedfd_;
  sockaddr_in addr_;
  std::map<std::string, int> clients_nickname_;

 public:
  Server();
  Server(uint16_t port);
  void Work();
  void Start();
  auto Search(const std::string& nickname);
  auto NoResul();
};

class Client {
 private:
  int socketfd_;
  sockaddr_in serv_addr_;
  int port_;
  std::string nickname;

 public:
  Client();
  int Start(int argc, char* argv[]);
};

struct argv_s {
  int socket;
  std::string nick_cl;
  Server* serv;
};

struct Recive_argv {
  int socket;
  QTextEdit* outputEdit;
};

std::ostream& operator<<(std::ostream& out, const Massage& mas);

std::istream& operator>>(std::istream& in, Massage& mas);
