/* 
sample float data;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class data implements LabCommSample {

  public interface Handler extends LabCommHandler {
    public void handle_data(float value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return data.class;
    }
    
    public String getName() {
      return "data";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_data(data.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, float value) throws IOException {
    e.begin(data.class);
    e.encodeFloat(value);
    e.end(data.class);
  }
  
  public static float decode(LabCommDecoder d) throws IOException {
    float result;
    result = d.decodeFloat();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    37, 
  };

}
