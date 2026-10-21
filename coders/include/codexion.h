/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:43 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 14:10:00 by alvespin         ###   ########.fr       */
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

# define FAIL 1
# define SUCCESS 0
# define ZERO_CODERS 2
# define MS_IN_US 1000
# define FIFO 0
# define EDF 1

typedef struct s_coder	t_coder;
typedef struct s_hub	t_hub;
typedef struct s_dongle	t_dongle;

typedef struct s_hub
{
	int					n_coders;
	int					burnout_time;
	int					compile_time;
	int					debug_time;
	int					refactor_time;
	int					target_compiles;
	int					cooldown;
	int					scheduler;
	int					start_sim;
	int					stop_sim;
	int					ready_count;
	int					start_time;
	pthread_mutex_t		sim_lock;
	pthread_mutex_t		print_lock;
	pthread_cond_t		cond;
	t_coder				*coders;
}						t_hub;

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

typedef struct s_dongle
{
	int					is_available;
	int					last_use_time;
	pthread_mutex_t		lock;
	pthread_cond_t		cond;
	t_heap				heap;
}						t_dongle;

typedef struct s_coder
{
	int					id;
	int					completed_compiles;
	int					last_compile;
	t_hub				*hub;
	t_dongle			*first_dongle;
	t_dongle			*second_dongle;
	pthread_mutex_t		lock;
}						t_coder;

int						ft_parser(int ac, char **av, t_hub *hub);
int						init_sim(t_hub *hub, t_coder **coders,
							t_dongle **dongles);
pthread_t				*init_threads(t_coder *coders);

void					*coder_routine(void *arg);
int						release_dongle(t_dongle *dongle);
int						get_dongles(t_coder *coder);
int						wait_for_start(t_coder *coder);
void					*single_coder(t_coder *coder, t_dongle *dongle);

void					remove_request_top(t_heap *heap);
void					pre_register_heaps(t_dongle *first, t_dongle *second,
							t_coder *coder, int metric);

void					*monitor_routine(void *arg);

t_dongle				*init_dongles(int n);
t_coder					*init_coders(int n);
int						init_hub(t_hub *hub);
void					sim_start(t_coder *coders);

void					wakeup_coders(t_coder *coders);
int						get_curr_time(void);
int						print_log(char *str, t_coder *coder, t_hub *hub);
int						check_stop_sim(t_hub *hub);
int						better_sleep(t_coder *coder, int time);
void					wait_on_threads(pthread_t *ts, int n);
void					swap(t_request *x, t_request *y);
int						pick_metric(t_coder *coder);

t_dongle				*dongle_cleanup(t_dongle *dongle, int32_t n,
							int32_t m_clean, int32_t cond_clean);
int32_t					hub_cleanup(t_hub *hub, int32_t c, int32_t s_lock,
							int32_t p_lock);
int32_t					cleanup_all(t_dongle *dongles, t_coder *coders,
							t_hub *hub);

#endif