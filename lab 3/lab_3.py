#!/usr/bin/python
__author__ = 'chukwuyem'
import re
import sys

class task:
    '''implement of tasks'''

    def __init__(self, task_number):
        self.task_number = int(task_number)
        self.activities = []
        self.resource_claims = dict()
        self.resource_alloc = dict()
        self.current_activity = 0
        self.computing = False
        self.current_computing_cycles = -1
        self.time_left_computing = -1
        self.total_time = 0
        self.total_waiting_time = 0
        self.final_status = ''

    def acquire_resource(self, res_num, value):
        self.resource_alloc[res_num] += value

    def release_resource(self, res_num, value):
        self.resource_alloc[res_num] -= value

class resource:
    ''' implementation of resource '''

    def __init__(self, resource_number, resource_value):
        self.resource_number = int(resource_number)
        self.resource_value = int(resource_value)

    def lend_permission(self, value):
        if value <= self.resource_value: return True
        else: return False


    def lend_resource(self, value):
        self.resource_value -= value

    def reacquire_resource(self, value):
        self.resource_value += value



def resource_index(res_id, res_list):
    return [x for x, y in enumerate(res_list) if y.resource_number == res_id][0]

def task_index(t_id, t_list):
    return [x for x, y in enumerate(t_list) if y.task_number == t_id][0]


def input_proc(filename):
    input_file = open(filename, 'r')
    list_f = input_file.readlines()
    main_task_list = []
    main_resource_list = []

    #getting resources
    for x, y in enumerate(list_f[0].split(' ')[2:], start=1):
        main_resource_list.append(resource(x, y))

    #creating tasks
    for x in range(int(list_f[0].split(' ')[0])):
        main_task_list.append(task(x+1))

    #adding task activities
    for x in list_f[1:]:
        if x[0]!= '\n':
            x = re.sub('\t+', '', re.sub(' +', ' ', x)) #remove multiple whitespace and tabs

            #x.split(' ')[1] is task number

            [t.activities.append(x.replace('\n', '')) for t in main_task_list if t.task_number== int(x.split(' ')[1])]

    #setting up resource allocation for each task
    res_num_list = [x.resource_number for x in main_resource_list]
    for t in main_task_list:
        for r in res_num_list: t.resource_alloc[r] = 0

    return main_task_list, main_resource_list

def orm(verbose, task_list, resource_list):
    '''
    this is the optimistic resource manager function
    orm_task_list is the list of all running task
    orm_paused_tasks is the list of all tasks that have been blocked due to ungrantable requests
    orm_buffer_tasks is a buffer list of all tasks just released from paused tasks whose requests
    were granted in the current cycle. In the next cycle, they are added to orm_task_list
    the main while loop, checks if there is a task in these lists. If all are empty, then every task
    has been terminated or aborted
    :param verbose: if true, it prints each cycle run
    :param task_list: the list of all tasks
    :param resource_list: the list of all resources
    :return: not return, just prints
    '''
    orm_task_list = task_list
    orm_resource_list = resource_list
    orm_stopped_tasks = []
    orm_paused_tasks = []
    orm_buffer_tasks = [] #tasks just release from paused, added on next loop
    orm_released_resources = [] #buffer list for released resources to be available in succeeding cycle
    current_cycle = 0


    while len(orm_task_list) > 0 or len(orm_paused_tasks) > 0 or len(orm_buffer_tasks) > 0:

        if verbose: print 'During', current_cycle, '-', current_cycle+1

        #add all released resource back to resource list
        for x in orm_released_resources:
            orm_resource_list[resource_index(x[0], orm_resource_list)].reacquire_resource(x[1])

        for x in orm_resource_list: #
            if verbose: print '\t*Resource', x.resource_number, 'has', x.resource_value, 'units'

        [orm_task_list.append(x) for x in orm_buffer_tasks]
        orm_buffer_tasks = []

        # print '\t*testing'
        # print '\t',['Task'+str(x.task_number) for x in orm_task_list]

        #first check blocked tasks
        if verbose: print '\tFirst check blocked tasks'
        for x in orm_paused_tasks:
            x.total_time += 1
            if x.activities[x.current_activity].split(' ')[0] == 'request':
                re_num = int(x.activities[x.current_activity].split(' ')[2])
                request_value = int(x.activities[x.current_activity].split(' ')[3])
                #check if the request can be fulfilled
                if orm_resource_list[resource_index(re_num, orm_resource_list)].lend_permission(request_value):
                    #the request CAN be fulfilled
                    orm_resource_list[resource_index(re_num, orm_resource_list)].lend_resource(request_value)
                    x.acquire_resource(re_num, request_value)
                    if verbose: print '\t\tTask', x.task_number, 'completes its request'
                    #add to buffer task
                    orm_buffer_tasks.append(x)
                    #delete from paused task
                    del orm_paused_tasks[task_index(x.task_number,orm_paused_tasks)]
                else: #the request CANNOT be fulfilled
                    if verbose: print '\t\tTask', str(x.task_number)+'\'s request cannot be granted'
                    x.total_waiting_time += 1
            else:
                print 'ERROR: It\'s not a request'
                break

        #run of task list
        if len(orm_task_list) > 0:
            for x in orm_task_list:
                if verbose: print '\tworking on', x.task_number
                x.total_time += 1
                if not x.computing:
                    if x.activities[x.current_activity].split(' ')[0] == 'initiate':
                        if verbose: print '\tTask', x.task_number, 'initiates'
                        x.current_activity += 1

                    elif x.activities[x.current_activity].split(' ')[0] == 'request':
                        re_num = int(x.activities[x.current_activity].split(' ')[2])
                        request_value = int(x.activities[x.current_activity].split(' ')[3])
                        #check if the request can be fulfilled
                        if orm_resource_list[resource_index(re_num, orm_resource_list)].lend_permission(request_value):
                            #the request CAN be fulfilled
                            orm_resource_list[resource_index(re_num, orm_resource_list)].lend_resource(request_value)
                            x.acquire_resource(re_num, request_value)
                            if verbose: print '\tTask', x.task_number, 'completes its request'
                            x.current_activity += 1
                        else:
                            if verbose: print '\tTask', str(x.task_number)+'\'s request cannot be completed'
                            orm_paused_tasks.append(x)
                            x.total_waiting_time += 1
                            del orm_task_list[task_index(x.task_number, orm_task_list)]

                    elif x.activities[x.current_activity].split(' ')[0] == 'release':
                        re_num = int(x.activities[x.current_activity].split(' ')[2])
                        request_value = int(x.activities[x.current_activity].split(' ')[3])
                        x.release_resource(re_num, request_value)
                        orm_resource_list[resource_index(re_num, orm_resource_list)].reacquire_resource(request_value)
                        if verbose: print '\tTask', x.task_number, 'releases', request_value, 'units of', re_num
                        x.current_activity += 1

                    elif x.activities[x.current_activity].split(' ')[0] == 'compute':
                        x.computing = True
                        x.time_left_computing = int(x.activities[x.current_activity].split(' ')[2]) - 1
                        #because this cycle is one computing cycle
                        if verbose: print '\tTask', x.task_number, 'is computing'

                    elif x.activities[x.current_activity].split(' ')[0] == 'terminate':
                        if verbose: print '\tTask', x.task_number, 'is completed'
                        x.total_time -= 1
                        x.final_status = 'TERMINATED'
                        orm_stopped_tasks.append(x)
                        del orm_task_list[task_index(x.task_number, orm_task_list)]

                    else:
                        print 'Activity:', x.activities[x.current_activity].split(' ')[0], ' =UNRECOGNIZED'
                        #return 1

                else: #task is computing
                    print 'Task', x.task_number, 'is computing'
                    x.time_left_computing -= 1
                    if x.time_left_computing == 0:
                        x.computing = False
                        x.current_activity += 1

        else: #task list is empty
            if len(orm_buffer_tasks) == 0: #buffer list is also empty hence no task can be run = DEADLOCK
                if verbose: print '\tA DEADLOCK HAS BEEN DETECTED'
                lowest_task_index = task_index(min([x.task_number for x in orm_paused_tasks]), orm_paused_tasks)

                #releasing all its allocations
                for x in orm_paused_tasks[lowest_task_index].resource_alloc.keys(): #x is a resource number
                    orm_resource_list[resource_index(x, orm_resource_list)].reacquire_resource(orm_paused_tasks
                                                                                               [lowest_task_index].resource_alloc[x])
                    #no need to deduct since the object itself is no longer useful

                #aborting the task
                orm_paused_tasks[lowest_task_index].final_status = 'ABORTED'
                orm_stopped_tasks.append(orm_paused_tasks[lowest_task_index])
                if verbose: print 'Task', orm_paused_tasks[lowest_task_index].task_number, 'has been aborted'
                del orm_paused_tasks[lowest_task_index]

                #populating task list
                orm_task_list = orm_paused_tasks
                #delete paused task list
                orm_paused_tasks = []

        current_cycle += 1

    print '\n'
    print 'Optimistic Resource Manager'
    print 'Task || Total time || Waiting Time || Percentage'

    for x in orm_stopped_tasks:
        if x.final_status == 'TERMINATED':
            print x.task_number, ' || ', x.total_time, ' || ', x.total_waiting_time, ' || ',\
                (float(x.total_waiting_time)/float(x.total_time)) * 100
        else:
            print 'Task', x.task_number, 'was aborted'


def can_complete(a_task, a_res_list):
    '''
    :param a_task: a task
    :param a_res_list: a resource list
    :return: given a task and a list of resoources with their quantities, it returns true if the the task
    can be terminated
    '''
    for res_id in a_task.resource_alloc.keys():
        if a_task.resource_claims[res_id] - a_task.resource_alloc[res_id] >  resource_index(res_id, a_res_list):
            return False
    return True

def safe_state(curr_task_list, curr_resource_list, current_task_index):
    '''
    :param curr_task_list: the current list of tasks that are running that can be terminated
    :param curr_resource_list: the current list of resources and their amounts
    :param current_task_index: the index of the task who's making a request in the curr_task_list
    :return:
    given a task, a task list and a resource list with quantity for each resource, the function shows if
    the task's request will result in a safe state or not
    '''
    mock_task_list = curr_task_list
    mock_resource_list = curr_resource_list

    #mock granting the request
    re_num = int(mock_task_list[current_task_index].activities[mock_task_list[current_task_index].current_activity].split(' ')[2])
    request_val = int(mock_task_list[current_task_index].activities[mock_task_list[current_task_index].current_activity].split(' ')[3])
    mock_resource_list[resource_index(re_num, mock_resource_list)].lend_permission(request_val)
    mock_task_list[current_task_index].acquire_resource(re_num, request_val)

    while len(mock_task_list) > 1:
        mock_complete_task_id = -1
        iter = 0
        #looking for a task that can complete
        while iter < len(mock_task_list) and mock_complete_task_id == -1:
            if can_complete(mock_task_list[iter], mock_resource_list) == True: mock_complete_task_id = mock_task_list[iter].task_number
            iter += 1
        if mock_complete_task_id == -1: #no task can complete
            return False
        else:
            #releasing the task's resources
            p = task_index(mock_complete_task_id, mock_task_list) #index of the task identified above
            for x in mock_task_list[p].resource_alloc.keys():
                mock_resource_list[resource_index(x, mock_resource_list)].reacquire_resource(mock_task_list[p].resource_alloc[x])
            #terminating the task and removing it from task list
            del mock_task_list[p]

    return True #the only returns if mock_task_list has one task left, meaning all the others successfully terminate


def bankers(verbose, task_list, resource_list):
    '''
    this is the banker's algorithm, also a resource managing algorithm like the ORM
    :param verbose: if verbose is true, the algorithm is run explicitly, printing each action that occurs
    :param task_list: the list of all tasks that will be run
    :param resource_list: the list of all resources with their total quantities
    :return: no returns, just prints
    the while loop works like the while loop of the optimistic resource manager
    so are the variables
    '''
    bank_task_list = task_list
    bank_resource_list = resource_list
    bank_stopped_tasks = []
    bank_paused_tasks = []
    bank_buffer_tasks = [] #same function as counterpart in orm
    bank_released_resources= []
    current_cycle = 0

    print '\n'

    while len(bank_task_list) > 0 or len(bank_buffer_tasks) > 0:
        if verbose: print 'During', current_cycle, '-', current_cycle+1

        #add all released reources
        for x in bank_released_resources:
            bank_resource_list[resource_index(x[0], bank_resource_list)].reacquire_resource(x[1])

        [bank_task_list.append(x) for x in bank_buffer_tasks]
        bank_buffer_tasks = []

        #first check blocked tasks
        if verbose: print '\tFirst check blocked tasks'
        for x in bank_paused_tasks:
            x.total_time += 1
            if x.activities[x.current_activity].split(' ')[0] == 'request':
                curr_running_tasks = bank_task_list
                curr_running_tasks.append(x)
                #check if the request can be fulfilled
                if safe_state(curr_running_tasks, bank_resource_list, task_index(x.task_number, curr_running_tasks)) == True:
                    #state is safe so request can be granted
                    re_num = int(x.activities[x.current_activity].split(' ')[2])
                    request_value = int(x.activities[x.current_activity].split(' ')[3])
                    bank_resource_list[resource_index(re_num, bank_resource_list)].lend_resource(request_value)
                    x.acquire_resource(re_num, request_value)
                    if verbose: print '\t\tTask', x.task_number, 'completes its request'
                    #add to buffer task
                    bank_buffer_tasks.append(x)
                    #delete from paused task
                    del bank_paused_tasks[task_index(x.task_number, bank_paused_tasks)]
                else: #the request CANNOT Be fulfilled
                    if verbose: print '\t\tTask', str(x.task_number)+'\'s request cannot be completed'
                    x.total_waiting_time += 1
            else:
                print 'ERROR: It\'s not a request'
                break

        #run of task list
        if len(bank_task_list) > 0:
            for x in bank_task_list:
                if verbose: print '\tworking on', x.task_number
                x.total_time += 1
                if not x.computing:
                    if x.activities[x.current_activity].split(' ')[0] == 'initiate':
                        if verbose: print '\tTask', x.task_number, 'initiates'
                        res_num = int(x.activities[x.current_activity].split(' ')[2])
                        res_claim = int(x.activities[x.current_activity].split(' ')[3])
                        if res_claim > bank_resource_list[resource_index(res_num, bank_resource_list)].resource_value:
                            #initial claim is larger than what is available
                            x.final_status = 'ABORTED'
                            print '\tTask', x.task_number, ' has been aborted due to possessing insatiable claims'
                            bank_stopped_tasks.append(x)
                            del bank_task_list[task_index(x.task_number, bank_task_list)]
                        else:
                            x.resource_claims[res_num] = res_claim
                            x.current_activity += 1

                    elif x.activities[x.current_activity].split(' ')[0] == 'request':
                        re_num = int(x.activities[x.current_activity].split(' ')[2])
                        request_value = int(x.activities[x.current_activity].split(' ')[3])
                        if x.resource_claims[re_num] > request_value:
                            #the request concurs with initial claims
                            curr_running_tasks = bank_task_list
                            #check if request can be fulfilled
                            if safe_state(curr_running_tasks, bank_resource_list, task_index(x.task_number, curr_running_tasks)) == True:
                                #state is safe so request can be granted
                                bank_resource_list[resource_index(re_num, bank_resource_list)].lend_resource(request_value)
                                x.acquire_resource(re_num, request_value)
                                if verbose: print '\t\tTask', x.task_number, 'completes its request'
                                x.current_activity += 1
                            else:
                                if verbose: print '\tTask', str(x.task_number)+'\'s request cannot be completed'
                                bank_paused_tasks.append(x)
                                x.total_waiting_time += 1
                                del bank_task_list[task_index(x.task_number, bank_task_list)]
                        else:
                            #request does not concur with initial claims
                            x.final_status = 'ABORTED'
                            print '\tTask', x.task_number, 'has been aborted due to insatiable request'
                            bank_stopped_tasks.append(x)
                            del bank_task_list[task_index(x.task_number, bank_task_list)]

                    elif x.activities[x.current_activity].split(' ')[0] == 'release':
                        re_num = int(x.activities[x.current_activity].split(' ')[2])
                        request_value = int(x.activities[x.current_activity].split(' ')[3])
                        x.release_resource(re_num, request_value)
                        bank_resource_list[resource_index(re_num, bank_resource_list)].reacquire_resource(request_value)
                        if verbose: print '\tTask', x.task_number, 'releases', request_value, 'units of', re_num
                        x.current_activity += 1

                    elif x.activities[x.current_activity].split(' ')[0] == 'compute':
                        x.computing = True
                        x.time_left_computing = int(x.activities[x.current_activity].split(' ')[2]) - 1
                        #because this cycle is one computing cycle
                        if verbose: print '\tTask', x.task_number, 'is computing'

                    elif x.activities[x.current_activity].split(' ')[0] == 'terminate':
                        if verbose: print '\tTask', x.task_number, 'is completed'
                        x.total_time -= 1
                        x.final_status = 'TERMINATED'
                        bank_stopped_tasks.append(x)
                        del bank_task_list[task_index(x.task_number, bank_task_list)]

                    else:
                        print 'Activity:', x.activities[x.current_activity].split(' ')[0], ' =UNRECOGNIZED'

                else: #task is computing
                    print 'Task', x.task_number, 'is computing'
                    x.time_left_computing -= 1
                    if x.time_left_computing == 0:
                        x.computing = False
                        x.current_activity += 1

        else:  #task list is empty
            if len(bank_buffer_tasks) == 0:
                print 'ERROR: Something is Wrong. This is a DEADLOCK'

        current_cycle += 1

    print '\n'
    print 'Banker\'s Algorithm'
    print 'Task || Total time || Waiting Time || Percentage'

    total_time = 0
    total_waiting_time = 0
    for x in bank_stopped_tasks:
        if x.final_status == 'TERMINATED':
            total_time += x.total_time
            total_waiting_time += x.total_waiting_time
            print x.task_number, ' || ', x.total_time, ' || ', x.total_waiting_time, ' || ',\
                (float(x.total_waiting_time)/float(x.total_time)) * 100
        else:
            print 'Task', x.task_number, 'was aborted'

    print 'total', total_time, ' || ', total_waiting_time, ' || '#, (float(total_waiting_time)/float(total_time)) * 100




def main():
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
        v = False
        if len(sys.argv) > 2 and sys.argv[2]== '--verbose':
            v = True

        list_tasks, list_resources = input_proc(input_file)

        # for z in list_tasks:
        #     print 'Task',z.task_number, '\n', z.activities, '\n', z.resource_alloc

        #print resource_index(2, main_resource_list)

        orm(v, list_tasks, list_resources)
        # print 'length of main_task_list is ', len(list_tasks)
        # for x in list_resources:
        #     print 'resource', x.resource_number, 'has', x.resource_value

        list_tasks, list_resources = input_proc(input_file)
        bankers(v, list_tasks, list_resources)
    else:
        print 'You did not enter a file'


main()