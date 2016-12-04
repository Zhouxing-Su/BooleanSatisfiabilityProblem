////////////////////////////////
/// usage : 1.  utilities.
///
/// note :  1.
////////////////////////////////

#ifndef SZX_IRP_UTILITY_H
#define SZX_IRP_UTILITY_H


#include <algorithm>
#include <atomic>
#include <chrono>
#include <random>
#include <string>
#include <ratio>

#include <cstdlib>
#include <ctime>


#pragma region Flag
//#define TIMER_ENABLE_GET_CPU_TIME  true
#if TIMER_ENABLE_GET_CPU_TIME
#ifdef _WIN32   // Windows
#include <Windows.h>
#else   // Posix/Linux
#include <sys/resource.h>
#include <sys/times.h>
#endif
#endif // TIMER_ENABLE_GET_CPU_TIME
#pragma endregion Flag


namespace szx {

/// if there is "#define x  y", VERBATIM_STRINGIFY(x) will get "x".
#define VERBATIM_STRINGIFY(x)  #x
/// if there is "#define x  y", RESOLVED_STRINGIFY(x) will get "y".
#define RESOLVED_STRINGIFY(x)  VERBATIM_STRINGIFY(x)

#define VERBATIM_CONCAT(a, b)  a##b
#define VERBATIM_CONCAT2(a, b, c)  a##b##c
#define VERBATIM_CONCAT3(a, b, c, d)  a##b##c##d
#define RESOLVED_CONCAT(a, b)  VERBATIM_CONCAT(a, b)
#define RESOLVED_CONCAT2(a, b, c)  VERBATIM_CONCAT2(a, b, c)
#define RESOLVED_CONCAT3(a, b, c, d)  VERBATIM_CONCAT3(a, b, c, d)


#pragma region Arr
template<typename T, typename IndexType = int>
class Arr {
public:
    /// it is always valid before copy assignment due to no reallocation.
    using Iterator = T*;
    using ConstIterator = T const *;

    explicit Arr() : arr(nullptr), len(0) {}
    explicit Arr(IndexType length) { allocate(length); }
    explicit Arr(IndexType length, T *data) : arr(data), len(length) {}
    explicit Arr(IndexType length, const T &defaultValue) {
        allocate(length);
        std::fill(arr, arr + length, defaultValue);
    }

    Arr(const Arr &a) : Arr(a.len) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr(Arr &&a) : Arr() {
        *this = std::move(a);
    }

    Arr& operator=(const Arr &a) {
        if (this != &a) {
            clear();
            init(a.len);
            copyData(a.arr);
        }
        return *this;
    }
    Arr& operator=(Arr &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length) {
        if (arr == nullptr) {
            allocate(length);
            return true;
        }
        return false;
    }

    /// remove all items.
    void clear() {
        delete[] arr;
        arr = nullptr;
    }

    /// set all data to 0.
    void reset() { memset(arr, 0, sizeof(T) * len); }

    T& at(IndexType i) { return arr[i]; }
    const T& at(IndexType i) const { return arr[i]; }

    Iterator begin() { return arr; }
    Iterator end() { return (arr + len); }
    ConstIterator begin() const { return arr; }
    ConstIterator end() const { return (arr + len); }
    T& front() { return at(0); }
    T& back() { return at(len - 1); }
    const T& front() const { return at(0); }
    const T& back() const { return at(len - 1); }

    IndexType size() const { return len; }
    bool empty() const { return (len == 0); }

protected:
    /// must not be called except init.
    void allocate(IndexType length) {
        arr = new T[length];
        len = length;
    }

    void copyData(T *data) {
        // EXTEND[szx][1]: what if data is shorter than arr?
        // OPTIMIZE[szx][8]: use memcpy() if all callers are POD type.
        copy(data, data + len, arr);
    }


    T *arr;
    IndexType len;
};


template<typename T, typename IndexType = int>
class Arr2D : public Arr<T, IndexType> {
public:
    explicit Arr2D() : len1(0), len2(0) {}
    explicit Arr2D(IndexType length1, IndexType length2) { allocate(length1, length2); }
    explicit Arr2D(IndexType length1, IndexType length2, const T &defaultValue) {
        allocate(length1, length2);
        std::fill(arr, arr + len, defaultValue);
    }

    Arr2D(const Arr2D &a) : Arr2D(a.len1, a.len2) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr2D(Arr2D &&a) : Arr2D() {
        *this = std::move(a);
    }

    Arr2D& operator=(const Arr2D &a) {
        if (this != &a) {
            clear();
            init(a.len1, a.len2);
            copyData(a.arr);
        }
        return *this;
    }
    Arr2D& operator=(Arr2D &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len1 = a.len1;
            len2 = a.len2;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr2D() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length1, IndexType length2) {
        if (arr == nullptr) {
            allocate(length1, length2);
            return true;
        }
        return false;
    }

    using Arr<T, IndexType>::at;
    T& at(IndexType i1, IndexType i2) { return arr[i1 * len2 + i2]; }
    const T& at(IndexType i1, IndexType i2) const { return arr[i1 * len2 + i2]; }

    using Arr<T, IndexType>::begin;
    using Arr<T, IndexType>::end;
    using Arr<T, IndexType>::front;
    using Arr<T, IndexType>::back;
    Iterator begin(IndexType i1) { return arr + (i1 * len2); }
    Iterator end(IndexType i1) { return arr + (i1 * len2) + len2; }
    ConstIterator begin(IndexType i1) const { return arr + (i1 * len2); }
    ConstIterator end(IndexType i1) const { return arr + (i1 * len2) + len2; }
    T& front(IndexType i1) { return at(i1, 0); }
    T& back(IndexType i1) { return at(i1, len - 1); }
    const T& front(IndexType i1) const { return at(i1, 0); }
    const T& back(IndexType i1) const { return at(i1, len - 1); }

    IndexType size1() const { return len1; }
    IndexType size2() const { return len2; }

protected:
    /// must not be called except init.
    void allocate(IndexType length1, IndexType length2) {
        len1 = length1;
        len2 = length2;
        len = length1 * length2;
        arr = new T[len];
    }


    IndexType len1;
    IndexType len2;
};


template<typename T, typename IndexType = int>
class Arr3D : public Arr<T, IndexType> {
public:
    explicit Arr3D() : len1(0), len2(0), len3(0), len2len3(0) {}
    explicit Arr3D(IndexType length1, IndexType length2, IndexType length3) { allocate(length1, length2, length3); }
    explicit Arr3D(IndexType length1, IndexType length2, IndexType length3, const T &defaultValue) {
        allocate(length1, length2, length3);
        std::fill(arr, arr + len, defaultValue);
    }

    Arr3D(const Arr3D &a) : Arr3D(a.len1, a.len2, a.len3) {
        if (this != &a) { copyData(a.arr); }
    }
    Arr3D(Arr3D &&a) : Arr3D() {
        *this = std::move(a);
    }

    Arr3D& operator=(const Arr3D &a) {
        if (this != &a) {
            clear();
            init(a.len1, a.len2, a.len3);
            copyData(a.arr);
        }
        return *this;
    }
    Arr3D& operator=(Arr3D &&a) {
        if (this != &a) {
            delete[] arr;
            arr = a.arr;
            len1 = a.len1;
            len2 = a.len2;
            len3 = a.len3;
            len2len3 = a.len2len3;
            len = a.len;
            a.arr = nullptr;
        }
        return *this;
    }

    ~Arr3D() { clear(); }

    /// allocate memory if it has not been init before.
    bool init(IndexType length1, IndexType length2, IndexType length3) {
        if (arr == nullptr) {
            allocate(length1, length2, length3);
            return true;
        }
        return false;
    }

    using Arr<T, IndexType>::at;
    T& at(IndexType i1, IndexType i2, IndexType i3) { return arr[i1 * len2len3 + i2 * len3 + i3]; }
    const T& at(IndexType i1, IndexType i2, IndexType i3) const { return arr[i1 * len2len3 + i2 * len3 + i3]; }

    using Arr<T, IndexType>::begin;
    using Arr<T, IndexType>::end;
    Iterator begin(IndexType i1) const { return arr + (i1 * len2len3); }
    Iterator end(IndexType i1) const { return arr + (i1 * len2len3) + len2len3; }
    Iterator begin(IndexType i1, IndexType i2) const { return arr + (i1 * len2len3 + i2 * len3); }
    Iterator end(IndexType i1, IndexType i2) const { return arr + (i1 * len2len3 + i2 * len3) + len3; }

    IndexType size1() const { return len1; }
    IndexType size2() const { return len2; }
    IndexType size3() const { return len3; }

protected:
    /// must not be called except init.
    void allocate(IndexType length1, IndexType length2, IndexType length3) {
        len1 = length1;
        len2 = length2;
        len3 = length3;
        len2len3 = len2 * len3;
        len = length1 * len2len3;
        arr = new T[len];
    }


    IndexType len1;
    IndexType len2;
    IndexType len3;
    IndexType len2len3;
};
#pragma endregion Arr


class IntID {
public:
    static int generate() { return id.fetch_add(1); }

private:
    static std::atomic<int> id;
};


class RandSeed {
public:
    static int generate() {
        return static_cast<int>(std::time(nullptr) + std::clock());
    }
};


class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Millisecond = std::chrono::milliseconds;
    using TickCount = long long;
    using Duration = double;


    static constexpr TickCount MillisecondsPerSecond = std::milli::den;
    static constexpr TickCount MicrosecondsPerSecond = std::micro::den;


    Timer(const Millisecond &duration, const TimePoint &startTime = Clock::now())
        : startTime(startTime), endTime(startTime + duration) {}
    Timer(TickCount durationInMillisecond, const TimePoint &startTime = Clock::now())
        : Timer(Millisecond(durationInMillisecond), startTime) {}

    static double getDuration(const TimePoint &start, const TimePoint &end, Duration tickPerSecond = 1) {
        return std::chrono::duration_cast<Millisecond>(end - start).count() * tickPerSecond / MillisecondsPerSecond;
    }

    // there is no need to free the pointer. the format of the format string is 
    // the same as std::strftime() in http://en.cppreference.com/w/cpp/chrono/c/strftime.
    static const char* getLocalTime(const char *format = "%Y-%m-%d(%a)%H:%M:%S") {
        static constexpr int DateBufSize = 64;
        static char buf[DateBufSize];
        time_t t = time(NULL);
        tm *date = localtime(&t);
        strftime(buf, DateBufSize, format, date);
        return buf;
    }

    #if TIMER_ENABLE_GET_CPU_TIME
    static TickCount getCpuTimeInMillisecond() {
        #ifdef _WIN32   // Windows
        FILETIME creationTime, exitTime, kernelTime, userTime;
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        return ((static_cast<TickCount>(userTime.dwHighDateTime) << (sizeof(int) * 8)) | userTime.dwLowDateTime) / 10;
        #else   // Posix/Linux
        rusage r;
        getrusage(RUSAGE_SELF, &r);
        return r.ru_utime.tv_sec * MicrosecondsPerSecond + r.ru_utime.tv_usec;
        #endif
    }
    #endif // TIMER_ENABLE_GET_CPU_TIME

    bool isTimeOut() const {
        return (Clock::now() > endTime);
    }

    Millisecond restTime() const {
        return std::chrono::duration_cast<Millisecond>(endTime - Clock::now());
    }

    Millisecond elapsedTime() const {
        return std::chrono::duration_cast<Millisecond>(Clock::now() - startTime);
    }


    const TimePoint startTime;
    const TimePoint endTime;

private:	// forbidden operators.
    Timer& operator=(const Timer &) { return *this; }
};


class Directory {
public:
    static void makeSureDirExist(const std::string &dir) {
        #ifdef WIN32
        std::string cmd("mkdir \"" + dir + "\" 2> nul");
        #else
        std::string cmd("mkdir -p \"" + dir + "\" 2> /dev/null");
        #endif
        system(cmd.c_str());
    }
};

}


#endif  // SZX_IRP_UTILITY_H
