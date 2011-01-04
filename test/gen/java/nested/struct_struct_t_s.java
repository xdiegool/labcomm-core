/* 
sample struct_struct_t struct_struct_t_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_struct_t_s implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_struct_struct_t_s(struct_struct_t value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_struct_t_s.class;
    }
    
    public String getName() {
      return "struct_struct_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_struct_t_s(struct_struct_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_struct_t value) throws IOException {
    e.begin(struct_struct_t_s.class);
    struct_struct_t.encode(e, value);
    e.end(struct_struct_t_s.class);
  }
  
  public static struct_struct_t decode(LabCommDecoder d) throws IOException {
    struct_struct_t result;
    result = struct_struct_t.decode(d);
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 1 fields
    0, 0, 0, 17, 
      0, 0, 0, 1, 
      // struct 'a'
      0, 0, 0, 1, 
      97, 
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
