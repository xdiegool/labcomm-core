package se.lth.control.labcomm;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

public class DecoderChannel implements Decoder {

  private DataInputStream in;
  private DecoderRegistry registry;

  private DecoderChannel(InputStream in, DecoderRegistry reg) throws IOException {
    this.in = new DataInputStream(in);
    registry = reg;
  }
  public DecoderChannel(InputStream in) throws IOException {
    this(in, new DecoderRegistry());
  }

  private void processSampleDef() throws IOException {
    int index = decodePacked32();
    String name = decodeString();
    int signature_length = decodePacked32();
    byte[] signature = new byte[signature_length];
    ReadBytes(signature, signature_length);
    registry.add(index, name, signature);
  }	   


  private void processTypeDef(int len) throws IOException {
       System.out.println("Got TypeDef: skipping "+len+" bytes"); 
       for(int i=0; i<len; i++) {
           decodeByte();		  
       }
  }

  private void processTypeBinding(int len) throws IOException {
       System.out.println("Got TypeBinding: skipping "+len+" bytes"); 
       for(int i=0; i<len; i++) {
           decodeByte();		  
       }
  }

  private void processPragma(int len) throws IOException {
       System.out.println("Got Pragma: skipping "+len+" bytes"); 
       for(int i=0; i<len; i++) {
           decodeByte();		  
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
	  DecoderRegistry.Entry e = registry.get(tag);
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
	  d.decodeAndHandle(this, h);
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
    registry.add(dispatcher, handler);
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
}

