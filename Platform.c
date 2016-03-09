/*
htop - linux/Platform.c
(C) 2014 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Platform.h"

#include <math.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*{
#include "Action.h"
#include "MainPanel.h"
#include "BatteryMeter.h"
#include "LinuxProcess.h"
#include "SignalsPanel.h"
}*/

#ifndef CLAMP
#define CLAMP(x,low,high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))
#endif

void setProccessList(LinuxProcessList* this){
	int i;
	//ProcessList* pl = &(this->super);
	//ProcessList_init(pl, Class(LinuxProcess), usersTable, pidWhiteList, userId);
	FILE* file = fopen(PROCSTATFILE, "r");
	if (file == NULL) {
	  //CRT_fatalError("Cannot open " PROCSTATFILE);
	   assert(0);
	}
	char buffer[PROC_LINE_LENGTH + 1];
	int cpusNum = -1;
	do {
	   cpusNum++;
	  char * s = fgets(buffer, PROC_LINE_LENGTH, file);
	  (void) s;
	} while (String_startsWith(buffer, "cpu"));
	this->super.cpuCount = cpusNum;
	this->cpus = xCalloc(cpusNum, sizeof(CPUData));

	for (i = 0; i < cpusNum; i++) {
	  this->cpus[i].totalTime = 1;
	  this->cpus[i].totalPeriod = 1;
	}

	CPUData* cpus = xCalloc(cpusNum, 4*sizeof(CPUData));
	for (i = 0; i < 4; i++) {
	  cpus[i].totalTime = 1;
	  cpus[i].totalPeriod = 1;
	}
}

int Platform_getUptime() {
	double uptime = 0;
	FILE* fd = fopen(PROCDIR "/uptime", "r");
	if (fd) {
		int n = fscanf(fd, "%64lf", &uptime);
		fclose(fd);
		if (n <= 0) return 0;
	}
	return (int) floor(uptime);
}

void Platform_getLoadAverage(double* one, double* five, double* fifteen) {
	int activeProcs, totalProcs, lastProc;
	*one = 0; *five = 0; *fifteen = 0;
	FILE *fd = fopen(PROCDIR "/loadavg", "r");
	if (fd) {
		int total = fscanf(fd, "%32lf %32lf %32lf %32d/%32d %32d", one, five, fifteen,
				&activeProcs, &totalProcs, &lastProc);
		(void) total;
		assert(total == 6);
		fclose(fd);
	}
}

int Platform_getMaxPid() {
	FILE* file = fopen(PROCDIR "/sys/kernel/pid_max", "r");
	if (!file) return -1;
	int maxPid = 4194303;
	int match = fscanf(file, "%32d", &maxPid);
	(void) match;
	fclose(file);
	return maxPid;
}

double LinuxProcessList_scanCPUTime(LinuxProcessList* this) {
	int i;
	FILE* file = fopen(PROCSTATFILE, "r");
	if (file == NULL) {
		//CRT_fatalError("Cannot open " PROCSTATFILE);
		assert(0);
	}
	int cpus = this->super.cpuCount;
	assert(cpus > 0);
	for (i = 0; i <= cpus; i++) {
		char buffer[PROC_LINE_LENGTH + 1];
		unsigned long long int usertime, nicetime, systemtime, idletime;
		unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
		ioWait = irq = softIrq = steal = guest = guestnice = 0;
		// Depending on your kernel version,
		// 5, 7, 8 or 9 of these fields will be set.
		// The rest will remain at zero.
		char* ok = fgets(buffer, PROC_LINE_LENGTH, file);
		if (!ok) buffer[0] = '\0';
		if (i == 0)
			sscanf(buffer,   "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",         &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
		else {
			int cpuid;
			sscanf(buffer, "cpu%4d %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu", &cpuid, &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
			//assert(cpuid == i - 1);
		}
		// Guest time is already accounted in usertime
		usertime = usertime - guest;
		nicetime = nicetime - guestnice;
		// Fields existing on kernels >= 2.6
		// (and RHEL's patched kernel 2.4...)
		unsigned long long int idlealltime = idletime + ioWait;
		unsigned long long int systemalltime = systemtime + irq + softIrq;
		unsigned long long int virtalltime = guest + guestnice;
		unsigned long long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;
		CPUData* cpuData = &(this->cpus[i]);
		// Since we do a subtraction (usertime - guest) and cputime64_to_clock_t()
		// used in /proc/stat rounds down numbers, it can lead to a case where the
		// integer overflow.
		#define WRAP_SUBTRACT(a,b) (a > b) ? a - b : 0
		cpuData->userPeriod = WRAP_SUBTRACT(usertime, cpuData->userTime);
		cpuData->nicePeriod = WRAP_SUBTRACT(nicetime, cpuData->niceTime);
		cpuData->systemPeriod = WRAP_SUBTRACT(systemtime, cpuData->systemTime);
		cpuData->systemAllPeriod = WRAP_SUBTRACT(systemalltime, cpuData->systemAllTime);
		cpuData->idleAllPeriod = WRAP_SUBTRACT(idlealltime, cpuData->idleAllTime);
		cpuData->idlePeriod = WRAP_SUBTRACT(idletime, cpuData->idleTime);
		cpuData->ioWaitPeriod = WRAP_SUBTRACT(ioWait, cpuData->ioWaitTime);
		cpuData->irqPeriod = WRAP_SUBTRACT(irq, cpuData->irqTime);
		cpuData->softIrqPeriod = WRAP_SUBTRACT(softIrq, cpuData->softIrqTime);
		cpuData->stealPeriod = WRAP_SUBTRACT(steal, cpuData->stealTime);
		cpuData->guestPeriod = WRAP_SUBTRACT(virtalltime, cpuData->guestTime);
		cpuData->totalPeriod = WRAP_SUBTRACT(totaltime, cpuData->totalTime);
		#undef WRAP_SUBTRACT
		cpuData->userTime = usertime;
		cpuData->niceTime = nicetime;
		cpuData->systemTime = systemtime;
		cpuData->systemAllTime = systemalltime;
		cpuData->idleAllTime = idlealltime;
		cpuData->idleTime = idletime;
		cpuData->ioWaitTime = ioWait;
		cpuData->irqTime = irq;
		cpuData->softIrqTime = softIrq;
		cpuData->stealTime = steal;
		cpuData->guestTime = virtalltime;
		cpuData->totalTime = totaltime;
	}
	double period = (double)this->cpus[0].totalPeriod / cpus;
	fclose(file);
	return period;
}

void LinuxProcessList_scanMemoryInfo(ProcessList* this) {
	unsigned long long int swapFree = 0;
	unsigned long long int shmem = 0;
	unsigned long long int sreclaimable = 0;

	FILE* file = fopen(PROCMEMINFOFILE, "r");
	if (file == NULL) {
		//CRT_fatalError("Cannot open " PROCMEMINFOFILE);
		assert(0);
	}
	char buffer[128];
	while (fgets(buffer, 128, file)) {
		#define tryRead(label, variable) (String_startsWith(buffer, label) && sscanf(buffer + strlen(label), " %32llu kB", variable))
		switch (buffer[0]) {
			case 'M':
				if (tryRead("MemTotal:", &this->totalMem)) {}
				else if (tryRead("MemFree:", &this->freeMem)) {}
				else if (tryRead("MemShared:", &this->sharedMem)) {}
			break;
			case 'B':
				if (tryRead("Buffers:", &this->buffersMem)) {}
			break;
			case 'C':
			if (tryRead("Cached:", &this->cachedMem)) {}
			break;
			case 'S':
			switch (buffer[1]) {
			case 'w':
				if (tryRead("SwapTotal:", &this->totalSwap)) {}
				else if (tryRead("SwapFree:", &swapFree)) {}
			break;
			case 'h':
				if (tryRead("Shmem:", &shmem)) {}
			break;
			case 'R':
				if (tryRead("SReclaimable:", &sreclaimable)) {}
			break;
		}
		break;
	}
	#undef tryRead
	}
	this->usedMem = this->totalMem - this->freeMem;
	this->cachedMem = this->cachedMem + sreclaimable - shmem;
	this->usedSwap = this->totalSwap - swapFree;
	this->freeSwap = swapFree;
	fclose(file);
}

double Platform_setCPUValues(CPUData* cpuData) {
	//CPUData* cpuData = malloc(sizeof(CPUData));//NULL;	//&(pl->cpus[cpu]);
	double total = (double) ( cpuData->totalPeriod == 0 ? 1 : cpuData->totalPeriod);
	/*LinuxProcessList* pl = (LinuxProcessList*) this->pl;*/
	double percent;
	double* v = malloc(CPU_METER_ITEMCOUNT * sizeof(double)) ;//= this->values;
	v[CPU_METER_NICE] = cpuData->nicePeriod / total * 100.0;
	v[CPU_METER_NORMAL] = cpuData->userPeriod / total * 100.0;
	if (0) {	//this->pl->settings->detailedCPUTime
		v[CPU_METER_KERNEL]  = cpuData->systemPeriod / total * 100.0;
		v[CPU_METER_IRQ]     = cpuData->irqPeriod / total * 100.0;
		v[CPU_METER_SOFTIRQ] = cpuData->softIrqPeriod / total * 100.0;
		v[CPU_METER_STEAL]   = cpuData->stealPeriod / total * 100.0;
		v[CPU_METER_GUEST]   = cpuData->guestPeriod / total * 100.0;
		v[CPU_METER_IOWAIT]  = cpuData->ioWaitPeriod / total * 100.0;
		//Meter_setItems(this, 8);
	if (0) {		//this->pl->settings->accountGuestInCPUMeter
		percent = v[0]+v[1]+v[2]+v[3]+v[4]+v[5]+v[6];
	} else {
		percent = v[0]+v[1]+v[2]+v[3]+v[4];
	}
	} else {
		v[2] = cpuData->systemAllPeriod / total * 100.0;
		v[3] = (cpuData->stealPeriod + cpuData->guestPeriod) / total * 100.0;
		//Meter_setItems(this, 4);
		percent = v[0]+v[1]+v[2]+v[3];
	}
	percent = CLAMP(percent, 0.0, 100.0);
	if (isnan(percent)) percent = 0.0;
	return percent;
}

void Platform_setMemoryValues() {	//Meter* this
	ProcessList* pl =NULL; //(ProcessList*) this->pl;
	long int usedMem = pl->usedMem;
	long int buffersMem = pl->buffersMem;
	long int cachedMem = pl->cachedMem;
	usedMem -= buffersMem + cachedMem;
	/*this->total = pl->totalMem;
	this->values[0] = usedMem;
	this->values[1] = buffersMem;
	this->values[2] = cachedMem;*/
}

void Platform_setSwapValues() {		//Meter* this
	/*ProcessList* pl = (ProcessList*) this->pl;
	this->total = pl->totalSwap;
	this->values[0] = pl->usedSwap;*/
}

char* Platform_getProcessEnv(pid_t pid) {
	char procname[32+1];
	snprintf(procname, 32, "/proc/%d/environ", pid);
	FILE* fd = fopen(procname, "r");
	char *env = NULL;
	if (fd) {
		size_t capacity = 4096, size = 0, bytes;
		env = xMalloc(capacity);
		while (env && (bytes = fread(env+size, 1, capacity-size, fd)) > 0) {
			size += bytes;
			capacity *= 2;
			env = xRealloc(env, capacity);
		}
		fclose(fd);
		if (size < 2 || env[size-1] || env[size-2]) {
		if (size + 2 < capacity) {
			env = xRealloc(env, capacity+2);
		}
		env[size] = 0;
		env[size+1] = 0;
		}
	}
	return env;
}
