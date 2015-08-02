def hello(data):
    print data

a = {"1": lambda : hello("hello"), "2": lambda :hello("world")}

a["1"]()
a["2"]()
