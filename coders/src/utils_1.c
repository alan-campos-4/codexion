/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:43:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:59:58 by alvespin         ###   ########.fr       */
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

int	better_sleep(t_coder *coder, int time)
{
	int	start_time;

	if (!coder)
		return (FAIL);
	start_time = get_curr_time();
	while (1)
	{
		if (check_stop_sim(coder->hub))
			return (FAIL);
		if (get_curr_time() - start_time > time)
			return (SUCCESS);
		usleep(MS_IN_US);
	}
	return (SUCCESS);
}

int	check_stop_sim(t_hub *hub)
{
	int	state;

	pthread_mutex_lock(&hub->sim_lock);
	state = hub->stop_sim;
	pthread_mutex_unlock(&hub->sim_lock);
	return (state);
}

int	print_log(char *str, t_coder *coder, t_hub *hub)
{
	int		id;
	long long	timestamp;

	id = coder->id;
	pthread_mutex_lock(&hub->print_lock);
	timestamp = (get_curr_time() - hub->start_time) / MS_IN_US;
	if (check_stop_sim(hub) && (strcmp(str, "has burned out")))
	{
		pthread_mutex_unlock(&hub->print_lock);
		return (FAIL);
	}
	printf("%lld %i %s\n", timestamp, id, str);
	pthread_mutex_unlock(&hub->print_lock);
	return (SUCCESS);
}

int	get_curr_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (((int)time.tv_sec * 1000000) + time.tv_usec);
}

void	wait_on_threads(pthread_t *ts, int n)
{
	int	i;

	i = 0;
	while (i < n)
	{
		pthread_join(ts[i], NULL);
		i++;
	}
}
