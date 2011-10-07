#ifdef __WIN32
#include "serialport_win.cpp"
#endif

#ifdef __APPLE__
#include "serialport_lin.cpp"
#endif

#ifdef __linux__
#include "serialport_lin.cpp"
#endif
