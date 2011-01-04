/* 
sample struct {
  int a;
} struct_array_s[2];
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_array_s implements LabCommSample {

  public int a;
  
  public interface Handler extends LabCommHandler {
    public void handle_struct_array_s(struct_array_s[] value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_array_s.class;
    }
    
    public String getName() {
      return "struct_array_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_array_s(struct_array_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_s[] value) throws IOException {
    e.begin(struct_array_s.class);
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].a);
    }
    e.end(struct_array_s.class);
  }
  
  public static struct_array_s[] decode(LabCommDecoder d) throws IOException {
    struct_array_s[] result;
    {
      int i_0_max = 2;
      result = new struct_array_s[i_0_max];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = new struct_array_s();
        result[i_0].a = d.decodeInt();
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      // struct { 1 fields
      0, 0, 0, 17, 
        0, 0, 0, 1, 
        // int 'a'
        0, 0, 0, 1, 
        97, 
        0, 0, 0, 35, 
      // }
    // }
  };

}
