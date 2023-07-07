# Coding style

The NashiOS coding style is a set of rules for writing code. It provides a set of guidelines for creating code that is easy to read and understand and make the codebase more useful.

For spaces, parentheses, brace placement, etc, use the format specified in the [.clang-format](/.clang-format) file.

## Naming

You should use full worlds for names, not abbreviations or single letters. The name should be descriptive and unambiguous (except for standard abbreviations).

#### Good:

```c
size_t buffer_length = 0;
uint32_t gdt_address = 0;
```

#### Bad:

```c
size_t b_len = 0;
size_t addr = 0;
```

Use SCREAMING_CASE for constants and snake_case for variable, function names and structures.

#### Good:

```c
#define GLOBAL_DESCRIPTOR_ENTRIES 6
struct global_description 
{
    uint32_t address;
};
struct global_description gdt = {};
size_t length = -1;
```

#### Bad:

```c
#define global_DEscriptor_Entries 6
struct GlobalDescription 
{
    uint32_t address;
};
struct global_description GDT = {};
size_t totalLength = -1;
```
