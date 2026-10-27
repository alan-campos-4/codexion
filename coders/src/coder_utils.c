/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:56:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 14:01:40 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	coder_do(coder, coder->hub->burnout_time);
	return (NULL);
}
