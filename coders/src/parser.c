/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/11/04 19:12:20 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Checks that the number given is within the range and INT should have.
 */
static int	is_int(long n, int sign)
{
	if (n * sign > INT_MAX)
		return (1);
	if (n * sign < INT_MIN)
		return (1);
	return (0);
}

/**
 * Custom atoi parser that prevents overflow or underflow.
 * Returns -1 if at any point the string passed contains invalid characters
 * or exceeds the limits of an int.
 */
static int	ft_atoi(char *str)
{
	long	result;
	int		sign;
	int		i;

	i = 0;
	sign = 1;
	result = 0;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		result = (str[i] - '0') + (result * 10);
		if (is_int(result, sign))
			return (-1);
		i++;
	}
	return ((int)result * sign);
}

/**
 * Assign all the values found to the corresponding values in the hub.
 */
static int	fill_hub(t_hub *hub, int *values, char *scheduler)
{
	hub->n_coders = values[0];
	hub->burnout_time = values[1];
	hub->compile_time = values[2];
	hub->debug_time = values[3];
	hub->refactor_time = values[4];
	hub->n_compiles_required = values[5];
	hub->cooldown = values[6];
	if (!strcmp(scheduler, "fifo"))
		hub->scheduler = FIFO;
	else if (!strcmp(scheduler, "edf"))
		hub->scheduler = EDF;
	else
		return (1);
	return (0);
}

/**
 * Iterate through the command line arguments the program receives,
 * and stores them in arrays to then assign them to the hub attributes.
 */
int	parse_args(int argc, char **argv, t_hub *hub)
{
	int		values[8];
	int		counter;
	int		i;
	int		n;

	if (argc != 9)
		return (1);
	i = 1;
	counter = 0;
	while (i < argc - 1)
	{
		n = ft_atoi(argv[i]);
		if (n <= 0)
			return (1);
		values[counter++] = n;
		i++;
	}
	return (fill_hub(hub, values, argv[8]));
}
