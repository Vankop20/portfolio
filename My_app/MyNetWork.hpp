#pragma once

#include <arpa/inet.h>
#include <sys/types.h>
#include <string>
#include <iostream>

std::string InetIPv4(unsigned long);

std::string InetIPv4(unsigned long);

int Socket();

void Bind(int, sockaddr_in*);

void Listen(int);

int Accept(int, sockaddr_in*);

int Close(int);

int Connect(int, struct sockaddr_in*);

int Send(int sockfd, char* buf, int size = 2048);

int Send(int sockfd, int* buf, int size = sizeof(int));

int Send(int sockfd, unsigned char* buf, int size = 2048);

int Recv(int sockfd, int* buf, int size = sizeof(int));

int Recv(int sockfd, char* buf, int size = 2048);

int Recv(int sockfd, unsigned char* buf, int size = 2048);

void Inet_pton(int, const char*, void*);
