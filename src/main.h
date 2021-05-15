#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "libs/lib_lista_int/lista.h"
#include "libs/lib_thread/lista_th.h"
#include "libs/lib_thread/threads.h"
//#include "libs/lib_thread/avl.h"
//#include "libs/lib_thread/avl_tree.h"

#define INSTALL_OMP //Install_openmp
//#define INSTALL_THREAD_IO_PARALLEL

#define install_parallel
#define INSTALL_DEBUG exit(0xA);

#ifdef INSTALL_OMP
	#include <omp.h>
	#define install_openmp
#endif // INSTALL_OMP

#ifdef INSTALL_THREAD_IO_PARALLEL
    #define install_parallel_io
#endif // INSTALL_THREAD_IO_PARALLEL

#define HOLD 0
#define PROCEED 1
#define NEXT 2

#define N 50000
#define N_TOTAL 100000
#define QTD_WORD 10000

#endif //_MAIN_H
