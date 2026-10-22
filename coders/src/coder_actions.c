/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 16:33:11 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/22 15:18:17 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * The coder's compile action.
 * - Logs the action
 * - Updates completed_compiles and last_compile_time.
 * - Releases the dongles.
 */
static int	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = get_time(0);
	pthread_mutex_unlock(&coder->lock);
	if (log_status(coder->hub, coder->id, LOG_COMPILE))
		return (1);
	if (coder_sleep(coder, (int)coder->hub->compile_time))
		return (1);
	pthread_mutex_lock(&coder->lock);
	coder->completed_compiles += 1;
	pthread_mutex_unlock(&coder->lock);
	if (release_dongle(coder->first_dongle))
		return (1);
	if (release_dongle(coder->second_dongle))
		return (1);
	return (0);
}

/**
 * The coder's debug or refactor action.
 * - Logs the corresponding action.
 * - Sleeps for the corresponding amount of time.
 */
static int	coder_debug(t_coder *coder)
{
	if (log_status(coder->hub, coder->id, LOG_DEBUG))
		return (1);
	if (coder_sleep(coder, coder->hub->debug_time))
		return (1);
	return (0);
}

static int	coder_refactor(t_coder *coder)
{
	if (log_status(coder->hub, coder->id, LOG_REFACTOR))
		return (1);
	if (coder_sleep(coder, coder->hub->refactor_time))
		return (1);
	return (0);
}

/**
 * The coder's sleep action.
 * - Sleep for the given amount of time. Can stop at any moment.
 */
int	coder_sleep(t_coder *coder, long time)
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
			return (1);
		if (get_time(0) - start_time > time_to_wait)
			return (0);
		usleep(MS_TO_US);
	}
	return (0);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (wait_for_others(coder))
		return (NULL);
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_time = coder->hub->start_time;
	pthread_mutex_unlock(&coder->lock);
	if (coder->hub->n_coders == 1)
		return (single_coder(coder, coder->first_dongle));
	if (coder->id % 2 == 0 || coder->id == coder->hub->n_coders)
	{
		if (coder_sleep(coder, (coder->hub->compile_time) / 2))
			return (NULL);
	}
	while (1)
	{
		if (get_dongles(coder)
				|| coder_compile(coder)
				|| coder_debug(coder)
				|| coder_refactor(coder))
			break;
	}
	return (NULL);
}
