---
layout: default
title: "Project Software Architecture Overview"
nav_order: 3
has_children: true
---

# Project Architecture Overview

The project is built on a four-layer architecture designed to decouple the high-level application logic from the underlying hardware and the Real-Time Operating System (RTOS).

* **Application Layer:** Contains business logic and high-level tasks (e.g., hello_task) that remain entirely agnostic of the MCU and OS.

* **Service Layer:** Provides shared platform utilities, such as an asynchronous logging framework, that bridge the application and abstraction layers.

* **OSAL (Operating System Abstraction Layer):** A C++17 wrapper that provides a unified interface for RTOS primitives like threads, queues, and semaphores.

* **MCAL (Microcontroller Abstraction Layer):** Wraps NXP SDK drivers to handle low-level board initialization and peripheral management.


# The MCAL: Board Initialization

The MCAL layer serves as the entry point for hardware setup, ensuring that the Cortex-M7 core is properly configured before any application logic runs.

**Hardware Setup:** Manages the initialization of the MPU, clocks, and the debug console through the `Board::InitHardware()` method.

**SDK Isolation:** All NXP-specific includes, such as `pin_mux.h` and `clock_config.h`, are contained within the MCAL implementation to prevent hardware leakage into other layers.


# The OSAL: C++17 RTOS Abstraction

To ensure portability and safety, the OSAL transforms FreeRTOS into a type-safe, statically allocated C++ environment.

**Static Allocation Strategy:** The `StaticThread`, `StaticQueue`, and `StaticSemaphore` templates utilize pre-allocated memory buffers to avoid runtime heap fragmentation.

**FreeRTOS Integration:** The implementation maps C++ calls to FreeRTOS-specific functions like `xTaskCreateStatic` and `xQueueCreateStatic`.

**Memory Safety:** Control blocks (TCBs) and stack buffers are managed as private member arrays within the static classes.


# The Service Layer: Asynchronous Logger

A key achievement was moving from blocking PRINTF calls to a thread-safe, asynchronous logging service.

**Producer-Consumer Pattern:** Application tasks (producers) format messages and push them into a `StaticQueue`, while a dedicated low-priority task (consumer) handles the slow serial transmission.

**Agnostic Interface:** The service provides a single `Log(LogLevel, format, ...)` entry point that supports `printf`-style arguments and captured variable states.

**Routing Logic:** The logger can differentiate between `Info`, `Warning`, and `Error` levels, and is prepared for "Memory-Only" logging for high-speed events.


# Agnostic Application Logic
The final result is a `main.cpp` that acts solely as an integrator. 
The actual application tasks only interact with `Service::Logger` and `Osal::Thread`, making the code portable to any platform where the OSAL and Service Layer are implemented.


# Major Hurdles & Technical Lessons

## The Global Constructor Fiasco

Because the project uses a `-nostdlib` configuration in the makefile, global C++ objects were not being initialized automatically.

**The Problem:** Global objects like the `Logger` or `helloTask` remained as zero-initialized memory, causing `NULL` pointer exceptions when calling their methods.

**The Solution:** Explicitly calling `__libc_init_array()` at the start of `main()` ensured that the linker's .init_array section was processed, properly constructing all global objects before they were used.












