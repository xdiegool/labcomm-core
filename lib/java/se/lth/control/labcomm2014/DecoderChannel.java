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
          int index = decodePacked32();
          String name = decodeString();
          int signature_length = decodePacked32();
          byte[] signature = new byte[signature_length];
          ReadBytes(signature, signature_length);
          def_registry.add(index, name, signature);
        } break;
        case Constant.SAMPLE_REF: {
          int index = decodePacked32();
          String name = decodeString();
          int signature_length = decodePacked32();
          byte[] signature = new byte[signature_length];
          ReadBytes(signature, signature_length);
          ref_registry.add(index, name, signature);
        } break;
        default: {
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
          // decodeAndHandle throws Exception and not IOException because
          // the user provided handler might throw anything
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

  public SampleDispatcher decodeSampleRef() throws IOException {
    int index = in.readInt();
    try {
      DecoderRegistry.Entry e = ref_registry.get(index);
      return e.getDispatcher();
    } catch (NullPointerException e) {
      // Handles both the case where index == 0 and unregistered sample
      return null;
    }

  }
    
}

