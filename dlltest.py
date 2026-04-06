from ctypes import *
from src.obj.State import Precinct

lib = CDLL("C:\\Users\\benja\\OneDrive\\Documents\\Projects\\BARDS\\dlltest.dll")

lib.bigger.argtypes = [c_char_p, c_int]
foo = create_string_buffer(b"foo bar bat BAZ", 32)

lib.bigger(foo, 43)
print(foo.value.decode('utf-8'))
