#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// XML spec
// https://www.w3.org/TR/REC-xml/
// (This parser does not follow the spec closely)
// test with:
// find ~ -name '*.xml' -exec sh -c "echo {} && ./parse_xml {} | wc -l" \;
// some XML constructs:
// https://stackoverflow.com/a/60802260

static inline int is_ws(char c){
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

#include <sys/stat.h>

int is_regular_file(const char *path){
    struct stat s;
    stat(path, &s);
    return S_ISREG(s.st_mode);
}

char* read_text(const char *path, size_t *n_bytes){
    if (!is_regular_file(path)) return NULL;
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    rewind(f);
    char *text = malloc(n + 1);
    size_t m = fread(text, 1, n, f);
    if (n != m){
        free(text);
        fclose(f);
        return NULL;
    }
    *n_bytes = n;
    text[n] = '\0';
    fclose(f);
    return text;
}

typedef struct XMLReader {
    char *data;
    size_t size;
    size_t pos;
} XMLReader;

void print_attribute(
    XMLReader *reader,
    size_t name_start,
    size_t name_end,
    size_t value_start,
    size_t value_end
){
    char *c = reader->data;
    printf("    attribute\n        %.*s = \"%.*s\"\n",
        (int)(name_end - name_start),
        c + name_start,
        (int)(value_end - value_start),
        c + value_start
    );
}

void print_element_start(XMLReader *reader, size_t name_start, size_t name_end){
    int length = name_end - name_start;

    printf("    <%.*s>\n", length, reader->data + name_start);
}

void print_element_end(
    XMLReader *reader,
    size_t name1_start,
    size_t name1_end,
    size_t name2_start,
    size_t name2_end
){
    size_t n1 = name1_end - name1_start;
    size_t n2 = name2_end - name2_start;

    if (n1 != n2 || 0 != memcmp(reader->data + name1_start, reader->data + name2_start, n1)){
        // TODO inline fprintf into xr_fail
        fprintf(stderr, "%.*s vs %.*s\n",
            (int)n1,
            reader->data + name1_start,
            (int)n2,
            reader->data + name2_start
        );
    }

    printf("    </%.*s>\n", (int)n1, reader->data + name1_start);
}

void print_data(XMLReader *reader, size_t start, size_t end){
    int all_whitespace = 1;
    for (size_t i = start; i < end; i++){
        if (!is_ws(reader->data[i])){
            all_whitespace = 0;
            break;
        }
    }
    if (all_whitespace) return;

    int length = end - start;

    printf("    data\n        %.*s\n", length, reader->data + start);
}

void xr_init(XMLReader *reader, const char *path){
    reader->data = read_text(path, &reader->size);
    reader->pos = 0;

    if (!reader->data){
        fprintf(stderr, "Failed to open %s\n", path);
    }
}

size_t xr_pos(XMLReader *reader){
    return reader->pos;
}

size_t xr_available(XMLReader *reader){
    return reader->size - xr_pos(reader);
}

char* xr_ptr(XMLReader *reader){
    return reader->data + xr_pos(reader);
}

void xr_fail(XMLReader *reader, const char *expected){
    size_t n = xr_available(reader);
    if (n > 100) n = 100;

    fprintf(stderr,
        "Failed to parse at byte %zu.\nExpected: %s\nRemaining bytes: %zu\nNext few bytes: %.*s",
        xr_pos(reader),
        expected,
        xr_available(reader),
        (int)n,
        xr_ptr(reader)
    );

    // crash hard for debugging
    if (1) *((int*)NULL) = 0xDEAD;
}

char xr_peek(XMLReader *reader){
    if (!xr_available(reader)) xr_fail(reader, "Not end of file.");
    return xr_ptr(reader)[0];
}

int xr_check(XMLReader *reader, const char *string){
    size_t n = strlen(string);

    if (xr_available(reader) < n) return 0;

    return 0 == memcmp(string, xr_ptr(reader), n);
}

char xr_next(XMLReader *reader){
    if (!xr_available(reader)) xr_fail(reader, "Not end of file.");
    char c = xr_peek(reader);
    reader->pos++;
    return c;
}

void xr_skip_ws(XMLReader *reader){
    while (xr_available(reader) && is_ws(xr_peek(reader))) xr_next(reader);
}

void xr_skip(XMLReader *reader, const char *string){
    if (!xr_check(reader, string)) xr_fail(reader, string);

    reader->pos += strlen(string);
}

int xr_check_skip(XMLReader *reader, const char *string){
    if (xr_check(reader, string)){
        reader->pos += strlen(string);
        return 1;
    }
    return 0;
}

int is_name_start_char(char sc){
    unsigned char c = *(unsigned char*)&sc;
    return
        c == ':' ||
        ('A' <= c && c <= 'Z') ||
        c == '_' ||
        ('a' <= c && c <= 'z') ||
        (0xC0 <= c && c <= 0xD6) ||
        (0xD8 <= c && c <= 0xF6);
}

int is_name_char(char sc){
    unsigned char c = *(unsigned char*)&sc;
    return is_name_start_char(c) ||
        c == '-' ||
        c == '.' ||
        ('0' <= c && c <= '9') ||
        c == 0xB7;
}

void xr_parse_name(XMLReader *reader){
    // TODO UTF-8 support
    if (!xr_available(reader)) xr_fail(reader, "NameStartChar but reached end of file");
    if (!is_name_start_char(xr_peek(reader))) xr_fail(reader, "NameChar");
    xr_next(reader);
    while (is_name_char(xr_peek(reader))) xr_next(reader);
}

void xr_parse_string(XMLReader *reader){
    // TODO handle escaped stuff
    char c = xr_peek(reader);
    if (c != '"' && c != '\'') xr_fail(reader, "quote character when parsing string.");
    xr_next(reader);

    while (1){
        if (!xr_available(reader)) xr_fail(reader, "\" but reached end of file.");

        if (xr_peek(reader) == c){
            xr_next(reader);
            return;
        }

        xr_next(reader);
    }
}

void xr_parse_attribute(XMLReader *reader){
    size_t name_start = xr_pos(reader);
    xr_parse_name(reader);
    size_t name_end = xr_pos(reader);

    xr_skip_ws(reader);
    xr_skip(reader, "=");
    xr_skip_ws(reader);

    size_t value_start = xr_pos(reader) + 1;
    xr_parse_string(reader);
    size_t value_end = xr_pos(reader) - 1;

    print_attribute(reader, name_start, name_end, value_start, value_end);
}

void xr_parse_prolog(XMLReader *reader){
    printf("prolog\n");
    xr_skip(reader, "<?xml");
    xr_skip_ws(reader);

    while (1){
        if (!xr_available(reader)) xr_fail(reader, "?> while parsing prolog.");

        if (xr_check_skip(reader, "?>")) return;

        xr_parse_attribute(reader);
        xr_skip_ws(reader);
    }
    printf("\n");
}

void xr_parse_comment(XMLReader *reader){
    xr_skip(reader, "<!--");

    while (1){
        if (!xr_available(reader)) xr_fail(reader, "--> while parsing comment");

        if (xr_check_skip(reader, "-->")) return;

        xr_next(reader);
    }
}

void xr_parse_char_data(XMLReader *reader){
    while (xr_available(reader)){

        // TODO strip comments?
        if (xr_check(reader, "<!--")){
            xr_parse_comment(reader);
            continue;
        }

        if (xr_check(reader, "<")) return;

        xr_next(reader);
    }
}

void xr_parse_element(XMLReader *reader);

void xr_parse_element_content(XMLReader *reader){
    while (xr_available(reader)){
        xr_skip_ws(reader);

        if (xr_check(reader, "</")) return;

        if (xr_check(reader, "<")){
            xr_parse_element(reader);
            continue;
        }

        size_t data_start = xr_pos(reader);
        xr_parse_char_data(reader);
        size_t data_end = xr_pos(reader);

        print_data(reader, data_start, data_end);
    }
}

void xr_parse_declaration(XMLReader *reader, const char *prefix);

void xr_parse_entities(XMLReader *reader){
    // TODO parse entities properly
    xr_skip(reader, "[");

    while (1){
        xr_skip_ws(reader);

        if (!xr_available(reader)) xr_fail(reader, "] while parsing entities.");

        if (xr_check_skip(reader, "]")) return;

        xr_parse_declaration(reader, "<!ENTITY");
    }
}

void xr_parse_declaration(XMLReader *reader, const char *prefix){
    printf("%s declaration>\n", prefix);
    xr_skip(reader, prefix);

    while (1){
        xr_skip_ws(reader);

        if (!xr_available(reader)) xr_fail(reader, "> while parsing DOCTYPE declaration");

        if (xr_check_skip(reader, ">")) return;

        char c = xr_peek(reader);
        if (c == '"' || c == '\''){
            xr_parse_string(reader);
            continue;
        }

        // TODO comments

        if (xr_check(reader, "[")){
            xr_parse_entities(reader);
            continue;
        }

        xr_parse_name(reader);
    }
}

void xr_parse_processing_instruction(XMLReader *reader){
    printf("processing instruction\n");
    xr_skip(reader, "<?");
    size_t name_start = xr_pos(reader);
    xr_parse_name(reader);
    size_t name_end = xr_pos(reader);
    printf("    %.*s\n", (int)(name_end - name_start), reader->data + name_start);

    while (1){
        xr_skip_ws(reader);

        if (!xr_available(reader)) xr_fail(reader, "?> while parsing processing instruction.");

        if (xr_check_skip(reader, "?>")) return;

        xr_parse_attribute(reader);
    }
}

void xr_parse_cdata(XMLReader *reader){
    xr_skip(reader, "<![CDATA[");

    while (1){
        if (!xr_available(reader)) xr_fail(reader, "]]> while parsing <![CDATA[");

        if (xr_check_skip(reader, "]]>")) return;

        xr_next(reader);
    }
}

void xr_parse_element(XMLReader *reader){
    if (xr_check(reader, "<!DOCTYPE")){
        xr_parse_declaration(reader, "<!DOCTYPE");
        return;
    }

    if (xr_check(reader, "<?")){
        xr_parse_processing_instruction(reader);
        return;
    }

    if (xr_check(reader, "<!--")){
        xr_parse_comment(reader);
        return;
    }

    if (xr_check(reader, "<![CDATA[")){
        xr_parse_cdata(reader);
        return;
    }

    xr_skip(reader, "<");

    printf("element\n");

    // parse element name: <name ...
    size_t name1_start = xr_pos(reader);
    xr_parse_name(reader);
    size_t name1_end = xr_pos(reader);

    print_element_start(reader, name1_start, name1_end);

    while (1){
        xr_skip_ws(reader);

        if (!xr_available(reader)) xr_fail(reader, "> while parsing element.");

        // end of start tag: <name ... / >
        if (xr_check_skip(reader, "/")){
            xr_skip_ws(reader);
            xr_skip(reader, ">");
            return;
        }

        // fully parse start tag: <name ... >
        if (xr_check_skip(reader, ">")){
            xr_parse_element_content(reader);

            xr_skip_ws(reader);
            xr_skip(reader, "</");

            // check if start tag name matches end tag name
            size_t name2_start = xr_pos(reader);
            xr_parse_name(reader);
            size_t name2_end = xr_pos(reader);

            print_element_end(reader, name1_start, name1_end, name2_start, name2_end);

            xr_skip_ws(reader);
            xr_skip(reader, ">");
            return;
        }

        xr_parse_attribute(reader);
    }
}

void xr_parse(XMLReader *reader){
    // skip UTF-8 byteorder mark if it exists
    xr_check_skip(reader, "\xEF" "\xBB" "\xBF");
    xr_skip_ws(reader);

    if (xr_check(reader, "<?xml")){
        xr_parse_prolog(reader);
    }

    while (1){
        xr_skip_ws(reader);

        if (!xr_available(reader)) break;

        xr_parse_element(reader);
    }
}

void xr_free(XMLReader *reader){
    free(reader->data);
    reader->data = NULL;
    reader->pos = 0;
    reader->size = 0;
}

int main(int argc, char **argv){
    const char *path = "test.svg";

    if (argc >= 2){
        path = argv[1];
    }

    XMLReader reader;
    xr_init(&reader, path);
    xr_parse(&reader);
    xr_free(&reader);

    return 0;
}
