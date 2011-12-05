#include <stdlib.h>
#include "cmdqueue.h"

void enqueue(queue *q, char *command)
{
	queue_node *node = (queue_node *)malloc(sizeof(queue_node));
	node->command = command;
	if(q->first == NULL)
		q->first = q->last = node;
	else {
		q->last->next = node;
		q->last = node;
	}
	node->next = NULL;

	q->size++;
}

char* dequeue(queue *q)
{
	char* result;
	queue_node* tmp;

	if(!q->first) {
		return 0;
	}

	result = q->first->command;
	tmp = q->first;
	if(q->first == q->last)
		q->first = q->last = NULL;
	else
		q->first = q->first->next;
	free(tmp);
	q->size--;
	return result;
}

void init_queue(queue *q)
{
	q->size = 0;
	q->first = q->last = NULL;
}