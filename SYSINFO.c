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
int* getCPUCoresLoad(int numOfCores){
	++numOfCores;
	float* res = (float*) malloc(numOfCores * sizeof(float));
	float** arrA = (float**) malloc(numOfCores * sizeof(float*)); //a pointer-to-pointer-to-float
	float** arrB = (float**) malloc(numOfCores * sizeof(float*)); //a pointer-to-pointer-to-float
	unsigned int i;
	long double a[numOfCores][4], b[numOfCores][4], loadavg;
	FILE *fp;
	char dump[50];
	float tempRes;

    fp = fopen("/proc/stat","r");
    for(i=0;i<numOfCores;i++){
    	arrA[i] = (float*) malloc(4 * sizeof(float));
    	fscanf(fp,"%*s %f %f %f %f",&arrA[i][0], &arrA[i][1], &arrA[i][2], &arrA[i][3]);
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[i][0],&a[i][1],&a[i][2],&a[i][3]);
    }
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    for(i=0;i<numOfCores;i++){
    	arrB[i] = (float*) malloc(4 * sizeof(float));
    	fscanf(fp,"%f %f %f %f",&arrB[i][0], &arrB[i][1], &arrB[i][2], &arrB[i][3]);
    	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[i][0],&b[i][1],&b[i][2],&b[i][3]);
    }
    fclose(fp);

    for(i=0;i<numOfCores;i++){
    	printf("Got Bs: %f %f %f %f\t",&arrB[i][0], &arrB[i][1], &arrB[i][2], &arrB[i][3]);
    	tempRes = ((arrB[i][0]+arrB[i][1]+arrB[i][2]) - (arrA[i][0]+arrA[i][1]+arrA[i][2])) /
    			((arrB[i][0]+arrB[i][1]+arrB[i][2]+arrB[i][3]) - (arrA[i][0]+arrA[i][1]+arrA[i][2]+arrA[i][3]));
    	printf("Inserting: %f\n", tempRes);
    	tempRes = ((b[i][0]+b[i][1]+b[i][2]) - (a[i][0]+a[i][1]+a[i][2])) / ((b[i][0]+b[i][1]+b[i][2]+b[i][3]) - (a[i][0]+a[i][1]+a[i][2]+a[i][3]));
    	printf("Inserting222: %f\n", tempRes);
    	res[i] = tempRes;
    }
    for(i=0;i<numOfCores+1;i++)
    	printf("result %d: %f\n", i, res[i]);

	return res;
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
