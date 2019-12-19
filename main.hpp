#pragma once
#include <iostream>
#include <thread>
#include <stdio.h>
#include "buzzer.hpp"

//Simulate key stroke
#ifdef WINDOWS
#include <windows.h>
#else
extern "C" {
    #include <xdo.h>
}
#endif

int main(int argc, char** argv);

void pressSpace();