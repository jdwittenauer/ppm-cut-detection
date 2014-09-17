/**
 * Windows Multimedia Timer class definition file.
 *
 * Provides basic timer functionality which calculates running time for certain
 * part of your program. Very useful for analyzing algorithm running time. 
 * After a WinTimer object is created, calling the Start() member function 
 * starts the timer running. The Stop() member function stops the timer. Once
 * the timer is stopped, Start() must be called again to reset the timer.
 * GetElapsedSeconds() returns the number of seconds elapsed. Calling 
 * GetElapsedSeconds() while the timer is still running gives you the elapsed 
 * time up to that point.
 *
 * Note: 
 * This WinTimer class implementation is based on Windows platform. This is a 
 * multimedia timer which provides resolution of about one to ten milliseconds. 
 */

#ifndef WIN_TIMER_H
#define WIN_TIMER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm")   // linked-in winmm.lib

class WinTimer
{
public:
    WinTimer();
    void Start();
    void Stop();
    double GetElapsedSeconds();

private:
    DWORD beginTime;
    DWORD endTime;
    bool isRunning;

    void SetTime(DWORD& time);
};

WinTimer::WinTimer()
    : isRunning(false)
{
}

void WinTimer::Start()
{
    isRunning = true;
    SetTime(beginTime);
}

void WinTimer::Stop()
{
    isRunning = false;
    SetTime(endTime);
}

double WinTimer::GetElapsedSeconds()
{
    if (isRunning)
        SetTime(endTime);

    return (endTime - beginTime) / 1000.0;
}

void WinTimer::SetTime(DWORD& time)
{
    time = timeGetTime();
}

#endif