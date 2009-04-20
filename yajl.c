#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
#include "ruby.h"

static int parse_null(void * ctx) {
    return 1;
}

static int parse_boolean(void * ctx, int boolean) {
    return 1;
}

static int parse_number(void * ctx, const char * s, unsigned int l) {
    return 1;
}

static int parse_string(void * ctx, const unsigned char * stringVal, unsigned int stringLen) {
    return 1;
}

static int parse_map_key(void * ctx, const unsigned char * stringVal, unsigned int stringLen) {
    return 1;
}

static int parse_start_map(void * ctx) {
    return 1;
}

static int parse_end_map(void * ctx) {
    return 1;
}

static int parse_start_array(void * ctx) {
    return 1;
}

static int parse_end_array(void * ctx) {
    return 1;
}

static yajl_callbacks callbacks = {
    parse_null,
    parse_boolean,
    NULL,
    NULL,
    parse_number,
    parse_string,
    parse_start_map,
    parse_map_key,
    parse_end_map,
    parse_start_array,
    parse_end_array
};

// ruby-specific awesomeness

static VALUE t_parse(VALUE self, VALUE io) {
    yajl_handle hand;
    yajl_status stat;
    int bufferSize = 1024;
    yajl_parser_config cfg = {1, 1};
    VALUE ctx = rb_hash_new();
    
    // allocate our parser
    hand = yajl_alloc(&callbacks, &cfg, NULL, (void *) &ctx);
    VALUE parsed = rb_str_new2("");
    ID io_read = rb_intern("read");
    VALUE rbufsize = INT2NUM(bufferSize);
    
    // now parse from the IO
    while (rb_io_eof(io) == Qfalse) {
        rb_funcall(io, io_read, 1, rbufsize, parsed);
        
        stat = yajl_parse(hand, (unsigned char const *)RSTRING_PTR(parsed), RSTRING_LEN(parsed));

        if (stat != yajl_status_ok &&
            stat != yajl_status_insufficient_data)
        {
            unsigned char * str = yajl_get_error(hand, 1, (unsigned char const *)RSTRING_PTR(parsed), RSTRING_LEN(parsed));
            fprintf(stderr, (const char *) str);
            yajl_free_error(hand, str);
            break;
        }
    }
    
    // parse any remaining buffered data
    stat = yajl_parse_complete(hand);
    yajl_free(hand);
    return ctx;
}

VALUE cYajl;
VALUE cNative;

void Init_yajl() {
    cYajl = rb_define_module("Yajl");
    cNative = rb_define_module_under(cYajl, "Native");
    rb_define_module_function(cNative, "parse", t_parse, 1);
}