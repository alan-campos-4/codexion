/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   take_dongle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:04:31 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/20 15:47:28 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int	    fill(t_hub *hub, int num);
void	create_threads(t_hub *hub);
void	join_threads(t_hub *hub);
int	    setup_program(t_hub *hub);
long	get_time(long start_time);
void	log_status(t_hub *hub, t_coder *coder, char *msg);
int	    stop_program(t_hub *hub);



static int	take_dongle(t_coder *coder, int side)
{
	t_dongle	*dongle;

	if (stop_program(coder->hub))
		return (0);
	if (side)
		dongle = coder->dongle_l;
	else
		dongle = coder->dongle_r;
	pthread_mutex_lock(&dongle->lock);
	if (!dongle->is_available)
	{
		pthread_mutex_unlock(&dongle->lock);
		usleep(1000);
		return (1);
	}
	else
	{
		dongle->is_available = 0;
		pthread_mutex_unlock(&dongle->lock);
		return (0);
	}
}

static int	release_dongle(t_coder *coder, int side)
{
	t_dongle	*dongle;

	if (stop_program(coder->hub))
		return (0);
	if (side)
		dongle = coder->dongle_l;
	else
		dongle = coder->dongle_r;
	pthread_mutex_lock(&dongle->lock);
	dongle->is_available = 1;
	pthread_mutex_unlock(&dongle->lock);
	return (0);
}

void    *coder_routine(void *arg)
{
    t_coder *coder;
    t_hub   *hub;

    coder = (t_coder *)arg;
    hub = coder->hub;
    while (!stop_program(hub))
	{
		while (take_dongle(coder, 1))
			;
        if (stop_program(hub))
		    break;
		while (take_dongle(coder, 0))
			;
        if (stop_program(hub))
		    break;
        log_status(hub, coder, "has taken the dongles");
        coder->n_compiles++;
        release_dongle(coder, 1);
        release_dongle(coder, 0);
        log_status(hub, coder, "releases the dongles");
        break;
    }
    return (NULL);
}










int main(void)
{
	t_hub		hub;
    t_coder     *coders;
    t_dongle    *dongles;

    if (fill(&hub, 5) == 1)
		return (1);
	if (setup_program(&hub) == 1)
		return (1);
	create_threads(&hub);
	join_threads(&hub);
    return (0);
}

int	fill(t_hub *hub, int num)
{
    hub->n_coders = 5;
	hub->time_burnout = 800;
	hub->time_compile = 200;
	hub->time_debug = 200;
	hub->time_refactor = 200;
	hub->n_compiles_required = 2;
	hub->dongle_cooldown = 20;
	hub->scheduler = FIFO;
    return (0);
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
	if (pthread_mutex_init(&hub->log_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->stop_lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&hub->request_lock, NULL) != 0)
		return (1);
	return (0);
}

long	get_time(long start_time)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time);
}

void	log_status(t_hub *hub, t_coder *coder, char *msg)
{
	long	time;

	time = get_time(0) - hub->start_time;
	if (time == -1)
		return ;
	pthread_mutex_lock(&hub->log_lock);
	if (stop_program(hub) && strcmp(msg, LOG_BURNOUT) != 1)
	{
		pthread_mutex_unlock(&hub->log_lock);
		return ;
	}
	printf("%ld %d [D%i, D%i] %s\n", time, coder->dongle_l->id,
        coder->dongle_r->id, coder->id, msg);
	pthread_mutex_unlock(&hub->log_lock);
}

int	stop_program(t_hub *hub)
{
	int	result;

	pthread_mutex_lock(&hub->stop_lock);
	result = hub->stop_flag;
	pthread_mutex_unlock(&hub->stop_lock);
	return (result);
}
