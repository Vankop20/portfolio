#include "MyLib.hpp"

std::ostream& operator<<(std::ostream& out, const Massage& mas) {
  out << mas.GetNick() << ':' << mas.GetMassege();
  return out;
}

Massage::Massage() {
  type_ = 0;
  massage_ = "";
}

Massage::Massage(int type, int size_n, const std::string& nick,
                 const std::string massage) {
  type_ = type;
  massage_ += static_cast<char>(type);
  massage_ += static_cast<char>(size_n);
  massage_ += nick;
  massage_ += massage;
  massage_ += '\0';
  massage_.shrink_to_fit();
}

Massage::Massage(int type, int size_n, const std::string& nick,
                 const std::vector<unsigned char>& massage) {
  type_ = type;
  v_m_.reserve(massage.size() + nick.size() + 2);
  v_m_.push_back(static_cast<unsigned char>(type));
  v_m_.push_back(static_cast<unsigned char>(size_n));
  for (int i = 0; i < size_n; ++i) {
    v_m_.push_back(nick[i]);
  }
  for (int i = 0; i < massage.size(); ++i) {
    v_m_.push_back(massage[i]);
  }
  v_m_.shrink_to_fit();
}

Massage::Massage(const Massage& other) {
  type_ = other.type_;
  massage_ = other.massage_;
  massage_.shrink_to_fit();
  v_m_ = other.v_m_;
  v_m_.shrink_to_fit();
}

Massage::Massage(const std::string& massage) {
  type_ = massage[0];
  if (type_ == 2 || type_ == 3) {
    massage_ = massage;
    massage_.shrink_to_fit();
  } else {
    v_m_.clear();
    for (auto it = massage.begin(); it < massage.end(); ++it) {
      v_m_.push_back(*it);
    }
  }
}

Massage::Massage(const QString& massage) {
  std::string new_mas = massage.toStdString();
  massage_ = "";
  if (new_mas[0] != '/') {
    type_ = 2;
    massage_ += static_cast<char>(2);
    int i = 0;
    massage_.resize(new_mas.size() + 2, 0);
    while (new_mas[i] != ':') {
      massage_[i + 2] = new_mas[i];
      ++i;
    }
    massage_[1] = i;
    for (++i; i < new_mas.size(); ++i) {
      massage_[i + 1] = new_mas[i];
    }
    massage_[i + 1] = '\0';
    massage_.shrink_to_fit();
  } else {
    type_ = 3;
    massage_ += static_cast<char>(3);
    massage_.resize(new_mas.size() + 1, 0);
    massage_[1] = new_mas.size() - 1;
    for (int i = 1; i < new_mas.size(); ++i) {
      massage_[i + 1] = new_mas[i];
    }
  }
}

Massage::Massage(const std::vector<unsigned char>& massage) {
  type_ = massage[0];
  if (type_ != 2 && type_ != 3) {
    v_m_ = massage;
  } else {
    massage_.clear();
    for (auto it = massage.begin(); it < massage.end(); ++it) {
      massage_.push_back(*it);
    }
    massage_.shrink_to_fit();
  }
}

int Massage::GetType() const { return type_; }

std::string Massage::GetNick() const {
  std::string nick = "";
  for (int i = 0; i < massage_[1]; ++i) {
    nick.push_back(massage_[i + 2]);
  }
  return nick;
}

std::string Massage::GetCaller() {
  std::string nick = "";
  int size = static_cast<int>(v_m_[1]);
  for (int i = 0; i < size; ++i) {
    nick.push_back((char)v_m_[i + 2]);
  }
  return nick;
}

char* Massage::GetRefMassege() {
  char* res = new char[massage_.size()];
  for (int i = 0; i < massage_.size(); ++i) {
    res[i] = massage_[i];
  }
  return res;
}

unsigned char* Massage::GetRefVideo() { return &v_m_[0]; }

std::string Massage::GetMassege() const {
  std::string massage;
  massage = massage_.substr(2 + static_cast<int>(massage_[1]));
  return massage;
}

std::vector<unsigned char> Massage::GetVideo() {
  std::vector<unsigned char> video;
  auto i = v_m_.begin() + 1;
  i += (v_m_[1] + 1);
  for (; i < v_m_.end(); ++i) {
    video.push_back(*i);
  }
  return video;
}

int Massage::Size() const {
  if (type_ == 2 || type_ == 3) {
    return massage_.size();
  }
  return v_m_.size();
}

std::istream& operator>>(std::istream& in, Massage& mas) {
  std::string new_mas;
  in >> new_mas;
  mas.massage_ = "";
  if (new_mas[0] != '/') {
    mas.type_ = 2;
    mas.massage_ += static_cast<char>(2);
    int i = 0;
    mas.massage_.resize(new_mas.size() + 2, 0);
    while (new_mas[i] != ':') {
      mas.massage_[i + 2] = new_mas[i];
      ++i;
    }
    mas.massage_[1] = i;
    for (++i; i < new_mas.size(); ++i) {
      mas.massage_[i + 1] = new_mas[i];
    }
    mas.massage_[i + 1] = '\0';
    mas.massage_.shrink_to_fit();
  } else {
    mas.type_ = 3;
    mas.massage_ += static_cast<char>(3);
    mas.massage_.resize(new_mas.size() + 1, 0);
    mas.massage_[1] = new_mas.size() - 1;
    for (int i = 1; i < new_mas.size(); ++i) {
      mas.massage_[i + 1] = new_mas[i];
    }
  }
  return in;
}

Client::Client() {
  port_ = MyPort;
  socketfd_ = Socket();
  serv_addr_.sin_family = AF_INET;
  serv_addr_.sin_port = htons(port_);
  Inet_pton(AF_INET, "0.0.0.0", &serv_addr_.sin_addr.s_addr);
}

int Client::Start(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // Окно входа
  QWidget* loginWindow = new QWidget;
  QLineEdit* loginLineEdit = new QLineEdit;
  QPushButton* loginButton = new QPushButton("Войти");
  QHBoxLayout* loginLayout = new QHBoxLayout;
  loginLayout->addWidget(loginLineEdit);
  loginLayout->addWidget(loginButton);
  loginWindow->setLayout(loginLayout);

  // Обработчик нажатия кнопки "Войти"
  QObject::connect(loginButton, &QPushButton::clicked, [=]() {
    QString buffer0 = loginLineEdit->text();
    nickname = buffer0.toStdString();
    Connect(socketfd_, &serv_addr_);
    int size_n = nickname.size();
    Send(socketfd_, &size_n);
    Send(socketfd_, &(nickname[0]), size_n);
    Recv(socketfd_, &size_n);
    loginWindow->close();
    if (size_n == 1) {
      std::cout << "connecting successfull" << std::endl;
      // Окно с полем ввода сообщения и кнопкой
      QWidget* messageWindow = new QWidget;
      QLineEdit* messageLineEdit = new QLineEdit;
      QLineEdit* callLineEdit = new QLineEdit;
      QPushButton* messageButton = new QPushButton("Отправить");
      QPushButton* callButton = new QPushButton("Позвонить");
      QLabel* messageLabel = new QLabel("Введите сообщение:");
      QLabel* callLabel = new QLabel("Введите собеседника:");
      QTextEdit* outputEdit = new QTextEdit;
      QVBoxLayout* messageLayout = new QVBoxLayout;
      QHBoxLayout* messageButtonLayout = new QHBoxLayout;
      outputEdit->setReadOnly(true);
      messageLayout->addWidget(outputEdit);
      messageLayout->addWidget(messageLabel);
      messageButtonLayout->addStretch();
      messageButtonLayout->addWidget(messageButton);
      messageLayout->addWidget(messageLineEdit);
      messageLayout->addWidget(callLabel);
      messageLayout->addWidget(callLineEdit);
      messageButtonLayout->addWidget(callButton);
      messageButtonLayout->addStretch();
      messageLayout->addLayout(messageButtonLayout);
      messageWindow->setLayout(messageLayout);

      pthread_t pid_recv;
      Recive_argv r_argv;
      r_argv.socket = socketfd_;
      r_argv.outputEdit = outputEdit;
      pthread_create(&pid_recv, NULL, Receiving, (void*)(&r_argv));

      // Обработчик нажатия кнопки "Отправить"
      QObject::connect(messageButton, &QPushButton::clicked, [=]() {
        QString buffer = messageLineEdit->text();
        QString buffer2 = callLineEdit->text();

        QString send_mas = buffer2;
        send_mas += ':';
        send_mas += buffer;

        Massage mas(send_mas);
        int mas_size = mas.Size();
        Send(socketfd_, &mas_size);
        Send(socketfd_, mas.GetRefMassege(), mas.Size());

        QString my_mas = QString(&nickname[0]);
        my_mas += ':';
        my_mas += buffer;
        outputEdit->append(my_mas);
      });

      QObject::connect(callButton, &QPushButton::clicked, [=]() {
        QString buffer0 = callLineEdit->text();
        QString call = "/";
        call += buffer0;

        Massage mas(call);
        int mas_size = mas.Size();
        Send(socketfd_, &mas_size);
        Send(socketfd_, mas.GetRefMassege(), mas.Size());

        // настройка видео
        cv::VideoCapture cap(0);
        if (!cap.isOpened()) {
          std::cerr << "Ошибка: не удалось открыть камеру" << std::endl;
        }
        int img_quality = 20;

        namedWindow("caller", cv::WINDOW_NORMAL);

        // настройка аудио
        int err;

        // Открытие устройства для записи
        snd_pcm_t* capture_handle;
        if ((err = snd_pcm_open(&capture_handle, "default",
                                SND_PCM_STREAM_CAPTURE, 0)) < 0) {
          std::cout << "error open audio device" << std::endl;
        }

        snd_pcm_hw_params_t* hw_params;
        snd_pcm_hw_params_alloca(&hw_params);
        snd_pcm_hw_params_any(capture_handle, hw_params);
        snd_pcm_hw_params_set_access(capture_handle, hw_params,
                                     SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(capture_handle, hw_params,
                                     SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2);
        snd_pcm_hw_params_set_rate(capture_handle, hw_params, 8000, 0);
        snd_pcm_hw_params_set_period_size(capture_handle, hw_params, 1024 / 2,
                                          0);
        snd_pcm_hw_params(capture_handle, hw_params);

        // Буфер для записи и отправки звука
        std::vector<unsigned char> buffer;
        buffer.resize(1024 * 2, 0);

        // отправка видео и аудио
        // отправка видео и аудио
        while (true) {
          // оправка видео
          cv::Mat frame;
          cap.read(frame);  // Захватываем текущий кадр
          cv::imshow("caller", frame);
          if (cv::waitKey(10) == 27) {
            break;
          }
          std::vector<uchar> encoded_frame;
          encoded_frame.reserve(20000);
          std::vector<int> compression_params;
          compression_params.push_back(
              cv::IMWRITE_JPEG_QUALITY);  // Выбираем тип компрессии
          compression_params.push_back(img_quality);
          imencode(".jpg", frame, encoded_frame,
                   compression_params);  // Конвертируем кадр в JPEG-формат
          int frame_size =
              encoded_frame.size();  // Определяем размер текущего кадра
          std::string nick = mas.GetNick();
          Massage new_mas(4, nick.size(), nick, encoded_frame);
          int mas_size = new_mas.Size();
          Send(socketfd_, &mas_size);  // Отправляем размер текущего кадра
          int bytes = Send(socketfd_, new_mas.GetRefVideo(),
                           mas_size);  // Отправляем текущий кадр
          // оправка аудио
          int read_count = snd_pcm_readi(capture_handle, &buffer[0], 1024 / 2);
          if (read_count == -EPIPE) {
            // Overrun occurred
            snd_pcm_prepare(capture_handle);
          } else if (read_count < 0) {
            // Error occurred
            std::cout << "Error while reading audio: " << snd_strerror(read_count) << '\n';
          } else if (read_count != 1024 / 2) {
            // Short read occurred
            std::cout << "Short read, read " << read_count << " frames instead of " << 1024 / 2 << '\n';
          }
          Massage au_mas(5, nick.size(), nick, buffer);
          mas_size = au_mas.Size();
          Send(socketfd_, &mas_size);  // Отправляем размер аудио
          bytes = Send(socketfd_, au_mas.GetRefVideo(),
                       mas_size);  // Отправляем текущее аудио
        }
        cap.release();
        cv::destroyAllWindows();
        snd_pcm_close(capture_handle);
      });

      messageWindow->show();
    }
    std::cout << "connecting faild" << std::endl;
    return 0;
  });

  loginWindow->show();
  return app.exec();
}

Server::Server() { port_ = MyPort; }

Server::Server(uint16_t port) { port_ = port; }

void Server::Work() {
  while (true) {
    Listen(sockedfd_);
    int nw_cl = Accept(sockedfd_, NULL);
    std::string nick_cl;
    int size;
    Recv(nw_cl, &size);
    nick_cl.resize(size, 0);
    Recv(nw_cl, &(nick_cl[0]), size);
    pthread_mutex_lock(&total_mutex);
    int res = 0;
    if (clients_nickname_.find(nick_cl) != clients_nickname_.end()) {
      std::cout << "new client denied access" << std::endl;
      Send(nw_cl, &res);
      Close(nw_cl);
    } else {
      res = 1;
      clients_nickname_.emplace(nick_cl, nw_cl);
      clients_socketfd_.push_back(nw_cl);
      std::cout << "new client: " << nick_cl
                << " num client: " << clients_socketfd_.size() << std::endl;
      argv_s* my_data = new argv_s;
      my_data->nick_cl = nick_cl;
      Send(nw_cl, &res);
      my_data->socket = nw_cl;
      my_data->serv = this;
      pthread_create(clients_, NULL, chating, (void*)my_data);
    }
    pthread_mutex_unlock(&total_mutex);
  }
}

void Server::Start() {
  std::cout << "Creating a socket..." << std::endl;
  sockedfd_ = Socket();
  std::cout << "Socket creation successfully" << std::endl;
  std::cout << "Start binding" << std::endl;
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port_);
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  std::cout << "IP:" << InetIPv4(INADDR_ANY) << std::endl;
  Bind(sockedfd_, &addr_);
  std::cout << "bind successfull" << std::endl;
  std::cout << "Start listening" << std::endl;
  Work();
}

auto Server::Search(const std::string& nickname) {
  pthread_mutex_lock(&total_mutex);
  auto res = clients_nickname_.find(nickname);
  pthread_mutex_unlock(&total_mutex);
  return res;
}

auto Server::NoResul() {
  pthread_mutex_lock(&total_mutex);
  auto res = clients_nickname_.end();
  pthread_mutex_unlock(&total_mutex);
  return res;
}

void* chating(void* argv) {
  argv_s* my_data = reinterpret_cast<argv_s*>(argv);
  while (true) {
    int size;
    std::string buf;
    Recv(my_data->socket, &size);
    buf.resize(size, 0);
    Recv(my_data->socket, &buf[0], size);
    Massage mas(buf);
    if (buf[0] == 2 || buf[0] == 3) {
      Massage new_mas(mas.GetType(), my_data->nick_cl.size(), my_data->nick_cl,
                      mas.GetMassege());
      auto Fd = (my_data->serv)->Search(mas.GetNick());
      int new_size = new_mas.Size();
      Send(Fd->second, &new_size);
      Send(Fd->second, new_mas.GetRefMassege(), new_size);
    } else {
      Massage new_mas(mas.GetType(), my_data->nick_cl.size(), my_data->nick_cl,
                      mas.GetVideo());
      auto Fd = (my_data->serv)->Search(mas.GetCaller());
      int new_size = new_mas.Size();
      Send(Fd->second, &new_size);
      Send(Fd->second, new_mas.GetRefVideo(), new_size);
    }
  }
}

void* Receiving(void* argv) {
  int socket = ((reinterpret_cast<Recive_argv*>(argv))->socket);
  QTextEdit* outputEdit = ((reinterpret_cast<Recive_argv*>(argv))->outputEdit);
  while (true) {
    int size = 0;
    Recv(socket, &size);
    if (size > 3) {
      std::vector<unsigned char> buf;
      buf.resize(size, 0);
      int len = Recv(socket, &buf[0], size);
      snd_pcm_t* playback_handle;
      snd_pcm_hw_params_t* hw_params;
      if (buf[0] == 2) {
        QString buffer;
        std::string new_buf;
        new_buf.resize(size, 0);
        memcpy(&new_buf[0], reinterpret_cast<char*>(&buf[0]), size);
        Massage mas(new_buf);
        buffer = QString(&(mas.GetNick())[0]) + ':' +
                 QString(&(mas.GetMassege())[0]);
        outputEdit->append(buffer);
      } else if (buf[0] == 3) {
        // создание окна для видео
        Massage mas(buf);
        std::cout << mas.GetNick() << " starts the call" << std::endl;
        namedWindow(mas.GetNick(), cv::WINDOW_NORMAL);
        cv::resizeWindow(mas.GetNick(), 640, 480);

        // открытие потоков для аудио
        //  Открытие устройства для воспроизведения
        int err;
        if ((err = snd_pcm_open(&playback_handle, "default",
                                SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
          std::cout << "open audio" << std::endl;
        }
        // настройка устройства для воспроизведения
        snd_pcm_hw_params_alloca(&hw_params);
        snd_pcm_hw_params_any(playback_handle, hw_params);
        snd_pcm_hw_params_set_access(playback_handle, hw_params,
                                     SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(playback_handle, hw_params,
                                     SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2);
        snd_pcm_hw_params_set_rate(playback_handle, hw_params, 8000, 0);
        snd_pcm_hw_params_set_period_size(playback_handle, hw_params, 1024 / 2,
                                          0);
        snd_pcm_hw_params(playback_handle, hw_params);
      } else if (buf[0] == 4 && buf.size() > 2000) {
        Massage mas(buf);
        cv::Mat frame;
        std::vector<uchar> video = mas.GetVideo();
        // Декодирование видеопотока и отображение на экране
        frame = cv::imdecode(cv::Mat(video), cv::IMREAD_COLOR);
        if (frame.empty()) {
          std::cout << "Failed to decode video stream" << std::endl;
          break;
        }
        cv::imshow(mas.GetCaller(), frame);
        if (cv::waitKey(10) == 27) {
          break;
        }
      } else if (buf[0] == 5) {
        Massage mas(buf);
        snd_pcm_hw_params(playback_handle, hw_params);
        std::vector<unsigned char> buffer = mas.GetVideo();
        int read_count = snd_pcm_writei(playback_handle, &buffer[0], 1024 / 2);
        if (read_count == -EPIPE) {
          // Overrun occurred
          snd_pcm_prepare(playback_handle);
        } else if (read_count < 0) {
          // Error occurred
          std::cout << "Error while reading audio: " << snd_strerror(read_count) << '\n';
        } else if (read_count != 1024 / 2) {
          // Short read occurred
          std::cout << "Short read, read " << read_count << " frames instead of " << 1024 << '\n';
        }
      }
    }
  }
  return 0;
}
