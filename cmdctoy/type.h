/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_TYPE
#define INC_TYPE

#include <stddef.h>

enum apivalue_type
  {
    apivalue_type_success,
    apivalue_type_error_buffer_too_small,
    apivalue_type_error_not_compatible,
    apivalue_type_zero = 0
  };

enum apivalue_type_arithmetic
  {
    apivalue_type_arithmetic_integer,
    apivalue_type_arithmetic_floating,
    apivalue_type_arithmetics
  };

enum apivalue_type_floating
  {
    apivalue_type_floating_float,
    apivalue_type_floating_double,
    apivalue_type_floating_long_double,
    apivalue_type_floatings
  };

enum apivalue_type_integer
  {
    apivalue_type_integer_char,
    apivalue_type_integer_short,
    apivalue_type_integer_int,
    apivalue_type_integer_long,
    apivalue_type_integer_longlong,
    apivalue_type_integer_size_t,
    apivalue_type_integer_enum,
    apivalue_type_integers
  };

enum apivalue_type_object
  {
    apivalue_type_object_void,
    apivalue_type_object_arithmetic,
    apivalue_type_object_array,
    apivalue_type_object_struct,
    apivalue_type_object_union,
    apivalue_type_object_pointer,
    apivalue_type_objects
  };

enum apivalue_type_partition
  {
    apivalue_type_partition_function,
    apivalue_type_partition_object,
    apivalue_type_partitions
  };

enum apivalue_type_sign
  {
    apivalue_type_sign_implementation_defined,
    apivalue_type_signed,
    apivalue_type_unsigned,
    apivalue_type_signs
  };

struct api_type;
struct sd_type;
struct type;
struct type_array;
struct type_arithmetic;
struct type_enum;
struct type_floating;
struct type_function;
struct type_integer;
struct type_object;
struct type_struct;
struct type_struct_alignment;
struct type_struct_member;
struct type_union;
struct type_union_alignment;
struct type_union_members;

typedef enum apivalue_type apifunction_type_api_initialize(struct api_type *);
typedef enum apivalue_type apifunction_type_compatible_types(struct api_type *, struct type *, struct type *);
typedef struct type * apifunction_type_fulltype_of_type(struct api_type *, struct type *);

extern apifunction_type_api_initialize api_type_initialize;

struct api_type
  {
    apifunction_type_api_initialize * api_initialize;
    apifunction_type_compatible_types * compatible_types;
    apifunction_type_fulltype_of_type * fulltype_of_type;
    size_t type_count;
    struct sd_type * types;
    int pointer_hint;
  };

struct sd_type
  {
    char * identifier;
    struct type * type;
  };

struct type
  {
    enum apivalue_type_partition partition;
    char * nice_name;
  };

struct type_function
  {
    struct type type;
  };

struct type_object
  {
    struct type type;
    enum apivalue_type_object object_type;
    size_t alignment;
    size_t size;
  };

struct type_array
  {
    struct type_object object;
    struct type_object * element_type;
    size_t element_count;
  };

struct type_arithmetic
  {
    struct type_object object;
    enum apivalue_type_arithmetic arithmetic_type;
  };

struct type_floating
  {
    struct type_arithmetic arithmetic;
    enum apivalue_type_floating floating_type;
  };

struct type_integer
  {
    struct type_arithmetic arithmetic;
    enum apivalue_type_integer integer_type;
    enum apivalue_type_sign sign;
  };

struct type_enum
  {
    struct type_integer integer;
    char * tag;
    size_t value_count;
    struct type_enum_value * values;
  };

struct type_enum_value
  {
    char * name;
    void * value;
  };

/* Not intended for use outside of sizeof and offsetof */
struct type_enum_alignment
  {
    struct type_enum inner;
    struct type_enum_value values[1];
  };

struct type_pointer
  {
    struct type_object object;
    struct type * referenced_type;
  };

struct type_struct
  {
    struct type_object object;
    char * tag;
    size_t member_count;
    struct type_struct_member * members;
  };

struct type_struct_member
  {
    char * name;
    struct type_object * type;
    /* The offset has a special meaning, for bit-fields */
    size_t offset;
    size_t bitfield_width;
  };

/* Not intended for use outside of sizeof and offsetof */
struct type_struct_alignment
  {
    struct type_struct inner;
    struct type_struct_member members[1];
  };

struct type_union
  {
    struct type_object object;
    char * tag;
    size_t member_count;
    struct type_union_mmeber * members;
  };

struct type_union_member
  {
    char * name;
    struct type_object * type;
  };

/* Not intended for use outside of sizeof and offsetof */
struct type_union_alignment
  {
    struct type_union inner;
    struct type_union_member members[1];
  };

#endif /* INC_TYPE */
