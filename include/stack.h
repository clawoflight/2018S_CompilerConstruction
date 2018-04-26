/**
 * @file stack.h
 * @brief Simple stack lib
 *
 * This makes no safety checks. Only use it if the algorithm can guarantee that
 * the stack will never be overfilled or overpopped.
 *
 * @author bennett
 * @date 2018-04-18
 */
#ifndef STACK_H
#define STACK_H

#define STACK_MAX 4096

struct stack_t {
	void *data[STACK_MAX];
	int top;
};

#define STACK_INITIALIZER \
	{ \
		.top = -1 \
	}

#define stack_push(stackptr, entry) \
	{ \
		(stackptr)->data[++(stackptr)->top] = (entry); \
	}

#define stack_pop(stackptr) ((stackptr)->data[(stackptr)->top--])

#define stack_reset(stackptr) \
	{ \
		memset((stackptr), 0, sizeof(*(stackptr))); \
		(stackptr)->top = -1; \
	}

#endif
