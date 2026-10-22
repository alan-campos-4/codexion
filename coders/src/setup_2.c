/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 14:01:58 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 17:05:16 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_dongle	*init_dongles(int n)
{
	t_dongle	*dongles;
	int		i;

	dongles = malloc(sizeof(t_dongle) * n);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < n)
	{
		(dongles + i)->heap.size = 0;
		(dongles + i)->is_available = 1;
		(dongles + i)->last_use_time = 0;
		if (pthread_mutex_init(&(dongles + i)->lock, NULL))
			return (dongle_cleanup(dongles, n, i, i));
		if (pthread_cond_init(&(dongles + i)->cond, NULL))
			return (dongle_cleanup(dongles, n, i + 1, i));
		i++;
	}
	return (dongles);
}

t_coder	*init_coders(int n)
{
	t_coder	*coders;
	int	i;

	coders = malloc(sizeof(t_coder) * n);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < n)
	{
		(coders + i)->id = i + 1;
		(coders + i)->completed_compiles = 0;
		(coders + i)->last_compile_time = 0;
		if (pthread_mutex_init(&(coders + i)->lock, NULL))
		{
			while (i-- > 0)
				pthread_mutex_destroy(&(coders + i)->lock);
			free(coders);
			return (NULL);
		}
		i++;
	}
	return (coders);
}

int	init_hub(t_hub *hub)
{
	if (!hub)
		return (1);
	hub->stop_flag = 0;
	hub->start_flag = 0;
	hub->ready_count = 0;
	if (pthread_mutex_init(&hub->stop_lock, NULL))
		return (hub_cleanup(hub, 0, 0, 0));
	if (pthread_mutex_init(&hub->log_lock, NULL))
		return (hub_cleanup(hub, 0, 0, 0));
	else if (pthread_mutex_init(&hub->lock, NULL))
		return (hub_cleanup(hub, 0, 0, 1));
	else if (pthread_cond_init(&hub->cond, NULL))
		return (hub_cleanup(hub, 0, 1, 1));
	else
		return (0);
}