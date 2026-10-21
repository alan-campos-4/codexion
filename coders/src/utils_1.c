/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:43:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 17:10:29 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Get the time in miliseconds.
 * get_time(0) returns the current time.
 */
long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((((int)tv.tv_sec * 1000000) + tv.tv_usec) - start_time);
}
/*
//return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time);

int	get_curr_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (((int)time.tv_sec * 1000000) + time.tv_usec);
}*/

/**
 * Makes the coder sleep for a given amount of time. Can stop at any moment.
 */
int	better_sleep(t_coder *coder, int time)
{
	int	start_time;

	if (!coder)
		return (1);
	start_time = get_time(0);
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (get_time(0) - start_time > time)
			return (0);
		usleep(MS_IN_US);
	}
	return (0);
}

/**
 * Savely checks the program's stop flag, returning whether
 * the execution shoudl stop (1) or continue(0).
 */
int	stop_program(t_hub *hub)
{
	int	state;

	pthread_mutex_lock(&hub->stop_lock);
	state = hub->stop_flag;
	pthread_mutex_unlock(&hub->stop_lock);
	return (state);
}

int	log_status(char *str, t_coder *coder, t_hub *hub)
{
	int		id;
	long long	timestamp;

	id = coder->id;
	pthread_mutex_lock(&hub->log_lock);
	timestamp = (get_time(0) - hub->start_time) / MS_IN_US;
	if (stop_program(hub) && (strcmp(str, "has burned out")))
	{
		pthread_mutex_unlock(&hub->log_lock);
		return (1);
	}
	printf("%lld %i %s\n", timestamp, id, str);
	pthread_mutex_unlock(&hub->log_lock);
	return (0);
}
