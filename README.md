*This project has been created as part of the 42 curriculum by alvespin.*

---
# Codexion

## Description

This is a project .. POSIX Threads

It is an extension of the Dining Philosophers problem, first proposed by Djisktra...

#### Project Structure

#### Object Structure

#### Libraries
```c
# include <pthread.h>   // for POSIX threads
# include <stdio.h>	// for printf()
# include <stdlib.h>	// for atoi(), atol(), NULL
# include <string.h>	// for strcmp()
# include <unistd.h>	// for usleep()
# include <sys/time.h>	// for struct timeval

typedef struct s_program
{
	int			n_coders;
	size_t			time_burnout;
	size_t			time_compile;
	size_t			time_debug;
	size_t			time_refactor;
	int			compiles_target;
	size_t			dongle_cooldown;
	int			request_counter; // Global source for request_order
	char			*scheduler;
	long			start_time_ms;
	int			stop_flag; // Shared flag
	t_dongle		*dongles;
	t_coder			*coders;
	pthread_mutex_t		request_lock; // Protects request_counter
	pthread_mutex_t		stop_lock; // Protects the should_stop bool flag
	pthread_mutex_t		log_lock; // Protects the printf inside print_log
}
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

