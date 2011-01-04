/* 
sample int_t int_t_s;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class int_t_s implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_int_t_s(int value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return int_t_s.class;
    }
    
    public String getName() {
      return "int_t_s";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_int_t_s(int_t_s.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int value) throws IOException {
    e.begin(int_t_s.class);
    e.encodeInt(value);
    e.end(int_t_s.class);
  }
  
  public static int decode(LabCommDecoder d) throws IOException {
    int result;
    result = d.decodeInt();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    0, 0, 0, 35, 
  };

}
