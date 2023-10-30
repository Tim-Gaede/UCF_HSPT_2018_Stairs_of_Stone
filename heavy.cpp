/*
 * To solve this problem we will be doing some creative pruning. There is another
 * solution, using meet in the middle BFS, if want to read that implementation
 * see the Java solution.
 *
 * The naive way to solve this problem is fairly simple. Starting at every valid
 * ending position, DFS through the grid until we either hit 0 or cannot "move to"
 * any of the adjacent blocks. The cost of each move is just the difference in the
 * height we are looking for and the height the tower is initially. We cannot move
 * to blocks where the height is too short. After we complete all DFS searches we
 * take the smallest final cost as our answer.
 *
 * Obviously, this method is too slow.  But we can use some clever tricks to make
 * it much faster.  We know that if we are currently at height h, we need to find
 * the shortest path from our position to a block with height 0 that doesn't reuse
 * any blocks we have already used. If we ignore that last requirement, the minimum
 * cost from our position and height will always be the same, and no other path
 * starting there will ever be cheaper.  We can precompute this paths cost for small
 * h values very easily. Then, we can use this precomputation as a best case outcome.
 * If we know our current cost + the cost of the "best case outcome" from this
 * position will not improve our global minimum, we can safely break out of this path.
 *
 * Using this "best case outcome" for pruning will massively improve our performance
 * in the worst case, which is when all the blocks are very tall and of very similar
 * height.  This is because in nearly all cases, our best case will only match the
 * global minimum, allowing us to break out early.
 *
 */


#include <bits/stdc++.h>
using namespace std;

int DX[] = { 1,  1,  1,  0,  0, -1, -1, -1};
int DY[] = {-1,  0,  1, -1,  1, -1,  0,  1};
long minVal = (long)1e12;
int puzzle[8][8];
int precomp[8][8][4];

struct path{
   int x;
   int y;
   int cost;
};

bool operator<(const path &a, const path &b){
   // Return whether "a" is less than "b"
   return a.cost < b.cost;
}

// This is the Depth-First Search will will use to solve the problem
void dfs(int x, int y, bool used[8][8], long cost, int nextHeight){
   if(nextHeight == -1){
      minVal = min(minVal, cost);
      return;
   }

   /*
    * We know this path is safe to break out of if the cost is already >= the global
    * minimum OR the cost + the best case outcome (minimum theoretical cost) is >=
    * the global minimum.
    */
   if(cost >= minVal) return;

   if(nextHeight < 4){
      if(precomp[x][y][nextHeight]+cost >= minVal) return;
   }

   /*
    * Here we continue on our DFS search.
    */
   for(int d=0; d<8; d++){
      int nx = x + DX[d];
      int ny = y + DY[d];

      // Check if it is OK to move to (nx, ny)
      if(nx < 0 || ny < 0 || nx >= 8 || ny >= 8) continue;
      if(used[nx][ny]) continue;
      if(puzzle[nx][ny] < nextHeight) continue;

      // Mark (nx, ny) as "used" and continue DFS from there
      used[nx][ny] = true;
      dfs(nx, ny, used, cost+puzzle[nx][ny], nextHeight-1);
      used[nx][ny] = false;
   }
}

// This is the DFS we will use for precomputation, so it has no pruning
int dfsX(int x, int y, bool used[8][8], int nextHeight){
   if(nextHeight == -1){
      return 0;
   }

   // Keep track of the cheapest path from this state, which we will return
   int best = (int)1e9;
   for(int d=0; d<8; d++){
      int nx = x + DX[d];
      int ny = y + DY[d];

      // Check if it is OK to move to (nx, ny)
      if(nx < 0 || ny < 0 || nx >= 8 || ny >= 8) continue;
      if(used[nx][ny]) continue;
      if(puzzle[nx][ny] < nextHeight) continue;

      // Mark (nx, ny) as "used" and continue DFS from there
      used[nx][ny] = true;
      best = min(best, dfsX(nx, ny, used, nextHeight-1)+puzzle[nx][ny]);
      used[nx][ny] = false;
   }
   return best;
}

int main(){
   int runs; cin >> runs;
   for(int r = 0; r<runs; r++){
      int h; cin >> h;
      minVal = (long) 1e12;

      // Read in the puzzle map
      for(int i=0; i<8; i++)
         for(int j=0; j<8; j++)
            cin >> puzzle[i][j];

      // Initialize the precomputation table
      for(int i=0; i<8; i++)
         for(int j=0; j<8; j++)
            for(int k=0; k<4; k++)
               precomp[i][j][k] = 0;

      // Precompute all small cases for pruning
      for(int s=0; s<4; s++){
         for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
               bool used[8][8];
               for(int k=0; k<8; k++)
                  for(int w=0; w<8; w++)
                     used[k][w] = false;

               used[i][j] = true;
               precomp[i][j][s] = dfsX(i, j, used, s);
            }
         }
      }

      /* This is a small optimization to improve initial path in worst case. It
       * guarantees we will start at the "lowest down" initial height which can
       * improve the initial global minimum we find.
       */
      vector<path> starts;
      for(int i=0; i<8; i++)
         for(int j=0; j<8; j++)
            if(puzzle[i][j] >= h){
               path p;
               p.x = i;
               p.y = j;
               p.cost = puzzle[i][j];
               starts.push_back(p);
            }

      sort(starts.begin(), starts.end());
      for(int i=0; i<starts.size(); i++){
         bool used[8][8];
         for(int j=0; j<8; j++)
            for(int k=0; k<8; k++)
               used[j][k] = false;

         used[starts[i].x][starts[i].y] = true;
         dfs(starts[i].x, starts[i].y, used, starts[i].cost, h-1);
      }

      /*
       * I am storing the cost as the sum of the heights of the blocks we take, but
       * we only pay the cost reduce them to the needed heights. This always gives
       * us a specific discount based on the total height, so here I calculate it
       * and subtract it from our minimum cost.
       */
      int sum = 0;
      for(int i=1; i<=h; i++)
         sum += i;
      cout << "Crypt #" << r+1 << ": " << minVal-sum << endl;
   }

   return 0;
}
