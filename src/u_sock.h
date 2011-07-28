/*$Id: s_dc.cc,v 26.132 2009/11/24 04:26:37 al Exp $ -*- C++ -*-
 * Copyright (C) 2011 Felix Salfelder
 * Authors: Felix Salfelder, 
 *          Markus Meissner
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * TCP socket stream stuff.
 * don't know how to do this right.
 * 
 */
#ifndef XSOCKET_H
#define XSOCKET_H

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>

//#include "general.h"
//#include "xstring.h"


using namespace std;
enum SOCKET_TYPE {
  UNIX,
  TCP,
  UDP
};
#define MAX_LISTEN_QUEUE 256


//namespace TOOLS {
//namespace NET {

#define DEFINE_EXCEPTION(CLASS,PARENT) \
    class CLASS : public PARENT { \
          public: \
                        CLASS(const std::string& msg) : PARENT(msg) { } /*, #CLASS) { } */\
        };


DEFINE_EXCEPTION(SocketException, Exception);

/**
 * @brief the SocketStream handles the actual data
 *        writing and reading from the socket.
 */
class SocketStream : public ios {
  public:
    enum EOL {eol};
    int fd;

    SocketStream() :  ios() {}
    SocketStream(int fd) : 
      ios(),
      fd(fd) {}
    SocketStream(const SocketStream& obj) :
      ios(),
      fd(obj.fd) {}
    virtual ~SocketStream();

    const std::string get(int len);
    const std::string operator>>(int len);

    void flush();
    void send(const std::string& data);
    void send(const std::string& data, int len);
    SocketStream& operator<<(const std::string& data);
    SocketStream& operator<<(const char* data);
    SocketStream& operator<<(const int data);
    SocketStream& operator<<(const double data);
    SocketStream& operator<<(const EOL){
      flush();
      return *this;
    }
};
/**
 * @brief the Socket, is an abstract OO approach
 *        to handle (actually wrap the C functions) sockets
 */
class Socket {
  protected:
    int fd;
    uint16_t port;
    struct sockaddr_in addr;
    SOCKET_TYPE type; // ??
    SocketStream* stream;

  public:
    enum EOL {eol};
    Socket(): fd(0), port(0), stream(0){}

    Socket(SOCKET_TYPE type, short unsigned port);
    virtual ~Socket();
    template<class T>
      SocketStream& operator<<(const T& data);
};

template<>
SocketStream& Socket::operator<<(const Socket::EOL& ){
  assert(stream);
  return *stream << SocketStream::eol;
}

template<class T>
SocketStream& Socket::operator<<(const T& data){
  assert(stream);
  return *stream << data;
}
/**
 * @brief ServerSocket does the obvious, it sets up a server
 *        socket, which then can be used to listen to and recive connections
 */
class ServerSocket : public Socket {
  public:
    ServerSocket(SOCKET_TYPE type, short unsigned port);
    virtual ~ServerSocket();

    SocketStream* listen();
};
/**
 * @brief ClientSocket does the obvious, it can connect to any socket
 */
class ClientSocket : public Socket {
  public:
    ClientSocket(SOCKET_TYPE type, short unsigned port, const std::string& target);
    virtual ~ClientSocket();
};
/**
 * @brief StreamSelecter wants an arbitrary number of (Socket)Streams as
 *        input - it wraps the select() mechanism.
 */
class StreamSelecter {
  private:
    fd_set socks;
    std::vector<SocketStream*> streams;

  public:
    void add_stream(SocketStream* stream);
    SocketStream* select();
    SocketStream* select(unsigned int usecs);
};

//}
//}

//using namespace TOOLS::NET;



SocketStream::~SocketStream() {
  close(fd);
}

inline SocketStream& SocketStream::operator<<(const std::string& data) {
  size_t len = data.length();
  ssize_t n = ::write(fd, data.c_str(), len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}

void SocketStream::flush() {
//   ::flush(fd);
}

inline const std::string SocketStream::get(int len) {
  char buf[len+1];
  bzero(&buf, len+1);

  ssize_t n = ::read(fd, &buf, len);
  if(n < 0)
    throw SocketException("Could not read from socket");
  return buf;
}

inline const std::string SocketStream::operator>>(int len) {
  return get(len);
}

inline Socket::Socket(SOCKET_TYPE type, short unsigned port) : fd(0), port(port), type(type), stream(0) {
  bzero((char*) &addr, sizeof(addr));

  if(type == TCP)
    fd = socket(AF_INET, SOCK_STREAM, 0);
  else if(type == UDP)
    fd = socket(AF_INET, SOCK_DGRAM, 0);
  else if(type == UNIX)
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);

  if(fd < 0)
    throw SocketException("Could not create socket");
}

inline Socket::~Socket() {
  delete stream;
  // hmmm no fd because SocketStream does the job
}


// glib bug in htons!
#pragma GCC diagnostic ignored "-Wconversion"
inline ServerSocket::ServerSocket(SOCKET_TYPE type, uint16_t port) : Socket(type, port) {
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if(bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    throw SocketException("Could not bind to address/port");

  stream = new SocketStream(fd);
}
#pragma GCC diagnostic warning "-Wconversion"

inline ServerSocket::~ServerSocket() {
  if(stream != NULL)
    delete stream;
}

inline SocketStream* ServerSocket::listen() {
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  bzero((char*) &client_addr, client_addr_len);
  ::listen(fd, MAX_LISTEN_QUEUE);

  int client_fd = accept(fd, (struct sockaddr*) &client_addr, (socklen_t*)
      &client_addr_len);
  if(client_fd < 0)
    throw SocketException("Error during accaptance of remote client");
  return new SocketStream(client_fd);
}

#pragma GCC diagnostic ignored "-Wconversion"
inline ClientSocket::ClientSocket(SOCKET_TYPE type, short unsigned port, const
    std::string& target) : Socket(type, port) {
  addr.sin_port = htons((unsigned short int) port);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(target.c_str());

  //socklen_t addr_len = sizeof(addr);
  if(connect(fd, (struct sockaddr*) &addr, sizeof(addr))) {
    perror("connect:");
    throw SocketException("Could not connect to given target");

  }

  stream = new SocketStream(fd);
}
#pragma GCC diagnostic warning "-Wconversion"

ClientSocket::~ClientSocket() { }

#endif
// vim:ts=8:sw=2:et:
