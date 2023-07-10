# This stub generator is horrible...  the resulting code is horrible...  please... rewrite

import json
import sys

with open('fd_types.json', 'r') as json_file:
    json_object = json.load(json_file)

header = open(sys.argv[1], "w")
body = open(sys.argv[2], "w")

namespace = json_object["namespace"]
entries = json_object["entries"]

print("#ifndef HEADER_" + json_object["name"].upper(), file=header)
print("#define HEADER_" + json_object["name"].upper(), file=header)
print("", file=header)
for extra in json_object["extra_header"]:
    print(extra, file=header)
print("", file=header)

print(f'#include "{sys.argv[1]}"', file=body)

print('#pragma GCC diagnostic ignored "-Wunused-parameter"', file=body)
print('#pragma GCC diagnostic ignored "-Wunused-variable"', file=body)

print('#ifdef _DISABLE_OPTIMIZATION', file=body)
print('#pragma GCC optimize ("O0")', file=body)
print('#endif', file=body)

print('#define SOURCE_fd_src_flamenco_types_fd_types_c', file=body)
print('#include "fd_types_custom.c"', file=body)

type_map = {
    "int64_t": "long",
    "uint64_t": "ulong",
    "ulong": "ulong",
    "uchar": "uchar"
}

def do_vector_header(n, f):
    if "modifier" in f and f["modifier"] == "compact":
        print(f'  ushort {f["name"]}_len;', file=header)
    else:
        print(f'  ulong {f["name"]}_len;', file=header)

    if f["element"] == "uchar":
        print(f'  {f["element"]}* {f["name"]};', file=header)
    elif f["element"] == "ulong":
        print(f'  {f["element"]}* {f["name"]};', file=header)
    elif f["element"] == "uint":
        print(f'  {f["element"]}* {f["name"]};', file=header)
    else:
        print(f'  {n}_{f["element"]}_t* {f["name"]};', file=header)

def deque_elem_type(n, f):
  if f["element"] == "uchar":
    return "uchar"
  elif f["element"] == "ulong":
      return "ulong"
  elif f["element"] == "uint":
      return "uint"
  else:
      return f'{n}_{f["element"]}_t'

def deque_prefix(n, f):
    return f'deq_{deque_elem_type(n, f)}'

def do_deque_header(n, f):
    print(f'  {deque_elem_type(n, f)} * {f["name"]};', file=header)

def do_map_header(n, f):
    element_type = deque_elem_type(n, f)
    print(f'  {element_type}_mapnode_t * {f["name"]}_pool;', file=header)
    print(f'  {element_type}_mapnode_t * {f["name"]}_root;', file=header)

def do_option_header(n, f):
      if f["element"] == "ulong":
          print(f'  {f["element"]}* {f["name"]};', file=header)
      elif f["element"] == "uint":
          print(f'  {f["element"]}* {f["name"]};', file=header)
      else:
          print(f'  {n}_{f["element"]}_t* {f["name"]};', file=header)

def do_array_header(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        print(f'  {f["element"]} {f["name"]}[{length}];', file=header)
    elif f["element"] == "ulong":
        print(f'  {f["element"]} {f["name"]}[{length}];', file=header)
    elif f["element"] == "uint":
        print(f'  {f["element"]} {f["name"]}[{length}];', file=header)
    else:
        print(f'  {n}_{f["element"]}_t {f["name"]}[{length}];', file=header)

fields_header = {
    "char" :              lambda n, f: print(f'  char {f["name"]};',              file=header),
    "char*" :             lambda n, f: print(f'  char* {f["name"]};',             file=header),
    "char[32]" :          lambda n, f: print(f'  char {f["name"]}[32];',          file=header),
    "char[7]" :           lambda n, f: print(f'  char {f["name"]}[7];',           file=header),
    "double" :            lambda n, f: print(f'  double {f["name"]};',            file=header),
    "long" :              lambda n, f: print(f'  long {f["name"]};',              file=header),
    "uint" :              lambda n, f: print(f'  uint {f["name"]};',              file=header),
    "uint128" :           lambda n, f: print(f'  uint128 {f["name"]};',           file=header),
    "uchar" :     lambda n, f: print(f'  uchar {f["name"]};',     file=header),
    "uchar[32]" : lambda n, f: print(f'  uchar {f["name"]}[32];', file=header),
    "ulong" :     lambda n, f: print(f'  ulong {f["name"]};',     file=header),
    "ushort" :            lambda n, f: print(f'  ushort {f["name"]};',            file=header),
    "vector" :            lambda n, f: do_vector_header(n, f),
    "deque":              lambda n, f: do_deque_header(n, f),
    "array":              lambda n, f: do_array_header(n, f),
    "option" :            lambda n, f: do_option_header(n, f),
    "map" :               lambda n, f: do_map_header(n, f),
}

def do_vector_body_decode(n, f):
    if "modifier" in f and f["modifier"] == "compact":
        print(f'  err = fd_bincode_compact_u16_decode(&self->{f["name"]}_len, ctx);', file=body)
    else:
        print(f'  err = fd_bincode_uint64_decode(&self->{f["name"]}_len, ctx);', file=body)
    print(f'  if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
    print(f'  if (self->{f["name"]}_len != 0) {{', file=body)
    el = f'{n}_{f["element"]}'
    el = el.upper()

    if f["element"] == "uchar":
        print(f'    self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 8UL, self->{f["name"]}_len );', file=body)
        print(f'    err = fd_bincode_bytes_decode(self->{f["name"]}, self->{f["name"]}_len, ctx);', file=body)
        print(f'    if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)

    else:
        if f["element"] == "ulong":
            print(f'    self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 8UL, self->{f["name"]}_len );', file=body)
        elif f["element"] == "uint":
            print(f'    self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 8UL, sizeof(uint)*self->{f["name"]}_len );', file=body)
        else:
            print(f'    self->{f["name"]} = ({n}_{f["element"]}_t *)fd_valloc_malloc( ctx->valloc, {el}_ALIGN, {el}_FOOTPRINT*self->{f["name"]}_len);', file=body)

        print(f'    for( ulong i = 0; i < self->{f["name"]}_len; ++i) {{', file=body)

        if f["element"] == "ulong":
            print(f'      err = fd_bincode_uint64_decode(self->{f["name"]} + i, ctx);', file=body)
        elif f["element"] == "uint":
            print(f'      err = fd_bincode_uint32_decode(self->{f["name"]} + i, ctx);', file=body)
        else:
            print(f'      {n}_{f["element"]}_new(self->{f["name"]} + i);', file=body)
            print(f'    }}', file=body)
            print(f'    for( ulong i = 0; i < self->{f["name"]}_len; ++i ) {{', file=body)
            print(f'      err = {n}_{f["element"]}_decode(self->{f["name"]} + i, ctx);', file=body)

        print(f'      if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
        print('    }', file=body)

    print('  } else', file=body)
    print(f'    self->{f["name"]} = NULL;', file=body)


def do_deque_body_decode(n, f):
    if "modifier" in f and f["modifier"] == "compact":
        print(f'  ushort {f["name"]}_len;', file=body)
        print(f'  err = fd_bincode_compact_u16_decode( &{f["name"]}_len, ctx );', file=body)
    else:
        print(f'  ulong {f["name"]}_len;', file=body)
        print(f'  err = fd_bincode_uint64_decode( &{f["name"]}_len, ctx );', file=body)
    print(f'  if ( FD_UNLIKELY(err) ) return err;', file=body)
    if "max" in f:
        print(f'  self->{f["name"]} = {deque_prefix(n, f)}_alloc( ctx->valloc );', file=body)
    else:
        print(f'  self->{f["name"]} = {deque_prefix(n, f)}_alloc( ctx->valloc, {f["name"]}_len );', file=body)
    print(f'  if ( {f["name"]}_len > {deque_prefix(n, f)}_max(self->{f["name"]}) ) return FD_BINCODE_ERR_SMALL_DEQUE;', file=body)

    print(f'  for (ulong i = 0; i < {f["name"]}_len; ++i) {{', file=body)
    print(f'    {deque_elem_type(n, f)} * elem = {deque_prefix(n, f)}_push_tail_nocopy(self->{f["name"]});', file=body);

    if f["element"] == "ulong":
        print(f'    err = fd_bincode_uint64_decode(elem, ctx);', file=body)
    elif f["element"] == "uint":
        print(f'    err = fd_bincode_uint32_decode(elem, ctx);', file=body)
    else:
        print(f'    {n}_{f["element"]}_new(elem);', file=body)
        print(f'    err = {n}_{f["element"]}_decode(elem, ctx);', file=body)
    print(f'    if ( FD_UNLIKELY(err) ) return err;', file=body)

    print('  }', file=body)

def do_map_body_decode(n, f):
    element_type = deque_elem_type(n, f)
    mapname = element_type + "_map"
    nodename = element_type + "_mapnode_t"

    if "modifier" in f and f["modifier"] == "compact":
        print(f'  ushort {f["name"]}_len;', file=body)
        print(f'  err = fd_bincode_compact_u16_decode(&{f["name"]}_len, ctx);', file=body)
    else:
        print(f'  ulong {f["name"]}_len;', file=body)
        print(f'  err = fd_bincode_uint64_decode(&{f["name"]}_len, ctx);', file=body)
    print('  if ( FD_UNLIKELY(err) ) return err;', file=body)

    if "minalloc" in f:
        print(f'  self->{f["name"]}_pool = {mapname}_alloc(ctx->valloc, fd_ulong_max({f["name"]}_len, {f["minalloc"]}));', file=body)
    else:
        print(f'  self->{f["name"]}_pool = {mapname}_alloc(ctx->valloc, {f["name"]}_len);', file=body)
    print(f'  self->{f["name"]}_root = NULL;', file=body)
    print(f'  for (ulong i = 0; i < {f["name"]}_len; ++i) {{', file=body)
    print(f'    {nodename}* node = {mapname}_acquire(self->{f["name"]}_pool);', file=body);
    print(f'    {n}_{f["element"]}_new(&node->elem);', file=body)
    print(f'    err = {n}_{f["element"]}_decode(&node->elem, ctx);', file=body)
    print(f'    if ( FD_UNLIKELY(err) ) return err;', file=body)
    print(f'    {mapname}_insert(self->{f["name"]}_pool, &self->{f["name"]}_root, node);', file=body)
    print('  }', file=body)

def do_array_body_decode(n, f):

    el = f'{n}_{f["element"]}'
    el = el.upper()

    length = f["length"]

    if f["element"] == "uchar":
        print(f'  err = fd_bincode_bytes_decode( self->{f["name"]}, {length}, ctx );', file=body)
        print(f'  if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
        return

    print(f'  for (ulong i = 0; i < {length}; ++i) {{', file=body)

    if f["element"] == "ulong":
        print(f'    err = fd_bincode_uint64_decode(self->{f["name"]} + i, ctx);', file=body)
    elif f["element"] == "uint":
        print(f'    err = fd_bincode_uint32_decode(self->{f["name"]} + i, ctx);', file=body)
    else:
        print(f'    err = {n}_{f["element"]}_decode(self->{f["name"]} + i, ctx);', file=body)
    print('    if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)

    print('  }', file=body)

def do_option_body_decode(n, f):
    print('  {', file=body)
    print('    uchar o;', file=body)
    print('    err = fd_bincode_option_decode( &o, ctx );', file=body)
    print('    if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
    print('    if( o ) {', file=body)
    if f["element"] == "ulong":
        print(f'      self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 8, sizeof(ulong) );', file=body)
        print(f'      err = fd_bincode_uint64_decode( self->{f["name"]}, ctx );', file=body)
    elif f["element"] == "uint":
        print(f'      self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 8, sizeof(uint) );', file=body)
        print(f'      err = fd_bincode_uint32_decode( self->{f["name"]}, ctx );', file=body)
    else:
        el = f'{n}_{f["element"]}'
        el = el.upper()
        print(f'      self->{f["name"]} = ({n}_{f["element"]}_t*)fd_valloc_malloc( ctx->valloc, {el}_ALIGN, {el}_FOOTPRINT );', file=body)
        print(f'      {n}_{f["element"]}_new( self->{f["name"]} );', file=body)
        print(f'      err = {n}_{f["element"]}_decode( self->{f["name"]}, ctx );', file=body)
    print('      if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
    print('    } else', file=body)
    print(f'      self->{f["name"]} = NULL;', file=body)
    print('  }', file=body)

def do_string_decode(n, f):
    print('  ulong slen;', file=body)
    print('  err = fd_bincode_uint64_decode( &slen, ctx );', file=body)
    print('  if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
    print(f'  self->{f["name"]} = fd_valloc_malloc( ctx->valloc, 1, slen + 1 );', file=body)
    print(f'  err = fd_bincode_bytes_decode( (uchar *)self->{f["name"]}, slen, ctx );', file=body)
    print('  if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)
    print(f"  self->{f['name']}[slen] = '\\0';", file=body)

def do_ulong_decode(n, f):
    if "modifier" in f and f["modifier"] == "varint":
        print(f'  err = fd_bincode_varint_decode(&self->{f["name"]}, ctx);', file=body),
    else:
        print(f'  err = fd_bincode_uint64_decode(&self->{f["name"]}, ctx);', file=body),
    print('  if( FD_UNLIKELY( err!=FD_BINCODE_SUCCESS ) ) return err;', file=body)

fields_body_decode = {
    "char" :      lambda n, f: print(f"""  err = fd_bincode_uint8_decode((uchar *) &self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "char*" :     lambda n, f: do_string_decode(n, f),
    "char[32]" :  lambda n, f: print(f"""  err = fd_bincode_bytes_decode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "char[7]" :   lambda n, f: print(f"""  err = fd_bincode_bytes_decode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "double" :    lambda n, f: print(f"""  err = fd_bincode_double_decode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "long" :      lambda n, f: print(f"""  err = fd_bincode_uint64_decode((ulong *) &self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uint" :      lambda n, f: print(f"""  err = fd_bincode_uint32_decode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uint128" :   lambda n, f: print(f"""  err = fd_bincode_uint128_decode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uchar" :     lambda n, f: print(f"""  err = fd_bincode_uint8_decode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uchar[32]" : lambda n, f: print(f"""  err = fd_bincode_bytes_decode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "ulong" :     lambda n, f: do_ulong_decode(n, f),
    "ushort" :    lambda n, f: print(f"""  err = fd_bincode_uint16_decode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "vector" :    lambda n, f: do_vector_body_decode(n, f),
    "deque":      lambda n, f: do_deque_body_decode(n, f),
    "array":      lambda n, f: do_array_body_decode(n, f),
    "option" :    lambda n, f: do_option_body_decode(n, f),
    "map" :       lambda n, f: do_map_body_decode(n, f),
}

# encode

def do_vector_body_encode(n, f):
    if "modifier" in f and f["modifier"] == "compact":
        print(f'  err = fd_bincode_compact_u16_encode(&self->{f["name"]}_len, ctx);', file=body)
    else:
        print(f'  err = fd_bincode_uint64_encode(&self->{f["name"]}_len, ctx);', file=body)
    print(f'  if ( FD_UNLIKELY(err) ) return err;', file=body)
    print(f'  if (self->{f["name"]}_len != 0) {{', file=body)

    if f["element"] == "uchar":
        print(f'    err = fd_bincode_bytes_encode(self->{f["name"]}, self->{f["name"]}_len, ctx);', file=body)
        print(f'    if ( FD_UNLIKELY(err) ) return err;', file=body)

    else:
        print(f'    for (ulong i = 0; i < self->{f["name"]}_len; ++i) {{', file=body)

        if f["element"] == "ulong":
            print(f'      err = fd_bincode_uint64_encode(self->{f["name"]} + i, ctx);', file=body)
        elif f["element"] == "uint":
            print(f'      err = fd_bincode_uint32_encode(self->{f["name"]} + i, ctx);', file=body)
        else:
            print(f'      err = {n}_{f["element"]}_encode(self->{f["name"]} + i, ctx);', file=body)
            print('      if ( FD_UNLIKELY(err) ) return err;', file=body)

        print('    }', file=body)

    print('  }', file=body)


def do_deque_body_encode(n, f):
    print(f'  if ( self->{f["name"]} ) {{', file=body)

    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = (ushort){deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    err = fd_bincode_compact_u16_encode(&{f["name"]}_len, ctx);', file=body)
    else:
        print(f'    ulong {f["name"]}_len = {deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    err = fd_bincode_uint64_encode(&{f["name"]}_len, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)

    print(f'    for ( {deque_prefix(n, f)}_iter_t iter = {deque_prefix(n, f)}_iter_init( self->{f["name"]} ); !{deque_prefix(n, f)}_iter_done( self->{f["name"]}, iter ); iter = {deque_prefix(n, f)}_iter_next( self->{f["name"]}, iter ) ) {{', file=body)
    print(f'      {deque_elem_type(n, f)} * ele = {deque_prefix(n, f)}_iter_ele( self->{f["name"]}, iter );', file=body)

    if f["element"] == "uchar":
        print('      err = fd_bincode_uint8_encode(ele, ctx);', file=body)
    elif f["element"] == "ulong":
        print('      err = fd_bincode_uint64_encode(ele, ctx);', file=body)
    elif f["element"] == "uint":
        print('      err = fd_bincode_uint32_encode(ele, ctx);', file=body)
    else:
        print(f'      err = {n}_{f["element"]}_encode(ele, ctx);', file=body)
        print('      if ( FD_UNLIKELY(err) ) return err;', file=body)

    print('    }', file=body)

    print('  } else {', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = 0;', file=body)
        print(f'    err = fd_bincode_compact_u16_encode(&{f["name"]}_len, ctx);', file=body)
    else:
        print(f'    ulong {f["name"]}_len = 0;', file=body)
        print(f'    err = fd_bincode_uint64_encode(&{f["name"]}_len, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)
    print('  }', file=body)

def do_map_body_encode(n, f):
    element_type = deque_elem_type(n, f)
    mapname = element_type + "_map"
    nodename = element_type + "_mapnode_t"

    print(f'  if (self->{f["name"]}_root) {{', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = (ushort){mapname}_size(self->{f["name"]}_pool, self->{f["name"]}_root);', file=body)
        print(f'    err = fd_bincode_compact_u16_encode(&{f["name"]}_len, ctx);', file=body)
    else:
        print(f'    ulong {f["name"]}_len = {mapname}_size(self->{f["name"]}_pool, self->{f["name"]}_root);', file=body)
        print(f'    err = fd_bincode_uint64_encode(&{f["name"]}_len, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)

    print(f'    for ( {nodename}* n = {mapname}_minimum(self->{f["name"]}_pool, self->{f["name"]}_root); n; n = {mapname}_successor(self->{f["name"]}_pool, n) ) {{', file=body);
    print(f'      err = {n}_{f["element"]}_encode(&n->elem, ctx);', file=body)
    print('      if ( FD_UNLIKELY(err) ) return err;', file=body)
    print('    }', file=body)
    print('  } else {', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = 0;', file=body)
        print(f'    err = fd_bincode_compact_u16_encode(&{f["name"]}_len, ctx);', file=body)
    else:
        print(f'    ulong {f["name"]}_len = 0;', file=body)
        print(f'    err = fd_bincode_uint64_encode(&{f["name"]}_len, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)
    print('  }', file=body)

def do_array_body_encode(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        print(f'  err = fd_bincode_bytes_encode(self->{f["name"]}, {length}, ctx);', file=body)
        print('  if ( FD_UNLIKELY(err) ) return err;', file=body)

    else:
        print(f'  for (ulong i = 0; i < {length}; ++i) {{', file=body)

        if f["element"] == "ulong":
            print(f'    err = fd_bincode_uint64_encode(self->{f["name"]} + i, ctx);', file=body)
        elif f["element"] == "uint":
            print(f'    err = fd_bincode_uint32_encode(self->{f["name"]} + i, ctx);', file=body)
        else:
            print(f'    err = {n}_{f["element"]}_encode(self->{f["name"]} + i, ctx);', file=body)
        print('    if ( FD_UNLIKELY(err) ) return err;', file=body)

        print('  }', file=body)

def do_option_body_encode(n, f):
    print(f'  if (self->{f["name"]} != NULL) {{', file=body)
    print('    err = fd_bincode_option_encode(1, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)

    if f["element"] == "ulong":
        print(f'    err = fd_bincode_uint64_encode(self->{f["name"]}, ctx);', file=body)
    elif f["element"] == "uint":
        print(f'    err = fd_bincode_uint32_encode(self->{f["name"]}, ctx);', file=body)
    else:
        print(f'    err = {n}_{f["element"]}_encode(self->{f["name"]}, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)
    print('  } else {', file=body)
    print('    err = fd_bincode_option_encode(0, ctx);', file=body)
    print('    if ( FD_UNLIKELY(err) ) return err;', file=body)
    print('  }', file=body)

def do_string_encode(n, f):
    print(f'  ulong slen = strlen( (char *) self->{f["name"]} );', file=body)
    print('  err = fd_bincode_uint64_encode(&slen, ctx);', file=body)
    print('  if ( FD_UNLIKELY(err) ) return err;', file=body)
    print(f'  err = fd_bincode_bytes_encode((uchar *) self->{f["name"]}, slen, ctx);', file=body)
    print('  if ( FD_UNLIKELY(err) ) return err;', file=body)

def do_ulong_encode(n, f):
    if "modifier" in f and f["modifier"] == "varint":
        print(f'  err = fd_bincode_varint_encode(self->{f["name"]}, ctx);', file=body),
    else:
        print(f'  err = fd_bincode_uint64_encode(&self->{f["name"]}, ctx);', file=body),
    print('  if ( FD_UNLIKELY(err) ) return err;', file=body)

fields_body_encode = {
    "char" :              lambda n, f: print(f"""  err = fd_bincode_uint8_encode((uchar *) &self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "char*" :             lambda n, f: do_string_encode(n, f),
    "char[32]" :          lambda n, f: print(f"""  err = fd_bincode_bytes_encode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "char[7]" :           lambda n, f: print(f"""  err = fd_bincode_bytes_encode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "double" :            lambda n, f: print(f"""  err = fd_bincode_double_encode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "long" :              lambda n, f: print(f"""  err = fd_bincode_uint64_encode((ulong *) &self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uint" :              lambda n, f: print(f"""  err = fd_bincode_uint32_encode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uint128" :           lambda n, f: print(f"""  err = fd_bincode_uint128_encode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uchar" :     lambda n, f: print(f"""  err = fd_bincode_uint8_encode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "uchar[32]" : lambda n, f: print(f"""  err = fd_bincode_bytes_encode(&self->{f["name"]}[0], sizeof(self->{f["name"]}), ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "ulong" :     lambda n, f: do_ulong_encode(n, f),
    "ushort" :            lambda n, f: print(f"""  err = fd_bincode_uint16_encode(&self->{f["name"]}, ctx);\n  if ( FD_UNLIKELY(err) ) return err;""", file=body),
    "vector" :            lambda n, f: do_vector_body_encode(n, f),
    "deque" :             lambda n, f: do_deque_body_encode(n, f),
    "array" :             lambda n, f: do_array_body_encode(n, f),
    "option" :            lambda n, f: do_option_body_encode(n, f),
    "map" :               lambda n, f: do_map_body_encode(n, f),
}

# size

def do_vector_body_size(n, f):
    print('  size += sizeof(ulong);', file=body)
    if f["element"] == "uchar":
        print(f'  size += self->{f["name"]}_len;', file=body)
    elif f["element"] == "ulong":
        print(f'  size += self->{f["name"]}_len * sizeof(ulong);', file=body)
    elif f["element"] == "uint":
        print(f'  size += self->{f["name"]}_len * sizeof(uint);', file=body)
    else:
        print(f'  for (ulong i = 0; i < self->{f["name"]}_len; ++i)', file=body)
        print(f'    size += {n}_{f["element"]}_size(self->{f["name"]} + i);', file=body)

def do_deque_body_size(n, f):
    print(f'  if ( self->{f["name"]} ) {{', file=body)

    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = (ushort){deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    size += fd_bincode_compact_u16_size(&{f["name"]}_len);', file=body)
    else:
        print('    size += sizeof(ulong);', file=body)

    if f["element"] == "uchar":
        print(f'    ulong {f["name"]}_len = {deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    size += {f["name"]}_len;', file=body)
    elif f["element"] == "ulong":
        print(f'    ulong {f["name"]}_len = {deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    size += {f["name"]}_len * sizeof(ulong);', file=body)
    elif f["element"] == "uint":
        print(f'    ulong {f["name"]}_len = {deque_prefix(n, f)}_cnt(self->{f["name"]});', file=body)
        print(f'    size += {f["name"]}_len * sizeof(uint);', file=body)
    else:
        print(f'    for ( {deque_prefix(n, f)}_iter_t iter = {deque_prefix(n, f)}_iter_init( self->{f["name"]} ); !{deque_prefix(n, f)}_iter_done( self->{f["name"]}, iter ); iter = {deque_prefix(n, f)}_iter_next( self->{f["name"]}, iter ) ) {{', file=body)
        print(f'      {deque_elem_type(n, f)} * ele = {deque_prefix(n, f)}_iter_ele( self->{f["name"]}, iter );', file=body)
        print(f'      size += {n}_{f["element"]}_size(ele);', file=body)
        print('    }', file=body)

    print('  } else {', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print('    size += 1;', file=body)
    else:
        print('    size += sizeof(ulong);', file=body)
    print('  }', file=body)

def do_map_body_size(n, f):
    element_type = deque_elem_type(n, f)
    mapname = element_type + "_map"
    nodename = element_type + "_mapnode_t"

    print(f'  if (self->{f["name"]}_root) {{', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print(f'    ushort {f["name"]}_len = (ushort){mapname}_size(self->{f["name"]}_pool, self->{f["name"]}_root);', file=body)
        print(f'    size += fd_bincode_compact_u16_size(&{f["name"]}_len);', file=body)
    else:
        print('    size += sizeof(ulong);', file=body)
    print(f'    for ( {nodename}* n = {mapname}_minimum(self->{f["name"]}_pool, self->{f["name"]}_root); n; n = {mapname}_successor(self->{f["name"]}_pool, n) ) {{', file=body);
    print(f'      size += {n}_{f["element"]}_size(&n->elem);', file=body)
    print('    }', file=body)
    print('  } else {', file=body)
    if "modifier" in f and f["modifier"] == "compact":
        print('    size += 1;', file=body)
    else:
        print('    size += sizeof(ulong);', file=body)
    print('  }', file=body)

def do_array_body_size(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        print(f'  size += {length};', file=body)
    elif f["element"] == "ulong":
        print(f'  size += {length} * sizeof(ulong);', file=body)
    elif f["element"] == "uint":
        print(f'  size += {length} * sizeof(uint);', file=body)
    else:
        print(f'  for (ulong i = 0; i < {length}; ++i)', file=body)
        print(f'    size += {n}_{f["element"]}_size(self->{f["name"]} + i);', file=body)

def do_option_body_size(n, f):
    print('  size += sizeof(char);', file=body)
    print(f'  if (NULL !=  self->{f["name"]}) {{', file=body)

    if f["element"] == "ulong":
        print('    size += sizeof(ulong);', file=body)
    elif f["element"] == "uint":
        print('    size += sizeof(uint);', file=body)
    else:
        el = f'{n}_{f["element"]}'
        el = el.upper()
        print(f'    size += {n}_{f["element"]}_size(self->{f["name"]});', file=body)
    print('  }', file=body)

def do_string_size(n, f):
    print(f'  size += sizeof(ulong) + strlen(self->{f["name"]});', file=body)

fields_body_size = {
    "char" :              lambda n, f: print('  size += sizeof(char);', file=body),
    "char*" :             lambda n, f: do_string_size(n, f),
    "char[32]" :          lambda n, f: print('  size += sizeof(char) * 32;', file=body),
    "char[7]" :           lambda n, f: print('  size += sizeof(char) * 7;', file=body),
    "double" :            lambda n, f: print('  size += sizeof(double);', file=body),
    "long" :              lambda n, f: print('  size += sizeof(long);', file=body),
    "uint" :              lambda n, f: print('  size += sizeof(uint);', file=body),
    "uint128" :           lambda n, f: print('  size += sizeof(uint128);', file=body),
    "uchar" :     lambda n, f: print('  size += sizeof(char);', file=body),
    "uchar[32]" : lambda n, f: print('  size += sizeof(char) * 32;', file=body),
    "ulong" :     lambda n, f: print('  size += sizeof(ulong);', file=body),
    "ushort" :            lambda n, f: print('  size += sizeof(ushort);', file=body),
    "vector" :            lambda n, f: do_vector_body_size(n, f),
    "deque" :             lambda n, f: do_deque_body_size(n, f),
    "array" :             lambda n, f: do_array_body_size(n, f),
    "option" :            lambda n, f: do_option_body_size(n, f),
    "map" :               lambda n, f: do_map_body_size(n, f),
}

# new

def do_array_body_new(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        pass
    elif f["element"] == "ulong":
        pass
    elif f["element"] == "uint":
        pass
    else:
        print(f'  for (ulong i = 0; i < {length}; ++i)', file=body)
        print(f'    {n}_{f["element"]}_new(self->{f["name"]} + i);', file=body)

def do_pass():
    pass

fields_body_new = {
    "char" :              lambda n, f: do_pass(),
    "char*" :             lambda n, f: do_pass(),
    "char[32]" :          lambda n, f: do_pass(),
    "char[7]" :           lambda n, f: do_pass(),
    "double" :            lambda n, f: do_pass(),
    "long" :              lambda n, f: do_pass(),
    "uint" :              lambda n, f: do_pass(),
    "uint128" :           lambda n, f: do_pass(),
    "uchar" :     lambda n, f: do_pass(),
    "uchar[32]" : lambda n, f: do_pass(),
    "ulong" :     lambda n, f: do_pass(),
    "ushort" :            lambda n, f: do_pass(),
    "vector" :            lambda n, f: do_pass(),
    "deque" :             lambda n, f: do_pass(),
    "array" :             lambda n, f: do_array_body_new(n, f),
    "option" :            lambda n, f: do_pass(),
    "map" :               lambda n, f: do_pass()
}

# destroy

def do_vector_body_destroy(n, f):
    print(f'  if (NULL != self->{f["name"]}) {{', file=body)
    if f["element"] == "uchar":
        pass
    elif f["element"] == "ulong":
        pass
    elif f["element"] == "uint":
        pass
    else:
        print(f'    for (ulong i = 0; i < self->{f["name"]}_len; ++i)', file=body)
        print(f'      {n}_{f["element"]}_destroy(self->{f["name"]} + i, ctx);', file=body)
    print(f'    fd_valloc_free( ctx->valloc, self->{f["name"]} );', file=body)
    print(f'    self->{f["name"]} = NULL;', file=body)
    print('  }', file=body)


def do_deque_body_destroy(n, f):
    print(f'  if ( self->{f["name"]} ) {{', file=body)
    if f["element"] == "uchar":
        pass
    elif f["element"] == "ulong":
        pass
    elif f["element"] == "uint":
        pass
    else:
        print(f'    for ( {deque_prefix(n, f)}_iter_t iter = {deque_prefix(n, f)}_iter_init( self->{f["name"]} ); !{deque_prefix(n, f)}_iter_done( self->{f["name"]}, iter ); iter = {deque_prefix(n, f)}_iter_next( self->{f["name"]}, iter ) ) {{', file=body)
        print(f'      {deque_elem_type(n, f)} * ele = {deque_prefix(n, f)}_iter_ele( self->{f["name"]}, iter );', file=body)
        print(f'      {n}_{f["element"]}_destroy(ele, ctx);', file=body)
        print('    }', file=body)
    print(f'    fd_valloc_free( ctx->valloc, {deque_prefix(n, f)}_delete( {deque_prefix(n, f)}_leave( self->{f["name"]}) ) );', file=body)
    print(f'    self->{f["name"]} = NULL;', file=body)
    print('  }', file=body)

def do_map_body_destroy(n, f):
    element_type = deque_elem_type(n, f)
    mapname = element_type + "_map"
    nodename = element_type + "_mapnode_t"

    print(f'  for ( {nodename}* n = {mapname}_minimum(self->{f["name"]}_pool, self->{f["name"]}_root); n; n = {mapname}_successor(self->{f["name"]}_pool, n) ) {{', file=body);
    print(f'    {n}_{f["element"]}_destroy(&n->elem, ctx);', file=body)
    print('  }', file=body)
    print(f'  fd_valloc_free( ctx->valloc, {mapname}_delete({mapname}_leave( self->{f["name"]}_pool) ) );', file=body)
    print(f'  self->{f["name"]}_pool = NULL;', file=body)
    print(f'  self->{f["name"]}_root = NULL;', file=body)

def do_array_body_destroy(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        pass
    elif f["element"] == "ulong":
        pass
    elif f["element"] == "uint":
        pass
    else:
        print(f'  for (ulong i = 0; i < {length}; ++i)', file=body)
        print(f'    {n}_{f["element"]}_destroy(self->{f["name"]} + i, ctx);', file=body)

def do_option_body_destroy(n, f):
    print(f'  if (NULL != self->{f["name"]}) {{', file=body)
    if f["element"] == "ulong":
        pass
    elif f["element"] == "uint":
        pass
    else:
        print(f'    {n}_{f["element"]}_destroy(self->{f["name"]}, ctx);', file=body)

    print(f'    fd_valloc_free( ctx->valloc, self->{f["name"]});', file=body)
    print(f'    self->{f["name"]} = NULL;', file=body)
    print('  }', file=body)

fields_body_destroy = {
    "char" :              lambda n, f: do_pass(),
    "char*" :             lambda n, f: print(f"""  if (NULL != self->{f["name"]}) {{\n    fd_valloc_free( ctx->valloc, self->{f["name"]});\n    self->{f["name"]} = NULL;\n  }}""", file=body),
    "char[32]" :          lambda n, f: do_pass(),
    "char[7]" :           lambda n, f: do_pass(),
    "double" :            lambda n, f: do_pass(),
    "long" :              lambda n, f: do_pass(),
    "uint" :              lambda n, f: do_pass(),
    "uint128" :           lambda n, f: do_pass(),
    "uchar" :     lambda n, f: do_pass(),
    "uchar[32]" : lambda n, f: do_pass(),
    "ulong" :     lambda n, f: do_pass(),
    "ushort" :            lambda n, f: do_pass(),
    "vector" :            lambda n, f: do_vector_body_destroy(n, f),
    "deque" :             lambda n, f: do_deque_body_destroy(n, f),
    "array" :             lambda n, f: do_array_body_destroy(n, f),
    "option" :            lambda n, f: do_option_body_destroy(n, f),
    "map" :               lambda n, f: do_map_body_destroy(n, f),
}

# walk

fields_body_vector_walk = {
    "double" :            lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 5, "double", level + 1);', file=body),
    "long" :              lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 6, "long", level + 1);', file=body),
    "uint" :              lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 7, "uint", level + 1);', file=body),
    "uint128" :           lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 8, "uint128", level + 1);', file=body),
    "ulong" :     lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 11, "ulong", level + 1);', file=body),
    "ushort" :            lambda n, f: print(f'  fun(self->{f["name"]} + i, "{f["name"]}", 12, "ushort", level + 1);', file=body),
}

def do_vector_body_walk(n, f):

    if f["element"] == "uchar":
        print(f'  fun(self->{f["name"]}, "{f["name"]}", 2, "{f["element"]}", level + 1);', file=body),
        return
    else:
        print(f'  if (self->{f["name"]}_len != 0) {{', file=body)
        print(f'    fun(NULL, NULL, 30, "{f["name"]}", level++);', file=body)
        print(f'    for (ulong i = 0; i < self->{f["name"]}_len; ++i)', file=body)

    if f["element"] in fields_body_vector_walk:
        body.write("    ")
        fields_body_vector_walk[f["element"]](namespace, f)
    else:
        print(f'      {n}_{f["element"]}_walk(self->{f["name"]} + i, fun, "{f["element"]}", level + 1);', file=body)

    print(f'    fun(NULL, NULL, 31, "{f["name"]}", --level);', file=body)
    print('  }', file=body)

def do_deque_body_walk(n, f):
#    print(f'  if ( self->{f["name"]} ) {{', file=body)
    print(f'    fun(self->{f["name"]}, "{f["name"]}", 36, "{f["name"]}", level++);', file=body)
    print(f'    if (NULL != self->{f["name"]}) ', file=body)
    print(f'     for ( {deque_prefix(n, f)}_iter_t iter = {deque_prefix(n, f)}_iter_init( self->{f["name"]} ); !{deque_prefix(n, f)}_iter_done( self->{f["name"]}, iter ); iter = {deque_prefix(n, f)}_iter_next( self->{f["name"]}, iter ) ) {{', file=body)
    print(f'      {deque_elem_type(n, f)} * ele = {deque_prefix(n, f)}_iter_ele( self->{f["name"]}, iter );', file=body)

    if f["element"] == "uchar":
        print('      fun(ele, "ele", 1, "long", level + 1);', file=body),
    elif f["element"] == "ulong":
        print('      fun(ele, "ele", 6, "long", level + 1);', file=body),
    elif f["element"] == "uint":
        print('      fun(ele, "ele", 7, "uint", level + 1);', file=body),
    else:
        print(f'      {n}_{f["element"]}_walk(ele, fun, "{f["name"]}", level + 1);', file=body)

    print('    }', file=body)
    print(f'    fun(self->{f["name"]}, "{f["name"]}", 37, "{f["name"]}", --level);', file=body)
#    print('  }', file=body)

def do_map_body_walk(n, f):
    print(f'  //fun(&self->{f["name"]}, "{f["name"]}", 17, "map");', file=body),

#    element_type = deque_elem_type(n, f)
#    mapname = element_type + "_map"
#    nodename = element_type + "_mapnode_t"
#
#    if "modifier" in f and f["modifier"] == "compact":
#        print(f'  ushort {f["name"]}_len = (ushort){mapname}_size(self->{f["name"]}, self->{f["name"]}_root);', file=body)
#        print(f'  fd_walk_short_u16(&{f["name"]}_len, ctx);', file=body)
#    else:
#        print(f'  ulong {f["name"]}_len = {mapname}_size(self->{f["name"]}, self->{f["name"]}_root);', file=body)
#        print(f'  fd_bincode_uint64_walk(&{f["name"]}_len, ctx);', file=body)
#    print(f'  for ( {nodename}* n = {mapname}_minimum(self->{f["name"]}, self->{f["name"]}_root); n; n = {mapname}_successor(self->{f["name"]}, n) ) {{', file=body);
#    print(f'      {n}_{f["element"]}_walk(&n->elem, ctx);', file=body)
#    print('  }', file=body)

def do_array_body_walk(n, f):
    length = f["length"]

    if f["element"] == "uchar":
        print(f'fd_bincode_bytes_walk(self->{f["name"]}, {length}, ctx);', file=body)
        return

    print(f'  for (ulong i = 0; i < {length}; ++i)', file=body)

    if f["element"] in fields_body_vector_walk:
        body.write("  ")
        fields_body_vector_walk[f["element"]](namespace, f)
    else:
        print(f'    {n}_{f["element"]}_walk(self->{f["name"]} + i, fun, "{f["element"]}", level + 1);', file=body)

fields_body_option_walk = {
    "char" :              lambda n, f: print(f'  //fun(&self->{f["name"]}, "{f["name"]}", 1, "char", level + 1);', file=body),
    "char*" :             lambda n, f: print(f'  //fun(self->{f["name"]}, "{f["name"]}", 2, "char*", level + 1);', file=body),
    "char[32]" :          lambda n, f: print(f'  //fun(self->{f["name"]}, "{f["name"]}", 3, "char[32]", level + 1);', file=body),
    "char[7]" :           lambda n, f: print(f'  //fun(self->{f["name"]}, "{f["name"]}", 4, "char[7]", level + 1);', file=body),
    "double" :            lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 5, "double", level + 1);', file=body),
    "long" :              lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 6, "long", level + 1);', file=body),
    "uint" :              lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 7, "uint", level + 1);', file=body),
    "uint128" :           lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 8, "uint128", level + 1);', file=body),
    "uchar" :     lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 9, "uchar", level + 1);', file=body),
    "uchar[32]" : lambda n, f: print(f'  //fun(self->{f["name"]}, "{f["name"]}", 10, "uchar[32]", level + 1);', file=body),
    "ulong" :     lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 11, "ulong", level + 1);', file=body),
    "ushort" :            lambda n, f: print(f'  fun(self->{f["name"]}, "{f["name"]}", 12, "ushort", level + 1);', file=body),
}

def do_option_body_walk(n, f):
    if f["element"] in fields_body_option_walk:
        fields_body_option_walk[f["element"]](namespace, f)
    else:
        print(f'  // fun(&self->{f["name"]}, "{f["name"]}", 16, "option", level + 1);', file=body),

fields_body_walk = {
    "char" :              lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 1, "char", level + 1);', file=body),
    "char*" :             lambda n, f, e: print(f'  fun(self->{e}{f["name"]}, "{f["name"]}", 2, "char*", level + 1);', file=body),
    "char[32]" :          lambda n, f, e: print(f'  fun(self->{e}{f["name"]}, "{f["name"]}", 3, "char[32]", level + 1);', file=body),
    "char[7]" :           lambda n, f, e: print(f'  fun(self->{e}{f["name"]}, "{f["name"]}", 4, "char[7]", level + 1);', file=body),
    "double" :            lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 5, "double", level + 1);', file=body),
    "long" :              lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 6, "long", level + 1);', file=body),
    "uint" :              lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 7, "uint", level + 1);', file=body),
    "uint128" :           lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 8, "uint128", level + 1);', file=body),
    "uchar" :     lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 9, "uchar", level + 1);', file=body),
    "uchar[32]" : lambda n, f, e: print(f'  fun(self->{e}{f["name"]}, "{f["name"]}", 10, "uchar[32]", level + 1);', file=body),
    "ulong" :     lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 11, "ulong", level + 1);', file=body),
    "ushort" :            lambda n, f, e: print(f'  fun(&self->{e}{f["name"]}, "{f["name"]}", 12, "ushort", level + 1);', file=body),
    "vector" :            lambda n, f, e: do_vector_body_walk(n, f),
    "deque" :             lambda n, f, e: do_deque_body_walk(n, f),
    "array" :             lambda n, f, e: do_array_body_walk(n, f),
    "option" :            lambda n, f, e: do_option_body_walk(n, f),
    "map" :               lambda n, f, e: do_map_body_walk(n, f),
}

# Map different names for the same times into how firedancer knows them
for entry in entries:
    if "fields" in entry:
      for f in entry["fields"]:
          if f["type"] in type_map:
              f["type"] = type_map[f["type"]]
          if "element" in f:
              if f["element"] in type_map:
                  f["element"] = type_map[f["element"]]
    if "variants" in entry:
      for v in entry["variants"]:
          if "type" in v and v["type"] in type_map:
              v["type"] = type_map[v["type"]]

# Generate one instance of the fd_deque.c template for each unique element type.
deque_element_types = set()

map_element_types = dict()

for entry in entries:
    # Create the dynamic vector types needed for this entry
    if "fields" in entry:
      for f in entry["fields"]:
          if f["type"] == "deque":
              element_type = deque_elem_type(namespace, f)
              if element_type in deque_element_types:
                  continue

              dp = deque_prefix(namespace, f)
              if "max" in f:
                  print("#define DEQUE_NAME " + dp, file=header)
                  print("#define DEQUE_T " + element_type, file=header)
                  print(f'#define DEQUE_MAX {f["max"]}', file=header)
                  print('#include "../../util/tmpl/fd_deque.c"', file=header)
                  print("#undef DEQUE_NAME", file=header)
                  print("#undef DEQUE_T", file=header)
                  print("#undef DEQUE_MAX", file=header)
                  print(f'static inline {element_type} *', file=header)
                  print(f'{dp}_alloc( fd_valloc_t valloc ) {{', file=header)
                  print(f'  void * mem = fd_valloc_malloc( valloc, {dp}_align(), {dp}_footprint());', file=header)
                  print(f'  return {dp}_join( {dp}_new( mem ) );', file=header)
                  print("}", file=header)
              else:
                  print("#define DEQUE_NAME " + dp, file=header)
                  print("#define DEQUE_T " + element_type, file=header)
                  print('#include "../../util/tmpl/fd_deque_dynamic.c"', file=header)
                  print("#undef DEQUE_NAME", file=header)
                  print("#undef DEQUE_T\n", file=header)
                  print(f'static inline {element_type} *', file=header)
                  print(f'{dp}_alloc( fd_valloc_t valloc, ulong len ) {{', file=header)
                  if "growth" in f:
                      print(f'  ulong max = len + {f["growth"]};', file=header) # Provide headroom
                  else:
                      print(f'  ulong max = len + len/5 + 10;', file=header) # Provide headroom
                  print(f'  void * mem = fd_valloc_malloc( valloc, {dp}_align(), {dp}_footprint( max ));', file=header)
                  print(f'  return {dp}_join( {dp}_new( mem, max ) );', file=header)
                  print("}", file=header)

              deque_element_types.add(element_type)

          if f["type"] == "map":
            element_type = deque_elem_type(namespace, f)
            if element_type in map_element_types:
                continue

            mapname = element_type + "_map"
            nodename = element_type + "_mapnode"
            print(f"typedef struct {nodename} {nodename}_t;", file=header)
            print(f"#define REDBLK_T {nodename}_t", file=header)
            print(f"#define REDBLK_NAME {mapname}", file=header)
            print(f"#define REDBLK_IMPL_STYLE 1", file=header)
            print(f'#include "../../util/tmpl/fd_redblack.c"', file=header)
            print(f"#undef REDBLK_T", file=header)
            print(f"#undef REDBLK_NAME", file=header)
            print(f"struct {nodename} {{", file=header)
            print(f"    {element_type} elem;", file=header)
            print(f"    ulong redblack_parent;", file=header)
            print(f"    ulong redblack_left;", file=header)
            print(f"    ulong redblack_right;", file=header)
            print(f"    int redblack_color;", file=header)
            print("};", file=header)
            print(f'static inline {nodename}_t *', file=header)
            print(f'{mapname}_alloc( fd_valloc_t valloc, ulong len ) {{', file=header)
            print(f'  void * mem = fd_valloc_malloc( valloc, {mapname}_align(), {mapname}_footprint(len));', file=header)
            print(f'  return {mapname}_join({mapname}_new(mem, len));', file=header)
            print("}", file=header)

            map_element_types[element_type] = f["key"]

    if "comment" in entry and "type" in entry and entry["type"] != "enum":
      print(f'/* {entry["comment"]} */', file=header)

    n = f'{namespace}_{entry["name"]}'

    if "attribute" in entry:
        a = f'__attribute__{entry["attribute"]} '
    else:
        a = ""

    if "type" in entry and entry["type"] == "struct":
      print(f'struct {a}{n} {{', file=header)
      for f in entry["fields"]:
          if f["type"] in fields_header:
              fields_header[f["type"]](namespace, f)
          else:
               print(f'  {namespace}_{f["type"]}_t {f["name"]};', file=header)

      print("};", file=header)
      print(f'typedef struct {n} {n}_t;', file=header)

    elif "type" in entry and entry["type"] == "enum":
      print(f'union {a}{n}_inner {{', file=header)

      empty = True
      for v in entry["variants"]:
          if "type" in v:
            empty = False
            if v["type"] in fields_header:
                fields_header[v["type"]](namespace, v)
            else:
                print(f'  {namespace}_{v["type"]}_t {v["name"]};', file=header)
      if empty:
          print('  uchar nonempty; /* Hack to support enums with no inner structures */ ', file=header)

      print("};", file=header)
      print(f"typedef union {n}_inner {n}_inner_t;\n", file=header)

      if "comment" in entry:
        print("/* " + entry["comment"] + " */", file=header)

      print(f"struct {a}{n} {{", file=header)
      print('  uint discriminant;', file=header)
      print(f'  {n}_inner_t inner;', file=header)
      print("};", file=header)
      print(f"typedef struct {n} {n}_t;", file=header)

    print(f"#define {n.upper()}_FOOTPRINT sizeof({n}_t)", file=header)
    print(f"#define {n.upper()}_ALIGN (8UL)", file=header)
    print("", file=header)

print("", file=header)
print("FD_PROTOTYPES_BEGIN", file=header)
print("", file=header)

for entry in entries:
    if "attribute" in entry:
        continue
    n = namespace + "_" + entry["name"]

    if entry["type"] == "enum":
        print(f"void {n}_new_disc({n}_t* self, uint discriminant);", file=header)
    print(f"void {n}_new({n}_t* self);", file=header)
    print(f"int {n}_decode({n}_t* self, fd_bincode_decode_ctx_t * ctx);", file=header)
    print(f"int {n}_encode({n}_t const * self, fd_bincode_encode_ctx_t * ctx);", file=header)
    print(f"void {n}_destroy({n}_t* self, fd_bincode_destroy_ctx_t * ctx);", file=header)
    print(f"void {n}_walk({n}_t* self, fd_walk_fun_t fun, const char *name, int level);", file=header)
    print(f"ulong {n}_size({n}_t const * self);", file=header)
    print("", file=header)

    if entry["type"] == "enum":
        for i, v in enumerate(entry["variants"]):
            print(f'FD_FN_PURE uchar {n}_is_{v["name"]}({n}_t const * self);', file=header)
            print(f'FD_FN_PURE uchar {n}_is_{v["name"]}({n}_t const * self) {{', file=body)
            print(f'  return self->discriminant == {i};', file=body)
            print("}", file=body)
        print("enum {", file=header)

        for i, v in enumerate(entry["variants"]):
            print(f'{n}_enum_{v["name"]} = {i},', file=header)
        print("}; ", file=header)

    if entry["type"] == "enum":
        print(f'void {n}_inner_new({n}_inner_t* self, uint discriminant);', file=body)
        print(f'int {n}_inner_decode({n}_inner_t* self, uint discriminant, fd_bincode_decode_ctx_t * ctx) {{', file=body)
        print(f'  {n}_inner_new(self, discriminant);', file=body)
        print('  int err;', file=body)
        print('  switch (discriminant) {', file=body)

        for i, v in enumerate(entry["variants"]):
            print(f'  case {i}: {{', file=body)
            if "type" in v:
                if v["type"] in fields_body_decode:
                    body.write("  ")
                    fields_body_decode[v["type"]](namespace, v)
                    print('    return FD_BINCODE_SUCCESS;', file=body)
                else:
                    print(f'    return {namespace}_{v["type"]}_decode(&self->{v["name"]}, ctx);', file=body)
            else:
                print('    return FD_BINCODE_SUCCESS;', file=body)
            print('  }', file=body)

        print('  default: return FD_BINCODE_ERR_ENCODING;', file=body);

        print('  }', file=body)
        print("}", file=body)

        print(f'int {n}_decode({n}_t* self, fd_bincode_decode_ctx_t * ctx) {{', file=body)
        if "compact" in entry and entry["compact"]:
            print('  ushort tmp = 0;', file=body)
            print('  int err = fd_bincode_compact_u16_decode(&tmp, ctx);', file=body)
            print('  self->discriminant = tmp;', file=body)
        else:
            print('  int err = fd_bincode_uint32_decode(&self->discriminant, ctx);', file=body)

        print('  if ( FD_UNLIKELY(err) ) return err;', file=body)
        print(f'  return {namespace}_{entry["name"]}_inner_decode(&self->inner, self->discriminant, ctx);', file=body)
        print("}", file=body)
    else:
      print(f'int {n}_decode({n}_t* self, fd_bincode_decode_ctx_t * ctx) {{', file=body)
      print('  int err;', file=body)
      for f in entry["fields"]:
          if f["type"] in fields_body_decode:
              fields_body_decode[f["type"]](namespace, f)
          else:
              print(f'  err = {namespace}_{f["type"]}_decode(&self->{f["name"]}, ctx);', file=body)
              print('  if ( FD_UNLIKELY(err) ) return err;', file=body)
      print('  return FD_BINCODE_SUCCESS;', file=body)
      print("}", file=body)

    if entry["type"] == "enum":
      print(f'void {n}_inner_new({n}_inner_t* self, uint discriminant) {{', file=body)
      print('  switch (discriminant) {', file=body)
      for i, v in enumerate(entry["variants"]):
        print(f'  case {i}: {{', file=body)
        if "type" in v:
            if v["type"] in fields_body_new:
                fields_body_new[v["type"]](namespace, v)
            else:
                print(f'    {namespace}_{v["type"]}_new(&self->{v["name"]});', file=body)
        print('    break;', file=body)
        print('  }', file=body)
      print('  default: break; // FD_LOG_ERR(( "unhandled type"));', file=body)
      print('  }', file=body)
      print("}", file=body)

      print(f'void {n}_new_disc({n}_t* self, uint discriminant) {{', file=body)
      print('  self->discriminant = discriminant;', file=body)
      print(f'  {namespace}_{entry["name"]}_inner_new(&self->inner, self->discriminant);', file=body)
      print("}", file=body)
      print(f'void {n}_new({n}_t* self) {{', file=body)
      print(f'  {namespace}_{entry["name"]}_new_disc(self, UINT_MAX);', file=body) # Invalid by default
      print("}", file=body)
    else:
      print(f'void {n}_new({n}_t* self) {{', file=body)
      print(f'  fd_memset(self, 0, sizeof({n}_t));', file=body)
      for f in entry["fields"]:
          if f["type"] in fields_body_new:
              fields_body_new[f["type"]](namespace, f)
          else:
              print(f'  {namespace}_{f["type"]}_new(&self->{f["name"]});', file=body)
      print("}", file=body)

    if entry["type"] == "enum":
      print(f'void {n}_inner_destroy({n}_inner_t* self, uint discriminant, fd_bincode_destroy_ctx_t * ctx) {{', file=body)
      print('  switch (discriminant) {', file=body)
      for i, v in enumerate(entry["variants"]):
        print(f'  case {i}: {{', file=body)
        if "type" in v:
            if v["type"] in fields_body_destroy:
                fields_body_destroy[v["type"]](namespace, v)
            else:
                print(f'    {namespace}_{v["type"]}_destroy(&self->{v["name"]}, ctx);', file=body)
        print('    break;', file=body)
        print('  }', file=body)
      print('  default: break; // FD_LOG_ERR(( "unhandled type" ));', file=body)
      print('  }', file=body)
      print("}", file=body)

      print(f'void {n}_destroy({n}_t* self, fd_bincode_destroy_ctx_t * ctx) {{', file=body)
      print(f'  {namespace}_{entry["name"]}_inner_destroy(&self->inner, self->discriminant, ctx);', file=body)
      print("}", file=body)
    else:
      print(f'void {n}_destroy({n}_t* self, fd_bincode_destroy_ctx_t * ctx) {{', file=body)
      for f in entry["fields"]:
          if f["type"] in fields_body_destroy:
              fields_body_destroy[f["type"]](namespace, f)
          else:
              print(f'  {namespace}_{f["type"]}_destroy(&self->{f["name"]}, ctx);', file=body)
      print("}", file=body)
    print("", file=body)

    print(f'void {n}_walk({n}_t* self, fd_walk_fun_t fun, const char *name, int level) {{', file=body)
    print(f'  fun(self, name, 32, "{n}", level++);', file=body)

    if entry["type"] == "enum":
        print(f'  // enum {namespace}_{f["type"]}_walk(&self->{f["name"]}, fun, "{f["name"]}", level + 1);', file=body)

        print('  switch (self->discriminant) {', file=body)
        for i, v in enumerate(entry["variants"]):
          if "type" in v:
            print(f'  case {i}: {{', file=body)
            if v["type"] in fields_body_walk:
                fields_body_walk[v["type"]](namespace, v, "inner.")
            else:
                print(f'    {namespace}_{v["type"]}_walk(&self->inner.{v["name"]}, fun, "{v["name"]}", level + 1);', file=body)
            print('    break;', file=body)
            print('  }', file=body)
        print('  }', file=body)
    else:
        for f in entry["fields"]:
            if f["type"] in fields_body_walk:
                fields_body_walk[f["type"]](namespace, f, "")
            else:
                print(f'  {namespace}_{f["type"]}_walk(&self->{f["name"]}, fun, "{f["name"]}", level + 1);', file=body)

    print(f'  fun(self, name, 33, "{n}", --level);', file=body)
    print("}", file=body)

    print(f'ulong {n}_size({n}_t const * self) {{', file=body)

    if entry["type"] == "enum":
      print('  ulong size = 0;', file=body)
      print('  size += sizeof(uint);', file=body)
      print('  switch (self->discriminant) {', file=body)
      for i, v in enumerate(entry["variants"]):
          if "type" in v:
            print(f'  case {i}: {{', file=body)
            if v["type"] in fields_body_size:
                body.write("  ")
                fields_body_size[v["type"]](namespace, v)
            else:
                print(f'    size += {namespace}_{v["type"]}_size(&self->inner.{v["name"]});', file=body)
            print('    break;', file=body)
            print('  }', file=body)
      print('  }', file=body)

    else:
      print('  ulong size = 0;', file=body)
      for f in entry["fields"]:
          if f["type"] in fields_body_size:
              fields_body_size[f["type"]](namespace, f)
          else:
              print(f'  size += {namespace}_{f["type"]}_size(&self->{f["name"]});', file=body)

    print('  return size;', file=body)
    print("}", file=body)
    print("", file=body)
    if entry["type"] == "enum":
        print(f'int {n}_inner_encode({n}_inner_t const * self, uint discriminant, fd_bincode_encode_ctx_t * ctx) {{', file=body)
        first = True
        for i, v in enumerate(entry["variants"]):
            if "type" in v:
              if first:
                  print('  int err;', file=body)
                  print('  switch (discriminant) {', file=body)
                  first = False
              print(f'  case {i}: {{', file=body)
              if v["type"] in fields_body_encode:
                  body.write("  ")
                  fields_body_encode[v["type"]](namespace, v)
              else:
                  print(f'    err = {namespace}_{v["type"]}_encode(&self->{v["name"]}, ctx);', file=body)
                  print('    if ( FD_UNLIKELY(err) ) return err;', file=body)
              print('    break;', file=body)
              print('  }', file=body)
        if not first:
            print('  }', file=body)
        print('  return FD_BINCODE_SUCCESS;', file=body)
        print("}", file=body)

        print(f'int {n}_encode({n}_t const * self, fd_bincode_encode_ctx_t * ctx) {{', file=body)
        print('  int err;', file=body)
        print('  err = fd_bincode_uint32_encode(&self->discriminant, ctx);', file=body)
        print('  if ( FD_UNLIKELY(err) ) return err;', file=body)
        print(f'  return {namespace}_{entry["name"]}_inner_encode(&self->inner, self->discriminant, ctx);', file=body)
        print("}", file=body)
    else:
      print(f'int {n}_encode({n}_t const * self, fd_bincode_encode_ctx_t * ctx) {{', file=body)
      print('  int err;', file=body)
      if "fields" in entry:
        for f in entry["fields"]:
            if f["type"] in fields_body_encode:
                fields_body_encode[f["type"]](namespace, f)
            else:
                print(f'  err = {namespace}_{f["type"]}_encode(&self->{f["name"]}, ctx);', file=body)
                print('  if ( FD_UNLIKELY(err) ) return err;', file=body)
      print('  return FD_BINCODE_SUCCESS;', file=body)
      print("}", file=body)
    print("", file=body)

for (element_type,key) in map_element_types.items():
    mapname = element_type + "_map"
    nodename = element_type + "_mapnode_t"
    print(f'#define REDBLK_T {nodename}', file=body)
    print(f'#define REDBLK_NAME {mapname}', file=body)
    print(f'#define REDBLK_IMPL_STYLE 2', file=body)
    print(f'#include "../../util/tmpl/fd_redblack.c"', file=body)
    print(f'#undef REDBLK_T', file=body)
    print(f'#undef REDBLK_NAME', file=body)
    print(f'long {mapname}_compare({nodename} * left, {nodename} * right) {{', file=body)
    if key == "pubkey" or key == "account" or key == "key":
        print(f'  return memcmp(left->elem.{key}.uc, right->elem.{key}.uc, sizeof(right->elem.{key}));', file=body)
    else:
        print(f'  return (long)(left->elem.{key} - right->elem.{key});', file=body)
    print("}", file=body)

print("FD_PROTOTYPES_END", file=header)
print("", file=header)
print("#endif // HEADER_" + json_object["name"].upper(), file=header)
