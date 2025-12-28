#include "./item.h"

#define INVENTORY_CAP 12

typedef struct {
    Item items[INVENTORY_CAP];
    int inv_sz;
} Inventory;
