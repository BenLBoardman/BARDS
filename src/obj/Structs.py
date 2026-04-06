from ctypes import *

from src.obj.Precinct import Precinct
from src.obj.State import State

class c_precinct_t(Structure):
    _fields_ = [("pop", c_uint),
                ("neighbors", c_void_p),
                ("num_neighbors", c_uint),
                ("index", c_uint),
                ("cname", c_char * 128),
                ("district", c_void_p)
    ]

    def __init__(self, precinct: Precinct):
        self.pop = precinct.pop
        self.cntNeighbprs = len(precinct.neighbors)
        self.index = precinct.index
        self.name = create_string_buffer(precinct.name.encode('utf-8')).raw
        self.district = precinct.district

    def makeCStructs(state: State):
        c_precincts = []
        for precinct in state.precincts:
            c_precincts.append(c_precinct_t(precinct))
        return c_precincts

