/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:47:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/27 13:59:12 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Removes the request on the top of the heap.
 */
void	remove_request_top(t_heap *heap)
{
	if (!heap || !heap->size)
		return ;
	if (&heap->requests[0] != &heap->requests[heap->size - 1])
		swap(&heap->requests[0], &heap->requests[heap->size - 1]);
	heap->size -= 1;
	return ;
}

/**
 * Moves the smallest request to the top of the heap.
 * - The index of the parent request is the one on top.
 * - If the value of the parent request is bigger than the
 * 	value of current one, swap them.
 */
static void	bubble_up(t_request *requests, int i)
{
	int	parent_idx;

	while (i > 0)
	{
		parent_idx = (i - 1);
		if (requests[i].value < requests[parent_idx].value)
			swap(&requests[parent_idx], &requests[i]);
		else
			break ;
		i = parent_idx;
	}
}

/**
 * Adds a request to the given heap and increases the size.
 * The request with the smallest metric will be placed
 * at the top of the heap.
 */
void	add_request(t_heap *heap, int id, int metric)
{
	if (!heap || (heap->size == 2))
		return ;
	heap->requests[heap->size].id = id;
	heap->requests[heap->size].value = metric;
	bubble_up(heap->requests, heap->size);
	heap->size += 1;
}

/**
 * Safely add the requests from the coder to the two dongles' heaps.
 */
void	add_requests_heap(t_coder *coder, t_dongle *first, t_dongle *second,
		int metric)
{
	pthread_mutex_lock(&first->lock);
	add_request(&first->heap, coder->id, metric);
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_lock(&second->lock);
	add_request(&second->heap, coder->id, metric);
	pthread_mutex_unlock(&second->lock);
}

/*
pthread_mutex_lock(&first->lock);
pthread_mutex_lock(&second->lock);
add_request(&first->heap, coder->id, metric);
add_request(&second->heap, coder->id, metric);
pthread_mutex_unlock(&second->lock);
pthread_mutex_unlock(&first->lock);
*/
