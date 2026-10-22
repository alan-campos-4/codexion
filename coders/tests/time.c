/* Program to demonstrate time taken by function fun() */
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void	wait_sleep(long time_in_ms)
{
    clock_t start;
    clock_t end;
    double  cpu_time_used;

    start = clock();
    usleep(time_in_ms * 1000);
    end = clock();
	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
	printf("%f miliseconds have passed\n", cpu_time_used);
}

int	main(void)
{
    wait_sleep(300);
    wait_sleep(800);
}