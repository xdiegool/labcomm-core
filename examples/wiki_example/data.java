/* 
sample float data;
*/
import java.io.IOException;
import se.lth.control.labcomm.Decoder;
import se.lth.control.labcomm.Dispatcher;
import se.lth.control.labcomm.Encoder;
import se.lth.control.labcomm.Handler;
import se.lth.control.labcomm.Sample;

public class data implements Sample {

  public interface Handler extends Handler {
    public void handle_data(float value) throws Exception;
  }
  
  public static void register(Decoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(Encoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements Dispatcher {
    
    public Class getSampleClass() {
      return data.class;
    }
    
    public String getName() {
      return "data";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(Decoder d,
                                Handler h) throws Exception {
      ((Handler)h).handle_data(data.decode(d));
    }
    
  }
  
  public static void encode(Encoder e, float value) throws IOException {
    e.begin(data.class);
    e.encodeFloat(value);
    e.end(data.class);
  }
  
  public static float decode(Decoder d) throws IOException {
    float result;
    result = d.decodeFloat();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    37, 
  };

}
