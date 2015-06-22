package se.lth.control.labcomm2014;

import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import se.lth.control.labcomm2014.Decoder;
import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.SampleDispatcher;
import se.lth.control.labcomm2014.SampleHandler;

public class TypeDef implements BuiltinType {
    private int index;
    private String name;
    private byte signature[];

  public int getIndex() {
    return index;
  }

  public String getName() {
    return name;
  }

  public String toString() {
     return getName();
  }

  public byte[] getSignature() {
    return signature;
  }

  public void dump() {
      System.out.print("=== TypeDef "+getName()+"( "+Integer.toHexString(getIndex())+") : ");
      for (byte b : signature) {
        System.out.print(String.format("0x%02X ", b));
      }
      System.out.println();
  }
  public interface Handler extends SampleHandler {
    public void handle_TypeDef(TypeDef value) throws Exception;
  }

  public static void register(Decoder d, Handler h) throws IOException {
    d.register(Dispatcher.singleton(), h);
  }

  public static void register(Encoder e) throws IOException {
    register(e,false);
  }

  public static void register(Encoder e, boolean sendMetaData) throws IOException {
    throw new IOException("cannot send TypeDefs");
  }

 static class Dispatcher implements SampleDispatcher<TypeDef> {

    private static Dispatcher singleton;

    public synchronized static Dispatcher singleton() {
      if(singleton==null) singleton=new Dispatcher();
      return singleton;
    }

    public Class<TypeDef> getSampleClass() {
      return TypeDef.class;
    }

    public String getName() {
      return "TypeDef";
    }

    public byte getTypeDeclTag() {
      throw new Error("Should not be called");
    }

    public boolean isSample() {
      throw new Error("Should not be called");
    }
    public boolean hasStaticSignature() {
      throw new Error("Should not be called");
    }

    /** return the flat signature. Intended use is on decoder side */
    public byte[] getSignature() {
      return null; // not used for matching
    }

    public int getNumIntentions() {
        return 0;
    }

    public byte[] getIntentionBytes() {
        return new byte[0];
    }

    public void encodeTypeDef(Encoder e, int index) throws IOException{
      throw new Error("Should not be called");
    }

    public void registerTypeDeps(Encoder e) throws IOException{
      throw new Error("Should not be called");
    }

//    public boolean canDecodeAndHandle() {
//      return true;
//    }

    public void decodeAndHandle(Decoder d,
                                SampleHandler h) throws Exception {
      ((Handler)h).handle_TypeDef(TypeDef.decode(d));
    }

    public boolean hasDependencies() {
        return false;
    }
  }

  public static void encode(Encoder e, TypeDef value) throws IOException {
    throw new Error("Should not be called");
  }

  protected TypeDef() {
  }

  public TypeDef(int index, String name, byte sig[]) {
      this.index = index;
      this.name = name;
      this.signature = sig;
  }

  public static TypeDef decode(Decoder d) throws IOException {
    TypeDef result;
    int index = d.decodePacked32();
    int numIntentions = d.decodePacked32();
    if(numIntentions != 1) {
        System.out.println("WARNING: #intentions == "+numIntentions);
    }
    int keylen = d.decodePacked32();
    if(keylen != 0) {
        System.out.println("WARNING: keylen == "+keylen);
    }
    String name = d.decodeString();
    int siglen= d.decodePacked32();
    byte sig[] = new byte[siglen];
    for(int i=0; i<siglen;i++){
        sig[i] = d.decodeByte();
    }
    result = new TypeDef(index, name, sig);
    return result;
  }
}

