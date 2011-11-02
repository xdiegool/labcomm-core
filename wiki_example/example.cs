using System;
using se.lth.control.labcomm;
/* 
sample struct {
  int sequence;
  struct {
    boolean last;
    string data;
  } line[_];
} log_message;
*/

public class log_message : LabCommSample {

  public static class struct_line {
    public boolean last;
    public String data;
  }
  
  public int sequence;
  public struct_line[] line;
  
  public interface Handler : LabCommHandler {
    void handle(log_message value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(log_message);
    }
    
    public String getName() {
      return "log_message";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(log_message.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, log_message value) {
    e.begin(typeof(log_message));
    e.encodeInt(value.sequence);
    e.encodeInt(value.line.GetLength(0));
    int i_0_max = value.line.GetLength(0);
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeBoolean(value.line[i_0].last);
      e.encodeString(value.line[i_0].data);
    }
    e.end(typeof(log_message));
  }
  
  public static log_message decode(LabCommDecoder d) {
    log_message result;
    result = new log_message();
    result.sequence = d.decodeInt();
    {
      int i_0_max = d.decodeInt();
      result.line = new struct_line[i_0_max]
      ;
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
/* 
sample float data;
*/

public class data : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(float value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(data);
    }
    
    public String getName() {
      return "data";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(data.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, float value) {
    e.begin(typeof(data));
    e.encodeFloat(value);
    e.end(typeof(data));
  }
  
  public static float decode(LabCommDecoder d) {
    float result;
    result = d.decodeFloat();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    0, 0, 0, 37, 
  };

}
