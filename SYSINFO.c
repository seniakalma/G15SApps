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
	long double a[4], b[4], loadavg;
	FILE *fp;
	char dump[50];

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));

	return loadavg;
}
//First line - Avg for all cores, 2nd - for first core, ..
double getCPULoadByLine(int line){
	int i;
	long double a[4], b[4], loadavg;
	FILE *fp;
	char dump[50];

    fp = fopen("/proc/stat","r");
    for(i=0;i<line;i++)
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));

	return loadavg;
}
double getCPULoadByCore(int core){
	//First line - Avg for all cores, 2nd - for first core, ..
	return getCPULoadByLine(core+1);
}

// TODO: FINISH THIS, READ ALL DATA AT ONCE.
int** getCPUCoresLoad(int numOfCores){
	int i;
	long double a[4], b[4], loadavg;
	FILE *fp;
	char dump[50];

    fp = fopen("/proc/stat","r");
    for(i=1;i<numOfCores+1;i++)
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));

	return NULL;
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
