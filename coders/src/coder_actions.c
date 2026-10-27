/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 13:54:00 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * The coder's compile action.
 * - Logs the action
 * - Updates n_compiles and last_compile_time.
 * - Releases the dongles.
 */
static int	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = get_time(0);
	coder->n_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (log_status(coder->hub, coder->id, LOG_COMPILE))
		return (1);
	if (coder_do(coder, coder->hub->compile_time))
		return (1);
	//pthread_mutex_lock(&coder->lock);
	//coder->n_compiles += 1;
	//pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->dongle_l))
		return (1);
	if (release_dongle(coder->dongle_r))
		return (1);
	return (0);
}

/**
 * The coder's debug action.
 */
static int	coder_debug(t_coder *coder)
{
	if (log_status(coder->hub, coder->id, LOG_DEBUG))
		return (1);
	if (coder_do(coder, coder->hub->debug_time))
		return (1);
	return (0);
}

/**
 * The coder's debug action.
 */
static int	coder_refactor(t_coder *coder)
{
	if (log_status(coder->hub, coder->id, LOG_REFACTOR))
		return (1);
	if (coder_do(coder, coder->hub->refactor_time))
		return (1);
	return (0);
}

/* These functions are kept separate for better performance. :) */

/**
 * Make the coder perform the given action, i.e.: wait for the
 * given amount of time. Can stop at any moment.
 */
int	coder_do(t_coder *coder, long time)
{
	long	start_time;
	long	time_to_wait;

	if (!coder)
		return (1);
	start_time = get_time(0);
	time_to_wait = time * MS_TO_US;
	while (1)
	{
		if (stop_program(coder->hub))
		{
			log_status(coder->hub, coder->id, "stopped");
			return (1);
		}
		if (get_time(0) - start_time > time_to_wait)
			return (0);
		usleep(100);
	}
	return (0);
}

/**
 * The coders' threads routine.
 * - Wait for the other actors.
 * - Initialize last_compile_time.
 * - If a coder is in an even position, wait for a time to avoid a race.
 * - Perform all dongle actions in a loop until the program
 * is indicated to stop.
 */
void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_hub	*hub;

	coder = (t_coder *)arg;
	hub = coder->hub;
	if (wait_for_others(hub))
		return (NULL);
	if (hub->n_coders == 1)
		return (single_coder(coder, coder->dongle_l));
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = hub->start_time;
	pthread_mutex_unlock(&coder->lock);
	if (coder->id % 2 == 0 || coder->id == hub->n_coders)
	{
		if (coder_do(coder, (hub->compile_time) / 2))
			return (NULL);
	}
	while (1)
	{
		if (try_get_dongles(coder) || coder_compile(coder)
			|| coder_debug(coder) || coder_refactor(coder))
			break ;
	}
	return (NULL);
}
