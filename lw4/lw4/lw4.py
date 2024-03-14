from os import remove
import csv
from collections import defaultdict
from collections import deque
import itertools

def read_table_from_file(file_name):
    table = []
    with open(file_name, encoding='utf-8') as r_file:
        file_reader = csv.reader(r_file, delimiter = ";")
        
        for item in file_reader:
            table.append(item)
            
    return table

def do_e_closure(automat, state):
    result = []
    states = []
    if '/' in state:
        states = state.split('/')
        result.extend(states)
    else:
        states.append(state)
        result.append(state)

    stack = []
    to_result = set()
      
    for st in states:
        stack.append(st)
        
        while len(stack) != 0:
            temp_state = stack.pop(0)
            for trans, to_state in automat[temp_state]:
                if trans == 'e' and to_state != temp_state:
                    to_result.add(to_state)
                    if not ('e', temp_state) in automat[to_state]:
                        stack.append(to_state)
                        
    result.extend(list(to_result))
    return result

def read_NKA_from_file(file_name):
      result = defaultdict(list)
      transitions = set()
      table = read_table_from_file(file_name)
      final_states = []
      
      for i in range(1, len(table[1])):
          for j in range(2, len(table)):
              st_name = table[1][i]
              
              if table[0][i] != '':
                   final_states.append(st_name)
                   
              if table[j][i] != '':
                  if ',' in table[j][i]:
                      states = table[j][i].split(',')
                      for state in states:
                          result[st_name].append((table[j][0], state))
                  else:
                      result[st_name].append((table[j][0], table[j][i]))
                      
                  transitions.add(table[j][0])
                  
      transitions = sorted(list(transitions))
      if 'e' in transitions:
            transitions.remove('e')
            transitions.append('e')
      return result, transitions, list(set(final_states))

def get_multimap_first_key(multimap):
    result = None
    count = 1
    for key in multimap:
        if count == 1:
            result = key
            count += 1
        else:
            break
    return result

def transform_NKA_to_DKA(automat, transitions, final_states):
    result = defaultdict(list)
    former_states = set()
    queue = deque()
    queue.append(get_multimap_first_key(automat))
    new_final_states = set()
      
    while len(queue) != 0:
        state = queue.popleft()
        if state in former_states:
            continue
        former_states.add(state)
        new_state = None
        e_closure = do_e_closure(automat, state)
        for fn_state in final_states:
            if fn_state in e_closure:
                new_final_states.add(state)
                new_final_states.add(fn_state)
        for transition in transitions:
            temp_state = []
            for e_clos in e_closure:
                for trans, to_state in automat[e_clos]:
                    if transition == trans and trans != 'e':
                        temp_state.append(to_state)
            new_state = '/'.join(list(set(sorted(temp_state))))

            if new_state != '':
                result[state].append((transition, new_state))
                queue.append(new_state)

    return result, list(new_final_states)

def rename_to_states(automat, new_states):
    result = defaultdict(list)
    print(new_states)
    
    for state in automat:
        temp_list = []
    
        for trans, to_state in automat[state]:
            new_to_state = to_state
            for new_state in new_states:
                if to_state in new_states[new_state]:
                    new_to_state = new_state                    
            temp_list.append((trans, new_to_state))
            
        result[state] = temp_list
    return result

def minimize_DKA(automat):
    result = defaultdict(list)
    new_states = defaultdict(list)
    past_states = set()
    is_full_minim = False
    
    for state in automat:
        if state in past_states:
            continue
        new_state = state
        list_states = []
        is_minim = False
        
        for st in automat:
            if state == st:
                continue
            if sorted(automat[state]) == sorted(automat[st]):
                is_minim = True
                list_states.append(st)
                list_states.append(state)
                new_state += '/' + st
                past_states.add(st)
                
        result[new_state] = automat[state]
        if is_minim:
            new_states[new_state] = list_states
            is_full_minim = True
                
    if is_full_minim:
        result = rename_to_states(result, new_states)
        
    return result

def do_multimap_of_all_transitions(automat, transitions):
    result = defaultdict(list)
    for transition in transitions:
        idx = 0            
        temp_list = [i for i in itertools.repeat('', len(automat))]
        
        for state in automat:
            for trans, to_state in automat[state]:
                if transition == trans:
                    temp_list[idx] = to_state
            idx += 1
            
        result[transition] = temp_list
    return result

def check_for_final_states(final_states, state):
    result = False
    for fn_state in final_states:
        if fn_state in state:
            result = True
            break
    return result

def do_writable_view(automat, transitions, final_states):
    table = [[''], ['']]
    
    if 'e' in transitions:
        transitions.remove('e')
        
    for state in automat:
        if check_for_final_states(final_states, state):
            table[0].append('F')
        else:
            table.append('')
        table[1].append(state)
        
    trans = do_multimap_of_all_transitions(automat, transitions)
    for transition in trans:
        temp_list = []
        temp_list.append(transition)
        for st in trans[transition]:
            temp_list.append(st)
        table.append(temp_list)
        
    return table

def write_automat_in_file(file_name, automat, transitions, final_states):
    table = do_writable_view(automat, transitions, final_states)
    print(table)
    with open(file_name, mode="w", encoding='utf-8') as w_file:
        file_writer = csv.writer(w_file, delimiter = ";", lineterminator="\r")
        for row in table:
            file_writer.writerow(row)
      

file_name = 'input.txt'
automat, transitions, final_states = read_NKA_from_file(file_name)
print(automat, ' ', transitions, ' ', final_states)

dka, final_states = transform_NKA_to_DKA(automat, transitions, final_states)

print('fin ', final_states)
print("dka")
for key in dka:
    print(key, ' ', dka[key])
    
dka = minimize_DKA(dka)
write_automat_in_file('out.csv', dka, transitions, final_states)