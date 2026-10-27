/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:56:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 15:54:57 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Make the coder perform the given action, i.e. wait for the
 * given amount of time.
 */
int	coder_do(long time)
{
	//long	start_time;

	/*start_time = get_time(0);
	while (1)
	{
		if (stop_program(coder->hub))
			return (1);
		if (get_time(start_time) > time)
			return (0);
		usleep(0);
	}*/
	usleep(time * 1000);
	return (0);
}

/**
 * The coder's release dongle action.
 * - Mark the dongle as used and available.
 * - Broadcast the condition.
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
 * This means that there is only one dongle, therefore
 * the normal algorithm will not work.
 * This lone coder is made to sleep until they burnout, effectively
 * ending the program.
 */
void	*single_coder(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_status(coder->hub, coder->id, LOG_DONGLE);
	coder_do(coder->hub->burnout_time);
	return (NULL);
}
