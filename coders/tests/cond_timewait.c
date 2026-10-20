/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cond_timewait.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:04:37 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/20 15:04:37 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

pthread_mutex_t fakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fakeCond = PTHREAD_COND_INITIALIZER;

void mywait(int timeInMs)
{
    struct timespec timeToWait;
    struct timeval now;
    int rt;

    gettimeofday(&now,NULL);

    timeToWait.tv_sec = now.tv_sec+5;
    timeToWait.tv_nsec = (now.tv_usec+1000UL*timeInMs)*1000UL;

    pthread_mutex_lock(&fakeMutex);
    rt = pthread_cond_timedwait(&fakeCond, &fakeMutex, &timeToWait);
    pthread_mutex_unlock(&fakeMutex);
    printf("\nDone\n");
}

void* fun(void* arg)
{
    printf("\nIn thread\n");
    mywait(10000);
    return (NULL);
}

int main()
{
    pthread_t   *thread;
    void        *ret;
    int         max;

    max = 5;
    thread = malloc(sizeof(pthread_t) * max);
    for (int i = 0; i < max; i++)
        pthread_create(&thread[i], NULL, fun, NULL);
    for (int i = 0; i < max; i++)
        pthread_join(thread[i], &ret);
    free(thread);
}