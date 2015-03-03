package se.lth.control.labcomm2006;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;

public class DecoderChannel implements Decoder {

  private DataInputStream in;
  private DecoderRegistry registry;

  public DecoderChannel(InputStream in) throws IOException {
    this.in = new DataInputStream(in);
    registry = new DecoderRegistry();
  }

  public void runOne() throws Exception {
    boolean done = false;
    while (!done) {
      int tag = decodeInt();
      switch (tag) {
	case Constant.SAMPLE: {
	  int index = decodeInt();
	  String name = decodeString();
	  ByteArrayOutputStream signature = new ByteArrayOutputStream();
	  collectFlatSignature(new EncoderChannel(signature));
	  registry.add(index, name, signature.toByteArray());
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

  private void collectFlatSignature(Encoder out) throws IOException {
    int type = decodeInt();
    out.encodeInt(type);
    switch (type) {
      case Constant.ARRAY: {
	int dimensions = decodeInt();
	out.encodeInt(dimensions);
	for (int i = 0 ; i < dimensions ; i++) {
	  out.encodeInt(decodeInt());
	}
	collectFlatSignature(out);
      } break;
      case Constant.STRUCT: {
	int fields = decodeInt();
	out.encodeInt(fields);
	for (int i = 0 ; i < fields ; i++) {
	  out.encodeString(decodeString());
	  collectFlatSignature(out);
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
      } break;
      default: {
	throw new IOException("Unimplemented type=" + type);
      }
    }
    out.end(null);
  }

  public void register(SampleDispatcher dispatcher, 
                       SampleHandler handler) throws IOException {
    registry.add(dispatcher, handler);
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
    int len = decodeInt() & 0xffffffff;
    byte[] chars = new byte[len];
    for(int i=0; i<len; i++) {
      chars[i] = in.readByte();
    }
    return new String(chars);
  }

}

