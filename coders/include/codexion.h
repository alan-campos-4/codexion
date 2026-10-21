/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:43 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:13:58 by alvespin         ###   ########.fr       */
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

/*** coder.c ***/
void					*coder_routine(void *arg);
int						wait_for_others(t_hub *hub);

/*** monitor.c ***/
void					*monitor_routine(void *arg);

/*** dongles.c ***/
int						get_dongles(t_coder *coder);

/*** heap.c ***/
int						pick_metric(t_coder *coder);
void					add_request(t_heap *heap, int id, int metric);
void					remove_request_top(t_heap *heap);
void					pre_register_heaps(t_dongle *first, t_dongle *second,
							t_coder *coder, int metric);

/*** setup.c ***/
int						setup_program(t_hub *hub);

/*** parser.c ****/
int						parse_args(t_hub *hub, int argc, char **argv);

/*** utils.c ***/
long					get_time(long start_time);
void					log_status(t_hub *hub, t_coder *coder, char *msg);
int						stop_program(t_hub *hub);

#endif