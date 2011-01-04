/* 
sample int_array_t int_array_t_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class int_array_t_s implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_int_array_t_s(int[][][][] value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return int_array_t_s.class;
    }
    
    public String getName() {
      return "int_array_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_int_array_t_s(int_array_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[][][][] value) throws IOException {
    e.begin(int_array_t_s.class);
    int i_0_max = 1;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      int i_1_max = 2;
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        int i_2_max = 3;
        int i_3_max = 4;
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
            e.encodeInt(value[i_0][i_1][i_2][i_3]);
          }
        }
      }
    }
    e.end(int_array_t_s.class);
  }
  
  public static int[][][][] decode(LabCommDecoder d) throws IOException {
    int[][][][] result;
    {
      int i_0_max = 1;
      result = new int[i_0_max][][][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = 2;
          result[i_0] = new int[i_1_max][][];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = 3;
              int i_3_max = 4;
              result[i_0][i_1] = new int[i_2_max][];
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                result[i_0][i_1][i_2] = new int[i_3_max];
                for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
                  result[i_0][i_1][i_2][i_3] = d.decodeInt();
                }
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [1]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 1, 
      // array [2]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 2, 
        // array [3, 4]
        0, 0, 0, 16, 
          0, 0, 0, 2, 
          0, 0, 0, 3, 
          0, 0, 0, 4, 
          0, 0, 0, 35, 
        // }
      // }
    // }
  };

}
