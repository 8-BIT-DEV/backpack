# Backpack

A library for data storage and file packing. Not the fasting but very simple.

# Example

```
#include <backpack.h>

int main()
{
  BACKPACK* bp = create_backpack();
  
  int mynumber = 50;
  char thatchar = 'A';
  
  put_backpack_node_data(bp, "number", sizeof(int), &mynumber, 0);
  put_backpack_node_data(bp, "letter", sizeof(char), &thatchar, 12);
  
  free_backpack(bp);
  return 0
}
```
