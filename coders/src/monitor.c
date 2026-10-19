/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:35:37 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/19 11:59:47 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Check if the coder has reached burnout
static int	coder_is_in_burnout(t_hub *hub, t_coder *coder)
{
	//long	time_since_last_compile;
	long	last_compile;

	pthread_mutex_lock(&coder->time_lock);
	last_compile = coder->last_compile;
	pthread_mutex_unlock(&coder->time_lock);
	//time_since_last_compile = get_time(0) - last_compile;
	return (get_time(0) - last_compile >= (hub->time_burnout * 1000));
}

// Check if the coder has reached the number of compiles required
static int	coder_has_finished(t_hub *hub, t_coder *coder)
{
	int	compiled;

	pthread_mutex_lock(&coder->time_lock);
	compiled = coder->n_compiles;
	pthread_mutex_unlock(&coder->time_lock);
	return (compiled >= hub->n_compiles_required);
}

/*
Loop through every coder once: log and stop early if anyone is
in burnout, otherwise track whether all of them have reached the
required number of compiles.
*/
static int	check_all_coders(t_hub *hub)
{
	t_coder	*coder;
	int		i;
	int		all_finished;

	i = 0;
	all_finished = 1;
	while (i < hub->n_coders)
	{
		coder = &hub->coders[i];
		if (coder_is_in_burnout(hub, coder))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->stop_flag = 1;
			pthread_mutex_unlock(&hub->stop_lock);
			log_status(hub, coder, LOG_BURNOUT);
			return (1);
		}
		if (!coder_has_finished(hub, coder))
			all_finished = 0;
		i++;
	}
	return (all_finished);
}

/*
Monitor thread: repeatedly checks all coders until either a
burnout is detected or everyone has finished compiling — then
sets should_stop and exits.
*/
void	*monitor_routine(void *arg)
{
	t_hub	*hub;

	hub = (t_hub *)arg;
	while (1)
	{
		if (check_all_coders(hub))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->stop_flag = 1;
			pthread_mutex_unlock(&hub->stop_lock);
			return (NULL);
		}
		usleep(2000);
	}
}
