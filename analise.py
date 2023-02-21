from os import listdir
from os.path import isfile, join
import json
import matplotlib.pyplot as plt
import numpy as np

def last_number(e):
    return int(e[28:].split('.')[0])
directory = "dados/c"
files = [directory + "/" + f for f in listdir(directory) if isfile(join(directory, f)) and "json" in f]
files.sort(key=last_number)
# for f in files:
#     print(f)
# exit(1)
total_files = 0
total_list  = []
ok_files    = 0
ok_list     = []
lost_files  = 0
lost_list   = []
out_of_order_files = 0
out_of_order_list = []
x_axis = []
arrays = []
test = 0
size = 1000
for f in files:
    with open(f, 'r') as json_file:
        data = json.load(json_file)
        total_files += data['total_pacotes']
        total_list.append(data['total_pacotes'])

        ok_files += data['pacotes_ok']
        ok_list.append(data['pacotes_ok'])

        lost_files  += data['pacotes_perdidos']
        lost_list.append(data['pacotes_perdidos'])

        out_of_order_files += data['pacotes_fora_de_ordem']
        out_of_order_list.append(data['pacotes_fora_de_ordem'])
        # arrays.append([data['total_pacotes'], data['pacotes_ok'], data['pacotes_perdidos'], data['pacotes_fora_de_ordem']])
        x_axis.append(str(test))
    test += 1
    if test == 15:
        total_array = np.array(total_list)
        ok_array = np.array(ok_list)
        lost_array = np.array(lost_list)
        out_of_order_array = np.array(out_of_order_list)

        plt.bar(x_axis, ok_array, color='r')
        plt.bar(x_axis, lost_array, bottom = ok_array, color='b')
        plt.bar(x_axis, out_of_order_array, bottom = ok_array + lost_array, color='g')

        plt.xlabel("Testes")
        plt.ylabel("Quantidade")
        plt.savefig('plot' + str(size) + '.png')

        qt_files = len(files)
        print(test)
        print("media total : %d" % (total_files/test))
        print("media ok : %d" % (ok_files/test))
        print("media lost : %d" % (lost_files/test))
        print("media ooo : %d" % (out_of_order_files/test))
        total_files = 0
        total_list  = []
        ok_files    = 0
        ok_list     = []
        lost_files  = 0
        lost_list   = []
        out_of_order_files = 0
        out_of_order_list = []
        x_axis = []
        arrays = []
        test = 0
        size *= 10
