# Some constant we will use
oo = 1000000000
dr = [-1,-1,-1,0,0,1,1,1]
dc = [-1,0,1,-1,1,-1,0,1]

# Read in number of cases and loop over them
testcases = int(input())
for testcase in range(1,testcases+1):
    # Read in case
    N = int(input())
    mat = []
    order = []
    for i in range(0,8):
        mat.extend(map(int,input().split()))
    for i in range(0,64):
        order.append((mat[i],i))

    # Sort them
    order.sort()

    # Search looking for answer
    best = oo
    predepth = 6
    pre = []
    for i in range(0,64):
        for j in range(0,predepth):
            prebest = oo

            def dfs3(idx, h, sum, mask):
                global prebest
                if(h>mat[idx]):
                    return
                nextsum = sum + mat[idx]-h
                if(h==0):
                    prebest = min(prebest,nextsum)
                    return
                for i in range(0,8):
                    r = (idx>>3)+dr[i]
                    c = (idx&7)+dc[i]
                    temp = r*8+c
                    if(r<0 or r>=8 or c < 0 or c>=8 or ((mask)&(1<<temp))!=0):
                        continue
                    dfs3(temp,h-1,nextsum,mask|(1<<temp))

            dfs3(i,j,0,1<<i)
            pre.append(prebest);
    for j in order:
        i = j[1]
        def dfs2(idx,h,sum,mask):
            global best
            if(h>mat[idx] or sum > best):
                return
            nextsum = sum + mat[idx]-h
            if(h==0):
                best = min(best,nextsum)
                return
            if(h<predepth and sum+pre[idx*predepth+h]>=best):
                return
            for i in range(0,8):
                r = (idx>>3)+dr[i]
                c = (idx&7)+dc[i]
                temp = r*8+c
                if(r<0 or r>=8 or c<0 or c>=8 or ((mask)&(1<<temp))!=0):
                    continue
                dfs2(temp,h-1,nextsum,mask|(1<<temp))

        dfs2(i,N,0,1<<i)
    print("Crypt #",testcase,": ",best,sep='')

