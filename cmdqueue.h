#ifndef H_COMMAND_QUEUE
#define H_COMMAND_QUEUE

struct queue_node_
{
	struct queue_node_ *next;
	char* command;
};

typedef struct queue_node_ queue_node;

typedef struct 
{
	queue_node *first;
	queue_node *last;
	int size;
} queue;

void enqueue(queue *q, char *command);

char* dequeue(queue *q);

void init_queue(queue *q);

#endif