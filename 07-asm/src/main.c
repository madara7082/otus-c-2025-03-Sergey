#include "main.h"

int data[] = {4, 8, 15, 16, 23, 42};
size_t data_length = sizeof(data) / sizeof(int);

struct node
{
    int data;
    struct node* nextElptr;
};

int main(void)
{
    struct node* el = NULL;
    struct node* oddEl = NULL;

    for (size_t i=1; i<=data_length; i++)
    {
        el = add_element(data[data_length-i], el);
    }

    for (size_t i=0; i<data_length; i++)
    {
        if (data[i] % 2)
        {
            oddEl = add_element(data[i], oddEl);
        }
    }
    
    printElement(el);
    printElement(oddEl);
    freeElement(el);
    freeElement(oddEl);
    return EXIT_SUCCESS;
}

/*Добавляет новый узел связанного списка в начало*/
struct node* add_element(int element, struct node* nextElementptr)
{
    struct node* newEl = malloc(sizeof(struct node));
    if (!newEl)
    {
        abort();
    }
    else
    {
        newEl->data = element;
        newEl->nextElptr = nextElementptr;
    }
    return newEl;
}

/*Вывод связанный список в консоль*/
void printElement(struct node* element)
{
    while(element != NULL)
    {
        printf("%d ", element->data);
        element = element->nextElptr;
    }
    printf("\n");
}

/*Освобождает выделенную память под связанный список*/
void freeElement(struct node* element)
{
    struct node* current = element;
    while (current) 
    {
        struct node* next = current->nextElptr;
        free(current);
        current = next;
    }
}
