#ifndef INCLUDE_COMMON_HPP_
#define INCLUDE_COMMON_HPP_

#include <iostream>
#include <string>
#include <boost/thread/mutex.hpp>

#include "logger.hpp"

#define SERVER_IP       "127.0.0.1"
#define SERVER_PORT     9900
#define SERVER_THREADS  10

static  boost::mutex    globalStreamLock;
#define lockStream()    globalStreamLock.lock()
#define unlockStream()  globalStreamLock.unlock()

#endif /* INCLUDE_COMMON_HPP_ */
