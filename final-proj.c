#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50

typedef struct Node
{
    char name[MAX_NAME_LENGTH];
    struct Node *next;
} Node;

typedef struct
{
    Node *front;
    Node *rear;
} Queue;

typedef struct
{
    Node *top;
} Stack;

int isQueueEmpty(const Queue *queue)
{
    return (queue->front == NULL);
}

int isStackEmpty(const Stack *stack)
{
    return (stack->top == NULL);
}

void enqueue(Queue *queue, const char *name)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strncpy(newNode->name, name, MAX_NAME_LENGTH - 1);
    newNode->name[MAX_NAME_LENGTH - 1] = '\0';
    newNode->next = NULL;

    if (isQueueEmpty(queue))
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

char *dequeue(Queue *queue)
{
    if (isQueueEmpty(queue))
    {
        return NULL;
    }
    else
    {
        Node *temp = queue->front;
        char *name = strdup(temp->name);
        queue->front = queue->front->next;
        if (queue->front == NULL)
        {
            queue->rear = NULL;
        }
        free(temp);
        return name;
    }
}

void push(Stack *stack, const char *name)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strncpy(newNode->name, name, MAX_NAME_LENGTH - 1);
    newNode->name[MAX_NAME_LENGTH - 1] = '\0';
    newNode->next = stack->top;
    stack->top = newNode;
}

char *pop(Stack *stack)
{
    if (isStackEmpty(stack))
    {
        return NULL;
    }
    else
    {
        Node *temp = stack->top;
        char *name = strdup(temp->name);
        stack->top = stack->top->next;
        free(temp);
        return name;
    }
}

void processCommand(const char *command, Queue *regularQueue, Stack *vipStack, int *supervisorPresent)
{
    char *commandCopy = strdup(command);
    char *token = strtok(commandCopy, ",");
    if (strcmp(token, "lineup") == 0)
    {
        char *name = strtok(NULL, ",");
        char *type = strtok(NULL, ",");
        if (*supervisorPresent || strcmp(type, "regular") == 0)
        {
            printf("%s client %s lines up at RegularQueue\n", type, name);
            enqueue(regularQueue, name);
        }
        else if (strcmp(type, "VIP") == 0)
        {
            printf("%s client %s lines up at VIPStack\n", type, name);
            push(vipStack, name);
        }
    }
    else if (strcmp(token, "serve") == 0)
    {
        if (!isStackEmpty(vipStack))
        {
            char *name = pop(vipStack);
            printf("Now serving %s from VIPStack\n", name);
            free(name);
        }
        else if (!isQueueEmpty(regularQueue))
        {
            char *name = dequeue(regularQueue);
            printf("Now serving %s from RegularQueue\n", name);
            free(name);
        }
        else
        {
            printf("No clients in the queue\n");
        }
    }
    else if (strcmp(token, "supervisor") == 0)
    {
        *supervisorPresent = 1;
        printf("Supervisor present\n");
        if (!isStackEmpty(vipStack))
        {
            printf("VIP clients transferred from VIPStack to RegularQueue\n");
            while (!isStackEmpty(vipStack))
            {
                char *name = pop(vipStack);
                printf("VIP client %s transferred from VIPStack to RegularQueue\n", name);
                enqueue(regularQueue, name);
            }
        }
    }
    else if (strcmp(token, "leave") == 0 && strcmp(strtok(NULL, ","), "supervisor") == 0)
    {
        *supervisorPresent = 0;
        printf("Supervisor not here\n");
    }

    free(commandCopy);
}

void corruptQueueSimulation(const char *inputFile)
{
    FILE *file = fopen(inputFile, "r");
    if (file == NULL)
    {
        printf("Failed to open the input file.\n");
        return;
    }

    Queue regularQueue = {NULL, NULL};
    Stack vipStack = {NULL};
    int supervisorPresent = 0;

    char command[100];
    while (fgets(command, sizeof(command), file))
    {
        command[strcspn(command, "\n")] = '\0';
        processCommand(command, &regularQueue, &vipStack, &supervisorPresent);
    }

    fclose(file);

    while (!isQueueEmpty(&regularQueue))
    {
        char *name = dequeue(&regularQueue);
        free(name);
    }

    while (!isStackEmpty(&vipStack))
    {
        char *name = pop(&vipStack);
        free(name);
    }
}

int main()
{
    corruptQueueSimulation("INPUT - 2.txt");
    return 0;
}
