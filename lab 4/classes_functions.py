__author__ = 'chukwuyem'
from random import randint



def rand_funct():
    gen_num = -1
    rand_num = randint(0, 100000)
    random_file = open( "random_numbers.txt")
    for i, line in enumerate(random_file, start=1):
        if i == rand_num:
            gen_num = line
            break
    return int(gen_num)

class process:
    ''' implementation of process'''

    def __init__(self, number, numRefs, S, A, B, C):
        self.number = int(number)   #process number
        self.numRefs = int(numRefs) #total number of references to be generated
        self.numRefsLeft = int(numRefs) #number of references not yet generated
        self.refSize = int(S)   #size of process (each reference is 0 to S-1)
        self.nextWordRef = (111 * self.number)  % self.refSize
        self.numFaults = 0  #number of page faults
        self.residTime = 0  #total residency time (sum of residency time for all evicted pages)
        self.numEvicts = 0
        self.A = int(A)
        self.B =int(B)
        self.C = int(C)



class frame:
    '''implementation of frame'''

    def __init__(self, number):
        self.number = number
        self.pageNum = -1
        self.procNum = -1
        self.firstLoadTime = -1
        self.loadTime = -1

def process_index(p_num, p_list): #return index of process in a list of processes
    for index, process in enumerate(p_list):
        if process.number == p_num:
            return index
    else:
        return -1

def frame_index(f_num, f_list): #return index of frame in a list of frames
    for index, frame in enumerate(f_list):
        if frame.number == f_num:
            return index
    else:
        return -1


#print rand_funct()

#rearranging a list
# a = [5, 2, 9, 12, 4]
# b = a.index(9)
# print [a[b]] + a[:b]+ a[b+1:]

# print (126/40)
#
# for x in reversed(range(0, (1000/40))):
#     print x,
