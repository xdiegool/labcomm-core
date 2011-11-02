/* 
sample struct {
  int sequence;
  struct {
    boolean last;
    string data;
  } line[_];
} log_message;
*/
import java.io.IOException;
import se.lth.control.labcomm.LabCommDecoder;
import se.lth.control.labcomm.LabCommDispatcher;
import se.lth.control.labcomm.LabCommEncoder;
import se.lth.control.labcomm.LabCommHandler;
import se.lth.control.labcomm.LabCommSample;

public class log_message implements LabCommSample {

  public static class struct_line {
    public boolean last;
    public String data;
  }
  
  public int sequence;
  public struct_line[] line;
  
  public interface Handler extends LabCommHandler {
    public void handle_log_message(log_message value) throws Exception;
  }
  
  public static void register(LabCommDecoder d, Handler h) throws IOException {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) throws IOException {
    e.register(new Dispatcher());
  }
  
  private static class Dispatcher implements LabCommDispatcher {
    
    public Class getSampleClass() {
      return log_message.class;
    }
    
    public String getName() {
      return "log_message";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d,
                                LabCommHandler h) throws Exception {
      ((Handler)h).handle_log_message(log_message.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, log_message value) throws IOException {
    e.begin(log_message.class);
    e.encodeInt(value.sequence);
    e.encodeInt(value.line.length);
    int i_0_max = value.line.length;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeBoolean(value.line[i_0].last);
      e.encodeString(value.line[i_0].data);
    }
    e.end(log_message.class);
  }
  
  public static log_message decode(LabCommDecoder d) throws IOException {
    log_message result;
    result = new log_message();
    result.sequence = d.decodeInt();
    {
      int i_0_max = d.decodeInt();
      result.line = new struct_line[i_0_max];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result.line[i_0] = new struct_line();
        result.line[i_0].last = d.decodeBoolean();
        result.line[i_0].data = d.decodeString();
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // struct { 2 fields
    0, 0, 0, 17, 
      0, 0, 0, 2, 
      // int 'sequence'
      0, 0, 0, 8, 
      115, 101, 113, 117, 101, 110, 99, 101, 
      0, 0, 0, 35, 
      // array [_] 'line'
      0, 0, 0, 4, 
      108, 105, 110, 101, 
      // array [_]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        // struct { 2 fields
        0, 0, 0, 17, 
          0, 0, 0, 2, 
          // boolean 'last'
          0, 0, 0, 4, 
          108, 97, 115, 116, 
          0, 0, 0, 32, 
          // string 'data'
          0, 0, 0, 4, 
          100, 97, 116, 97, 
          0, 0, 0, 39, 
        // }
      // }
    // }
  };

}
