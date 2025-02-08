#ifndef NBT_PARSER_H
#define NBT_PARSER_H
#include <stdint.h>
#include <stdio.h>

typedef enum {
    TAG_End = 0,
    TAG_Byte = 1,
    TAG_Short = 2,
    TAG_Int = 3,
    TAG_Long = 4,
    TAG_Float = 5,
    TAG_Double = 6,
    TAG_Byte_Array = 7,
    TAG_String = 8,
    TAG_List = 9,
    TAG_Compound = 10,
    TAG_Int_Array = 11,
    TAG_Long_Array = 12
} NBT_TagType;


typedef struct NBT_Tag NBT_Tag; // Forward declaration

typedef struct {
    int length;
    NBT_TagType element_type; // Only relevant for lists
    void* elements; // Array of elements (allocated dynamically)
} NBT_List;

typedef struct {
    int length;
    NBT_Tag** tags; // Array of pointers to named tags (dynamically allocated)
} NBT_Compound;

typedef union {
    int8_t byte;
    int16_t short_;
    int32_t int_;
    int64_t long_;
    float float_;
    double double_;
    struct { int length; int8_t* data; } byte_array;
    struct { int length; char* data; } string;
    NBT_List list;
    NBT_Compound compound;
    struct { int length; int32_t* data; } int_array;
    struct { int length; int64_t* data; } long_array;
} NBT_Value;
struct NBT_Tag {
    NBT_TagType type;
    char* name; // NULL if unnamed (like in lists)
    NBT_Value value;
};
void read_nbt_string(FILE *file);
void *skip_tag_underscore(char *str);
char *extract_tag_type(char *str);
NBT_Tag lexer(char *line, char *tag);
char *extract_name(char *str);
int extract_entry_count(char *line) ;
NBT_Tag create_compound_tag(char* line);
NBT_Tag create_tag(char *line);
NBT_Tag create_string_nbt_tag(const char *str);
char* skip_whitespaces(char *str);
NBT_Tag parse_nbt_root(FILE *file);
void parse_compound(FILE *file, NBT_Tag *compound);
void parse_list(FILE *file, NBT_Tag *list);
void toString_nbt(NBT_Tag *tag, int depth);
void print_indent(int depth);
int get_type(NBT_Tag tag);
void write_uint16(char *buffer, int *offset, uint16_t value);
void nbt_to_bytes(NBT_Tag *tag, char *buffer, int *offset);
char *serialize_nbt(NBT_Tag root, int *size);
char *extract_string_data(char *str);
NBT_Tag create_byte_nbt_tag(char *line);
int extract_byte_numeric_value(char *line);
#endif //NBT_PARSER_H
