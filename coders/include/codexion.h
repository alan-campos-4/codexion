/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:43 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/20 17:33:34 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define LOG_DONGLE_L "has taken a dongle to its left"
# define LOG_DONGLE_R "has taken a dongle to its right"
# define LOG_COMPILE "is compiling"
# define LOG_DEBUG "is debugging"
# define LOG_REFACTOR "is refactroing"
# define LOG_BURNOUT "burned out"

typedef struct s_hub	t_hub;
typedef struct s_coder	t_coder;

typedef enum s_scheduler
{
	FIFO,
	EDF,
}						t_scheduler;

typedef struct s_heap
{
	t_coder				**data;
	int					size;
}						t_heap;

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
	int					stop_flag;
	pthread_mutex_t		request_lock;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
	t_dongle			*dongles;
	t_coder				*coders;
}						t_hub;

/*** coder.c ***/
void					*coder_routine(void *arg);
void					assign_request_order(t_hub *hub, t_coder *coder);

/*** monitor.c ***/
void					*monitor_routine(void *arg);

/*** dongles.c ***/
int						take_both_dongles(t_hub *hub, t_coder *coder);
void					release_both_dongle(t_coder *coder);

/*** heap.c ***/
int						min_heap_push(t_hub *hub, t_heap *heap,
							t_coder *new_coder);
t_coder					*min_heap_pop(t_heap *heap, t_hub *hub);

void					pre_register_heaps(t_dongle *first, t_dongle *second,
							t_coder *coder, int metric);

/*** heap_utils.c ***/
long					calculate_priority(t_hub *hub, t_coder *coder);
int						is_higher_priority(t_hub *hub, t_coder *a, t_coder *b);
void					ft_swap(t_coder **a, t_coder **b);

/*** setup.c ***/
int						setup_program(t_hub *hub);

/*** parser.c ****/
int						parse_args(t_hub *hub, int argc, char **argv);

/*** utils.c ***/
long					get_time(long start_time);
long					get_curr_timestamp(t_hub *hub);
void					log_status(t_hub *hub, t_coder *coder, char *msg);
int						check_stop_flag(t_hub *hub);

#endif