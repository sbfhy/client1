//// Use of this source code is governed by a BSD-style license
//// that can be found in the License file.
////
//// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
//#include "Timestamp.h"
//
//#include <windows.h>
//#include <stdio.h>  
//
//#ifndef __STDC_FORMAT_MACROS
//#define __STDC_FORMAT_MACROS
//#endif
//
//#include <inttypes.h>
//
//using namespace muduo;
//
//static_assert(sizeof(Timestamp) == sizeof(int64_t),
//              "Timestamp is same size as int64_t");
//
//string Timestamp::ToString() const
//{
//    char buf[32] = { 0 };
//    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
//    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
//    snprintf(buf, sizeof(buf) - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
//    return buf;
//}
//
//string Timestamp::ToFormattedString(bool showMicroseconds) const
//{
//    char buf[64] = { 0 };
//    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
//    struct tm tm_time;
//    gmtime(&seconds, &tm_time);
//
//    if (showMicroseconds)
//    {
//        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
//        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
//            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
//            microseconds);
//    }
//    else
//    {
//        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
//            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
//            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
//    }
//    return buf;
//}
//
//Timestamp Timestamp::NowMicroSecond()
//{
//    // GetLocalTime() for the time in the system timezone, GetSystemTime() for UTC.
//    // If you want a seconds-since-epoch time, use SystemTimeToFileTime() or GetSystemTimeAsFileTime().
//    // windows有很多关于时间的函数都是使用FILETIME结构，这个结构代表了从1601年1月1日开始到现在的100纳秒间隔数  
//    // 其他平台则更多使用unix时间轴，从1970年1月1日00:00:00到现在的经过的秒数。两者之间要转换就要用到116444736000000000这个魔法数字
//    FILETIME    file_time;
//    SYSTEMTIME  system_time;
//    uint64_t    time;
//
//    GetSystemTime(&system_time);
//    SystemTimeToFileTime(&system_time, &file_time);
//    time = ((uint64_t)file_time.dwLowDateTime);
//    time += ((uint64_t)file_time.dwHighDateTime) << 32;     // time 即表示从1601年1月1日开始到现在的100纳秒间隔数
//    // 1秒内有10000000个100纳秒间隔，即 微秒数 = ((time - EPOCH_FILE_TIME) / 10000000)s  * 1000000
//    return Timestamp((time - EPOCH_FILE_TIME) / 10);
//}
//
//Timestamp Timestamp::NowMilliSecond()
//{
//    FILETIME    file_time;
//    SYSTEMTIME  system_time;
//    uint64_t    time;
//
//    GetSystemTime(&system_time);
//    SystemTimeToFileTime(&system_time, &file_time);
//    time = ((uint64_t)file_time.dwLowDateTime);
//    time += ((uint64_t)file_time.dwHighDateTime) << 32;
//    return Timestamp((time - EPOCH_FILE_TIME) / 10000);
//}
//
