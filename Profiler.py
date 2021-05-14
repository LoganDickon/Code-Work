# -*- coding: utf-8 -*-
"""
Created on Thu May 13 21:13:04 2021

@author: 123
"""
import cProfile, pstats, io
def Profile(fnc):
    ###A decorator that uses cProfile to profile a function###
    def inner(*args, **kwargs):
        pr = cProfile.Profile()
        pr.enable()
        retval = fnc(*args, **kwargs)
        pr.disable()
        s = io.StringIO()
        sortby = 'cumulative'
        ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
        ps.print_stats()
        print(s.getvalue())
        return retval
    return inner