#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
// the available amount of each resource
int available[NUMBER_OF_RESOURCES];
// the maximum demand of each customer
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
// the amount currently allocated to each customer
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
// the remaining need of each customer
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
pthread_mutex_t mutex;

int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);
bool is_safe();
void show_info();

int main(int argc, char *argv[])
{
    int i, j;
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) // initialize available with command
    {
        available[i] = atoi(argv[i + 1]);
    }
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) // initialize allocation to all 0
    {
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            allocation[i][j] = 0;
        }
    }
    FILE *fp;
    fp = fopen("in.txt", "r");
    char junk;
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) // initialize maximum with file
    {
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            fscanf(fp, "%d%c", &maximum[i][j], &junk);
        }
    }
    fclose(fp);
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
    show_info();

    char command[3];
    int request[NUMBER_OF_RESOURCES];
    int release[NUMBER_OF_RESOURCES];
    int customer_num;
    int return_value;
    while (true)
    {
        scanf("%s", command);
        if (strcmp(command, "RQ") == 0)
        {
            scanf("%d", &customer_num);
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                scanf("%d", &request[i]);
            }
            printf("PID\tRequest\n");
            printf("P%d\t", customer_num);
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                printf("%d ", request[i]);
            }
            printf("\n");
            return_value = request_resources(customer_num, request);
            if (return_value == 0)
            {
                printf("Safe state.\n");
                continue;
            }
            else if (return_value == -1)
            {
                printf("Unsafe state.\n");
                printf("Request denied.\n");
                continue;
            }
            else if (return_value == 1)
            {
                printf("Request denied.\n");
                continue;
            }
        }
        if (strcmp(command, "RL") == 0)
        {
            scanf("%d", &customer_num);
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                scanf("%d", &release[i]);
            }
            printf("PID\tRelease\n");
            printf("P%d\t", customer_num);
            for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
            {
                printf("%d ", release[i]);
            }
            printf("\n");
            return_value = release_resources(customer_num, release);
            if (return_value == -1)
            {
                printf("Release denied.\n");
            }
        }
        if (strcmp(command, "*") == 0)
        {
            show_info();
            continue;
        }
    }
    return 0;
}

int request_resources(int customer_num, int request[])
{
    int i;
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        if (request[i] > need[customer_num][i])
        {
            return 1;
        }
    }
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        if (request[i] > available[i])
        {
            return 1;
        }
    }
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }
    if (is_safe())
    {
        return 0;
    }
    else // recover
    {
        for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
        {
            available[i] += request[i];
            allocation[customer_num][i] -= request[i];
            need[customer_num][i] += request[i];
        }
        return -1;
    }
}

int release_resources(int customer_num, int release[])
{
    int i;
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        if (release[i] > allocation[customer_num][i])
        {
            return -1;
        }
    }
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i)
    {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
        need[customer_num][i] += release[i];
    }
    return 0;
}

bool is_safe()
{
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];
    int i, j, k;
    int flag = 0;
    for (i = 0; i < NUMBER_OF_RESOURCES; ++i) // initialize work
    {
        work[i] = available[i];
    }
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) // initialize finish
    {
        finish[i] = 0;
    }
    for (k = NUMBER_OF_CUSTOMERS; k > 0; --k)
    {
        for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
        {
            if (finish[i] == 0)
            {
                flag = 1;
                for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
                {
                    if (need[i][j] > work[j])
                    {
                        flag = 0;
                    }
                }
                if (flag == 1)
                {
                    finish[i] = 1;
                    for (j = 0; j < NUMBER_OF_RESOURCES; j++)
                    {
                        work[j] += allocation[i][j];
                    }
                }
            }
        }
    }
    flag = 1;
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        if (finish[i] == 0)
        {
            flag = 0;
            break;
        }
    }
    if (flag == 0)
        return false;
    else return true;
}

void show_info()
{
    int i, j;
    printf("Available\n");
    for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
    {
        printf("%d ", available[j]);
    }
    printf("\n");
    printf("PID\tMax\t\tAllocation\t\tNeed\n");
    for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i)
    {
        printf("P%d\t", i);
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            printf("%d ", maximum[i][j]);
        }
        printf("\t");
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            printf("%d ", allocation[i][j]);
        }
        printf("\t\t");
        for (j = 0; j < NUMBER_OF_RESOURCES; ++j)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
}
