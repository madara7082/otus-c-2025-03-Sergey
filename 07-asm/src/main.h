#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

struct node* add_element(int element, struct node* nextElementptr);
void printElement(struct node* element);
void freeElement(struct node* element);


#endif /* __MAIN_H */