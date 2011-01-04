/* 
sample void a_void;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class a_void implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_a_void() throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return a_void.class;
    }
    
    public String getName() {
      return "a_void";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      a_void.decode(d);
      ((Handler)h).handle_a_void();
    }
    
  }
  
  public static void encode(LabCommEncoder e) throws IOException {
    e.begin(a_void.class);
    e.end(a_void.class);
  }
  
  public static void decode(LabCommDecoder d) throws IOException {
  }
  
  private static byte[] signature = new byte[] {
    // void
    0, 0, 0, 17, 
    0, 0, 0, 0, 
  };

}
