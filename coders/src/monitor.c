/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:35:37 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 15:44:04 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Check if the coder has finished all its compiles. 
 */
static int	is_coder_finished(t_hub *hub, t_coder *coder)
{
	int	compiled;

	pthread_mutex_lock(&coder->lock);
	compiled = coder->n_compiles;
	pthread_mutex_unlock(&coder->lock);
	return (compiled >= hub->n_compiles_required);
}

/**
 * Check that a coder has not burnt out.
 * - If the time past since last_compile_time is smaller than burnout_time,
 * 	go on. If it is not set, check with start_time.
 * - If it's not activate the stop flag, log the burnout and wake_up,
 */
static int	is_coder_burntout(t_hub *hub, int id, int last_compile_time)
{
	if (last_compile_time)
	{
		if (get_time(last_compile_time) < hub->burnout_time)
			return (0);
	}
	else
	{
		if (get_time(hub->start_time) < hub->burnout_time)
			return (0);
	}
	log_status(hub, id, LOG_BURNOUT);
	return (1);
}

/**
 * Loop through every coder once: log and stop early if anyone is
 * in burnout, otherwise track whether all of them have reached the
 * required number of compiles.
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
		if (is_coder_burntout(hub, i + 1, coder->last_compile_time))
			return (1);
		if (!is_coder_finished(hub, coder))
			all_finished = 0;
		i++;
	}
	return (all_finished);
}

/**
 * The monitor's thread routine.
 * - Wait for the other actors.
 * - Check a coder's not burnt out.
 * - Check a coder's has done all its compiles.
 * - If not, reset the i and all_done variables
 */
void	*monitor_routine(void *arg)
{
	t_hub	*hub;

	hub = (t_hub *)arg;
	if (wait_for_others(hub))
		return (NULL);
	while (1)
	{
		if (check_all_coders(hub))
		{
			pthread_mutex_lock(&hub->stop_lock);
			hub->stop_flag = 1;
			pthread_mutex_unlock(&hub->stop_lock);
			wakeup_coders(hub->coders);
			return (NULL);
		}
		usleep(2000);
	}
	return (NULL);
}
