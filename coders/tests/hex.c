/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 18:38:13 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 19:01:12 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

int check_and_substract(int *num, int div)
{
    if (*num == 0)
        return (0);
    if (*num >= div)
    {
        *num = *num - div;
        return (1);
    }
    return (0);
}

void    divide(int num)
{
    int d = 0;
    int c = 0;
    int b = 0;
    int a = 0;

    d = check_and_substract(&num, 8);
    c = check_and_substract(&num, 4);
    b = check_and_substract(&num, 2);
    a = check_and_substract(&num, 1);
    printf("%i %i %i %i\n", d, c, b, a);
}

int main(void)
{
    divide(1);
    divide(2);
    divide(4);
    divide(8);
    divide(12);
    divide(16);
    return (0);
}