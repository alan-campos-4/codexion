/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 13:56:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:57:16 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	dongle->last_use_time = get_time(0);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

void	*single_coder(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_status("has taken dongle", coder, coder->hub);
	coder_sleep(coder, (int)coder->hub->burnout_time * MS_TO_US);
	return (NULL);
}