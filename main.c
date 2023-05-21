#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char name[50];
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

int isQueueEmpty(Queue *queue)
{
    return (queue->front == NULL);
}

int isStackEmpty(Stack *stack)
{
    return (stack->top == NULL);
}

void enqueue(Queue *queue, char *name)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->next = NULL;

    if (isQueueEmpty(queue))
    {
        queue->front = newNode;
        queue->rear = newNode;
    }
    else  //new node is linked to the current last node of the queue.
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
    else //The name is stored in the front node is then copied using strdup to allocate memory for a duplicate string.
    {
        Node *temp = queue->front;
        char *name = strdup(temp->name);
        queue->front = queue->front->next;
        free(temp);
        return name;
    }
}

void push(Stack *stack, char *name)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->next = stack->top; //current top of the
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

void processCommand(char *command, Queue *regularQueue, Stack *vipStack, int *supervisorPresent)
{
    char *token = strtok(command, ",");
    if (strcmp(token, "lineup") == 0)
    {
        char *name = strtok(NULL, ",");
        char *type = strtok(NULL, ",");
        if (*supervisorPresent)
        {
            printf("Regular client %s lines up at RegularQueue\n", name);
            enqueue(regularQueue, name);
        }
        else
        {
            if (strcmp(type, "regular") == 0)
            {
                printf("Regular client %s lines up at RegularQueue\n", name);
                enqueue(regularQueue, name);
            }
            else if (strcmp(type, "VIP") == 0)
            {
                printf("VIP client %s lines up at VIPStack\n", name);
                push(vipStack, name);
            }
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
        if (!isStackEmpty(vipStack))
        {
            printf("Supervisor present\n");
            printf("VIP clients transferred from VIPStack to RegularQueue\n");
            while (!isStackEmpty(vipStack))
            {
                char *name = pop(vipStack);
                printf("VIP client %s transferred from VIPStack to RegularQueue\n", name);
                enqueue(regularQueue, name);
            }
        }
        else
        {
            printf("Supervisor present\n");
        }
    }
    else if (strcmp(token, "leave") == 0)
    {
        char *type = strtok(NULL, ",");
        if (strcmp(type, "supervisor") == 0)
        {
            *supervisorPresent = 0;
            printf("Supervisor not here\n");
        }
    }
}

void corruptQueueSimulation(const char *inputFile)
{
    FILE *file = fopen(inputFile, "r");
    if (file == NULL)
    {
        printf("Failed to open the input file.\n");
        return;
    }

    //initialized two data structue
    Queue regularQueue = {NULL, NULL};
    Stack vipStack = {NULL};
    int supervisorPresent = 0;

    char command[100];
    while (fgets(command, sizeof(command), file))
    {
        command[strcspn(command, "\n")] = '\0'; // Remove newline character
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
    corruptQueueSimulation("INPUT - 1.txt");
    return 0;
}
