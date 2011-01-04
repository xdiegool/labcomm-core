/* 
sample int a_fixed_int_multi_array[2, 2, 2];
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class a_fixed_int_multi_array implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_a_fixed_int_multi_array(int[][][] value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return a_fixed_int_multi_array.class;
    }
    
    public String getName() {
      return "a_fixed_int_multi_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_a_fixed_int_multi_array(a_fixed_int_multi_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[][][] value) throws IOException {
    e.begin(a_fixed_int_multi_array.class);
    int i_0_max = 2;
    int i_1_max = 2;
    int i_2_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0][i_1][i_2]);
        }
      }
    }
    e.end(a_fixed_int_multi_array.class);
  }
  
  public static int[][][] decode(LabCommDecoder d) throws IOException {
    int[][][] result;
    {
      int i_0_max = 2;
      int i_1_max = 2;
      int i_2_max = 2;
      result = new int[i_0_max][][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = new int[i_1_max][];
        for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
          result[i_0][i_1] = new int[i_2_max];
          for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
            result[i_0][i_1][i_2] = d.decodeInt();
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2, 2, 2]
    0, 0, 0, 16, 
      0, 0, 0, 3, 
      0, 0, 0, 2, 
      0, 0, 0, 2, 
      0, 0, 0, 2, 
      0, 0, 0, 35, 
    // }
  };

}
