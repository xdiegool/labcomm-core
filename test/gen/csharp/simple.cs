using System;
using se.lth.control.labcomm;
/* 
sample int an_int;
*/

public class an_int : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(an_int);
    }
    
    public String getName() {
      return "an_int";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(an_int.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int value) {
    e.begin(typeof(an_int));
    e.encodeInt(value);
    e.end(typeof(an_int));
  }
  
  public static int decode(LabCommDecoder d) {
    int result;
    result = d.decodeInt();
    return result;
  }
  
  private static byte[] signature = new byte[] {
    0, 0, 0, 35, 
  };

}
/* 
sample int a_fixed_int_array[2];
*/

public class a_fixed_int_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_fixed_int_array);
    }
    
    public String getName() {
      return "a_fixed_int_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_fixed_int_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[] value) {
    e.begin(typeof(a_fixed_int_array));
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0]);
    }
    e.end(typeof(a_fixed_int_array));
  }
  
  public static int[] decode(LabCommDecoder d) {
    int[] result;
    {
      int i_0_max = 2;
      result = new int[i_0_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = d.decodeInt();
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      0, 0, 0, 35, 
    // }
  };

}
/* 
sample int a_fixed_int_multi_array[2, 2, 2];
*/

public class a_fixed_int_multi_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[,,] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_fixed_int_multi_array);
    }
    
    public String getName() {
      return "a_fixed_int_multi_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_fixed_int_multi_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[,,] value) {
    e.begin(typeof(a_fixed_int_multi_array));
    int i_0_max = 2;
    int i_1_max = 2;
    int i_2_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0, i_1, i_2]);
        }
      }
    }
    e.end(typeof(a_fixed_int_multi_array));
  }
  
  public static int[,,] decode(LabCommDecoder d) {
    int[,,] result;
    {
      int i_0_max = 2;
      int i_1_max = 2;
      int i_2_max = 2;
      result = new int[i_0_max, i_1_max, i_2_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
          for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
            result[i_0, i_1, i_2] = d.decodeInt();
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2, 2, 2]
    0, 0, 0, 16, 
      0, 0, 0, 3, 
      0, 0, 0, 2, 
      0, 0, 0, 2, 
      0, 0, 0, 2, 
      0, 0, 0, 35, 
    // }
  };

}
/* 
sample int a_fixed_int_array_array_array[2][2][2];
*/

public class a_fixed_int_array_array_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[][][] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_fixed_int_array_array_array);
    }
    
    public String getName() {
      return "a_fixed_int_array_array_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_fixed_int_array_array_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[][][] value) {
    e.begin(typeof(a_fixed_int_array_array_array));
    int i_0_max = 2;
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      int i_1_max = 2;
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        int i_2_max = 2;
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0][i_1][i_2]);
        }
      }
    }
    e.end(typeof(a_fixed_int_array_array_array));
  }
  
  public static int[][][] decode(LabCommDecoder d) {
    int[][][] result;
    {
      int i_0_max = 2;
      result = new int[i_0_max]
      [][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = 2;
          result[i_0] = new int[i_1_max]
          [];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = 2;
              result[i_0][i_1] = new int[i_2_max]
              ;
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                result[i_0][i_1][i_2] = d.decodeInt();
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [2]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 2, 
      // array [2]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 2, 
        // array [2]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 2, 
          0, 0, 0, 35, 
        // }
      // }
    // }
  };

}
/* 
sample int a_variable_int_array[_];
*/

public class a_variable_int_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_variable_int_array);
    }
    
    public String getName() {
      return "a_variable_int_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_variable_int_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[] value) {
    e.begin(typeof(a_variable_int_array));
    e.encodeInt(value.GetLength(0));
    int i_0_max = value.GetLength(0);
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0]);
    }
    e.end(typeof(a_variable_int_array));
  }
  
  public static int[] decode(LabCommDecoder d) {
    int[] result;
    {
      int i_0_max = d.decodeInt();
      result = new int[i_0_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        result[i_0] = d.decodeInt();
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [_]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 0, 
      0, 0, 0, 35, 
    // }
  };

}
/* 
sample int a_variable_int_multi_array[_, _, _];
*/

public class a_variable_int_multi_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[,,] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_variable_int_multi_array);
    }
    
    public String getName() {
      return "a_variable_int_multi_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_variable_int_multi_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[,,] value) {
    e.begin(typeof(a_variable_int_multi_array));
    e.encodeInt(value.GetLength(0));
    int i_0_max = value.GetLength(0);
    e.encodeInt(value.GetLength(1));
    int i_1_max = value.GetLength(1);
    e.encodeInt(value.GetLength(2));
    int i_2_max = value.GetLength(2);
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0, i_1, i_2]);
        }
      }
    }
    e.end(typeof(a_variable_int_multi_array));
  }
  
  public static int[,,] decode(LabCommDecoder d) {
    int[,,] result;
    {
      int i_0_max = d.decodeInt();
      int i_1_max = d.decodeInt();
      int i_2_max = d.decodeInt();
      result = new int[i_0_max, i_1_max, i_2_max]
      ;
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
          for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
            result[i_0, i_1, i_2] = d.decodeInt();
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [_, _, _]
    0, 0, 0, 16, 
      0, 0, 0, 3, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 0, 
      0, 0, 0, 35, 
    // }
  };

}
/* 
sample int a_variable_int_array_array_array[_][_][_];
*/

public class a_variable_int_array_array_array : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle(int[][][] value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_variable_int_array_array_array);
    }
    
    public String getName() {
      return "a_variable_int_array_array_array";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(a_variable_int_array_array_array.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, int[][][] value) {
    e.begin(typeof(a_variable_int_array_array_array));
    e.encodeInt(value.GetLength(0));
    int i_0_max = value.GetLength(0);
    for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
      e.encodeInt(value[i_0].GetLength(0));
      int i_1_max = value[i_0].GetLength(0);
      for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
        e.encodeInt(value[i_0][i_1].GetLength(0));
        int i_2_max = value[i_0][i_1].GetLength(0);
        for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
          e.encodeInt(value[i_0][i_1][i_2]);
        }
      }
    }
    e.end(typeof(a_variable_int_array_array_array));
  }
  
  public static int[][][] decode(LabCommDecoder d) {
    int[][][] result;
    {
      int i_0_max = d.decodeInt();
      result = new int[i_0_max]
      [][];
      for (int i_0 = 0 ; i_0 < i_0_max ; i_0++) {
        {
          int i_1_max = d.decodeInt();
          result[i_0] = new int[i_1_max]
          [];
          for (int i_1 = 0 ; i_1 < i_1_max ; i_1++) {
            {
              int i_2_max = d.decodeInt();
              result[i_0][i_1] = new int[i_2_max]
              ;
              for (int i_2 = 0 ; i_2 < i_2_max ; i_2++) {
                result[i_0][i_1][i_2] = d.decodeInt();
              }
            }
          }
        }
      }
    }
    return result;
  }
  
  private static byte[] signature = new byte[] {
    // array [_]
    0, 0, 0, 16, 
      0, 0, 0, 1, 
      0, 0, 0, 0, 
      // array [_]
      0, 0, 0, 16, 
        0, 0, 0, 1, 
        0, 0, 0, 0, 
        // array [_]
        0, 0, 0, 16, 
          0, 0, 0, 1, 
          0, 0, 0, 0, 
          0, 0, 0, 35, 
        // }
      // }
    // }
  };

}
/* 
sample struct {
  int a;
  int b;
} an_int_struct;
*/

public class an_int_struct : LabCommSample {

  public int a;
  public int b;
  
  public interface Handler : LabCommHandler {
    void handle(an_int_struct value);
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(an_int_struct);
    }
    
    public String getName() {
      return "an_int_struct";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      ((Handler)h).handle(an_int_struct.decode(d));
    }
    
  }
  
  public static void encode(LabCommEncoder e, an_int_struct value) {
    e.begin(typeof(an_int_struct));
    e.encodeInt(value.a);
    e.encodeInt(value.b);
    e.end(typeof(an_int_struct));
  }
  
  public static an_int_struct decode(LabCommDecoder d) {
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
/* 
sample void a_void;
*/

public class a_void : LabCommSample {

  public interface Handler : LabCommHandler {
    void handle();
  }
  
  public static void register(LabCommDecoder d, Handler h) {
    d.register(new Dispatcher(), h);
  }
  
  public static void register(LabCommEncoder e) {
    e.register(new Dispatcher());
  }
  
  private class Dispatcher : LabCommDispatcher {
    
    public Type getSampleClass() {
      return typeof(a_void);
    }
    
    public String getName() {
      return "a_void";
    }
    
    public byte[] getSignature() {
      return signature;
    }
    
    public void decodeAndHandle(LabCommDecoder d, LabCommHandler h) {
      a_void.decode(d);
      ((Handler)h).handle();
    }
    
  }
  
  public static void encode(LabCommEncoder e) {
    e.begin(typeof(a_void));
    e.end(typeof(a_void));
  }
  
  public static void decode(LabCommDecoder d) {
  }
  
  private static byte[] signature = new byte[] {
    // void
    0, 0, 0, 17, 
    0, 0, 0, 0, 
  };

}
