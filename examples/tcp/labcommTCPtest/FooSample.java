/* 
sample struct {
  int x;
  int y;
  long t;
  double d;
} FooSample;
*/
package labcommTCPtest;
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class FooSample implements LabCommSample {

  public int x;
  public int y;
  public long t;
  public double d;
  
  public interface Handler extends LabCommHandler {
    public void handle_FooSample(FooSample value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return FooSample.class;
    }
    
    public String getName() {
      return "FooSample";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_FooSample(FooSample.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, FooSample value) throws IOException {
    e.begin(FooSample.class);
    e.encodeInt(value.x);
    e.encodeInt(value.y);
    e.encodeLong(value.t);
    e.encodeDouble(value.d);
    e.end(FooSample.class);
  }
  
  public static FooSample decode(LabCommDecoder d) throws IOException {
    FooSample result;
    result = new FooSample();
    result.x = d.decodeInt();
    result.y = d.decodeInt();
    result.t = d.decodeLong();
    result.d = d.decodeDouble();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 4 fields
    17, 
      4, 
      // int 'x'
      1, 
      120, 
      35, 
      // int 'y'
      1, 
      121, 
      35, 
      // long 't'
      1, 
      116, 
      36, 
      // double 'd'
      1, 
      100, 
      38, 
    // }
  };

}
