package se.lth.control.labcomm;

import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import se.lth.control.labcomm.Decoder;
import se.lth.control.labcomm.DecoderChannel;
import se.lth.control.labcomm.SampleDispatcher;
import se.lth.control.labcomm.SampleHandler;

public class Typedef implements SampleType {
    private int index;
    private String name;
    private byte signature[];

  public int getIndex() {
    return index;
  }

  public String getName() {
    return name;
  }

  static void collectFlatSignature(Decoder in, 
                                   DecoderRegistry registry, 
                                   Encoder out, 
                                   boolean recurse) throws IOException {
    int type = in.decodePacked32();
    //System.out.println("cFS..."+in+"  --- type = "+String.format("0x%02X ", type));
    switch (type) {
      case Constant.ARRAY: {
        out.encodePacked32(type);
	int dimensions = in.decodePacked32();
	out.encodePacked32(dimensions);
	for (int i = 0 ; i < dimensions ; i++) {
	  out.encodePacked32(in.decodePacked32());
	}
	collectFlatSignature(in, registry, out, recurse);
      } break;
      case Constant.STRUCT: {
        out.encodePacked32(type);
	int fields = in.decodePacked32();
	out.encodePacked32(fields);
	for (int i = 0 ; i < fields ; i++) {
	  out.encodeString(in.decodeString());
	  collectFlatSignature(in, registry, out, recurse);
	}
      } break;
      case Constant.BOOLEAN:
      case Constant.BYTE:
      case Constant.SHORT:
      case Constant.INT:
      case Constant.LONG:
      case Constant.FLOAT:
      case Constant.DOUBLE:
      case Constant.STRING: {
        out.encodePacked32(type);
      } break;
      default: {
        if(recurse) {
          DecoderRegistry.Entry entry = registry.get(type);
          //System.out.println("...... ***** flattening signature for type "+type);
          if(entry != null) {
            //System.out.println("...... ***** entry "+entry.getName()+") != null");
            byte[] sig = entry.getSignature();
    
            //System.out.print("...... ***** sig :");
            //for (byte b : sig) {
            //  System.out.print(String.format("0x%02X ", b));
            //}
            //System.out.println();
  
            ByteArrayInputStream bis = new ByteArrayInputStream(sig);
            //System.out.println("...... ***** bis.available() :"+bis.available());
            try {
              collectFlatSignature(new DecoderChannel(bis, registry), registry, out, recurse);
            } catch(java.io.EOFException e) { 
              // When the "inner" signature is handled, we continue recursion on the outer level
              collectFlatSignature(in, registry, out, recurse);
            }
            bis.close();
          }else {
	    throw new IOException("Unknown type=" + String.format("0x%02X ", type));
          }
        }else {
          out.encodePacked32(type);
        }
      }
    }
  }

  public void dump() {
      System.out.print("=== Typedef "+getName()+"( "+Integer.toHexString(getIndex())+") : ");
      for (byte b : signature) {
        System.out.print(String.format("0x%02X ", b));
      }
      System.out.println();
  }
  public interface Handler extends SampleHandler {
    public void handle_Typedef(Typedef value) throws Exception;
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
  
 static class Dispatcher implements SampleDispatcher<Typedef> {
    
    private static Dispatcher singleton;
    
    public synchronized static Dispatcher singleton() {
      if(singleton==null) singleton=new Dispatcher();
      return singleton;
    }
    
    public Class<Typedef> getSampleClass() {
      return Typedef.class;
    }
    
    public String getName() {
      return "Typedef";
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
    
    public void encodeTypeDef(Encoder e, int index) throws IOException{
      throw new Error("Should not be called");
    }
    
//    public boolean canDecodeAndHandle() {
//      return true;
//    }
    
    public void decodeAndHandle(Decoder d,
                                Handler h) throws Exception {
      ((Handler)h).handle_Typedef(Typedef.decode(d));
    }
    
  }
  
  public static void encode(Encoder e, Typedef value) throws IOException {
    throw new Error("Should not be called");
  }
  
  public static Typedef decode(Decoder d) throws IOException {
    Typedef result;
    result = new Typedef();
    result.index = d.decodePacked32();
    result.name = d.decodeString();
    ByteArrayOutputStream signature = new ByteArrayOutputStream();
    ((DecoderChannel )d).collectFlatSignature(new EncoderChannel(signature, false), false);
    result.signature = signature.toByteArray();

    return result;
  }
}

