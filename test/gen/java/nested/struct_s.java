/* 
sample struct {
  int a;
  double bcd;
} struct_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class struct_s implements LabCommSample {

  public int a;
  public double bcd;
  
  public interface Handler extends LabCommHandler {
    public void handle_struct_s(struct_s value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return struct_s.class;
    }
    
    public String getName() {
      return "struct_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_struct_s(struct_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, struct_s value) throws IOException {
    e.begin(struct_s.class);
    e.encodeInt(value.a);
    e.encodeDouble(value.bcd);
    e.end(struct_s.class);
  }
  
  public static struct_s decode(LabCommDecoder d) throws IOException {
    struct_s result;
    result = new struct_s();
    result.a = d.decodeInt();
    result.bcd = d.decodeDouble();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 2 fields
    0, 0, 0, 17, 
      0, 0, 0, 2, 
      // int 'a'
      0, 0, 0, 1, 
      97, 
      0, 0, 0, 35, 
      // double 'bcd'
      0, 0, 0, 3, 
      98, 99, 100, 
      0, 0, 0, 38, 
    // }
  };

}
