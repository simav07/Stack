# Iron Stack

A dynamic stack implemented in C with a focus on reliability, data integrity, and error detection.

The project provides automatic memory management, stack state validation, memory protection, and error logging.

The main goal is to make the stack not only functional, but also able to detect invalid states and memory corruption.

## Features

- Dynamic memory allocation
- Automatic stack resizing
- Size and capacity validation
- Pointer and memory checks
- Canary protection for the stack structure
- Canary protection for allocated memory
- Poison values for unused memory
- DJB2 hash for data integrity checks
- Centralized validation with `StackVerify()`
- Error codes and error descriptions
- Error and stack state logging
- `StackDump()` for debugging
- Conditional compilation for debug features
- `ssize_t` for checking negative `size` and `capacity`

## Reliability

The stack uses several levels of protection:

```text
    Stack  -> Struct Canary
           -> Heap Canary   -> StackVerify (Pointers, Size/Capacity, Data Hash)
                                                                               ->Logging
```

These checks help detect:

- memory corruption;
- buffer overflows;
- invalid pointers;
- incorrect `size` or `capacity`;
- changes in stack data;
- invalid stack state.

## Canary Protection

Canary values are used to detect memory corruption.

They are placed:

- inside the `stack_t` structure;
- at the boundaries of the allocated memory.

If a canary value changes, the stack may have been damaged.

```text
┌──────────────┬──────────────────────────┬──────────────┐
│ Left Canary  │       Stack Data         │ Right Canary │
└──────────────┴──────────────────────────┴──────────────┘
```

The canaries are checked by `StackVerify()`.

## Poison Values

Unused memory is filled with a special `STACK_POISON` value.

This helps distinguish between:

- active stack elements;
- unused memory;
- memory released after `StackPop()`.

Poison values provide an additional level of memory protection and make debugging easier.

## Data Integrity

The project uses the DJB2 algorithm to check the integrity of stack data.

The hash is calculated for the active part of the stack:

```text
data[0 ... size - 1]
```

The calculated value is stored in the stack and checked later.

If the stack data is changed directly, the hash can detect this change.

DJB2 is used to detect accidental data corruption, not as a cryptographic protection.

## StackVerify

`StackVerify()` is the main function for checking the stack state.

It checks:

- stack pointers;
- `data` and `dataBegin`;
- `size` and `capacity`;
- canary values;
- allocated memory;
- data integrity using the hash.

If a problem is found, the function returns an error code.

This makes error checking centralized and easier to maintain.

