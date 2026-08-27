#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
#include <iostream>
#define ASSERT(info) std::cout << "\033[0;31m[" << __FILE__ << \
    "," << __LINE__ << "]" << info << "\033[0m" << std::endl; // red

#define INFO(info) std::cout << "\033[0;32m" << info << "\033[0m" << std::endl; // green

#else
#define ASSERT(info)
#define INFO(info)
#endif

#endif