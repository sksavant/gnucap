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
    mutable bool copied; // this is just an expriment. if you dont like it, dont use 
                         // copy and operator=
  public:
    enum EOL {eol};
    SocketStream() :  iostream(), fd(0), bufsize(64), copied(false) {
      buf = (char*)malloc((bufsize+1)*sizeof(char));
      buf[bufsize]=0;
    }


    SocketStream(int fd, unsigned bs=64) : 
      iostream(),
      fd(fd),
      bufsize(bs),
      copied(false)
    { 
      assert(bufsize);
      buf = (char*)malloc((1+bufsize)*sizeof(char));
      buf[bufsize]=0;
    }

    void operator=(const SocketStream& obj){
      fd=obj.fd;
      obj.copied=true;
      assert(bufsize==obj.bufsize);
      buf=obj.buf;
    }

    SocketStream(const SocketStream& obj) :
      ios(),  iostream(), fd(obj.fd), bufsize(obj.bufsize), buf(obj.buf)
    {
      assert(buf);
      obj.copied=true;
      assert(bufsize==obj.bufsize);
      //FIXME: check bufsize
    }
    virtual ~SocketStream();
  private:
    void read();

  public:
    bool at_end(){return cur==chunksize;}
    void flush();

    const std::string get(int len);
//    const std::string operator>>(int len);
    SocketStream& operator>>(double&);
    SocketStream& operator>>(char&);
    SocketStream& operator>>(unsigned char&);
    SocketStream& operator>>(int16_t&);
    SocketStream& operator>>(uint16_t&);
    SocketStream& operator>>(int32_t&);
    SocketStream& operator>>(int); // skip chars.


//    SocketStream operator=(const SocketStream& p ){
//        fd=p.fd;
//        return *this;
//    }


    void send(const string& data);
    void send(const double& data);
    void send(const string& data, int len);
    SocketStream& operator<<(const std::string& data);
//    SocketStream& operator<<(const std::string data);
    SocketStream& operator<<(const char* data); // good idea?
    SocketStream& operator<<(const char data);
    SocketStream& operator<<(const EOL){ flush(); return *this; }
    template<class T>
    SocketStream& operator<<(const T data);
    SocketStream& pad(const unsigned i){
      int j=i;
      const char x=NULL;
      while(j-->0)
        *this<<x;
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
    SocketStream* _stream;

    Socket(): fd(0), port(0), _stream(0), port_tries(1){}

    Socket(SOCKET_TYPE type, short unsigned port=0 );
    virtual ~Socket();
  protected: 
    short unsigned port_tries;
  public:
    virtual operator SocketStream() = 0;
    enum EOL {eol};
    template<class T>
      SocketStream& operator<<(const T data);
};

template<>
SocketStream& Socket::operator<<(const Socket::EOL& ){
  assert(_stream);
  return *_stream << SocketStream::eol;
}

template<class T>
SocketStream& Socket::operator<<(const T data){
  assert(_stream);
  return *_stream << data;
}
/**
 * @brief ServerSocket sets up a server
 *        socket, which then can be used to listen to and recive connections
 */
class ServerSocket : public Socket {
  public:
    ServerSocket(SOCKET_TYPE, string /*port*/, short unsigned /*tries*/){ assert(false);}
    ServerSocket(SOCKET_TYPE type, short unsigned port, short unsigned tries);
    virtual ~ServerSocket();

    SocketStream listen();
    operator SocketStream(){ assert(false);} // not implemented.
};
/**
 * @brief ClientSocket does the obvious, it can connect to any socket
 */
class ClientSocket : public Socket {
  public:
    ClientSocket(SOCKET_TYPE type, string port, const std::string& target);
    virtual ~ClientSocket();

    operator SocketStream(){ return*_stream;}
};
/**
 * @brief StreamSelecter wants an arbitrary number of (Socket)Streams as
 *        input - it wraps the select() mechanism.
 */

//}
//}

//using namespace TOOLS::NET;



SocketStream::~SocketStream() {
  if (!copied){
    trace1("SocketStream::~SocketStream closing", fd);
    close(fd);
    free(buf);
  }
}

inline SocketStream& SocketStream::operator<<(const std::string& data) {
  size_t len = data.length();
  ssize_t n = ::write(fd, data.c_str(), len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}

//inline SocketStream& SocketStream::operator<<(const std::string data) {
//  size_t len = data.length();
//  ssize_t n = ::write(fd, data.c_str(), len);
//  if(n < 0)
//    throw SocketException("Could not write to socket");
//  return *this;
//}

template<class T>
inline SocketStream& SocketStream::operator<<(const T data) {
  const int len =  (sizeof(T)/sizeof(char));
  union { 
    char c[len];
    T d;
  } convert;

  convert.d = data;

  ssize_t n = ::write(fd, convert.c, len);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}

inline SocketStream& SocketStream::operator<<(const char data) {

  ssize_t n = ::write(fd, &data, 1);
  if(n < 0)
    throw SocketException("Could not write to socket");
  return *this;
}



void SocketStream::flush() {
   //::flush(fd);
}

inline void SocketStream::read() {
  assert(chunksize == cur);
  assert(buf);
  cur = 0;
  ssize_t n = ::read(fd, buf, bufsize);
  if(n < 0)
    throw SocketException("SocketStream: Could not read from socket");
  chunksize = static_cast<unsigned>(n);
}

inline const string SocketStream::get(int len) {
//  char buf[len+1];
//  bzero(&buf, len+1);

  ssize_t n = ::read(fd, buf, len);
  if(n < 0)
    throw SocketException("Could not read from socket");
  return buf;
}

//inline const std::string &SocketStream::operator>>(int len) {
//  return get(len);
//}
//
inline SocketStream &SocketStream::operator>>(int len) {
  char x;
  for(unsigned i=0; i<len; ++i){
    *this >> x;
  }
}

inline SocketStream &SocketStream::operator>>(int32_t& d) {
  const uint_t len=sizeof(int32_t);
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
  const uint_t len=sizeof(int16_t);
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

inline SocketStream &SocketStream::operator>>(uint16_t& d) {
  const uint_t len=sizeof(int16_t);
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
    char c[sizeof(double)];
    double d;
  } convert;
  for(unsigned i=0; i<sizeof(double); ++i){
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

inline SocketStream &SocketStream::operator>>(unsigned char& c) {
  if( cur == chunksize ){
        read();
  }
  c = buf[cur++];
  return *this;
}

inline Socket::Socket(SOCKET_TYPE type, short unsigned port ) : fd(0), port(port), type(type), _stream(0) {
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
  delete _stream;
  // hmmm no fd because SocketStream does the job
}


// glib bug in htons!
#pragma GCC diagnostic ignored "-Wconversion"
inline ServerSocket::ServerSocket(SOCKET_TYPE type, uint16_t port, short
    unsigned port_tries=1) : Socket(type, port ) {
  if (port_tries == 0) return;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;

  addr.sin_port = htons(port);

  int b = -1;
  for( short unsigned p = port; p<port+port_tries; p++ ){
    trace2("ServerSocket::ServerSocket", p, port_tries);
    addr.sin_port = htons(p);
    b = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
    if (b >= 0) {
      trace1("listening", p);
      break;
    } else {
      trace2("cannot bind to", p, b);
    }
  }
  if (b<0)
    throw SocketException("Could not bind to address/port");

  _stream = new SocketStream(fd);
}
#pragma GCC diagnostic warning "-Wconversion"

inline ServerSocket::~ServerSocket() {
  if(_stream != NULL)
    delete _stream;
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
inline ClientSocket::ClientSocket(SOCKET_TYPE type, string port, const
    std::string& host) : Socket(type) {
  struct addrinfo *result, *rp;
  struct addrinfo hints;

  trace0("ClientSocket::ClientSocket " + host + " " + port );

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  if(type == TCP) {
    hints.ai_family = AF_INET;   
    hints.ai_socktype = SOCK_STREAM; 
  }    else if(type == UDP) {
    assert(false); // cleanup;
  }    else if(type == UNIX){
    assert(false);
  } else {
    assert(false);
  }
  trace3("looking up...", hints.ai_family, hints.ai_socktype, hints.ai_protocol );

  int s;
  s = getaddrinfo(host.c_str(),port.c_str(),&hints,&result);
  if (s)
    throw SocketException("Could not resolve "+host );


  if(fd)close(fd); // d'oh

  freeaddrinfo(result);
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    trace3("connecting...", rp->ai_family, rp->ai_socktype, rp->ai_protocol );
    rp->ai_family=hints.ai_family;

    fd = socket(rp->ai_family, rp->ai_socktype,0); //        rp->ai_protocol);

    if (fd == -1)
      continue;

    if (connect(fd, rp->ai_addr, rp->ai_addrlen) != -1) {
      trace1("ClientSocket::ClientSocket connected to " + host, fd );
      break; 
    }

    close(fd);
  }


  if(!rp){
    perror("connect:");
    throw SocketException("Could not connect to " + host);
  }

  _stream = new SocketStream(fd);
}
#pragma GCC diagnostic warning "-Wconversion"

ClientSocket::~ClientSocket() { }

#endif
// vim:ts=8:sw=2:et:
