*This project has been created as part of the 42 curriculum by alvespin.*

# Codexion

## Table of Contents

1. [Description](#description)
   - [Arguments](#arguments)
   - [Schedulers](#schedulers)
   - [Time Management](#time-management)
   - [Project Structure](#project-structure)
   - [Object Structure](#object-structure)
   - [Libraries](#libraries)
   - [Macros](#macros)
2. [Intructions](#instructions)
   - [Usage](#usage)
   - [Example](#examples)
3. [Blocking cases handled](#blocking-cases-handled)
4. [Thread synchronization mechanisms](#thread-synchronization-mechanisms)
5. [Resources](#resources)
   - [Documentation](#documentation)
   - [Guides](#guides)
   - [Projects](#projects)
   - [AI](#ai)

---

## Description

This is a project is about managing POSIX Threads and the values they contain.

It is an extension of the Dining Philosophers problem, first proposed by Djisktra to ... .
In this case we have a number of 'coders' with the same number of 'dongles' that need them in order to compile, or they will burnout.

All timestamps values and the variables that store them are declared with the `long` type.

#### Arguments

|Name|Description|Value|
|--|--|--|
| `number_of_coders`| Number of coders and dongles in the program. | integer >= 1 |
| `time_to_burnout`| Time it takes for a coder to burnout. | long integer >= 1 |
| `time_to_compile`| Time it takes for a coder to compile. | long integer >= 1 |
| `time_to_debug`| Time it takes for a coder to debug. | long >= 1 |
| `time_to_refactor`| Time it takes for a coder to refactor. | long >= 1 |
| `dongle_cooldown`| Time it takes for a dongle to become available after use. | long >= 1 |
| `number_of_compiles_required`| Amount of times a coder must compile before the program ends. | integer >= 1 |
| `scheduler`| Type of scheduler used to distribute dongles. | 'fifo' / 'edf' |

#### Schedulers

- **FIFO (First In, First Out)**: the dongle will be given to the coder that asked for it first.
- **EDF (Earliest Deadline First)**: the dongle will be given to the coder with the shortest deadline to burnout (last_compile +
time_to_burnout).

##### Time Management

All timestamps values printed in the log and arguments above are in miliseconds (ms). However, the sleep function used an timestamps created during runtime are in microseconds (us). This requires some attention:

1 ms   = 1000 us
500 ms = 500000 us
1 s

In order for the coders to not burn out this rule must be obbeyd:
- `time_to_burnout` > (`time_to_compile` + `time_to_debug` + `time_to_refactor` + `dongle_cooldown`)

The bigger the difference the least likely burnout becomes.

#### Project Structure

```
codexion
    coders/
        include/
        src/
    README.md
```

#### Object Structure

Coder
```ini
int			id;				// ID of the coder.
int			request_order;	// ...
int			n_compiles;		// Number of compiles done.
long			last_compile_time;	// Last time a compile was done.
long			priority_ms;	// ...
pthread_t		thread;			// Thread that runs the coder routine.
pthread_mutex_t		time_lock;		// Protects the time at which actions are executed.
t_dongle		*dongle_l;		// Dongle to the left of the coder.
t_dongle		*dongle_r;		// Dongle to the right of the coder.
t_hub			*hub;			// Pointer to the hub.
```

Dongle
```c
int			id;				// ID of the dongle.
int			is_available;	// 1 is the dongle is available, 0 if it's not
long			last_used_time;	    // Time at which the dongle was last used.
pthread_mutex_t		lock;			// ...
pthread_cond_t		cond;			// ...
t_heap			heap;			// ...
```

Hub
```c
int			n_coders;				// Number of coders.
long			time_burnout;			// Time to burnout.
long			time_compile;			// Time to compile.
long			time_debug;				// Time to debug.
long			time_refactor;			// Time to refactor.
int			n_compiles_required;	// Number of compiles required of each coder.
long			dongle_cooldown;		// Dongle cooldown.
t_scheduler		scheduler;				// Type of scheduler (FIFO / EDF).
int			request_counter;    // ...
long			start_time;		// Time that the execution starts.
int					stop_flag;		// Shared flag to stop the execution.
pthread_mutex_t		request_lock;	// Protects request_counter.
pthread_mutex_t		stop_lock;		// Protects the stop flag.
pthread_mutex_t		log_lock;		// Protects the printf inside log_status.
t_dongle		*dongles;	// Array of dongles in the execution.
t_coder			*coders;	// Array of coders in the execution.
```

#### Libraries
```c
# include <pthread.h>   // for POSIX threads
# include <stdio.h>		// for printf()
# include <stdlib.h>	// for atoi(), atol(), NULL
# include <string.h>	// for strcmp()
# include <unistd.h>	// for usleep()
# include <sys/time.h>	// for struct timeval
```

#### Macros
```c
// Miliseconds to microseconds
//  used because our timestamp values are in miliseconds and usleep() deals with microseconds.
# define MS_TO_US 1000

// Messages used for logging the state of each coder.
# define LOG_DONGLE "has taken a dongle"
# define LOG_COMPILE "is compiling"
# define LOG_DEBUG "is debugging"
# define LOG_REFACTOR "is refactoring"
# define LOG_BURNOUT "burned out"
```

## Instructions

#### Usage

In order to create the executable, run this on the coders/ directory.
```bash
make
```

To run the executable, you will need ro provide the all the arguments described:
```bash
./codexion <n_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_cooldown> <n_compiles> <time_to_refactor> <scheduler>
```

#### Examples
```bash
./codexion 5 340 100 100 10 10 10 edf   # Stress test for edf
```

## Blocking cases handled

|

## Thread synchronization mechanisms

|

## Resources

#### Documentation

- https://www.doxygen.nl/manual/docblocks.html#cppblock
- https://gcc.gnu.org/onlinedocs/cpp/Object-like-Macros.html
- https://www.man7.org/linux/man-pages/man3/usleep.3.html
- https://en.cppreference.com/c/chrono/clock_t

#### Guides

- https://www.geeksforgeeks.org/c/multithreading-in-c/
- https://www.geeksforgeeks.org/operating-systems/reader-writer-problem-using-monitors-pthreads/
- https://www.geeksforgeeks.org/cpp/how-to-measure-time-taken-by-a-program-in-c/

#### Projects

- https://github.com/furkangny/Philosophers
- https://github.com/jleiva-g/philosophers/
- https://github.com/browser22nd/codexion
- https://github.com/SaraFreitas-dev/Codexion

#### AI

The AI chatbot Claude was used incedentally to resolve and explain especific errors and unwanted results. No code was provided.
