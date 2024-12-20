/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <string.h>
#include "toydef.h"
#include "type.h"

/*
 * TODO: a particular C language compiler claims that defining a type within
 * usage of the offsetof macro is a "C23 extension," despite the alignof macro
 * being at least 12 years old, but probably much, much older.  It seems that
 * the new thing has broken the old thing, as usual, even when using "old thing mode."
 * File a bug with them.
 * Meanwhile, use all of the horrible alignof typedefs, below.
 * If that compiler's decision-makers agree and adjust it, restore the alignof macro
 * usage in all the "alignment" sections to what they were, in a previous iteration.
 * This TODO introduced on  2024-12-17
 * Update: bug filed: https://github.com/llvm/llvm-project/issues/120357
 */
#define align_hack(type) (offsetof(type, t))

/* "Self-describing" */

static char sd_enum_name_apivalue_type_success[] = "apivalue_type_success";
static enum apivalue_type sd_enum_value_apivalue_type_success = apivalue_type_success;
static char sd_enum_name_apivalue_type_error_buffer_too_small[] = "apivalue_type_error_buffer_too_small";
static enum apivalue_type sd_enum_value_apivalue_type_error_buffer_too_small = apivalue_type_error_buffer_too_small;
static char sd_enum_name_apivalue_type_error_not_compatible[] = "apivalue_type_error_not_compatible";
static enum apivalue_type sd_enum_value_apivalue_type_error_not_compatible = apivalue_type_error_not_compatible;
static char sd_enum_name_apivalue_type_zero[] = "apivalue_type_zero";
static enum apivalue_type sd_enum_value_apivalue_type_zero = apivalue_type_zero;

static struct type_enum_value sd_enum_values_apivalue_type[] =
  {
    { sd_enum_name_apivalue_type_success, &sd_enum_value_apivalue_type_success },
    { sd_enum_name_apivalue_type_error_buffer_too_small, &sd_enum_value_apivalue_type_error_buffer_too_small },
    { sd_enum_name_apivalue_type_error_not_compatible, &sd_enum_value_apivalue_type_error_not_compatible },
    { sd_enum_name_apivalue_type_zero, &sd_enum_value_apivalue_type_zero }
  };

typedef struct alignof_enum_apivalue_type { char c; enum apivalue_type t; } alignof_enum_apivalue_type;
static struct type_enum sd_enum_type_apivalue_type =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type),
          /* size */
          sizeof (enum apivalue_type)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type",
    /* value_count */
    countof(sd_enum_values_apivalue_type),
    /* values */
    sd_enum_values_apivalue_type
  };

static char sd_enum_name_apivalue_type_arithmetic_integer[] = "apivalue_type_arithmetic_integer";
static enum apivalue_type_arithmetic sd_enum_value_apivalue_type_arithmetic_integer = apivalue_type_arithmetic_integer;
static char sd_enum_name_apivalue_type_arithmetic_floating[] = "apivalue_type_arithmetic_floating";
static enum apivalue_type_arithmetic sd_enum_value_apivalue_type_arithmetic_floating = apivalue_type_arithmetic_floating;
static char sd_enum_name_apivalue_type_arithmetics[] = "apivalue_type_arithmetics";
static enum apivalue_type_arithmetic sd_enum_value_apivalue_type_arithmetics = apivalue_type_arithmetics;

static struct type_enum_value sd_enum_values_apivalue_type_arithmetic[] =
  {
    { sd_enum_name_apivalue_type_arithmetic_integer, &sd_enum_value_apivalue_type_arithmetic_integer },
    { sd_enum_name_apivalue_type_arithmetic_floating, &sd_enum_value_apivalue_type_arithmetic_floating },
    { sd_enum_name_apivalue_type_arithmetics, &sd_enum_value_apivalue_type_arithmetics }
  };

typedef struct alignof_enum_apivalue_type_arithmetic { char c; enum apivalue_type_arithmetic t; } alignof_enum_apivalue_type_arithmetic;
static struct type_enum sd_enum_type_apivalue_type_arithmetic =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_arithmetic"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_arithmetic),
          /* size */
          sizeof (enum apivalue_type_arithmetic)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_arithmetic",
    /* value_count */
    countof(sd_enum_values_apivalue_type_arithmetic),
    /* values */
    sd_enum_values_apivalue_type_arithmetic
  };

static char sd_enum_name_apivalue_type_floating_float[] = "apivalue_type_floating_float";
static enum apivalue_type_floating sd_enum_value_apivalue_type_floating_float = apivalue_type_floating_float;
static char sd_enum_name_apivalue_type_floating_double[] = "apivalue_type_floating_double";
static enum apivalue_type_floating sd_enum_value_apivalue_type_floating_double = apivalue_type_floating_double;
static char sd_enum_name_apivalue_type_floating_long_double[] = "apivalue_type_floating_long_double";
static enum apivalue_type_floating sd_enum_value_apivalue_type_floating_long_double = apivalue_type_floating_long_double;
static char sd_enum_name_apivalue_type_floatings[] = "apivalue_type_floatings";
static enum apivalue_type_floating sd_enum_value_apivalue_type_floatings = apivalue_type_floatings;

static struct type_enum_value sd_enum_values_apivalue_type_floating[] =
  {
    { sd_enum_name_apivalue_type_floating_float, &sd_enum_value_apivalue_type_floating_float },
    { sd_enum_name_apivalue_type_floating_double, &sd_enum_value_apivalue_type_floating_double },
    { sd_enum_name_apivalue_type_floating_long_double, &sd_enum_value_apivalue_type_floating_long_double },
    { sd_enum_name_apivalue_type_floatings, &sd_enum_value_apivalue_type_floatings }
  };

typedef struct alignof_enum_apivalue_type_floating { char c; enum apivalue_type_floating t; } alignof_enum_apivalue_type_floating;
static struct type_enum sd_enum_type_apivalue_type_floating =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_floating"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_floating),
          /* size */
          sizeof (enum apivalue_type_floating)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_floating",
    /* value_count */
    countof(sd_enum_values_apivalue_type_floating),
    /* values */
    sd_enum_values_apivalue_type_floating
  };

static char sd_enum_name_apivalue_type_integer_char[] = "apivalue_type_integer_char";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_char = apivalue_type_integer_char;
static char sd_enum_name_apivalue_type_integer_short[] = "apivalue_type_integer_short";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_short = apivalue_type_integer_short;
static char sd_enum_name_apivalue_type_integer_int[] = "apivalue_type_integer_int";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_int = apivalue_type_integer_int;
static char sd_enum_name_apivalue_type_integer_long[] = "apivalue_type_integer_long";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_long = apivalue_type_integer_long;
static char sd_enum_name_apivalue_type_integer_longlong[] = "apivalue_type_integer_longlong";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_longlong = apivalue_type_integer_longlong;
static char sd_enum_name_apivalue_type_integer_size_t[] = "apivalue_type_integer_size_t";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_size_t = apivalue_type_integer_size_t;
static char sd_enum_name_apivalue_type_integer_enum[] = "apivalue_type_integer_enum";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integer_enum = apivalue_type_integer_enum;
static char sd_enum_name_apivalue_type_integers[] = "apivalue_type_integers";
static enum apivalue_type_integer sd_enum_value_apivalue_type_integers = apivalue_type_integers;

static struct type_enum_value sd_enum_values_apivalue_type_integer[] =
  {
    { sd_enum_name_apivalue_type_integer_char, &sd_enum_value_apivalue_type_integer_char },
    { sd_enum_name_apivalue_type_integer_short, &sd_enum_value_apivalue_type_integer_short },
    { sd_enum_name_apivalue_type_integer_int, &sd_enum_value_apivalue_type_integer_int },
    { sd_enum_name_apivalue_type_integer_long, &sd_enum_value_apivalue_type_integer_long },
    { sd_enum_name_apivalue_type_integer_longlong, &sd_enum_value_apivalue_type_integer_longlong },
    { sd_enum_name_apivalue_type_integer_size_t, &sd_enum_value_apivalue_type_integer_size_t },
    { sd_enum_name_apivalue_type_integer_enum, &sd_enum_value_apivalue_type_integer_enum },
    { sd_enum_name_apivalue_type_integers, &sd_enum_value_apivalue_type_integers }
  };

typedef struct alignof_enum_apivalue_type_integer { char c; enum apivalue_type_integer t; } alignof_enum_apivalue_type_integer;
static struct type_enum sd_enum_type_apivalue_type_integer =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_integer"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_integer),
          /* size */
          sizeof (enum apivalue_type_integer)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_integer",
    /* value_count */
    countof(sd_enum_values_apivalue_type_integer),
    /* values */
    sd_enum_values_apivalue_type_integer
  };

static char sd_enum_name_apivalue_type_object_void[] = "apivalue_type_object_void";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_void = apivalue_type_object_void;
static char sd_enum_name_apivalue_type_object_arithmetic[] = "apivalue_type_object_arithmetic";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_arithmetic = apivalue_type_object_arithmetic;
static char sd_enum_name_apivalue_type_object_array[] = "apivalue_type_object_array";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_array = apivalue_type_object_array;
static char sd_enum_name_apivalue_type_object_struct[] = "apivalue_type_object_struct";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_struct = apivalue_type_object_struct;
static char sd_enum_name_apivalue_type_object_union[] = "apivalue_type_object_union";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_union = apivalue_type_object_union;
static char sd_enum_name_apivalue_type_object_pointer[] = "apivalue_type_object_pointer";
static enum apivalue_type_object sd_enum_value_apivalue_type_object_pointer = apivalue_type_object_pointer;
static char sd_enum_name_apivalue_type_objects[] = "apivalue_type_objects";
static enum apivalue_type_object sd_enum_value_apivalue_type_objects = apivalue_type_objects;

static struct type_enum_value sd_enum_values_apivalue_type_object[] =
  {
    { sd_enum_name_apivalue_type_object_void, &sd_enum_value_apivalue_type_object_void },
    { sd_enum_name_apivalue_type_object_arithmetic, &sd_enum_value_apivalue_type_object_arithmetic },
    { sd_enum_name_apivalue_type_object_array, &sd_enum_value_apivalue_type_object_array },
    { sd_enum_name_apivalue_type_object_struct, &sd_enum_value_apivalue_type_object_struct },
    { sd_enum_name_apivalue_type_object_union, &sd_enum_value_apivalue_type_object_union },
    { sd_enum_name_apivalue_type_object_pointer, &sd_enum_value_apivalue_type_object_pointer },
    { sd_enum_name_apivalue_type_objects, &sd_enum_value_apivalue_type_objects }
  };

typedef struct alignof_enum_apivalue_type_object { char c; enum apivalue_type_object t; } alignof_enum_apivalue_type_object;
static struct type_enum sd_enum_type_apivalue_type_object =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_object"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_object),
          /* size */
          sizeof (enum apivalue_type_object)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_object",
    /* value_count */
    countof(sd_enum_values_apivalue_type_object),
    /* values */
    sd_enum_values_apivalue_type_object
  };

static char sd_enum_name_apivalue_type_partition_function[] = "apivalue_type_partition_function";
static enum apivalue_type_partition sd_enum_value_apivalue_type_partition_function = apivalue_type_partition_function;
static char sd_enum_name_apivalue_type_partition_object[] = "apivalue_type_partition_object";
static enum apivalue_type_partition sd_enum_value_apivalue_type_partition_object = apivalue_type_partition_object;
static char sd_enum_name_apivalue_type_partitions[] = "apivalue_type_partitions";
static enum apivalue_type_partition sd_enum_value_apivalue_type_partitions = apivalue_type_partitions;

static struct type_enum_value sd_enum_values_apivalue_type_partition[] =
  {
    { sd_enum_name_apivalue_type_partition_function, &sd_enum_value_apivalue_type_partition_function },
    { sd_enum_name_apivalue_type_partition_object, &sd_enum_value_apivalue_type_partition_object },
    { sd_enum_name_apivalue_type_partitions, &sd_enum_value_apivalue_type_partitions }
  };

typedef struct alignof_enum_apivalue_type_partition { char c; enum apivalue_type_partition t; } alignof_enum_apivalue_type_partition;
static struct type_enum sd_enum_type_apivalue_type_partition =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_partition"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_partition),
          /* size */
          sizeof (enum apivalue_type_partition)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_partition",
    /* value_count */
    countof(sd_enum_values_apivalue_type_partition),
    /* values */
    sd_enum_values_apivalue_type_partition
  };

static char sd_enum_name_apivalue_type_sign_implementation_defined[] = "apivalue_type_sign_implementation_defined";
static enum apivalue_type_sign sd_enum_value_apivalue_type_sign_implementation_defined = apivalue_type_sign_implementation_defined;
static char sd_enum_name_apivalue_type_signed[] = "apivalue_type_signed";
static enum apivalue_type_sign sd_enum_value_apivalue_type_signed = apivalue_type_signed;
static char sd_enum_name_apivalue_type_unsigned[] = "apivalue_type_unsigned";
static enum apivalue_type_sign sd_enum_value_apivalue_type_unsigned = apivalue_type_unsigned;
static char sd_enum_name_apivalue_type_signs[] = "apivalue_type_signs";
static enum apivalue_type_sign sd_enum_value_apivalue_type_signs = apivalue_type_signs;

static struct type_enum_value sd_enum_values_apivalue_type_sign[] =
  {
    { sd_enum_name_apivalue_type_sign_implementation_defined, &sd_enum_value_apivalue_type_sign_implementation_defined },
    { sd_enum_name_apivalue_type_signed, &sd_enum_value_apivalue_type_signed },
    { sd_enum_name_apivalue_type_unsigned, &sd_enum_value_apivalue_type_unsigned },
    { sd_enum_name_apivalue_type_signs, &sd_enum_value_apivalue_type_signs }
  };

typedef struct alignof_enum_apivalue_type_sign { char c; enum apivalue_type_sign t; } alignof_enum_apivalue_type_sign;
static struct type_enum sd_enum_type_apivalue_type_sign =
  {
    /* integer */
    {
      /* arithmetic */
      {
        /* object */
        {
          /* type */
          {
            /* partition */
            apivalue_type_partition_object,
            /* nice_name */
            "enum apivalue_type_sign"
          },
          /* object_type */
          apivalue_type_object_arithmetic,
          /* alignment */
          align_hack(alignof_enum_apivalue_type_sign),
          /* size */
          sizeof (enum apivalue_type_sign)
        },
        /* arithmetic_type */
        apivalue_type_arithmetic_integer
      },
      /* integer_type */
      apivalue_type_integer_enum,
      /* sign */
      apivalue_type_sign_implementation_defined
    },
    /* tag */
    "apivalue_type_sign",
    /* value_count */
    countof(sd_enum_values_apivalue_type_sign),
    /* values */
    sd_enum_values_apivalue_type_sign
  };

typedef struct alignof_char { char c; char t; } alignof_char;
static struct type_integer sd_integer_char =
  {
    /* arithmetic */
    {
      /* object */
      {
        /* type */
        {
          /* partition */
          apivalue_type_partition_object,
          /* nice_name */
          "char"
        },
        /* object_type */
        apivalue_type_object_arithmetic,
        /* alignment */
        align_hack(alignof_char),
        /* size */
        sizeof (char)
      },
      /* arithmetic_type */
      apivalue_type_arithmetic_integer
    },
    /* integer_type */
    apivalue_type_integer_char,
    /* sign */
    apivalue_type_sign_implementation_defined
  };

typedef struct alignof_char_p { char c; char * t; } alignof_char_p;
static struct type_pointer sd_pointer_to_char =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "char *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_char_p),
      /* size */
      sizeof (char *)
    },
    /* referenced_type */
    &sd_integer_char.arithmetic.object.type
  };

static struct type_object sd_void =
  {
    /* type */
    {
      /* partition */
      apivalue_type_partition_object,
      /* nice_name */
      "void"
    },
    /* object_type */
    apivalue_type_object_void,
    /* alignment */
    0,
    /* size */
    0
  };

typedef struct alignof_void_p { char c; void * t; } alignof_void_p;
static struct type_pointer sd_pointer_to_void =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "void *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_void_p),
      /* size */
      sizeof (void *)
    },
    /* referenced_type */
    &sd_void.type
  };

typedef struct alignof_size_t { char c; size_t t; } alignof_size_t;
static struct type_integer sd_integer_size_t =
  {
    /* arithmetic */
    {
      /* object */
      {
        /* type */
        {
          /* partition */
          apivalue_type_partition_object,
          /* nice_name */
          "size_t"
        },
        /* object_type */
        apivalue_type_object_arithmetic,
        /* alignment */
        align_hack(alignof_size_t),
        /* size */
        sizeof (size_t)
      },
      /* arithmetic_type */
      apivalue_type_arithmetic_integer
    },
    /* integer_type */
    apivalue_type_integer_size_t,
    /* sign */
    apivalue_type_unsigned
  };

static struct type_struct_member sd_struct_members_type[] =
  {
    { "partition", &sd_enum_type_apivalue_type_partition.integer.arithmetic.object, offsetof(struct type, partition), 0 }
  };

typedef struct alignof_struct_type { char c; struct type t; } alignof_struct_type;
static struct type_struct sd_struct_type =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type),
      /* size */
      sizeof (struct type)
    },
    "type",
    /* member count */
    countof(sd_struct_members_type),
    /* members */
    sd_struct_members_type
  };

typedef struct alignof_struct_type_p { char c; struct type * t; } alignof_struct_type_p;
static struct type_pointer sd_pointer_to_struct_type =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_struct_type_p),
      /* size */
      sizeof (struct type *)
    },
    /* referenced_type */
    &sd_struct_type.object.type
  };

static struct type_struct_member sd_struct_members_type_function[] =
  {
    { "type", &sd_struct_type.object, offsetof(struct type_function, type), 0 }
  };

typedef struct alignof_struct_type_function { char c; struct type_function t; } alignof_struct_type_function;
static struct type_struct sd_struct_type_function =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_function"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_function),
      /* size */
      sizeof (struct type_function)
    },
    "type_function",
    /* member count */
    countof(sd_struct_members_type_function),
    /* members */
    sd_struct_members_type_function
  };

static struct type_struct_member sd_struct_members_type_object[] =
  {
    { "type", &sd_struct_type.object, offsetof(struct type_object, type), 0 },
    { "object_type", &sd_enum_type_apivalue_type_object.integer.arithmetic.object, offsetof(struct type_object, object_type), 0 },
    { "alignment", &sd_integer_size_t.arithmetic.object, offsetof(struct type_object, alignment), 0 },
    { "size", &sd_integer_size_t.arithmetic.object, offsetof(struct type_object, size), 0 }
  };

typedef struct alignof_struct_type_object { char c; struct type_object t; } alignof_struct_type_object;
static struct type_struct sd_struct_type_object =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_object"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_object),
      /* size */
      sizeof (struct type_object)
    },
    "type_object",
    /* member count */
    countof(sd_struct_members_type_object),
    /* members */
    sd_struct_members_type_object
  };

typedef struct alignof_struct_type_object_p { char c; struct type_object * t; } alignof_struct_type_object_p;
static struct type_pointer sd_pointer_to_struct_type_object =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_object *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_struct_type_object_p),
      /* size */
      sizeof (struct type_object *)
    },
    /* referenced_type */
    &sd_struct_type_object.object.type
  };

static struct type_struct_member sd_struct_members_type_array[] =
  {
    { "object", &sd_struct_type_object.object, offsetof(struct type_array, object), 0 },
    { "element_type", &sd_pointer_to_struct_type_object.object, offsetof(struct type_array, element_type), 0 },
    { "element_count", &sd_struct_type_object.object, offsetof(struct type_array, element_count), 0 }
  };

typedef struct alignof_struct_type_array { char c; struct type_array t; } alignof_struct_type_array;
static struct type_struct sd_struct_type_array =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_array"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_array),
      /* size */
      sizeof (struct type_array)
    },
    "type_array",
    /* member count */
    countof(sd_struct_members_type_array),
    /* members */
    sd_struct_members_type_array
  };

static struct type_struct_member sd_struct_members_type_arithmetic[] =
  {
    { "object", &sd_struct_type_object.object, offsetof(struct type_arithmetic, object), 0 },
    { "arithmetic_type", &sd_enum_type_apivalue_type_arithmetic.integer.arithmetic.object, offsetof(struct type_arithmetic, arithmetic_type), 0 }
  };

typedef struct alignof_struct_type_arithmetic { char c; struct type_arithmetic t; } alignof_struct_type_arithmetic;
static struct type_struct sd_struct_type_arithmetic =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_arithmetic"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_arithmetic),
      /* size */
      sizeof (struct type_arithmetic)
    },
    "type_arithmetic",
    /* member count */
    countof(sd_struct_members_type_arithmetic),
    /* members */
    sd_struct_members_type_arithmetic
  };

static struct type_struct_member sd_struct_members_type_floating[] =
  {
    { "arithmetic", &sd_struct_type_arithmetic.object, offsetof(struct type_floating, arithmetic), 0 },
    { "floating_type", &sd_enum_type_apivalue_type_floating.integer.arithmetic.object, offsetof(struct type_floating, floating_type), 0 }
  };

typedef struct alignof_struct_type_floating { char c; struct type_floating t; } alignof_struct_type_floating;
static struct type_struct sd_struct_type_floating =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_floating"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_floating),
      /* size */
      sizeof (struct type_floating)
    },
    "type_floating",
    /* member count */
    countof(sd_struct_members_type_floating),
    /* members */
    sd_struct_members_type_floating
  };

static struct type_struct_member sd_struct_members_type_integer[] =
  {
    { "arithmetic", &sd_struct_type_arithmetic.object, offsetof(struct type_integer, arithmetic), 0 },
    { "integer_type", &sd_enum_type_apivalue_type_integer.integer.arithmetic.object, offsetof(struct type_integer, integer_type), 0 },
    { "sign", &sd_enum_type_apivalue_type_sign.integer.arithmetic.object, offsetof(struct type_integer, sign), 0 }
  };

typedef struct alignof_struct_type_integer { char c; struct type_integer t; } alignof_struct_type_integer;
static struct type_struct sd_struct_type_integer =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_integer"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_integer),
      /* size */
      sizeof (struct type_integer)
    },
    "type_integer",
    /* member count */
    countof(sd_struct_members_type_integer),
    /* members */
    sd_struct_members_type_integer
  };

static struct type_struct_member sd_struct_members_type_enum_value[] =
  {
    { "name", &sd_pointer_to_char.object, offsetof(struct type_enum_value, name), 0 },
    { "value", &sd_pointer_to_void.object, offsetof(struct type_enum_value, value), 0 }
  };

typedef struct alignof_struct_type_enum_value { char c; struct type_enum_value t; } alignof_struct_type_enum_value;
static struct type_struct sd_struct_type_enum_value =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_enum_value"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_enum_value),
      /* size */
      sizeof (struct type_enum_value)
    },
    "type_enum_value",
    /* member count */
    countof(sd_struct_members_type_enum_value),
    /* members */
    sd_struct_members_type_enum_value
  };

typedef struct alignof_struct_type_enum_value_p { char c; struct type_enum_value * t; } alignof_struct_type_enum_value_p;
static struct type_pointer sd_pointer_to_struct_enum_value =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_enum_value *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_struct_type_enum_value_p),
      /* size */
      sizeof (struct enum_value *)
    },
    /* referenced_type */
    &sd_struct_type_enum_value.object.type
  };

static struct type_struct_member sd_struct_members_type_enum[] =
  {
    { "integer", &sd_struct_type_integer.object, offsetof(struct type_enum, integer), 0 },
    { "tag", &sd_pointer_to_char.object, offsetof(struct type_enum, tag), 0 },
    { "value_count", &sd_integer_size_t.arithmetic.object, offsetof(struct type_enum, value_count), 0 },
    { "values", &sd_pointer_to_struct_enum_value.object, offsetof(struct type_enum, values), 0 }
  };

typedef struct alignof_struct_type_enum { char c; struct type_enum t; } alignof_struct_type_enum;
static struct type_struct sd_struct_type_enum =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_enum"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_enum),
      /* size */
      sizeof (struct type_enum)
    },
    "type_enum",
    /* member count */
    countof(sd_struct_members_type_enum),
    /* members */
    sd_struct_members_type_enum
  };

static struct type_struct_member sd_struct_members_type_pointer[] =
  {
    { "object", &sd_struct_type_object.object, offsetof(struct type_pointer, object), 0 },
    { "referenced_type", &sd_pointer_to_struct_type.object, offsetof(struct type_pointer, referenced_type), 0 }
  };

typedef struct alignof_struct_type_pointer { char c; struct type_pointer t; } alignof_struct_type_pointer;
static struct type_struct sd_struct_type_pointer =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_pointer"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_pointer),
      /* size */
      sizeof (struct type_pointer)
    },
    "type_pointer",
    /* member count */
    countof(sd_struct_members_type_pointer),
    /* members */
    sd_struct_members_type_pointer
  };

/*
 * Reflection.  Here we see:
 *
 * row 0 reflects col. 0
 * row 1 reflects col. 1
 * row 2 reflects col. 2
 * row 3 reflects col. 3
 */
static struct type_struct_member sd_struct_members_type_struct_member[] =
  {
    { "name", &sd_pointer_to_char.object, offsetof(struct type_struct_member, name), 0 },
    { "type", &sd_pointer_to_struct_type_object.object, offsetof(struct type_struct_member, type), 0 },
    { "offset", &sd_integer_size_t.arithmetic.object, offsetof(struct type_struct_member, offset), 0 },
    { "bitfield_width", &sd_integer_size_t.arithmetic.object, offsetof(struct type_struct_member, bitfield_width), 0 }
  };

typedef struct alignof_struct_type_struct_member { char c; struct type_struct_member t; } alignof_struct_type_struct_member;
static struct type_struct sd_struct_type_struct_member =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_struct_member"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_struct_member),
      /* size */
      sizeof (struct type_struct_member)
    },
    "type_struct_member",
    /* member count */
    countof(sd_struct_members_type_struct_member),
    /* members */
    sd_struct_members_type_struct_member
  };

typedef struct alignof_struct_type_struct_member_p { char c; struct type_struct_member * t; } alignof_struct_type_struct_member_p;
static struct type_pointer sd_pointer_to_struct_type_struct_member =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_struct_member *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_struct_type_struct_member_p),
      /* size */
      sizeof (struct type_struct_member *)
    },
    /* referenced_type */
    &sd_struct_type_struct_member.object.type
  };

static struct type_struct_member sd_struct_members_type_struct[] =
  {
    { "object", &sd_struct_type_object.object, offsetof(struct type_struct, object), 0 },
    { "tag", &sd_pointer_to_char.object, offsetof(struct type_struct, tag), 0 },
    { "member_count", &sd_integer_size_t.arithmetic.object, offsetof(struct type_struct, member_count), 0 },
    { "members", &sd_pointer_to_struct_type_struct_member.object, offsetof(struct type_struct, members), 0 }
  };

typedef struct alignof_struct_type_struct { char c; struct type_struct t; } alignof_struct_type_struct;
static struct type_struct sd_struct_type_struct =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_struct"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_struct),
      /* size */
      sizeof (struct type_struct)
    },
    "type_struct",
    /* member count */
    countof(sd_struct_members_type_struct),
    /* members */
    sd_struct_members_type_struct
  };

static struct type_struct_member sd_struct_members_type_union_member[] =
  {
    { "name", &sd_pointer_to_char.object, offsetof(struct type_union_member, name), 0 },
    { "type", &sd_pointer_to_struct_type_object.object, offsetof(struct type_union_member, type), 0 }
  };

typedef struct alignof_struct_type_union_member { char c; struct type_union_member t; } alignof_struct_type_union_member;
static struct type_struct sd_struct_type_union_member =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_union_member"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_union_member),
      /* size */
      sizeof (struct type_union_member)
    },
    "type_union_member",
    /* member count */
    countof(sd_struct_members_type_union_member),
    /* members */
    sd_struct_members_type_union_member
  };

typedef struct alignof_struct_type_union_member_p { char c; struct type_union_member * t; } alignof_struct_type_union_member_p;
static struct type_pointer sd_pointer_to_struct_type_union_member =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_union_member *"
      },
      /* object_type */
      apivalue_type_object_pointer,
      /* alignment */
      align_hack(alignof_struct_type_union_member_p),
      /* size */
      sizeof (struct type_union_member *)
    },
    /* referenced_type */
    &sd_struct_type_union_member.object.type
  };

static struct type_struct_member sd_struct_members_type_union[] =
  {
    { "object", &sd_struct_type_object.object, offsetof(struct type_union, object), 0 },
    { "tag", &sd_pointer_to_char.object, offsetof(struct type_union, tag), 0 },
    { "member_count", &sd_integer_size_t.arithmetic.object, offsetof(struct type_union, member_count), 0 },
    { "members", &sd_pointer_to_struct_type_union_member.object, offsetof(struct type_union, members), 0 }
  };

typedef struct alignof_struct_type_union { char c; struct type_union t; } alignof_struct_type_union;
static struct type_struct sd_struct_type_union =
  {
    /* object */
    {
      /* type */
      {
        /* partition */
        apivalue_type_partition_object,
        /* nice_name */
        "struct type_union"
      },
      /* object_type */
      apivalue_type_object_struct,
      /* alignment */
      align_hack(alignof_struct_type_union),
      /* size */
      sizeof (struct type_union)
    },
    "type_struct",
    /* member count */
    countof(sd_struct_members_type_union),
    /* members */
    sd_struct_members_type_union
  };

static struct sd_type sd_types[] =
  {
    { "sd_enum_type_apivalue_type", &sd_enum_type_apivalue_type.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_arithmetic", &sd_enum_type_apivalue_type_arithmetic.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_floating", &sd_enum_type_apivalue_type_floating.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_integer", &sd_enum_type_apivalue_type_integer.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_object", &sd_enum_type_apivalue_type_object.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_partition", &sd_enum_type_apivalue_type_partition.integer.arithmetic.object.type },
    { "sd_enum_type_apivalue_type_sign", &sd_enum_type_apivalue_type_sign.integer.arithmetic.object.type },
    { "sd_integer_char", &sd_integer_char.arithmetic.object.type },
    { "sd_integer_size_t", &sd_integer_size_t.arithmetic.object.type },
    { "sd_pointer_to_char", &sd_pointer_to_char.object.type },
    { "sd_pointer_to_struct_enum_value", &sd_pointer_to_struct_enum_value.object.type },
    { "sd_pointer_to_struct_type", &sd_pointer_to_struct_type.object.type },
    { "sd_pointer_to_struct_type_object", &sd_pointer_to_struct_type_object.object.type },
    { "sd_pointer_to_struct_type_struct_member", &sd_pointer_to_struct_type_struct_member.object.type },
    { "sd_pointer_to_struct_type_union_member", &sd_pointer_to_struct_type_union_member.object.type },
    { "sd_pointer_to_void", &sd_pointer_to_void.object.type },
    { "sd_struct_type", &sd_struct_type.object.type },
    { "sd_struct_type_array", &sd_struct_type_array.object.type },
    { "sd_struct_type_arithmetic", &sd_struct_type_arithmetic.object.type },
    { "sd_struct_type_enum", &sd_struct_type_enum.object.type },
    { "sd_struct_type_enum_value", &sd_struct_type_enum_value.object.type },
    { "sd_struct_type_floating", &sd_struct_type_floating.object.type },
    { "sd_struct_type_function", &sd_struct_type_function.object.type },
    { "sd_struct_type_integer", &sd_struct_type_integer.object.type },
    { "sd_struct_type_object", &sd_struct_type_object.object.type },
    { "sd_struct_type_pointer", &sd_struct_type_pointer.object.type },
    { "sd_struct_type_struct", &sd_struct_type_struct.object.type },
    { "sd_struct_type_struct_member", &sd_struct_type_struct_member.object.type },
    { "sd_struct_type_union", &sd_struct_type_union.object.type },
    { "sd_struct_type_union_member", &sd_struct_type_union_member.object.type },
    { "sd_void", &sd_void.type },
    { NULL, NULL }
  };

static apifunction_type_compatible_types type_compatible_types;
static apifunction_type_fulltype_of_type type_fulltype_of_type;

static struct api_type api_type_defaults =
  {
    &api_type_initialize,
    &type_compatible_types,
    &type_fulltype_of_type,
    countof(sd_types) - 1,
    sd_types,
    0
  };

enum apivalue_type api_type_initialize(struct api_type * api)
  {
    unsigned char * hint[2];

    *api = api_type_defaults;
    /* Consider possible endianness */
    hint[0] = (unsigned char *) hint + 0;
    hint[1] = (unsigned char *) hint + 1;
    if (*(unsigned char *) (hint + 0) == *(unsigned char *) (hint + 1))
      /* Probably big-endian */
      api->pointer_hint = 1;
    return apivalue_type_success;
  }

/* This can be slow */
static enum apivalue_type type_compatible_types(struct api_type * api, struct type * type_a, struct type * type_b)
  {
    struct type_array * array_a;
    struct type_array * array_b;
    struct type_arithmetic * arithmetic_a;
    struct type_arithmetic * arithmetic_b;
    size_t count;
    struct type_enum * enum_a;
    struct type_enum * enum_b;
    struct type_enum_value * enum_value_a;
    struct type_enum_value * enum_value_b;
    struct type_floating * floating_a;
    struct type_floating * floating_b;
    size_t i;
    struct type_integer * integer_a;
    struct type_integer * integer_b;
    struct type_object * object_a;
    struct type_object * object_b;
    struct type_pointer * pointer_a;
    struct type_pointer * pointer_b;
    struct type_struct * struct_a;
    struct type_struct * struct_b;
    struct type_union * union_a;
    struct type_union * union_b;

    /* 2 types are compatible if they are the same type, believe it or not */
    if (type_a == type_b)
      return apivalue_type_success;
    if (type_a->partition != type_b->partition)
      return apivalue_type_error_not_compatible;
    switch (type_a->partition)
      {
        case apivalue_type_partition_function:
        /* TODO: Not yet implemented */
        return apivalue_type_error_not_compatible;

        case apivalue_type_partition_object:
        object_a = type_with_member_at_ptr(struct type_object, type, type_a);
        object_b = type_with_member_at_ptr(struct type_object, type, type_b);
        if (object_a->object_type != object_b->object_type)
          return apivalue_type_error_not_compatible;
        if (object_a->alignment != object_b->alignment)
          return apivalue_type_error_not_compatible;
        if (object_a->size!= object_b->size)
          return apivalue_type_error_not_compatible;
        switch (object_a->object_type)
          {
            case apivalue_type_object_void:
            return apivalue_type_success;

            case apivalue_type_object_arithmetic:
            arithmetic_a = type_with_member_at_ptr(struct type_arithmetic, object, object_a);
            arithmetic_b = type_with_member_at_ptr(struct type_arithmetic, object, object_b);
            if (arithmetic_a->arithmetic_type != arithmetic_b->arithmetic_type)
              return apivalue_type_error_not_compatible;
            switch (arithmetic_a->arithmetic_type)
              {
                case apivalue_type_arithmetic_integer:
                integer_a = type_with_member_at_ptr(struct type_integer, arithmetic, arithmetic_a);
                integer_b = type_with_member_at_ptr(struct type_integer, arithmetic, arithmetic_a);
                if (integer_a->integer_type != integer_b->integer_type)
                  return apivalue_type_error_not_compatible;
                if (integer_a->sign != integer_b->sign)
                  return apivalue_type_error_not_compatible;
                switch (integer_a->integer_type)
                  {
                    case apivalue_type_integer_char:
                    case apivalue_type_integer_short:
                    case apivalue_type_integer_int:
                    case apivalue_type_integer_long:
                    case apivalue_type_integer_longlong:
                    case apivalue_type_integer_size_t:
                    return apivalue_type_success;

                    case apivalue_type_integer_enum:
                    enum_a = type_with_member_at_ptr(struct type_enum, integer, integer_a);
                    enum_b = type_with_member_at_ptr(struct type_enum, integer, integer_b);
                    count = enum_a->value_count;
                    if (count != enum_b->value_count)
                      return apivalue_type_error_not_compatible;
                    if (enum_a->tag != enum_b->tag)
                      {
                        if (strcmp(enum_a->tag, enum_b->tag) != 0)
                          return apivalue_type_error_not_compatible;
                      }
                    if (enum_a->values != enum_b->values)
                      {
                        /* TODO: This expects the same order of values, which isn't right */
                        for (i = 0; i < count; ++i)
                          {
                            enum_value_a = enum_a->values + i;
                            enum_value_b = enum_b->values + i;
                            if (enum_value_a->name != enum_value_b->name)
                              {
                                if (strcmp(enum_value_a->name, enum_value_b->name) != 0)
                                  return apivalue_type_error_not_compatible;
                              }
                            if (enum_value_a->value != enum_value_b->value)
                              {
                                if (memcmp(enum_value_a->value, enum_value_b->value, object_a->size) != 0)
                                  return apivalue_type_error_not_compatible;
                              }
                          }
                      }
                    return apivalue_type_success;

                    default:
                    return apivalue_type_error_not_compatible;
                  }

                case apivalue_type_arithmetic_floating:
                floating_a = type_with_member_at_ptr(struct type_floating, arithmetic, arithmetic_a);
                floating_b = type_with_member_at_ptr(struct type_floating, arithmetic, arithmetic_b);
                if (floating_a->floating_type != floating_b->floating_type)
                  return apivalue_type_error_not_compatible;
                return apivalue_type_success;

                default:
                return apivalue_type_error_not_compatible;
              }
            return apivalue_type_error_not_compatible;

            case apivalue_type_object_array:
            array_a = type_with_member_at_ptr(struct type_array, object, object_a);
            array_b = type_with_member_at_ptr(struct type_array, object, object_b);
            if (array_a->element_count != array_b->element_count)
              return apivalue_type_error_not_compatible;
            return api->compatible_types(api, &array_a->element_type->type, &array_b->element_type->type);

            case apivalue_type_object_struct:
            struct_a = type_with_member_at_ptr(struct type_struct, object, object_a);
            struct_b = type_with_member_at_ptr(struct type_struct, object, object_b);
            if (struct_a->tag != struct_b->tag)
              {
                if (strcmp(struct_a->tag, struct_b->tag) != 0)
                  return apivalue_type_error_not_compatible;
              }
            if (struct_a->member_count != struct_b->member_count)
              return apivalue_type_error_not_compatible;
            /* TODO: Finish the member-checking, later */
            return apivalue_type_success;

            case apivalue_type_object_union:
            union_a = type_with_member_at_ptr(struct type_union, object, object_a);
            union_b = type_with_member_at_ptr(struct type_union, object, object_b);
            if (union_a->tag != union_b->tag)
              {
                if (strcmp(union_a->tag, union_b->tag) != 0)
                  return apivalue_type_error_not_compatible;
              }
            if (union_a->member_count != union_b->member_count)
              return apivalue_type_error_not_compatible;
            /* TODO: Finish the member-checking, later */
            return apivalue_type_success;

            case apivalue_type_object_pointer:
            pointer_a = type_with_member_at_ptr(struct type_pointer, object, object_a);
            pointer_b = type_with_member_at_ptr(struct type_pointer, object, object_b);
            return api->compatible_types(api, pointer_a->referenced_type, pointer_b->referenced_type);

            default:
            return apivalue_type_error_not_compatible;
          }
        return apivalue_type_error_not_compatible;

        default:
        return apivalue_type_error_not_compatible;
      }
    return apivalue_type_error_not_compatible;
  }

static struct type * type_fulltype_of_type(struct api_type * api, struct type * type)
  {
    struct type_arithmetic * arithmetic;
    struct type_integer * integer;
    struct type_object * object;

    (void) api;

    switch (type->partition)
      {
        case apivalue_type_partition_function:
        return &sd_struct_type_function.object.type;

        case apivalue_type_partition_object:
        object = type_with_member_at_ptr(struct type_object, type, type);
        switch (object->object_type)
          {
            case apivalue_type_object_void:
            return &sd_struct_type_object.object.type;

            case apivalue_type_object_arithmetic:
            arithmetic = type_with_member_at_ptr(struct type_arithmetic, object, object);
            switch (arithmetic->arithmetic_type)
              {
                case apivalue_type_arithmetic_integer:
                integer = type_with_member_at_ptr(struct type_integer, arithmetic, arithmetic);
                if (integer->integer_type == apivalue_type_integer_enum)
                  return &sd_struct_type_enum.object.type;
                return &sd_struct_type_integer.object.type;

                case apivalue_type_arithmetic_floating:
                return &sd_struct_type_floating.object.type;

                default:
                return NULL;
              }
            return NULL;

            case apivalue_type_object_array:
            return &sd_struct_type_array.object.type;

            case apivalue_type_object_struct:
            return &sd_struct_type_struct.object.type;

            case apivalue_type_object_union:
            return &sd_struct_type_union.object.type;

            case apivalue_type_object_pointer:
            return &sd_struct_type_pointer.object.type;

            default:
            return NULL;
          }
        return NULL;

        default:
        return NULL;
      }
    return NULL;
  }
