#include "nbt_parser.h"

#include <stdio.h>

#include "registry_utils.h"

#include <stdio.h>
#include <stdlib.h>

NBT_Tag parse_nbt_root(FILE *file) {
    char str[128];

    if (!fgets(str, sizeof(str), file)) {
        fprintf(stderr, "Error reading file.\n");
        exit(1);
    }

    char *clean_str = skip_whitespaces(str);
    clean_str[strlen(clean_str)] = '\0';

    NBT_Tag tag = create_tag(clean_str);

    if (tag.type == TAG_Compound) {
        parse_compound(file, &tag);
    }

    return tag;
}

void parse_compound(FILE *file, NBT_Tag *compound) {
    compound->value.compound.tags = malloc(sizeof(NBT_Tag *) * compound->value.compound.length);

    int tag_count = 0;

    for (int i = 0; i < compound->value.compound.length; i++) {
        char str[128];

        // Read the next line
        if (!fgets(str, sizeof(str), file)) {
            fprintf(stderr, "Unexpected end of file.\n");
            exit(1);
        }

        char *clean_str = skip_whitespaces(str);
        clean_str[strcspn(clean_str, "\r\n")] = '\0';


        if (*clean_str == '\0' || *clean_str == '{' || *clean_str == '}') {
            i--; //Not counting the curly brackets as iterations
            continue;
        }

        // Allocate and create the tag
        NBT_Tag *tag = malloc(sizeof(NBT_Tag));
        *tag = create_tag(clean_str);
        compound->value.compound.tags[tag_count++] = tag;


        if (tag->type == TAG_Compound) {
            parse_compound(file, tag);
        } else if (tag->type == TAG_List) {
            parse_list(file, tag);
        }
    }

    compound->value.compound.length = tag_count;
}

void parse_list(FILE *file, NBT_Tag *list) {
    //TODO TOO LAZY LOL
}


char *extract_tag_type(char *str) {
    char *after_type_index = strchr(str, '(');
    if (after_type_index == NULL) {
        return "";
    }

    size_t type_length = after_type_index - str;
    char *type = malloc(type_length + 1);

    strncpy(type, str, type_length);
    type[type_length] = '\0';

    //NBT_Tag tag = lexer(str, type);
    return type;
    free(type);
}

char *extract_name(char *str) {
    str = skip_whitespaces(str);


    char *name_start = strchr(str, '\'');
    if (name_start == NULL) {
        return NULL;
    }

    char *name_end = strchr(name_start + 1, '\'');
    size_t name_length = name_end - name_start - 1;
    char *name = malloc(name_length + 1);

    strncpy(name, name_start + 1, name_length);
    name[name_length] = '\0';

    return name;
}


int extract_entry_count(char *line) {
    char *index = strchr(line, ':');
    if (!index) {
        printf("Colon not found\n");
        return -1;
    }

    index = strchr(index + 1, ' ');
    if (!index) {
        printf("No space found after colon\n");
        return -1;
    }


    int count = atoi(index + 1);
    return count;
}

NBT_Tag lexer(char *line, char *tag) {
    if (strcmp(tag, "TAG_End") == 0) {
    }
    if (strcmp(tag, "TAG_Byte") == 0) {
        return create_byte_nbt_tag(line);
    }
    if (strcmp(tag, "TAG_Short") == 0) {
    }
    if (strcmp(tag, "TAG_Int") == 0) {
    }
    if (strcmp(tag, "TAG_Long") == 0) {
    }
    if (strcmp(tag, "TAG_Float") == 0) {
    }
    if (strcmp(tag, "TAG_Double") == 0) {
    }
    if (strcmp(tag, "TAG_Byte_Array") == 0) {
    }
    if (strcmp(tag, "TAG_String") == 0) {
        return create_string_nbt_tag(line);
    }
    if (strcmp(tag, "TAG_List") == 0) {
        //return parse_list(line, tag);
    }
    if (strcmp(tag, "TAG_Compound") == 0) {
        return create_compound_tag(line);
    }
    if (strcmp(tag, "TAG_Int_Array") == 0) {
    }
    if (strcmp(tag, "TAG_Long_Array") == 0) {
    }
}

NBT_Tag create_byte_nbt_tag(char *line) {
    NBT_Tag byte;
    byte.type = TAG_Byte;
    byte.name = extract_name(line);
    byte.value.byte = extract_byte_numeric_value(line);
    return byte;
}

int extract_byte_numeric_value(char *line) {
    char *colon = strchr(line, ':');
    if (!colon) {
        return -1;
    }
    char *number_start = strchr(colon + 1, ' ');
    int number = atoi(number_start + 1);
    return number;
}

NBT_Tag create_tag(char *line) {
    char *tag = extract_tag_type(line);
    NBT_Tag new_tag = lexer(line, tag);
    return new_tag;
}

NBT_Tag create_compound_tag(char *line) {
    NBT_Tag compound;
    NBT_Compound compound_tag;

    compound.type = TAG_Compound;
    compound.name = extract_name(line);
    compound_tag.length = extract_entry_count(line);
    compound_tag.tags = NULL;
    compound.value.compound = compound_tag;

    return compound;
}

void add_tag_to_compound(NBT_Tag *compound, NBT_Tag *new_tag) {
    if (compound->type == TAG_Compound) {
        compound->value.compound.tags = realloc(compound->value.compound.tags,
                                                (compound->value.compound.length + 1) * sizeof(NBT_Tag *));


        compound->value.compound.tags[compound->value.compound.length] = new_tag;
        compound->value.compound.length++;
    }
}

int get_string_length(char *string) {
    char *colon = strchr(string, ':');
    if (!colon) {
        return -1;
    }
    char *name_start_index = strchr(colon, '\'');
    if (!name_start_index) return -1;

    char *name_end_index = strchr(name_start_index + 1, '\'');
    if (!name_end_index) return -1;

    int length = name_end_index - name_start_index - 1;


    return length;
}


NBT_Tag create_string_nbt_tag(const char *str) {
    NBT_Tag tag;
    tag.type = TAG_String;
    tag.name = extract_name(str);

    tag.value.string.length = get_string_length(str);
    tag.value.string.data = malloc(tag.value.string.length + 1);

    if (!tag.value.string.data) {
        perror("Memory allocation failed");
        exit(1);
    }

    tag.value.string.data = extract_string_data(str);
    return tag;
}

char *extract_string_data(char *str) {
    char *colon = strchr(str, ':');
    if (colon == NULL) {
        return NULL;
    }
    colon++;
    while (*colon == ' ') {
        colon++;
    }

    char *end = strrchr(colon, '\'');
    if (end == NULL) {
        return NULL;
    }

    size_t length = end - colon;

    char *result = malloc(length + 1);
    if (result == NULL) {
        return NULL;
    }

    strncpy(result, colon, length);
    result[length] = '\0';

    return result;
}

int main() {
    FILE *file = fopen("F:\\MinecraftserverC\\Registries\\test.nbt", "rb");
    int size;

    NBT_Tag root = parse_nbt_root(file);
    toString_nbt(&root, 0);
    char *bytes = serialize_nbt(root, &size);
    for (int i = 0; i < size; i++) {
        printf("%02X ", (unsigned char) bytes[i]);
    }
    fclose(file);
    return 0;
}


char *skip_whitespaces(char *str) {
    while (isspace((unsigned char) *str)) {
        str++;
    }
    return str;
}

void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

void toString_nbt(NBT_Tag *tag, int depth) {
    if (!tag) return;

    print_indent(depth);

    // Print tag name if it has one
    if (tag->name) {
        printf("%s: ", tag->name);
    }

    // Handle different types of NBT tags
    switch (tag->type) {
        case TAG_Byte:
            printf("TAG_Byte(%d)\n", tag->value.byte);
            break;
        case TAG_Short:
            printf("TAG_Short(%d)\n", tag->value.short_);
            break;
        case TAG_Int:
            printf("TAG_Int(%d)\n", tag->value.int_);
            break;
        case TAG_Long:
            printf("TAG_Long(%lld)\n", (long long) tag->value.long_);
            break;
        case TAG_Float:
            printf("TAG_Float(%f)\n", tag->value.float_);
            break;
        case TAG_Double:
            printf("TAG_Double(%f)\n", tag->value.double_);
            break;
        case TAG_String:
            printf("TAG_String(%s')\n", tag->value.string.data);
            break;
        case TAG_Byte_Array:
            printf("TAG_Byte_Array[%d]\n", tag->value.byte_array.length);
            break;
        case TAG_Int_Array:
            printf("TAG_Int_Array[%d]\n", tag->value.int_array.length);
            break;
        case TAG_Long_Array:
            printf("TAG_Long_Array[%d]\n", tag->value.long_array.length);
            break;
        case TAG_List:
            printf("TAG_List[%d] {\n", tag->value.list.length);
            for (int i = 0; i < tag->value.list.length; i++) {
                toString_nbt((NBT_Tag *) tag->value.list.elements + i, depth + 1);
            }
            print_indent(depth);
            printf("}\n");
            break;
        case TAG_Compound:
            printf("TAG_Compound[%d] {\n", tag->value.compound.length);
            for (int i = 0; i < tag->value.compound.length; i++) {
                toString_nbt(tag->value.compound.tags[i], depth + 1);
            }
            print_indent(depth);
            printf("}\n");
            break;
        default:
            printf("Unknown Tag Type\n");
            break;
    }
}

int get_type(NBT_Tag tag) {
    switch (tag.type) {
        case TAG_End: return 0;
        case TAG_Byte: return 1;
        case TAG_Short: return 2;
        case TAG_Int: return 3;
        case TAG_Long: return 4;
        case TAG_Float: return 5;
        case TAG_Double: return 6;
        case TAG_Byte_Array: return 7;
        case TAG_String: return 8;
        case TAG_List: return 9;
        case TAG_Compound: return 10;
        case TAG_Int_Array: return 11;
        case TAG_Long_Array: return 12;
        default: return -1; // Invalid type
    }
}

void write_uint16(char *buffer, int *offset, uint16_t value) {
    buffer[*offset] = (value >> 8) & 0xFF; // High byte
    buffer[*offset + 1] = value & 0xFF; // Low byte
    *offset += 2;
}


void nbt_to_bytes(NBT_Tag *tag, char *buffer, int *offset) {
    buffer[*offset] = get_type(*tag);
    (*offset)++;

    if (tag->name) {
        uint16_t name_len = strlen(tag->name);
        write_uint16(buffer, offset, name_len); //name length
        memcpy(&buffer[*offset], tag->name, name_len); //name
        *offset += name_len;
    }

    switch (tag->type) {
        case TAG_Byte:
            buffer[*offset] = tag->value.byte;
            (*offset)++;
            break;
        case TAG_Short:
            write_uint16(buffer, offset, tag->value.short_);
            break;
        case TAG_Int:
            memcpy(&buffer[*offset], &tag->value.int_, sizeof(int32_t));
            *offset += 4;
            break;
        case TAG_Long:
            memcpy(&buffer[*offset], &tag->value.long_, sizeof(int64_t));
            *offset += 8;
            break;
        case TAG_String:
            write_uint16(buffer, offset, tag->value.string.length);
            memcpy(&buffer[*offset], tag->value.string.data, tag->value.string.length);
            *offset += tag->value.string.length;
            break;
        case TAG_Compound:
            for (int i = 0; i < tag->value.compound.length; i++) {
                nbt_to_bytes(tag->value.compound.tags[i], buffer, offset);
            }
            buffer[*offset] = TAG_End; // Compound ends with TAG_End (0x00)
            (*offset)++;
            break;
        case TAG_List:
            buffer[*offset] = tag->value.list.element_type; // List type
            (*offset)++;
            write_uint16(buffer, offset, tag->value.list.length); // List length
            for (int i = 0; i < tag->value.list.length; i++) {
                nbt_to_bytes(&((NBT_Tag *) tag->value.list.elements)[i], buffer, offset);
            }
            break;
        default:
            printf("Unsupported tag type\n");
    }
}


char *serialize_nbt(NBT_Tag root, int *size) {
    *size = 1024; // Assume a max buffer size (adjust as needed)
    char *buffer = malloc(*size);
    int offset = 0;
    nbt_to_bytes(&root, buffer, &offset);
    *size = offset; // Set final buffer size
    return buffer;
}
