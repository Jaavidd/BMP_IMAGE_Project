
tree=input("height : ")
tree=int(tree)

space=tree-1
hash=1
stumb=tree-1
while tree!=0:

    for i in range(space):
        print(" ",end="")


    for j in range(hash):
        print("#",end="")


    tree-=1
    print()
    space-=1
    hash+=2

for i in range(stumb):
    print(" ",end="")
print("#")