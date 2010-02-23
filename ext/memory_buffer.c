#include <ruby.h>
#include <constants.h>

ID buf_ivar_id;
ID index_ivar_id;

ID slice_method_id;

int GARBAGE_BUFFER_SIZE;

#define GET_BUF(self) rb_ivar_get(self, buf_ivar_id)

VALUE rb_thrift_memory_buffer_write(VALUE self, VALUE str) {
  VALUE buf = GET_BUF(self);
  rb_str_buf_cat(buf, RSTRING(str)->ptr, RSTRING(str)->len);
  return Qnil;
}

VALUE rb_thrift_memory_buffer_read(VALUE self, VALUE length_value) {
  int length = FIX2INT(length_value);
  
  VALUE index_value = rb_ivar_get(self, index_ivar_id);
  int index = FIX2INT(index_value);
  
  VALUE buf = GET_BUF(self);
  VALUE data = rb_funcall(buf, slice_method_id, 2, index_value, length_value);
  
  index += length;
  if (index > RSTRING(buf)->len) {
    index = RSTRING(buf)->len;
  }
  if (index >= GARBAGE_BUFFER_SIZE) {
    rb_ivar_set(self, buf_ivar_id, rb_funcall(buf, slice_method_id, 2, INT2FIX(index), INT2FIX(-1)));
    index = 0;
  }

  rb_ivar_set(self, index_ivar_id, INT2FIX(index));
  return data;
}

void Init_memory_buffer() {
  VALUE thrift_memory_buffer_class = rb_const_get(thrift_module, rb_intern("MemoryBuffer"));
  rb_define_method(thrift_memory_buffer_class, "write", rb_thrift_memory_buffer_write, 1);
  rb_define_method(thrift_memory_buffer_class, "read", rb_thrift_memory_buffer_read, 1);
  
  buf_ivar_id = rb_intern("@buf");
  index_ivar_id = rb_intern("@index");
  
  slice_method_id = rb_intern("slice");
  
  GARBAGE_BUFFER_SIZE = FIX2INT(rb_const_get(thrift_memory_buffer_class, rb_intern("GARBAGE_BUFFER_SIZE")));
}
