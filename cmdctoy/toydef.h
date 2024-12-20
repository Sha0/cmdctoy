/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_CMDCTOY_DEF
#define INC_CMDCTOY_DEF

#include <stddef.h>

/*
 * alignof derived from Mr. Chris M. Thomasson
 * Also found here: https://ideone.com/wL6wm
 */
#define alignof(type) (offsetof(struct {char c; type t;}, t))

#define countof(array) (sizeof (array) / sizeof *(array))
#define type_with_member_at_ptr(type, member, ptr) ((void *) (((char *) (ptr)) - offsetof(type, member)))

#endif /* INC_CMDCTOY_DEF */
