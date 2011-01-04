/* 
sample struct {
  int a;
  int b;
} an_int_struct;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class an_int_struct implements LabCommSample {

  public int a;
  public int b;
  
  public interface Handler extends LabCommHandler {
    public void handle_an_int_struct(an_int_struct value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return an_int_struct.class;
    }
    
    public String getName() {
      return "an_int_struct";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_an_int_struct(an_int_struct.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, an_int_struct value) throws IOException {
    e.begin(an_int_struct.class);
    e.encodeInt(value.a);
    e.encodeInt(value.b);
    e.end(an_int_struct.class);
  }
  
  public static an_int_struct decode(LabCommDecoder d) throws IOException {
    an_int_struct result;
    result = new an_int_struct();
    result.a = d.decodeInt();
    result.b = d.decodeInt();
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
      // int 'b'
      0, 0, 0, 1, 
      98, 
      0, 0, 0, 35, 
    // }
  };

}
