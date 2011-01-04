/* 
sample struct_array_t struct_array_t_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_array_t_s implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_struct_array_t_s(struct_array_t[] value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_array_t_s.class;
    }
    
    public String getName() {
      return "struct_array_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_array_t_s(struct_array_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_array_t[] value) throws IOException {
    e.begin(struct_array_t_s.class);
    struct_array_t.encode(e, value);
    e.end(struct_array_t_s.class);
  }
  
  public static struct_array_t[] decode(LabCommDecoder d) throws IOException {
    struct_array_t[] result;
    result = struct_array_t.decode(d);
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
