/* Program to demonstrate time taken by function fun() */
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - start_time);
}

int try(long last_used)
{
	return (get_time(last_used) >= 20);
}

int	main(void)
{
    for (int i=0; i<5; i++)
    {
        printf("- %ld \n", get_time(0));
        usleep(350 * 1000);
    }
    long start_time = get_time(0);
    for (int i=0; i<5; i++)
    {
        printf("- %ld \n", get_time(start_time));
        usleep(350 * 1000);
    }
    /*
    long    start_time;
    long    last_used;

    start_time = get_time(0);
    printf(" - %ld \n", get_time(start_time));
    usleep(300 * 1000);
    printf(" - %ld \n", get_time(start_time));
    last_used = get_time(0);
    while (1)
    {
        if (try(last_used))
            break ;
        usleep(1000);
    }
    printf(" - %ld \n", get_time(last_used));
    */
}
