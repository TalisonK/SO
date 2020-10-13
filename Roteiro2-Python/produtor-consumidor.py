import random
import sys
from threading import Thread, Lock, Condition
from time import sleep

mutex = Condition()
buffer = []
capacidade = 5


def produtor():
    global buffer
    r = 1
    while True:
        mutex.acquire()
        if len(buffer) == capacidade:
            print("buffer cheio")
            mutex.wait()
            r = 6
        else:
            if len(buffer) == 0:
                r = 1
                buffer.append(1)
            else:
                d = buffer[-1] + 1
                buffer.append(d)

            print("\033[1;32m Produziu, %d" % buffer[-1])

        mutex.notify()
        mutex.release()
        sleep(r)


def cliente():
    global buffer
    while True:

        mutex.acquire()
        if len(buffer) == 0:
            print("buffer vazio")
            mutex.wait()
            r = 1
        else:
            print("\033[1;31m Consumiu, buffer = %d" % buffer[-1])
            del (buffer[-1])

        mutex.notify()
        mutex.release()
        sleep(2)


tprodutor = Thread(target=produtor)
tcliente = Thread(target=cliente)

tprodutor.start()
tcliente.start()
