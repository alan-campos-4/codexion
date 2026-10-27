/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 10:42:14 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 10:43:49 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, char **argv)
{
	t_hub		hub;

    hub.n_coders = 5;
    hub.burnout_time = 300;
    hub.coders = malloc(sizeof(t_coder) * 5);
    free(hub.coders);
	return (0);
}