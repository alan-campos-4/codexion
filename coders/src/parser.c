/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 13:42:59 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/21 13:59:34 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_overflow(int n, int sign)
{
	if (n * sign > INT_MAX)
		return (FAIL);
	if (n * sign < INT_MIN)
		return (FAIL);
	return (SUCCESS);
}

static int	ft_atol(char *nptr)
{
	int	result;
	int	sign;

	while ((*nptr >= 9 && *nptr <= 13) || *nptr == ' ')
		nptr++;
	sign = 1;
	if (*nptr == '+' || *nptr == '-')
	{
		if (*nptr == '-')
			sign = -1;
		nptr++;
	}
	if (*nptr < '0' || *nptr > '9')
		return (-1);
	result = 0;
	while (*nptr)
	{
		if (*nptr < '0' || *nptr > '9')
			return (-1);
		result = (*nptr - '0') + (result * 10);
		if (check_overflow(result, sign))
			return (-1);
		nptr++;
	}
	return (result * sign);
}

static int	fill_hub(int *arr, char *s, t_hub *hub)
{
	hub->n_coders = arr[0];
	hub->burnout_time = arr[1];
	hub->compile_time = arr[2];
	hub->debug_time = arr[3];
	hub->refactor_time = arr[4];
	hub->target_compiles = arr[5];
	hub->cooldown = arr[6];
	if (!strcmp(s, "fifo"))
		hub->scheduler = FIFO;
	else if (!strcmp(s, "edf"))
		hub->scheduler = EDF;
	else
		return (FAIL);
	return (SUCCESS);
}

int	ft_parser(int ac, char **av, t_hub *hub)
{
	int	n_arr[7];
	int	i;
	int	n;

	if (ac != 9)
		return (FAIL);
	i = 1;
	while (i < ac - 1)
	{
		n = ft_atol(av[i]);
		if (n < 0)
			return (FAIL);
		n_arr[i - 1] = n;
		i++;
	}
	if (strcmp(av[i], "fifo") && strcmp(av[i], "edf"))
		return (FAIL);
	return (fill_hub(n_arr, av[i], hub));
}
