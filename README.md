*This project has been created as part of the 42 curriculum by alvespin.*

---
# Codexion

## Description

This is a project .. POSIX Threads

It is an extension of the Dining Philosophers problem, first proposed by Djisktra...

#### Project Structure

#### Object Structure

Coder
```c
int					id;				// ID of the coder.
int					request_order;	// ...
int					n_compiles;		// Number of compiles done.
long				last_compile;	// Last time a compile was done.
long				priority_ms;	// ...
pthread_t			thread;			// Thread that runs the coder routine.
pthread_mutex_t		time_lock;		// Protects the time at which actions are executed.
t_dongle			*dongle_l;		// Dongle to the left of the coder.
t_dongle			*dongle_r;		// Dongle to the right of the coder.
t_hub				*hub;			// Pointer to the hub.
```

Dongle
```c
int					id;				// ID of the dongle.
int					is_available;	// 1 is the dongle is available, 0 if it's not
long				last_user;	    // Time at which the dongle was last used.
pthread_mutex_t		lock;			// .
pthread_cond_t		cond;			// .
t_heap				heap;			// .
```

Hub
```c
int					n_coders;				// Number of coders.
long				time_burnout;			// Time to burnout.
long				time_compile;			// Time to compile.
long				time_debug;				// Time to debug.
long				time_refactor;			// Time to refactor.
int					n_compiles_required;	// Number of compiles required of each coder.
long				dongle_cooldown;		// Dongle cooldown.
t_scheduler			scheduler;				// Type of scheduler (FIFO / EDF).
int					request_counter;
long				start_time;		// Time that the execution starts.
int					stop_flag;		// Shared flag to stop the execution.
pthread_mutex_t		request_lock;	// Protects request_counter.
pthread_mutex_t		stop_lock;		// Protects the stop flag.
pthread_mutex_t		log_lock;		// Protects the printf inside log_status.
t_dongle			*dongles;	// Array of dongles in the execution.
t_coder				*coders;	// Array of coders in the execution.
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

## Instructions

|

## Blocking cases handled

|

## Thread synchronization mechanisms

|

## Resources

- https://www.geeksforgeeks.org/c/multithreading-in-c/
- https://www.geeksforgeeks.org/operating-systems/reader-writer-problem-using-monitors-pthreads/
- https://www.doxygen.nl/manual/docblocks.html#cppblock
- https://gcc.gnu.org/onlinedocs/cpp/Object-like-Macros.html

- https://github.com/furkangny/Philosophers
- https://github.com/jleiva-g/philosophers/
- https://github.com/browser22nd/codexion
- https://github.com/SaraFreitas-dev/Codexion

