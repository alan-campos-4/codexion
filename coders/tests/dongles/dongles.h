/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 17:38:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 12:11:55 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

typedef enum s_scheduler
{
	FIFO,
	EDF,
}						t_scheduler;

typedef struct s_request
{
	int					id;
	int					metric;
}						t_request;

typedef struct s_heap
{
	t_request			requests[2];
	int					size;
}						t_heap;

typedef struct s_hub	t_hub;
typedef struct s_coder	t_coder;

typedef struct dongle
{
	int					id;
	int					is_available;
	long				last_use;
	pthread_mutex_t		lock;
	pthread_cond_t		cond;
	t_heap				heap;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	int					request_order;
	int					n_compiles;
	long				last_compile;
	long				priority_ms;
	pthread_t			thread;
	pthread_mutex_t		lock;
	pthread_mutex_t		time_lock;
	t_dongle			*dongle_l;
	t_dongle			*dongle_r;
	t_hub				*hub;
}						t_coder;

typedef struct s_hub
{
	int					n_coders;
	long				time_burnout;
	long				time_compile;
	long				time_debug;
	long				time_refactor;
	int					n_compiles_required;
	long				dongle_cooldown;
	t_scheduler			scheduler;
	int					request_counter;
	long				start_time;
	int					start_flag;
	int					stop_flag;
	int					ready_count;
	pthread_mutex_t		lock;
	pthread_mutex_t		request_lock;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
	pthread_cond_t		cond;
	t_dongle			*dongles;
	t_coder				*coders;
}						t_hub;

long	get_time(long start_time);
void	log_status(t_hub *hub, t_coder *coder, char *msg);
int	    stop_program(t_hub *hub);

void    *coder_routine(void *arg);
void	sim_start(t_hub *hub);

void	pre_register_heaps(t_dongle *first, t_dongle *second, t_coder *coder,
		int metric);
int		pick_metric(t_coder *coder);
void	remove_request_top(t_heap *heap);

void	cleanup(t_hub *hub);
