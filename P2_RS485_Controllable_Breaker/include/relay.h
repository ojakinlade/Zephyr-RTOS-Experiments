#ifndef RELAY_H_
#define RELAY_H_

#include <stdbool.h>

int relay_init(void);
int relay_set(bool on);
bool relay_get(void);

#endif /* RELAY_H_ */
