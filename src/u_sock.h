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
class SocketStream : public iostream {

    int fd;
    const unsigned bufsize;
    unsigned chunksize;
    char* buf;
    unsigned cur;
  public:
    SocketStream() :  iostream(), fd(0), bufsize(64) {}
    enum EOL {eol};


    SocketStream(int fd, unsigned bs=64) : 
      iostream(),
      fd(fd),
      bufsize(bs)
    {}

    SocketStream(const SocketStream& obj) :
      ios(),  iostream(), fd(obj.fd), bufsize(obj.bufsize)
    {
      buf = (char*)malloc(sizeof(char)*bufsize);
    }
    virtual ~SocketStream();
  private:
    void read();

  public:
    void flush();

    const std::string get(int len);
//    const std::string operator>>(int len);
    SocketStream& operator>>(double&);
    SocketStream& operator>>(char&);
    SocketStream& operator>>(int16_t&);
    SocketStream& operator>>(int32_t&);


    SocketStream operator=(const SocketStream& p ){
        fd=p.fd;
        return *this;
    }


    void send(const string& data);
    void send(const double& data);
    void send(const string& data, int len);
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
  public:
    enum SOCKET_TYPE {
      UNIX,
      TCP,
      UDP
    };
  protected:
    int fd;
    uint16_t port;
    struct sockaddr_in addr;
    SOCKET_TYPE type; // ??
    SocketStream* stream;

    Socket(): fd(0), port(0), stream(0){}

    Socket(SOCKET_TYPE type, short unsigned port );
    virtual ~Socket();
  public:
    enum EOL {eol};
    template<class T>
      SocketStream& operator<<(const T data);
};

template<>
SocketStream& Socket::operator<<(const Socket::EOL& ){
  assert(stream);
  return *stream << SocketStream::eol;
}

template<class T>
SocketStream& Socket::operator<<(const T data){
  assert(stream);
  return *stream << data;
}
/**
 * @brief ServerSocket does the obvious, it sets up a server
 *        socket, which then can be used to listen to and recive connections
 */
class ServerSocket : public Socket {
  private: 
    short unsigned port_tries;
  public:
    ServerSocket(SOCKET_TYPE type, short unsigned port, short unsigned tries);
    virtual ~ServerSocket();

    SocketStream listen();
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

//}
//}

//using namespace TOOLS::NET;



SocketStream::~SocketStream() {
  close(fd);
  free(buf);
}

inline SocketStream& SocketStream::operator<<(const std::string& data) {
  size_t len = data.length();
  ssize_t n = ::write(fd, data.c_str(), len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}

inline SocketStream& SocketStream::operator<<(const double data) {
#define  DL  (sizeof(double)/sizeof(char))
  union { 
    char c[DL];
    double d;
  } convert;

  convert.d=data;

  ssize_t n = ::write(fd, convert.c, DL);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}

inline SocketStream& SocketStream::operator<<(const int data) {
  const int len =  (sizeof(int)/sizeof(char));
  union { 
    char c[len];
    int d;
  } convert;

  convert.d=data;

  ssize_t n = ::write(fd, convert.c, len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}


void SocketStream::flush() {
//   ::flush(fd);
}

inline void SocketStream::read() {
  assert(chunksize == cur);

  cur = 0;
  ssize_t n = ::read(fd, &buf, bufsize);
  if(n < 0)
    throw SocketException("Could not read from socket");
  chunksize = static_cast<unsigned>(n);
}

inline const string SocketStream::get(int len) {
  char buf[len+1];
  bzero(&buf, len+1);

  ssize_t n = ::read(fd, &buf, len);
  if(n < 0)
    throw SocketException("Could not read from socket");
  return buf;
}

//inline const std::string &SocketStream::operator>>(int len) {
//  return get(len);
//}

inline SocketStream &SocketStream::operator>>(int32_t& d) {
  const uint_t len=4;
  union { 
    char c[len];
    uint16_t d;
  } convert;
  for(unsigned i=0; i<len; ++i){
    *this >> convert.c[i];
  }
  d = convert.d;
  return *this;
}

inline SocketStream &SocketStream::operator>>(int16_t& d) {
  const uint_t len=2;
  union { 
    char c[len];
    uint16_t d;
  } convert;
  for(unsigned i=0; i<len; ++i){
    *this >> convert.c[i];
  }
  d = convert.d;
  return *this;
}

inline SocketStream &SocketStream::operator>>(double& d) {
  union { 
    char c[DL];
    double d;
  } convert;
  for(unsigned i=0; i<DL; ++i){
    *this >> convert.c[i];
  }
  d = convert.d;
  return *this;
}

// this is inefficient, but so what?
inline SocketStream &SocketStream::operator>>(char& c) {
  if( cur == chunksize ){
        read();
  }
  c = buf[cur++];
  return *this;
}

inline Socket::Socket(SOCKET_TYPE type, short unsigned port ) :
  fd(0), port(port), type(type), stream(0) {
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
inline ServerSocket::ServerSocket(SOCKET_TYPE type, uint16_t port, short
    unsigned port_tries=1) : Socket(type, port ), port_tries(port_tries) {
  if (port_tries == 0) return;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;

  addr.sin_port = htons(port);

  int b = -1;
  for( short unsigned p = port; p<port+port_tries; p++ ){
    trace1("ServerSocket::ServerSocket", p);
    addr.sin_port = htons(p);
    b = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
    if (b >= 0) {
      trace1("listening", p);
      break;
    }
  }
  if (b<0)
    throw SocketException("Could not bind to address/port");
  trace1("ServerSocket", port);

  stream = new SocketStream(fd);
}
#pragma GCC diagnostic warning "-Wconversion"

inline ServerSocket::~ServerSocket() {
  if(stream != NULL)
    delete stream;
}

inline SocketStream ServerSocket::listen() {
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  trace0("ServerSocket::listen");

  bzero((char*) &client_addr, client_addr_len);
  ::listen(fd, MAX_LISTEN_QUEUE);

  int client_fd = accept(fd, (struct sockaddr*) &client_addr, (socklen_t*)
      &client_addr_len);
  if(client_fd < 0)
    throw SocketException("Error during accaptance of remote client");
  return SocketStream(client_fd);
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
