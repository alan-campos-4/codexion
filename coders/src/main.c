/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:34:23 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 14:10:55 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	create_monitor(pthread_t *monitor, t_coder *coders)
{
	if (!pthread_create(monitor, NULL, monitor_routine, coders))
		return (SUCCESS);
	pthread_mutex_lock(&coders->hub->sim_lock);
	coders->hub->stop_sim = 1;
	coders->hub->start_sim = 1;
	pthread_cond_broadcast(&coders->hub->cond);
	pthread_mutex_unlock(&coders->hub->sim_lock);
	return (FAIL);
}

int	codexion(t_coder *coders, t_dongle *dongles, t_hub *hub)
{
	pthread_t	*c_threads;
	pthread_t	monitor;
	int		monitor_status;

	c_threads = init_threads(coders);
	if (!c_threads)
	{
		cleanup_all(dongles, coders, hub);
		return (FAIL);
	}
	monitor_status = create_monitor(&monitor, coders);
	if (monitor_status == FAIL)
		wait_on_threads(c_threads, hub->n_coders);
	else
	{
		sim_start(coders);
		wait_on_threads(c_threads, hub->n_coders);
		pthread_join(monitor, NULL);
	}
	free(c_threads);
	cleanup_all(dongles, coders, hub);
	return (monitor_status);
}

int	main(int ac, char **av)
{
	t_hub		hub;
	t_coder		*coders;
	t_dongle	*dongles;
	int			init_status;

	if (ft_parser(ac, av, &hub))
	{
		fprintf(stderr, "Invalid arguments! example usage: ./codexion"
			" 5 500 100 100 100 10 60 edf\n");
		return (FAIL);
	}
	init_status = init_sim(&hub, &coders, &dongles);
	if (init_status == SUCCESS)
		return (codexion(coders, dongles, &hub));
	return (init_status);
}
