from time import time



if __name__ == "__main__":
    h01 = dict()

    LOAD = 1000000

    for i in range(0, LOAD):
        h01[i] = i


    start = time()
    for i in range(0, LOAD):
        value = h01[i]
    end = time() - start

    print(end)
