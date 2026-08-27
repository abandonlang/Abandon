#ifndef _SAYERROR_H_
#define _SAYERROR_H_
#include <string>
void sayError(int line, int column, std::string info);
void sayError(std::string info);

void sayWarning(std::string info);
#endif