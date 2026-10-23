/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alvespin <alvespin@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 18:47:02 by alvespin          #+#    #+#             */
/*   Updated: 2026/10/23 17:15:11 by alvespin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		parent_idx = (i - 1);// / 2;
		if (requests[i].value < requests[parent_idx].value)
			swap(&requests[parent_idx], &requests[i]);
		else
			break ;
		i = parent_idx;
	}
}

/**
 * ...
 */
/*static void	heapify(t_request *requests, int parent, int size)
{
	int	left_child_idx;
	int	right_child_idx;
	int	least;

	least = parent;
	while (1)
	{
		left_child_idx = (parent * 2) + 1;
		right_child_idx = (parent * 2) + 2;
		if (left_child_idx < size && requests[left_child_idx].metric
			< requests[parent].metric)
			least = left_child_idx;
		if (right_child_idx < size && requests[right_child_idx].metric
			< requests[least].metric)
			least = right_child_idx;
		if (least == parent)
			break ;
		swap(&requests[least], &requests[parent]);
		parent = least;
	}
}*/

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
	//heapify(heap->requests, 0, heap->size);
	return ;
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
