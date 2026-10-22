/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:56:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/22 15:13:03 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Performed at the start of the coder and monitor routines.
 * Indicates the actor performing it is ready and loops cond_wait
 *  until all other actors in the program are ready.
 */
int	wait_for_others(t_coder *coder)
{
	pthread_mutex_lock(&coder->hub->lock);
	coder->hub->ready_count++;
	while (!coder->hub->start_flag)
		pthread_cond_wait(&coder->hub->cond, &coder->hub->lock);
	pthread_mutex_unlock(&coder->hub->lock);
	return (0);
}

/**
 * The coder's release dongle action.
 * - Marks the dongle as used and available.
 * - Broadcasts the condition.
 */
int	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	dongle->last_use_time = get_time(0);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

/**
 * Special action if the program only has one coder.
 * - ...
 */
void	*single_coder(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_status(coder->hub, coder->id, LOG_DONGLE);
	coder_sleep(coder, coder->hub->burnout_time);
	return (NULL);
}