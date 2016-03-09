/*
 * SYSINFO.h
 *
 *  Created on: Feb 16, 2016
 *      Author: senia
 */

#ifndef SYSINFO_H_
#define SYSINFO_H_
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <unistd.h>


double bytesToGigabytes(long long int num);
// CPU
int getNumberOfCores();
// From http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double getCPULoad();
double getCPULoadByLine(int line);
double getCPULoadByCore(int core);


// Memory
double getTotalPhysicalMem();
double getUsedPhysicalMem();

double getTotalVirtualMem();
double getUsedVirutalMem();

#endif /* SYSINFO_H_ */
