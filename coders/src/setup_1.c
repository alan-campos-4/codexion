/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:44:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/26 17:16:36 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Initialize the coder's threads.
 * Delete what has been assigned if anything goes wrong.
 */
pthread_t	*init_threads(t_coder *coders)
{
	pthread_t	*t;
	int		i;

	t = malloc(sizeof(pthread_t) * coders->hub->n_coders);
	if (!t)
		return (NULL);
	i = 0;
	while (i < coders->hub->n_coders)
	{
		if (pthread_create(t + i, NULL, &coder_routine, coders + i))
		{
			pthread_mutex_lock(&coders->hub->lock);
			coders->hub->start_flag = 1;
			coders->hub->stop_flag = 1;
			pthread_mutex_unlock(&coders->hub->lock);
			wakeup_coders(coders);
			while (i-- > 0)
				pthread_join(t[i], NULL);
			free(t);
			return (NULL);
		}
		i++;
	}
	return (t);
}

/**
 * Initialize the dongle array.
 * Delete what has been assigned if anything goes wrong.
 */
t_dongle	*init_dongles(int n)
{
	t_dongle	*dongles;
	int			i;

	dongles = malloc(sizeof(t_dongle) * n);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dongles[i].heap.size = 0;
		dongles[i].is_available = 1;
		dongles[i].last_use_time = 0;
		if (pthread_mutex_init(&dongles[i].lock, NULL))
			return (cleanup_dongles(dongles, n, i, i), NULL);
		if (pthread_cond_init(&dongles[i].cond, NULL))
			return (cleanup_dongles(dongles, n, i + 1, i), NULL);
		i++;
	}
	return (dongles);
}

/**
 * Initialize the coder array.
 * Delete what has been assigned if anything goes wrong.
 */
t_coder	*init_coders(int n)
{
	t_coder	*coders;
	int		i;

	coders = malloc(sizeof(t_coder) * n);
	if (!coders)
		return (NULL);
	i = 0;
	while (i < n)
	{
		coders[i].id = i + 1;
		coders[i].n_compiles = 0;
		coders[i].last_compile_time = 0;
		if (pthread_mutex_init(&coders[i].lock, NULL))
			return (cleanup_coders(coders, n, i), NULL);
		i++;
	}
	return (coders);
}

/**
 * Initialize the hub attributes that are not obtained from the
 * arguments, deleting what has been assigned if anything goes wrong.
 * Please the cleanup_hub() docstring for more info.
 */
int	init_hub(t_hub *hub)
{
	int	stop_lock_flag;
	int	log_lock_flag;
	int	lock_flag;

	hub->stop_flag = 0;
	hub->start_flag = 0;
	hub->ready_count = 0;
	stop_lock_flag = 0;
	log_lock_flag = 0;
	lock_flag = 0;
	if (pthread_mutex_init(&hub->stop_lock, NULL))
		return (1);
	if (pthread_mutex_init(&hub->log_lock, NULL))
		stop_lock_flag = 1;
	if (pthread_mutex_init(&hub->lock, NULL))
		log_lock_flag = 1;
	if (pthread_cond_init(&hub->cond, NULL))
		log_lock_flag = 1;
	if (stop_lock_flag || log_lock_flag || lock_flag)
		return (cleanup_hub(hub, (lock_flag * 4 + log_lock_flag * 2
				+ stop_lock_flag * 1)));
	return (0);
}

/**
 * Execute after parsing the program arguments.
 * Set up all the variables and attributes that are not obtained
 * fromt the command line.
 */
int	setup_program(t_hub *hub, t_coder *coders, t_dongle *dongles)
{
	int	i;

	if (!hub || !coders || !dongles)
		return (1);
	if (init_hub(hub))
		return (1);
	i = 0;
	hub->coders = coders;
	while (i < hub->n_coders)
	{
		coders[i].hub = hub;
		coders[i].dongle_l = &dongles[i];
		coders[i].dongle_r = (&dongles[(i + 1) % hub->n_coders]);
		i++;
	}
	return (0);
}




/*
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
		coders[i].last_compile_time = hub->start_time;
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
	hub->start_flag = 0;
	hub->stop_flag = 0;
	hub->ready_count = 0;
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
}*/

/*
int	init_coders(t_hub *hub, int size)
{
	t_coder	*coders;
	int		i;

	i = 0;
	coders = malloc(sizeof(t_coder) * size);
	if (!coders)
		return (1);
	while (i < size)
	{
		coders[i].id = i;
		coders[i].dongle_l = NULL;
		coders[i].dongle_r = NULL;
		coders[i].n_compiles = 0;
		coders[i].last_compile_time = 0;
		pthread_mutex_init(coders[i].lock_l, NULL);
		pthread_mutex_init(coders[i].lock_r, NULL);
		i++;
	}
	hub->coders = coders;
	return (0);
}

int	init_dongles(t_hub *hub, int size)
{
	t_dongle	*dongles;
	int			i;

	i = 0;
	dongles = malloc(sizeof(t_dongle) * size);
	if (!dongles)
		return (1);
	while (i < size)
	{
		dongles[i].is_available = 1;
		pthread_mutex_init(dongles[i].lock, NULL);
		i++;
	}
	hub->dongles = dongles;
	return (0);
}

int	parse_hub(int num_hub, char **argv)
{
	int	i;
	int	j;

	i = 1;
	while (i < num_hub)
	{
		j = 0;
		while (argv[i][j])
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

int	init(t_hub *hub, int argc, char **argv)
{
	if (parse_hub(argc - 1, argv))
		return (1);
	hub->n_coders = atoi(argv[1]);
	hub->time_burnout = atoi(argv[2]);
	hub->time_compile = atoi(argv[3]);
	hub->time_debug = atoi(argv[4]);
	hub->time_refactor = atoi(argv[5]);
	hub->compiles_target = atoi(argv[6]);
	hub->dongle_cooldown = atoi(argv[7]);
	if (!strcmp(argv[8], "fifo"))
		hub->scheduler = FIFO;
	else if (!strcmp(argv[8], "edf"))
		hub->scheduler = EDF;
	else
		return (1);
	init_coders(hub, hub->n_coders);
	init_dongles(hub, hub->n_coders);
	return (0);
}*/
