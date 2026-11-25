>>> import dis
>>> def myfunc(alist):
... 
KeyboardInterrupt
>>> def myfunc(adict): adict[1] = 2
... 
>>> myfunc
<function myfunc at 0x7767471732e0>
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (2)
              4 LOAD_FAST                0 (adict)
              6 LOAD_CONST               2 (1)
              8 STORE_SUBSCR
             12 RETURN_CONST             0 (None)
>>> def myfunc1(adict): adict[1];
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (2)
              4 LOAD_FAST                0 (adict)
              6 LOAD_CONST               2 (1)
              8 STORE_SUBSCR
             12 RETURN_CONST             0 (None)
>>> dis.dis(myfunc1)
  1           0 RESUME                   0
              2 LOAD_FAST                0 (adict)
              4 LOAD_CONST               1 (1)
              6 BINARY_SUBSCR
             10 POP_TOP
             12 RETURN_CONST             0 (None)
>>> def myfunc(adict): adict[1] = 230
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (230)
              4 LOAD_FAST                0 (adict)
              6 LOAD_CONST               2 (1)
              8 STORE_SUBSCR
             12 RETURN_CONST             0 (None)
>>> def func1(): print('func1');return 1
... 
>>> def func2(): print('func2');return 1
... 
>>> a = dict()
>>> a[func1()] = func2()
func2
func1
>>> a[func1()]
func1
1
>>> a[func1()] = func2()
func2
func1
>>> def myfunc(adict): adict[1][2] = 230
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (230)
              4 LOAD_FAST                0 (adict)
              6 LOAD_CONST               2 (1)
              8 BINARY_SUBSCR
             12 LOAD_CONST               3 (2)
             14 STORE_SUBSCR
             18 RETURN_CONST             0 (None)
>>> def myfunc(adict): a = 230
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (230)
              4 STORE_FAST               1 (a)
              6 RETURN_CONST             0 (None)
>>> 


>>> import dis
>>> def myfunc(adict): adict[1][2] = 230
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_CONST               1 (230)
              4 LOAD_FAST                0 (adict)
              6 LOAD_CONST               2 (1)
              8 BINARY_SUBSCR
             12 LOAD_CONST               3 (2)
             14 STORE_SUBSCR
             18 RETURN_CONST             0 (None)
>>> def myfunc(adict): adict[1][2]
... 
>>> dis.dis(myfunc)
  1           0 RESUME                   0
              2 LOAD_FAST                0 (adict)
              4 LOAD_CONST               1 (1)
              6 BINARY_SUBSCR
             10 LOAD_CONST               2 (2)
             12 BINARY_SUBSCR
             16 POP_TOP
             18 RETURN_CONST             0 (None)
>>> 








>>> def f0(): print('f0()'); return 0;
...
>>> def f1(): print('f1()'); return 0;
...
>>> a = [ [0] ]
>>> a[f0()][f1()]
f0()
f1()
0
>>> def f2(): print('f2()'); return 0;
...
>>> a[f0()][f1()] = f2()
f2()
f0()
f1()

