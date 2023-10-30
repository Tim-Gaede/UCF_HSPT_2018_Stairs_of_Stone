import java.util.*;

public class heavy {
   // Some constants
   static final int N = 8;
   // ~Worst case blocks to remove
   static final int inf = 100 * 11;
   
   static int h;
   static int[][] as;
   public static void main(String[] args) {
      Scanner in = new Scanner(System.in);
      int t = in.nextInt();
      for (int puzzle = 1; puzzle <= t; puzzle++) {
         // Read the input into some global variables (so solve() can use them)
         h = in.nextInt();
         as = new int[N][N];
         for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
               as[i][j] = in.nextInt();
            }
         }
         int ans = inf;
         // Brute force the middle point of the path
         for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
               // Every point of the path must be at least the needed height (h/2 for the middle)
               int middleHeight = h / 2;
               if (as[i][j] < middleHeight)
                  continue;
               
               // Find all possible paths going down (lower heights) and up (higher heights) (meet in the middle approach)
               ArrayList<Path> listDown = new ArrayList<>(), listUp = new ArrayList<>();
               solve(i, j, middleHeight - 1, -1, 0, 0L, listDown);
               solve(i, j, middleHeight + 1, 1, 0, 0L, listUp);
               
               // Remove repeats (cuts both lists in half roughly) and sort by cost
               reduce(listDown);
               reduce(listUp);
               
               // Brute force every pair of down/up
               // We will find a working Path from `up` for each `down` quickly
               // Since we sorted them by cost, we will be able to break after we find one
               // This is the key optimization
               for(Path down : listDown) {
                  for(Path up : listUp) {
                     // If the two masks conflict (meaning they both tried to use the same cell), skip this mask
                     // &~set(0,i,j) is done to ignore the shared cell of (i,j) whose cost was paid for outside the two dfs's
                     // It will turn off that bit so it doesn't show up as a conflict
                     // Ignoring that, any shared bits (i.e. nonzero bitwise AND) means they conflict
                     if((down.mask & up.mask & ~set(0,i,j)) != 0)
                        continue;
                     // Cost is the (down path cost) + (middle cell cost) + (up path cost)
                     int cost = down.cost + (as[i][j] - middleHeight) + up.cost;
                     // This is a candidate answer to try
                     ans = Math.min(cost, ans);
                     // Since cost will not go down on future iterations with this `down`, we can break out now
                     break;
                  }
               }
            }
         }
         System.out.printf("Crypt #%d: %d\n", puzzle, ans);
      }
   }

   static void reduce(ArrayList<Path> list) {
      Collections.sort(list, (a,b) -> {
         // If masks are the same, put lower costs first so we keep that one
         if(a.mask == b.mask)
            return Integer.compare(a.cost, b.cost);
         // Otherwise, sort by mask so that masks group up
         return Long.compare(a.mask, b.mask);
      });
      ArrayList<Path> result = new ArrayList<>();
      long lastMask = -1;
      for(Path path : list) {
         // If this mask is the same as our last mask, it is a repeat
         // We can skip this
         if(path.mask == lastMask) continue;
         // We have not yet seen this mask, so add it
         // This is the best version since we tiebreak sort by cost
         result.add(path);
         // Set lastMask to this mask, so we skip any more occurences of it
         // Since we sorted by mask, these will all be consecutive
         lastMask = path.mask;
      }
      // Move this result list into the parameter list
      list.clear();
      list.addAll(result);
      
      // Sort the list by cost, for reasons explained when this function is called
      Collections.sort(list, (a,b) -> a.cost - b.cost);
   }

   static void solve(int i, int j, int ch, int delta, int sum, long mask, ArrayList<Path> list) {
      // Flag this cell as visited in our bitmask
      // this is effectively a boolean[][] for which cells we have seen
      // except using the bits in a long
      mask = set(mask,i,j);
      // If we have satisfied all of the heights for our direction, we are done with this path
      if(ch == -1 || ch == h+1) {
         // Add this path to our list of candidates, and return
         list.add(new Path(mask,sum));
         return;
      }
      // Try all adjacent cells
      for (int ni = i - 1; ni <= i + 1; ni++) {
         for (int nj = j - 1; nj <= j + 1; nj++) {
            // Skip out of bounds
            if(ni < 0 || nj < 0 || ni >= N || nj >= N) continue;
            // Skip things we've visited in this half
            if(in(mask, ni, nj)) continue;
            // Every point of the path must be at least the needed height
            if(as[ni][nj] < ch) continue;
            // Add the cost and recurse down this path (adjusting ch by delta, to indicate we need 1 more or 1 less height next)
            solve(ni, nj, ch+delta, delta, sum + (as[ni][nj] - ch), mask, list);
         }
      }
   }

   // Check if a cell is in a bitmask
   static boolean in(long mask, int i, int j) {
      return (mask & (1L << (i*N+j))) != 0;
   }

   // Add a cell to a bitmask
   static long set(long mask, int i, int j) {
      return mask | (1L << (i*N+j));
   }

   // Class to hold a Path (consists of a used bitmask and a cost)
   static class Path {
      long mask;
      int cost;
      public Path(long mask, int sum) {
         this.mask = mask;
         this.cost = sum;
      }
   }
}
