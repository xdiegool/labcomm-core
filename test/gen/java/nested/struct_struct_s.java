/* 
sample struct {
  struct {
    int a;
  } a;
} struct_struct_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_struct_s implements LabCommSample {

  public static class struct_a {
    public int a;
  }
  
  public struct_a a;
  
  public interface Handler extends LabCommHandler {
    public void handle_struct_struct_s(struct_struct_s value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_struct_s.class;
    }
    
    public String getName() {
      return "struct_struct_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_struct_s(struct_struct_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_struct_s value) throws IOException {
    e.begin(struct_struct_s.class);
    e.encodeInt(value.a.a);
    e.end(struct_struct_s.class);
  }
  
  public static struct_struct_s decode(LabCommDecoder d) throws IOException {
    struct_struct_s result;
    result = new struct_struct_s();
    result.a = new struct_a();
    result.a.a = d.decodeInt();
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
