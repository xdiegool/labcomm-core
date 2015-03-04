package se.lth.control.labcomm2014;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

public class DecoderChannel implements Decoder {

  private DataInputStream in;
  private DecoderRegistry def_registry = new DecoderRegistry();
  private DecoderRegistry ref_registry = new DecoderRegistry();

  public DecoderChannel(InputStream in) throws IOException {
    this.in = new DataInputStream(in);
  }

  private void processSampleDef() throws IOException {
    int index = decodePacked32();
    String name = decodeString();
    int signature_length = decodePacked32();
    byte[] signature = new byte[signature_length];
    ReadBytes(signature, signature_length);
    def_registry.add(index, name, signature);
  }	   

  private void processSampleRef() throws IOException {
    int index = decodePacked32();
    String name = decodeString();
    int signature_length = decodePacked32();
    byte[] signature = new byte[signature_length];
    ReadBytes(signature, signature_length);
    ref_registry.add(index, name, signature);
  }	   

  private void processTypeDef(int len) throws IOException {
       try {
           processSample(Constant.TYPE_DEF);
      } catch(Exception ex) {
       int idx = decodePacked32();
       String name = decodeString(); 
       int siglen = decodePacked32();
       for(int i=0; i<siglen; i++) {
           byte b = decodeByte();		  
       }
      }
  }

  private void processTypeBinding(int len) throws IOException {
      try {
           processSample(Constant.TYPE_BINDING);
      } catch(Exception ex) {
          for(int i=0; i<len; i++) {
              decodeByte();		  
          }
      } 
  }

  private void processPragma(int len) throws IOException {
       for(int i=0; i<len; i++) {
           decodeByte();		  
       }
  }

  private void processSample(int tag) throws IOException {
	  DecoderRegistry.Entry e = def_registry.get(tag);
	  if (e == null) {
	    throw new IOException("Unhandled tag " + tag);
	  }
	  SampleDispatcher d = e.getDispatcher();
	  if (d == null) {
	    throw new IOException("No dispatcher for '" + e.getName() + "'");
	  }
	  SampleHandler h = e.getHandler();
	  if (h == null) {
	    throw new IOException("No handler for '" + e.getName() +"'");
	  }
      try {
        //XXX why does decodeAndHandle throw Exception and not IOException?
        d.decodeAndHandle(this, h);
      } catch (IOException ex) {
          throw ex;
      } catch (Exception ex) {
          ex.printStackTrace();
      }
  }	  

  public void runOne() throws Exception {
    boolean done = false;
    while (!done) {
      int tag = decodePacked32();
      int length = decodePacked32();
      switch (tag) {
        case Constant.VERSION: {
          String version = decodeString();
          if (! version.equals(Constant.CURRENT_VERSION)) {
            throw new IOException("LabComm version mismatch " +
			          version + " != " + Constant.CURRENT_VERSION);
          }
        } break;
        case Constant.SAMPLE_DEF: {
            processSampleDef();
        } break;
        case Constant.SAMPLE_REF: {
            processSampleRef();
        } break;
        case Constant.TYPE_DEF: {
            processTypeDef(length);
        } break;
        case Constant.TYPE_BINDING: {
            processTypeBinding(length);
        } break;
        case Constant.PRAGMA: {
            processPragma(length);
        } break;
        default: {
            processSample(tag);
            done = true;

        }
      }
    }
  }

  public void run() throws Exception {
    while (true) {
      runOne();
    }
  }

  public void register(SampleDispatcher dispatcher, 
                       SampleHandler handler) throws IOException {
    def_registry.add(dispatcher, handler);
  }

  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    ref_registry.add(dispatcher, null);
  }

  private void ReadBytes(byte[] result, int length) throws IOException {
      int offset = 0;
      while (offset < length) {
	int count = in.read(result, offset, length - offset);
	if (count <= 0) {
	  throw new EOFException(
	    "End of stream reached with " +
            (length - offset) + " bytes left to read");
        }
	offset += count;
      }
    }

  public boolean decodeBoolean() throws IOException {
    return in.readBoolean();
  }

  public byte decodeByte() throws IOException {
    return in.readByte();
  }
  
  public short decodeShort() throws IOException {
    return in.readShort();
  }

  public int decodeInt() throws IOException {
    return in.readInt();
  }

  public long decodeLong() throws IOException {
    return in.readLong();
  }

  public float decodeFloat() throws IOException {
    return in.readFloat();
  }

  public double decodeDouble() throws IOException {
    return in.readDouble();
  }

  public String decodeString() throws IOException {
    //in.readShort(); // HACK
    //return in.readUTF();
    int len = decodePacked32() & 0xffffffff;
    byte[] chars = new byte[len];
    for(int i=0; i<len; i++) {
      chars[i] = in.readByte();
    }
    return new String(chars);
  }

  public int decodePacked32() throws IOException {
    long res=0;
    byte i=0;
    boolean cont=true;

    do {
      byte c = in.readByte();
      res = (res << 7) | (c & 0x7f);
      cont = (c & 0x80) != 0;
      i++;
    } while(cont);

    return (int) (res & 0xffffffff);
  }

  public Class decodeSampleRef() throws IOException {
    int index = in.readInt();
    try {
      DecoderRegistry.Entry e = ref_registry.get(index);
      return e.getDispatcher().getSampleClass();
    } catch (NullPointerException e) {
      return null;
    }

  }
    
  /* Package visible methods for use from TypeDefParser */

  String getSampleName(int idx) {
    DecoderRegistry.Entry e = def_registry.get(idx); 
    return e.getName();  
  }

  byte[] getSampleSignature(int idx) {
    DecoderRegistry.Entry e = def_registry.get(idx); 
    return e.getSignature();  
  }
}

