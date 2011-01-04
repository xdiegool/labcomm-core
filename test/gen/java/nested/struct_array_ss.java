/* 
sample struct {
  int aa;
  boolean bb;
  int_array_ss ias;
} struct_array_ss[1][_][_];
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_array_ss implements LabCommSample {

  public int aa;
  public boolean bb;
  public int[][][][][] ias;
  
  public interface Handler extends LabCommHandler {
    public void handle_struct_array_ss(struct_array_ss[][][] value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_array_ss.class;
    }
    
    public String getName() {
      return "struct_array_ss";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_array_ss(struct_array_ss.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_ss[][][] value) throws IOException {
    e.begin(struct_array_ss.class);
    int i_0_max = 1;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].length);
      int i_1_max = value[i_0].length;
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        e.encodeInt(value[i_0][i_1].length);
        int i_2_max = value[i_0][i_1].length;
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0][i_1][i_2].aa);
          e.encodeBoolean(value[i_0][i_1][i_2].bb);
          int i_3_max = 1;
          for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
            e.encodeInt(value[i_0][i_1][i_2].ias[i_3].length);
            int i_4_max = value[i_0][i_1][i_2].ias[i_3].length;
            int i_5_max = 3;
            e.encodeInt(value[i_0][i_1][i_2].ias[i_3][0][0].length);
            int i_6_max = value[i_0][i_1][i_2].ias[i_3][0][0].length;
            for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
              for (int i_5 = 0 ; i_5 < i_5_max ; i_5++) {
                for (int i_6 = 0 ; i_6 < i_6_max ; i_6++) {
                  int i_7_max = 5;
                  for (int i_7 = 0 ; i_7 < i_7_max ; i_7++) {
                    e.encodeInt(value[i_0][i_1][i_2].ias[i_3][i_4][i_5][i_6][i_7]);
                  }
                }
              }
            }
          }
        }
      }
    }
    e.end(struct_array_ss.class);
  }
  
  public static struct_array_ss[][][] decode(LabCommDecoder d) throws IOException {
    struct_array_ss[][][] result;
    {
      int i_0_max = 1;
      result = new struct_array_ss[i_0_max][][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = d.decodeInt();
          result[i_0] = new struct_array_ss[i_1_max][];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = d.decodeInt();
              result[i_0][i_1] = new struct_array_ss[i_2_max];
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                result[i_0][i_1][i_2] = new struct_array_ss();
                result[i_0][i_1][i_2].aa = d.decodeInt();
                result[i_0][i_1][i_2].bb = d.decodeBoolean();
                {
                  int i_3_max = 1;
                  result[i_0][i_1][i_2].ias = new int[i_3_max][][][][];
                  for (int i_3 = 0 ; i_3 < i_3_max ; i_3++) {
                    {
                      int i_4_max = d.decodeInt();
                      int i_5_max = 3;
                      int i_6_max = d.decodeInt();
                      result[i_0][i_1][i_2].ias[i_3] = new int[i_4_max][][][];
                      for (int i_4 = 0 ; i_4 < i_4_max ; i_4++) {
                        result[i_0][i_1][i_2].ias[i_3][i_4] = new int[i_5_max][][];
                        for (int i_5 = 0 ; i_5 < i_5_max ; i_5++) {
                          result[i_0][i_1][i_2].ias[i_3][i_4][i_5] = new int[i_6_max][];
                          for (int i_6 = 0 ; i_6 < i_6_max ; i_6++) {
                            {
                              int i_7_max = 5;
                              result[i_0][i_1][i_2].ias[i_3][i_4][i_5][i_6] = new int[i_7_max];
                              for (int i_7 = 0 ; i_7 < i_7_max ; i_7++) {
                                result[i_0][i_1][i_2].ias[i_3][i_4][i_5][i_6][i_7] = d.decodeInt();
                              }
                            }
                          }
                        }
                      }
                    }
                  }
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
      // array [_]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        // array [_]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 0, 
          // struct { 3 fields
          0, 0, 0, 17, 
            0, 0, 0, 3, 
            // int 'aa'
            0, 0, 0, 2, 
            97, 97, 
            0, 0, 0, 35, 
            // boolean 'bb'
            0, 0, 0, 2, 
            98, 98, 
            0, 0, 0, 32, 
            // int_array_ss 'ias'
            0, 0, 0, 3, 
            105, 97, 115, 
            // array [1]
            0, 0, 0, 16, 
              0, 0, 0, 1, 
              0, 0, 0, 1, 
              // array [_, 3, _]
              0, 0, 0, 16, 
                0, 0, 0, 3, 
                0, 0, 0, 0, 
                0, 0, 0, 3, 
                0, 0, 0, 0, 
                // array [5]
                0, 0, 0, 16, 
                  0, 0, 0, 1, 
                  0, 0, 0, 5, 
                  0, 0, 0, 35, 
                // }
              // }
            // }
          // }
        // }
      // }
    // }
  };

}
