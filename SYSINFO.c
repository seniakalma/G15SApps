/*
 * SYSINFO.c
 *
 *  Created on: Feb 16, 2016
 *      Author: senia
 */
#include "SYSINFO.h"

double bytesToGigabytes(long long int num){
	return (double)((double)num/1024/1024/1024);
}
// ================ ## CPU ## ================
int getNumberOfCores(){
	return sysconf(_SC_NPROCESSORS_ONLN);	//Number of CPUs
}


static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void init(){
	FILE* file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
	&lastTotalSys, &lastTotalIdle);
	fclose(file);
}


double getCurrentValue(){
	double percent;
	FILE* file;
	unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;


	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
	&totalSys, &totalIdle);
	fclose(file);


	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
		totalSys < lastTotalSys || totalIdle < lastTotalIdle){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
		(totalSys - lastTotalSys);
		percent = total;
		total += (totalIdle - lastTotalIdle);
		percent /= total;
		percent *= 100;
	}
	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;


	return percent;
}




// ================ ## Memory ## ================
double getTotalPhysicalMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalPhysMem = memInfo.totalram;
    //Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(totalPhysMem);
}

double getUsedPhysicalMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(physMemUsed);
}

double getTotalVirtualMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalVirtualMem = memInfo.totalram;
    //Add other values in next statement to avoid int overflow on right hand side...
    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(totalVirtualMem);
}

double getUsedVirutalMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
    //Add other values in next statement to avoid int overflow on right hand side...
    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(virtualMemUsed);
}

double getTotalSwapMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long physMemUsed = memInfo.totalswap - memInfo.freeswap;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(physMemUsed);
}

double getUsedSwapMem(){
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;	//in bytes

    return bytesToGigabytes(physMemUsed);
}


