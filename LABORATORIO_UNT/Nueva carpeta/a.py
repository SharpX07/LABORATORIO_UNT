e1=1
e2=1/2
e3=1/4

total=0

for n in range(100):
    total+=(e1+e2-e3)
    e1=e1/8
    e2=e2/8
    e3=e3/8

print(10/7)
print(total)