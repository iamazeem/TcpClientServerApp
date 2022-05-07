#ifndef INCLUDE_COMMON_HPP_
#define INCLUDE_COMMON_HPP_

#include <iostream>
#include <string>
#include <boost/thread/mutex.hpp>

#include "logger.hpp"

static  boost::mutex    globalStreamLock;
#define lockStream()    globalStreamLock.lock()
#define unlockStream()  globalStreamLock.unlock()

#endif /* INCLUDE_COMMON_HPP_ */
