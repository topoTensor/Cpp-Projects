import json
import matplotlib.pyplot as plt
import numpy as np

time=[]
ys_0=[]
ys_1=[]

k=0.1
m=1.0
y_0=0.5
v=0.5
t_0=0.0

with open("history.json") as json_data:
    d=json.load(json_data)
    for i in d:
        time.append(i['t'])
        ys_0.append(i['y'][0])
        ys_1.append(i['y'][1])

time_np=np.array(time)
omega=np.sqrt(k/m)
        
plt.scatter(time, ys_0,label='euler position', alpha=0.5)
plt.scatter(time, ys_1,label='euler speed', alpha=0.5)

plt.scatter(time, y_0*np.cos(time_np*omega)+(v/omega)*np.sin(time_np*omega), label='original', alpha=0.5)

plt.legend()
plt.show()