__author__ = 'chukwuyem'

import random
import classes_functions
import sys

global MACH_SIZE #macine size
global PAGE_SIZE #page size
global REF_SIZE #size of process, i.e. references are from 0 to REF_SIZE - 1
global JOB_MIX #Numbers of jobs and other stuff
global NUM_REFS #number of references per job
global REPLACE_ALGO #the replacement algorithm

#algorithm explanation
#LIFO - return last frame to be loaded (always frame 0)
#random - random frame from list
#LRU - return frame with the oldest activity (hits/load/reload)

global process_list
global frames_table

global TOTAL_REFS #total number of references (used for main loop)

#creating processes and frames
def input_processing(M, P, S, J, N, R):

    #global declarations
    global MACH_SIZE
    global PAGE_SIZE
    global REF_SIZE
    global JOB_MIX
    global NUM_REFS
    global REPLACE_ALGO

    global process_list
    global frames_table
    process_list = []
    frames_table = []

    global TOTAL_REFS
    TOTAL_REFS = 0

    #setting global declarations
    MACH_SIZE = int(M)
    PAGE_SIZE = int(P)
    REF_SIZE = int(S)
    JOB_MIX = int(J)
    NUM_REFS = int(N)
    REPLACE_ALGO = R

    #creating processes
    if JOB_MIX == 1:
        p1 = classes_functions.process(1, NUM_REFS, REF_SIZE, 1, 0, 0)
        process_list.append(p1)
        TOTAL_REFS = NUM_REFS
    elif JOB_MIX == 2:
        for x in range(1, 5):
            p = classes_functions.process(x, NUM_REFS, REF_SIZE, 1, 0, 0)
            TOTAL_REFS += NUM_REFS
            process_list.append(p)
    elif JOB_MIX == 3:
        for x in range(1, 5):
            p = classes_functions.process(x, NUM_REFS, REF_SIZE, 0, 0, 0)
            TOTAL_REFS += NUM_REFS
            process_list.append(p)
    elif JOB_MIX == 4:
        p1 = classes_functions.process(1, NUM_REFS, REF_SIZE, 0.75, 0.25, 0)
        p2 = classes_functions.process(2, NUM_REFS, REF_SIZE, 0.75, 0, 0.25)
        p3 = classes_functions.process(3, NUM_REFS, REF_SIZE, 0.75, 0.125, 0.125)
        p4 = classes_functions.process(4, NUM_REFS, REF_SIZE, 0.5, 0.125, 0.125)
        TOTAL_REFS += (NUM_REFS * 4)
        process_list.extend([p1, p2, p3, p4])

    #creating frames
    for x in reversed(range(0, (MACH_SIZE/PAGE_SIZE))):
        frames_table.append(classes_functions.frame(x))


def pageHit(page_n, proc_n): #check if page already in frame table given page and process number
    for fr in frames_table:
        if fr.pageNum == page_n and fr.procNum == proc_n:
            return True, fr.number
    else:
        return False, -1

def freeFrame():
    for index_, fr in enumerate(frames_table):
        if fr.pageNum == -1:
            return True, index_
    else:
        return False, -1



def driver_pager(verbose):
    '''
    implementation of the driver and pager
    '''
    global MACH_SIZE
    global PAGE_SIZE
    global REF_SIZE
    global JOB_MIX
    global NUM_REFS
    global REPLACE_ALGO
    global process_list
    global frames_table
    global TOTAL_REFS

    #print 'len of process list', len(process_list)

    print ''
    print 'The machine size is', MACH_SIZE
    print 'The page size is', PAGE_SIZE
    print 'The process size is', REF_SIZE
    print 'The job mix number is', JOB_MIX
    print 'The number of references per process is', NUM_REFS
    print 'The replacement algorithm is', REPLACE_ALGO
    print 'The level of debugging output is', verbose
    print ''

    time = 1

    while TOTAL_REFS > 0: #main loop
        for pr in process_list: # for each process...->
            ref_quantum = 3
            while ref_quantum > 0 and pr.numRefsLeft > 0:   # <-...3 references are generated

                #simulate this reference for this process
                pg = pr.nextWordRef / PAGE_SIZE
                if int(verbose) > 0: print pr.number, 'references word', pr.nextWordRef, '(page', pg, ') at time', time, ':',

                #check if page in frame_table
                if (pageHit(pg, pr.number)[0] == True): #HIT
                    fr_n = pageHit(pg, pr.number)[1]
                    if int(verbose) > 0: print 'Hit in frame', fr_n
                    if REPLACE_ALGO.lower() == 'lru': #rearrange because this frame was just 'used'
                        f_in = classes_functions.frame_index(fr_n, frames_table)
                        frames_table = [frames_table[f_in]] + frames_table[:f_in] + frames_table[f_in+1:]

                #FAULT: if not, put it in
                #looking for free frames
                else:
                    if int(verbose) > 0: print 'Fault,',
                    pr.numFaults += 1
                    if freeFrame()[0] == True: #there is a free frame
                        f_ind = freeFrame()[1]
                        frames_table[f_ind].pageNum = pg
                        frames_table[f_ind].procNum = pr.number
                        frames_table[f_ind].firstLoadTime = time
                        frames_table[f_ind].loadTime = time
                        #no need the rearrange frametable..
                        if int(verbose) > 0: print 'using free frame', frames_table[f_ind].number

                    else:   #all frames are full... time to evict
                        if REPLACE_ALGO.lower() == 'lifo':
                            #finding last frame in, should be 0
                            newest_t = -1
                            newest_ind = -1
                            for i,x in enumerate(frames_table):
                                if x.firstLoadTime > newest_t:
                                    newest_ind = i

                            resid_time = time - frames_table[newest_ind].loadTime
                            evict_pr = classes_functions.process_index(frames_table[newest_ind].procNum, process_list)
                            process_list[evict_pr].residTime += resid_time
                            process_list[evict_pr].numEvicts += 1

                            if int(verbose) > 0: print 'evicting page', frames_table[newest_ind].pageNum, 'of', \
                                frames_table[newest_ind].procNum, 'from frame', frames_table[newest_ind].number

                            #replacing
                            frames_table[newest_ind].pageNum = pg
                            frames_table[newest_ind].procNum = pr.number
                            frames_table[newest_ind].loadTime = time

                        elif REPLACE_ALGO.lower() == 'random':
                            #getting random frame
                            rand_ind = random.randrange(0, len(frames_table))

                            resid_time = time - frames_table[rand_ind].loadTime
                            evict_pr = classes_functions.process_index(frames_table[rand_ind].procNum, process_list)
                            process_list[evict_pr].residTime += resid_time
                            process_list[evict_pr].numEvicts += 1

                            if int(verbose) > 0: print 'evicting page', frames_table[rand_ind].pageNum, 'of', \
                                frames_table[rand_ind].procNum, 'from frame', frames_table[rand_ind].number

                            #replacing
                            frames_table[rand_ind].pageNum = pg
                            frames_table[rand_ind].procNum = pr.number
                            frames_table[rand_ind].loadTime = time

                        elif REPLACE_ALGO.lower() == 'lru':
                            #from the way the program is built, lru is always the last frame in frames_table
                            lru_ind = len(frames_table) - 1

                            resid_time = time - frames_table[lru_ind].loadTime
                            evict_pr = classes_functions.process_index(frames_table[lru_ind].procNum, process_list)
                            process_list[evict_pr].residTime += resid_time
                            process_list[evict_pr].numEvicts += 1

                            if int(verbose) > 0: print 'evicting page', frames_table[lru_ind].pageNum, 'of', \
                                frames_table[lru_ind].procNum, 'from frame', frames_table[lru_ind].number

                            #replacing
                            frames_table[lru_ind].pageNum = pg
                            frames_table[lru_ind].procNum = pr.number
                            frames_table[lru_ind].loadTime = time
                            #rearranging
                            frames_table = [frames_table[lru_ind]] + frames_table[:lru_ind] + frames_table[lru_ind+1:]

                        else:
                            print 'There is Error'
                            return

                #calculate the next reference for this process
                random_num = classes_functions.rand_funct()
                if int(verbose)== 11: print pr.number, 'uses random number:', random_num
                y = random_num / 21474836478.0

                if y < pr.A:
                    pr.nextWordRef = (pr.nextWordRef + 1) % REF_SIZE
                elif y < (pr.A + pr.B):
                    pr.nextWordRef = (pr.nextWordRef - 5) % REF_SIZE
                elif y < (pr.A + pr.B + pr.C):
                    pr.nextWordRef = (pr.nextWordRef + 4) % REF_SIZE
                else:
                    pr.nextWordRef = random.randrange(0, REF_SIZE)

                ref_quantum -= 1
                pr.numRefsLeft -= 1
                time += 1
                TOTAL_REFS -= 1


    print ''
    for proc_ in process_list:
        print 'Process', proc_.number, 'had', proc_.numFaults, 'faults and', proc_.residTime/float(proc_.numEvicts), \
            'average residency'







def main():
    #do something
    verbose = sys.argv[7]
    #verbose = 1
    input_processing(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6])
    driver_pager(verbose)


main()


