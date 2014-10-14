package se.lth.control.labcomm;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

public class LabCommDecoderChannel implements LabCommDecoder {

  private DataInputStream in;
  private LabCommDecoderRegistry registry;

  public LabCommDecoderChannel(InputStream in) throws IOException {
    this.in = new DataInputStream(in);
    registry = new LabCommDecoderRegistry();
    String version = decodeString();
    if (! version.equals(LabComm.VERSION)) {
      throw new IOException("LabComm version mismatch " +
			    version + " != " + LabComm.VERSION);
    }
    System.err.println(LabComm.VERSION);
  }

  public void runOne() throws Exception {
    boolean done = false;
    while (!done) {
      int tag = decodePacked32();
      int length = decodePacked32();
      switch (tag) {
	case LabComm.SAMPLE: {
	  int index = decodePacked32();
	  String name = decodeString();
          int signature_length = decodePacked32();
//	  ByteArrayOutputStream signature = new ByteArrayOutputStream();
          byte[] signature = new byte[signature_length];
//	  collectFlatSignature(new LabCommEncoderChannel(signature, false));
          ReadBytes(signature, signature_length);
//	  registry.add(index, name, signature.toByteArray());
	  registry.add(index, name, signature);
	} break;
	default: {
	  LabCommDecoderRegistry.Entry e = registry.get(tag);
	  if (e == null) {
	    throw new IOException("Unhandled tag " + tag);
	  }
	  LabCommDispatcher d = e.getDispatcher();
	  if (d == null) {
	    throw new IOException("No dispatcher for '" + e.getName() + "'");
	  }
	  LabCommHandler h = e.getHandler();
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

  private void collectFlatSignature(LabCommEncoder out) throws IOException {
    int type = decodePacked32();
    out.encodePacked32(type);
    switch (type) {
      case LabComm.ARRAY: {
	int dimensions = decodePacked32();
	out.encodePacked32(dimensions);
	for (int i = 0 ; i < dimensions ; i++) {
	  out.encodePacked32(decodePacked32());
	}
	collectFlatSignature(out);
      } break;
      case LabComm.STRUCT: {
	int fields = decodePacked32();
	out.encodePacked32(fields);
	for (int i = 0 ; i < fields ; i++) {
	  out.encodeString(decodeString());
	  collectFlatSignature(out);
	}
      } break;
      case LabComm.BOOLEAN:
      case LabComm.BYTE:
      case LabComm.SHORT:
      case LabComm.INT:
      case LabComm.LONG:
      case LabComm.FLOAT:
      case LabComm.DOUBLE:
      case LabComm.STRING: {
      } break;
      default: {
	throw new IOException("Unimplemented type=" + type);
      }
    }
    out.end(null);
  }

  public void register(LabCommDispatcher dispatcher, 
                       LabCommHandler handler) throws IOException {
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

