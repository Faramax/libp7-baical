# libP7-Baical

## Overview

Open source, cross-platform, fastest library for sending logs, telemetry & trace data from your application. (C/C++/C#/Python)

## Features
 * C++/C/C#/Python support
 * Cross platform (Linux x86/x64, Windows x86/x64)
 * Small memory footprint (optional, min is 16KB) - used for embedded devices
 * Speed is priority, library designed to suit high load, for example average performance for Intel i7-870 is
   * 350 000 traces per second - 0,5% CPU, max ~3.5 million traces per second to network, ~10 million traces per second to file
   * 400 000 telemetry samples per second - 0,5% CPU, max ~3.8 million samples per second to network, ~11 million samples per second to file
 * Thread safe
 * Unicode support (UTF-8, UTF-32 for Linux, UTF-16 for Windows)
 * ANSI char support
 * No external dependencies
 * High-resolution time stamps (resolution depends on HW high-resolution performance counter, usually it is 100ns)
 * Different sinks (transport & storages) are supported:
   * Network (Baical server)
   * Binary File
   * Text File (Linux: UTF-8, Windows: UTF-16)
   * Console
   * Syslog (RFC 5424)
   * Auto (Baical server if reachable, else - binary file)
   * Null
 * Files rotation setting (by size or time)
 * Files max count setting (deleting old files automatically)
 * Remote management from Baical server (set verbosity per module, enable/disable telemetry counters)
 * Shared memory is used - create your trace and telemetry channels once and access it from any process module or class without passing handles
 * Crash handler or in case of user defined crash handler - special function to flush all P7 buffers for all P7 objects inside process in case of crush
 * Trace & telemetry files have compact binary format (due to speed requirements - binary files much more compact than raw text), export to text is available
 * Command line interface for configuration may be used in addition to application parameters
