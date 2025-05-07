#ifndef TIMEDSHAREDPTR_H
#define TIMEDSHAREDPTR_H

#include <iostream>
#include <cstddef>
#include <chrono>
#include <utility>

using namespace std;

using Clock = chrono::high_resolution_clock;
typedef chrono::time_point<chrono::high_resolution_clock> clockTime;
typedef chrono::milliseconds milliSeconds;

struct ControlTimedSharedPtr {
    void *          ptr;
    long            myUseCount;
    clockTime       TimedSharedPtrStartTime;
    milliSeconds    deletePtrInMs;

    ControlTimedSharedPtr(void * p, long deleteMeInMilliseconds)
      : ptr(p),
        myUseCount(1),
        TimedSharedPtrStartTime(Clock::now()),
        deletePtrInMs{chrono::milliseconds(deleteMeInMilliseconds)}
    {
        cout << "ControlTimedSharedPtr " << p
             << " start: "
             << chrono::duration_cast<chrono::milliseconds>(
                    Clock::now().time_since_epoch() -
                    TimedSharedPtrStartTime.time_since_epoch()
                ).count()
             << " ms\n";
    }

    ~ControlTimedSharedPtr() {
        // *Note*: we do NOT delete ptr here (type-erased), but we do print elapsed time
        cout << "ControlTimedSharedPtr " << ptr
             << " end: "
             << chrono::duration_cast<chrono::milliseconds>(
                    Clock::now().time_since_epoch() -
                    TimedSharedPtrStartTime.time_since_epoch()
                ).count()
             << " ms\n";
    }
};

template<typename T>
class TimedSharedPtr {
private:
    ControlTimedSharedPtr* _PtrToControl;

public:
    // default constructor
    TimedSharedPtr() : _PtrToControl(nullptr) {}

    // primary construct
    TimedSharedPtr(T* raw, long deleteMeInMilliseconds)
      : _PtrToControl(new ControlTimedSharedPtr(raw, deleteMeInMilliseconds))
    {}

    // default timeout 1000 ms
    explicit TimedSharedPtr(T* raw)
      : TimedSharedPtr(raw, 1000)
    {}

    // copy construct
    TimedSharedPtr(const TimedSharedPtr& other) noexcept
      : _PtrToControl(other._PtrToControl)
    {
        if (_PtrToControl) {
            _PtrToControl->myUseCount++;
        }
    }

    // copy assignment
    TimedSharedPtr& operator=(const TimedSharedPtr& other) noexcept {
        if (this == &other) return *this;

        // release current
        if (_PtrToControl && _PtrToControl->myUseCount-- == 0) {
            delete _PtrToControl;
        }

        // acquire new
        _PtrToControl = other._PtrToControl;
        if (_PtrToControl) {
            _PtrToControl->myUseCount++;
        }
        return *this;
    }

    // destructor
    ~TimedSharedPtr() noexcept {
        if (_PtrToControl && _PtrToControl->myUseCount-- == 0) {
            delete _PtrToControl;
        }
    }

    long use_count() const noexcept {
        return _PtrToControl ? _PtrToControl->myUseCount : 0;
    }

    T* get() const {
        if (!_PtrToControl) return nullptr;
        auto elapsed = Clock::now() - _PtrToControl->TimedSharedPtrStartTime;
        if (elapsed > _PtrToControl->deletePtrInMs) {
            cout << "Yeo! Expired ";
            return nullptr;
        }
        return static_cast<T*>(_PtrToControl->ptr);
    }
};

#endif