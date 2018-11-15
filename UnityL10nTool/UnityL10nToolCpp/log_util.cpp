#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include "log_util.h"
#include <WinSock.h>
#include <windows.h> 
#include <process.h>
#include <fstream>

static char log_file_prefix[64];
static char log_folder[1024] = ".";
static FILE *fp_log_file;
static int  log_level = LOG_LVL_INFO;

/**
* log level을 조정합니다.
*/
int LOGsetLevel(int log_lvl)
{
    int tmp = LOGgetLevel();

    log_level = log_lvl;

    return tmp;
}

/*
* log level을 최초로 요청할 때에는 환경변수의 설정을 읽습니다.
*/
int LOGgetLevel(void)
{
    char *log_env;
    static int is_env_check = 0;

    if(is_env_check == 0) {
        if((log_env = getenv("LOG_LEVEL")) == NULL) {
            log_level = LOG_LVL_INFO;
        } else {
            if(strcmp(log_env, "TRACE") == 0) {
                log_level = LOG_LVL_TRACE;
            } else if(strcmp(log_env, "DEBUG") == 0) {
                log_level = LOG_LVL_DEBUG;
            } else if(strcmp(log_env, "INFO") == 0) {
                log_level = LOG_LVL_INFO;
            } else if(strcmp(log_env, "WARNING") == 0) {
                log_level = LOG_LVL_WARNING;
            } else if(strcmp(log_env, "ERROR") == 0) {
                log_level = LOG_LVL_ERROR;
            } else if(strcmp(log_env, "FATAL") == 0) {
                log_level = LOG_LVL_FATAL;
            } else {
                log_level = LOG_LVL_INFO;
            }
        }
        is_env_check = 1;
    }
    return log_level;
}

/*
* Log file을 생성할 위치와 로그파일의 prefix를 설정함
* LOGset_log_info("/tmp", "mypgm");으로 설정하면
* 로그파일은 /tmp/mypgm-20160101.log 의 형태로 파일을 생성함
*/
int LOGsetInfo(const char *dir, const char *prefix)
{
    if(dir == NULL || dir[0] == 0x00) {
        fprintf(stderr, "log folder set error.\n");
        return -1;
    }
    if(prefix == NULL || prefix[0] == 0x00) {
        fprintf(stderr, "log file prefix set error.\n");
        return -1;
    }

    if(strcmp(dir, log_folder) == 0 && strcmp(prefix, log_file_prefix) == 0) {
        return 0;
    }

    strncpy(log_file_prefix, prefix, 64);
    strncpy(log_folder,      dir,    1024);

    if(fp_log_file != NULL) {
        fclose(fp_log_file);
        fp_log_file = NULL;
    }

    return 0;
}


/*
* LOGcreateFile 날짜가 변경되면 자동으로 새로운 파일이 생성됨
*/
static int LOGcreateFile(struct tm *tm1, const char *src_file)
{
    char filename[1024];
    char *ext;

    if(log_folder[0] == 0x00) {
        strcpy(log_folder, ".");
    }
    if(log_file_prefix[0] == 0x00) {
        strncpy(log_file_prefix, src_file, sizeof(log_file_prefix));
        if((ext = strchr(log_file_prefix, '.')) != NULL) {
            *ext = 0x00;
        }
    }
    _snprintf(filename, 1024, "%s/%s-%04d%02d%02d.log", log_folder, log_file_prefix, 1900 + tm1->tm_year, tm1->tm_mon + 1, tm1->tm_mday);

    if(fp_log_file != NULL) {
        fclose(fp_log_file);
        fp_log_file = NULL;
    }
	std::ifstream f(filename);
	if (f.good()) {
		f.close();
		DeleteFileA(filename);
	}
    if((fp_log_file = fopen(filename, "a")) == NULL) {
        return -1;
    }
    setvbuf(fp_log_file, NULL, _IONBF, 0);
    return 0;
}



// epoch time으로 변환할 상수
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// for timezone
struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

// gettimeofday in windows
int gettimeofday(struct timeval *tv/*, struct timezone *tz*/)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		// system time을 구하기
		GetSystemTimeAsFileTime(&ft);

		// unsigned 64 bit로 만들기
		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		// 100nano를 1micro로 변환하기
		tmpres /= 10;

		// epoch time으로 변환하기
		tmpres -= DELTA_EPOCH_IN_MICROSECS;

		// sec와 micorsec으로 맞추기
		tv->tv_sec = (tmpres / 1000000UL);
		tv->tv_usec = (tmpres % 1000000UL);
	}

	//// timezone 처리
	//if (NULL != tz)
	//{
	//	if (!tzflag)
	//	{
	//		_tzset();
	//		tzflag++;
	//	}
	//	tz->tz_minuteswest = _timezone / 60;
	//	tz->tz_dsttime = _daylight;
	//}

	return 0;
}

/*
* LOGlogging(...) 로그 파일을 생성함.
* log_type, 로그생성일시분초microseconds, process id, 소스파일, 함수명, 라인수, 오류 내용
* 의 format으로 로그를 생성함.
*/
int LOGlogging(char log_type, const char *src_file, const char *func, int line_no, const char *fmt, ...)
{
    va_list ap;
    int  sz = 0;
    struct timeval tv;
    struct tm *tm1;
    static int   day = -1;
    static int pid = -1;
    char   src_info[128];

	gettimeofday(&tv);
    tm1 = _localtime32(&tv.tv_sec);
    va_start(ap, fmt);

    if(pid == -1) {
        pid = _getpid();
    }
    /* 날짜가 변경되었으면 또는 최초 실행시에  */
    if(day != tm1->tm_mday) {
        if(LOGcreateFile(tm1, src_file) != 0) {
            return -1;
        }
        day = tm1->tm_mday;
    }

    sz += fprintf(fp_log_file, "(%c) ", log_type);
    sz += fprintf(fp_log_file, "%04d%02d%02d:%02d%02d%02d%06ld:%05d",
                                1900 + tm1->tm_year, tm1->tm_mon + 1, tm1->tm_mday,
                                tm1->tm_hour, tm1->tm_min, tm1->tm_sec, tv.tv_usec, pid);
    _snprintf(src_info, 128, "%s:%s(%d)", src_file, func, line_no);
    //sz += fprintf(fp_log_file, ":%-50.50s: ", src_info);
	sz += fprintf(fp_log_file, ":%s: ", src_info);
    sz += vfprintf(fp_log_file, fmt, ap);
    sz += fprintf(fp_log_file, "\n");
    va_end(ap);

    return sz;
}
