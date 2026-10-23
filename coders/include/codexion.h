/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:43 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 19:07:44 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define MS_TO_US 1000
# define LOG_DONGLE "has taken a dongle"
# define LOG_COMPILE "is compiling"
# define LOG_DEBUG "is debugging"
# define LOG_REFACTOR "is refactoring"
# define LOG_BURNOUT "burned out"

typedef enum s_scheduler
{
	FIFO,
	EDF,
}						t_scheduler;

typedef struct s_request
{
	int					id;
	int					value;
}						t_request;

typedef struct s_heap
{
	t_request			requests[2];
	int					size;
}						t_heap;

typedef struct s_coder	t_coder;
typedef struct s_hub	t_hub;

typedef struct s_dongle
{
	int					is_available;
	long				last_use_time;
	pthread_mutex_t		lock;
	pthread_cond_t		cond;
	t_heap				heap;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	int					n_compiles;
	long				last_compile_time;
	pthread_mutex_t		lock;
	t_dongle			*dongle_l;
	t_dongle			*dongle_r;
	t_hub				*hub;
}						t_coder;

typedef struct s_hub
{
	int					n_coders;
	long				burnout_time;
	long				compile_time;
	long				debug_time;
	long				refactor_time;
	long				n_compiles_required;
	int					cooldown;
	t_scheduler			scheduler;
	int					start_flag;
	int					stop_flag;
	int					ready_count;
	long				start_time;
	pthread_mutex_t		lock;
	pthread_mutex_t		stop_lock;
	pthread_mutex_t		log_lock;
	pthread_cond_t		cond;
	t_coder				*coders;
}						t_hub;

/***** coders.c *****/
void					*coder_routine(void *arg);
int						coder_sleep(t_coder *coder, long time);
int						wait_for_others(t_coder *coder);
int						release_dongle(t_dongle *dongle);
void					*single_coder(t_coder *coder, t_dongle *dongle);

/***** dongles.c *****/
int						try_get_dongles(t_coder *coder);

/***** monitor.c *****/
void					*monitor_routine(void *arg);
void					wakeup_coders(t_coder *coders);

/***** heap.c *****/
void					add_requests_heap(t_coder *coder, t_dongle *first,
							t_dongle *second, int metric);
void					add_request(t_heap *heap, int id, int metric);
void					remove_request_top(t_heap *heap);

/***** parser.c *****/
int						parse_args(int argc, char **argv, t_hub *hub);

/***** setup.c *****/
int						setup_program(t_hub *hub, t_coder **coders,
							t_dongle **dongles);

/***** cleanup.c *****/
void					dongle_cleanup(t_dongle *dongle, int n, int mutex_c,
							int cond_c);
void					coder_cleanup(t_coder *coders, int n, int mutex_c);
int						hub_cleanup(t_hub *hub, int flag);
int						cleanup_all(t_dongle *dongles, t_coder *coders,
							t_hub *hub);

/***** utils.c *****/
long					get_time(long start_time);
int						stop_program(t_hub *hub);
int						log_status(t_hub *hub, int id, char *str);
int						get_request_value(t_coder *coder);
void					swap(t_request *x, t_request *y);

pthread_t				*init_threads(t_coder *coders);


t_dongle				*init_dongles(int n);
t_coder					*init_coders(int n);
int						init_hub(t_hub *hub);
void					wait_for_start(t_coder *coders);

#endif