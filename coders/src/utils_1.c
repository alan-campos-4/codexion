/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:43:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 13:10:15 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Get the time in microseconds.
 * get_time(0) returns the current time.
 */
long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((((int)tv.tv_sec * 1000000) + tv.tv_usec) - start_time);
}
/*
return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time);
return (((int)time.tv_sec * 1000000) + time.tv_usec);
}*/

/**
 * Savely checks the program's stop flag, returning whether
 * the execution should stop (1) or continue (0).
 */
int	stop_program(t_hub *hub)
{
	int	state;

	pthread_mutex_lock(&hub->stop_lock);
	state = hub->stop_flag;
	pthread_mutex_unlock(&hub->stop_lock);
	return (state);
}

/**
 * Prints the status of a coder in the current timestamp.
 */
int	log_status(t_hub *hub, int id, char *str)
{
	long long	timestamp_in_ms;

	pthread_mutex_lock(&hub->log_lock);
	timestamp_in_ms = (get_time(0) - hub->start_time) / MS_TO_US;
	if (stop_program(hub))
	{
		printf("%lld %i %s (%i)\n",
				timestamp_in_ms, id, "stopped...", hub->stop_flag);
		pthread_mutex_unlock(&hub->log_lock);
		return (1);
	}
	printf("%lld %i %s (%i)\n", timestamp_in_ms, id, str, hub->stop_flag);
	pthread_mutex_unlock(&hub->log_lock);
	return (0);
}

/**
 * Return a value for the coder depending on the sceduler:
 * - for EDF, the last_compile_time + burnout_time.
 * - for FIFO, the current time.
 * This value is used by the coders when trying to obtain dongles,
 * the smaller value will be placed at the top of the request heap.
 */
int	get_request_value(t_coder *coder)
{
	if (coder->hub->scheduler == EDF)
		return (coder->last_compile_time
			+ (coder->hub->burnout_time * MS_TO_US));
	else
		return (get_time(0));
}

/**
 * Swap two requests.
 * Used by the heap functions.
 */
void	swap(t_request *x, t_request *y)
{
	t_request	tmp;

	tmp = *y;
	*y = *x;
	*x = tmp;
}
