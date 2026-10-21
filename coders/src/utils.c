/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:43:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 12:26:05 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Get the time in miliseconds.
 * get_time(0) returns the current time.
 * get_time(x) returns the difference between the current time and x.
 */
long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time);
}

// Get the current timestamp in milliseconds to use on the report
long	get_curr_timestamp(t_hub *hub)
{
	long	elapsed_ms;
	long	now_ms;

	if (!hub)
	{
		fprintf(stderr, "ERROR: Cannot compute timestamp - hubation "
			"data is missing.\n");
		return (-1);
	}
	now_ms = get_time(0);
	elapsed_ms = now_ms - hub->start_time;
	return (elapsed_ms);
}

/*
Print a timestamped event while serializing access to stdout.
Normal events are ignored once the hubation has stopped.
// Print by each event: timestamp_in_ms X has taken a dongle
// Lock/unlock the mutex to prevent threads from printing at the same time
*/
void	log_status(t_hub *hub, t_coder *coder, char *msg)
{
	long	time;

	time = get_time(0) - hub->start_time;
	if (time == -1)
		return ;
	pthread_mutex_lock(&hub->log_lock);
	if (stop_program(hub) && strcmp(msg, LOG_BURNOUT) != 1)
	{
		pthread_mutex_unlock(&hub->log_lock);
		return ;
	}
	printf("%ld %d %s\n", time, coder->id, msg);
	pthread_mutex_unlock(&hub->log_lock);
}

/*
Safely checks whether the hub has been told to stop.
Uses stop_lock so the shared should_stop flag can be read without
racing with the monitor thread that may update it.
*/
/**
 * Safely checks the hub's stop_flag with its dedicated mutex.
 */
int	stop_program(t_hub *hub)
{
	int	result;

	pthread_mutex_lock(&hub->stop_lock);
	result = hub->stop_flag;
	pthread_mutex_unlock(&hub->stop_lock);
	return (result);
}
