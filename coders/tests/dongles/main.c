/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/21 12:08:54 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 12:10:42 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dongles.h"


static int	init_dongles(t_hub *hub)
{
	int			i;
	t_dongle	*dongles;

	dongles = malloc(sizeof(t_dongle) * hub->n_coders);
	if (!dongles)
		return (1);
	i = 0;
	while (i < (hub->n_coders))
	{
		dongles[i].id = i + 1;
		dongles[i].is_available = 1;
		dongles[i].last_use = 0;
		if (pthread_mutex_init(&dongles[i].lock, NULL) != 0
			|| pthread_cond_init(&dongles[i].cond, NULL) != 0)
			return (1);
		i++;
	}
	hub->dongles = dongles;
	return (0);
}

/**
 * Initialize the compiler's coder array.
 */
static int	init_coders(t_hub *hub)
{
	int		i;
	t_coder	*coders;

	coders = malloc(sizeof(t_coder) * hub->n_coders);
	if (!coders)
		return (1);
	i = 0;
	while (i < (hub->n_coders))
	{
		coders[i].id = i + 1;
		coders[i].n_compiles = 0;
		coders[i].last_compile = hub->start_time;
		coders[i].hub = hub;
		coders[i].dongle_l = &hub->dongles[i];
		coders[i].dongle_r = (&hub->dongles[(i + 1) % hub->n_coders]);
		if (pthread_mutex_init(&coders[i].time_lock, NULL) != 0)
			return (1);
		i++;
	}
	hub->coders = coders;
	return (0);
}

int	setup_program(t_hub *hub)
{
	hub->start_time = get_time(0);
	hub->stop_flag = 0;
	hub->request_counter = 0;
	if (init_dongles(hub) == 1)
		return (1);
	if (init_coders(hub) == 1)
		return (1);
	if (pthread_mutex_init(&hub->lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->log_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->stop_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->request_lock, NULL) != 0)
		return (1);
	if (pthread_cond_init(&hub->cond, NULL) != 0)
		return (1);
	return (0);
}

int	fill(t_hub *hub, int num)
{
    hub->n_coders = num;
	hub->time_burnout = 800;
	hub->time_compile = 200;
	hub->time_debug = 200;
	hub->time_refactor = 200;
	hub->n_compiles_required = 2;
	hub->dongle_cooldown = 20;
	hub->scheduler = FIFO;
    return (0);
}

void	cleanup(t_hub *hub)
{
	int	i;

	i = 0;
	while (i < hub->n_coders)
	{
		free(hub->dongles[i].heap.requests);
		pthread_mutex_destroy(&hub->dongles[i].lock);
		pthread_cond_destroy(&hub->dongles[i].cond);
		pthread_mutex_destroy(&hub->coders[i].time_lock);
		i++;
	}
	pthread_mutex_destroy(&hub->log_lock);
	pthread_mutex_destroy(&hub->stop_lock);
	pthread_mutex_destroy(&hub->request_lock);
	free(hub->dongles);
	free(hub->coders);
}

void	create_threads(t_hub *hub)
{
	for (int i = 0; i < hub->n_coders; i++)
		pthread_create(&hub->coders[i].thread, NULL, coder_routine,
			&hub->coders[i]);
}

void	join_threads(t_hub *hub)
{
	for (int i = 0; i < hub->n_coders; i++)
		pthread_join(hub->coders[i].thread, NULL);
}

int main(int argc, char **argv)
{
	t_hub		hub;
    t_coder     *coders;
    t_dongle    *dongles;
	int			num;

	if (argc != 2)
		num = 5;
	else
		num = atoi(argv[1]);
    if (fill(&hub, num) == 1)
		return (1);
	if (setup_program(&hub) == 1)
		return (1);
	create_threads(&hub);
	sim_start(&hub);
	join_threads(&hub);
	cleanup(&hub);
    return (0);
}