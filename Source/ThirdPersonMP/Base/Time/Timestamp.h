//#pragma once
//
//// Use of this source code is governed by a BSD-style license
//// that can be found in the License file.
////
//// Author: Shuo Chen (chenshuo at chenshuo dot com)
//
//#include "ThirdPersonMP/Base/Common/Copyable.h"
//#include "ThirdPersonMP/Base/Define/DefineVariable.h"
//
//#include <string>
//
//using std::string;
//
//namespace muduo
//{
/////
///// Time stamp in UTC, in microseconds resolution.
/////
///// This class is immutable.
///// It's recommended to pass it by value, since it's passed in register on x64.
/////
//class Timestamp : public muduo::Copyable
//{
//public:
//    ///
//    /// Constucts an invalid Timestamp.
//    ///
//    Timestamp()
//        : microSecondsSinceEpoch_(0)
//    {
//    }
//
//    ///
//    /// Constucts a Timestamp at specific time
//    ///
//    /// @param MicroSecondsSinceEpoch
//    explicit Timestamp(int64_t microSecondsSinceEpochArg)
//        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
//    {
//    }
//
//    void Swap(Timestamp& that)
//    {
//        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
//    }
//
//    // default copy/assignment/dtor are Okay
//
//    string ToString() const;
//    string ToFormattedString(bool showMicroseconds = true) const;
//
//    bool IsValid() const { return microSecondsSinceEpoch_ > 0; }
//
//    // for internal usage.
//    int64_t MicroSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
//    time_t SecondsSinceEpoch() const
//    {
//        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
//    }
//
//    // Get time of now.
//    static Timestamp NowMicroSecond();              // 时间戳，单位微秒
//    static Timestamp NowMilliSecond();              // 时间戳，单位毫秒
//    static Timestamp GetInvalidTimestamp() { return Timestamp(); } 
//
//    static Timestamp FromUnixTime(time_t t) { return FromUnixTime(t, 0); }
//    static Timestamp FromUnixTime(time_t t, int microseconds)
//    {
//        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
//    }
//
//    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's.
//    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC) until 00:00:00 January 1, 1970.
//    // 这个数字是从1601年1月1日（UTC）到1970年1月1日00:00:00之间的100纳秒间隔数。
//    static const uint64_t EPOCH_FILE_TIME = ((uint64_t)116444736000000000ULL);
//    static const int kMicroSecondsPerSecond = 1000 * 1000;
//
//private:
//    int64_t microSecondsSinceEpoch_;
//};
//
//inline bool operator<(Timestamp lhs, Timestamp rhs)
//{
//    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
//}
//
//inline bool operator==(Timestamp lhs, Timestamp rhs)
//{
//    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
//}
//
/////
///// Gets time difference of two timestamps, result in seconds.
/////
///// @param high, low
///// @return (high-low) in seconds
///// @c double has 52-bit precision, enough for one-microsecond
///// resolution for next 100 years.
//inline double timeDifference(Timestamp high, Timestamp low)
//{
//    int64_t diff = high.MicroSecondsSinceEpoch() - low.MicroSecondsSinceEpoch();
//    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
//}
//
/////
///// Add @c seconds to given timestamp.
/////
///// @return timestamp+seconds as Timestamp
/////
//inline Timestamp addTime(Timestamp timestamp, double seconds)
//{
//    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
//    return Timestamp(timestamp.MicroSecondsSinceEpoch() + delta);
//}
//
//}  // namespace muduo
//
