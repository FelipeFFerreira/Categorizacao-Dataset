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

#define INSTALL_OMP //Install_openmp
#define INSTALL_DEBUG exit(0xA);

#ifdef INSTALL_OMP
	#include <omp.h>
#endif

#define HOLD 0
#define PROCEED 1
#define NEXT 2

#define N 100000
#define N_TOTAL 100000
#define QTD_WORD 1000


#endif //_MAIN_H
