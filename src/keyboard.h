#ifndef KEYBOARD_H
#define KEYBOARD_H

extern void keyboard_install();
extern void keyboard_handler(struct registers_t *r);
extern char kbdus[128];

#endif
