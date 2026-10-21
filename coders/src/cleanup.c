/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:06:45 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 17:07:59 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	*dongle_cleanup(t_dongle *dongle, int32_t n,
			int32_t m_clean, int32_t cond_clean)
{
	int32_t	i;

	if (!dongle)
		return (NULL);
	i = 0;
	while (i < n)
	{
		if (m_clean-- > 0)
			pthread_mutex_destroy(&(dongle + i)->lock);
		if (cond_clean-- > 0)
			pthread_cond_destroy(&(dongle + i)->cond);
		i++;
	}
	free(dongle);
	return (NULL);
}

static void	coder_cleanup(t_coder *coders, t_hub *hub)
{
	int32_t	i;

	if (!coders || !hub)
		return ;
	i = 0;
	while (i < hub->n_coders)
	{
		pthread_mutex_destroy(&coders[i].lock);
		i++;
	}
	free(coders);
}

int32_t	hub_cleanup(t_hub *hub, int32_t c,
			int32_t s_lock, int32_t p_lock)
{
	if (!hub)
		return (1);
	if (c)
		pthread_cond_destroy(&hub->cond);
	if (s_lock)
		pthread_mutex_destroy(&hub->lock);
	if (p_lock)
		pthread_mutex_destroy(&hub->log_lock);
	pthread_mutex_destroy(&hub->stop_lock);
	return (1);
}

int32_t	cleanup_all(t_dongle *dongles, t_coder *coders,
		t_hub *hub)
{
	int32_t	n;

	n = 0;
	if (hub)
		n = hub->n_coders;
	dongle_cleanup(dongles, n, n, n);
	coder_cleanup(coders, hub);
	hub_cleanup(hub, 1, 1, 1);
	return (0);
}