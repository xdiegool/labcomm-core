namespace se.lth.control.labcomm2006 {

  using System;
  using System.IO;
  using System.Text;
  using System.Runtime.InteropServices;

  public class DecoderChannel : Decoder {

    private Stream stream;
    private DecoderRegistry registry = new DecoderRegistry();
    byte[] buf = new byte[8];

    public DecoderChannel(Stream stream) {
      this.stream = stream;
    }

    public void runOne() {
      bool done = false;
      while (!done) {
	int tag = decodeInt();

	switch (tag) {
        case Constant.SAMPLE_DEF: {
          int index = decodeInt();
          String name = decodeString();
	  MemoryStream signature = new MemoryStream();
	  collectFlatSignature(new EncoderChannel(signature));
	  registry.add(index, name, signature.ToArray());
        } break;
        default: {
          DecoderRegistry.Entry e = registry.get(tag);
          if (e == null) {
            throw new IOException("Unhandled tag " + tag);
          }
          SampleDispatcher d = e.getSampleDispatcher();
          if (d == null) {
            throw new IOException("No dispatcher for '" + e.getName() + "'" + e.getSignature());
          }
          SampleHandler h = e.getHandler();
          if (h == null) {
            throw new IOException("No handler for '" + e.getName() +"'");
          }
          d.decodeAndHandle(this, h);
          done = true;
        } break;
	}
      }
    }

    public void run() {
      while (true) {
	runOne();
      }
    }

    private void collectFlatSignature(Encoder e) {
      int type = decodeInt();
      e.encodeInt(type);
      switch (type) {
        case Constant.ARRAY: {
          int dimensions = decodeInt();
          e.encodeInt(dimensions);
          for (int i = 0 ; i < dimensions ; i++) {
             e.encodeInt(decodeInt());
          }
          collectFlatSignature(e);
        } break;
        case Constant.STRUCT: {
          int fields = decodeInt();
          e.encodeInt(fields);
          for (int i = 0 ; i < fields ; i++) {
            e.encodeString(decodeString());
            collectFlatSignature(e);
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
      e.end(null);
    }

    public void register(SampleDispatcher dispatcher, 
			 SampleHandler handler) {
      registry.add(dispatcher, handler);
    }

    private void ReadBytes(byte[] result, int length) {
      int offset = 0;
      while (offset < length) {
	int count = stream.Read(result, offset, length - offset);
	if (count <= 0)
	  throw new EndOfStreamException(
	    String.Format("End of stream reached with {0} bytes left to read", 
			  length - offset));
	offset += count;
      }
    }

    private Int64 ReadInt(int length) {
      Int64 result = 0;
      ReadBytes(buf, length);
      for (int i = 0 ; i < length ; i++) {
	result = (result << 8) + buf[i];

      }
      return result;
    }

     public bool decodeBoolean() {
      return ReadInt(1) != 0;
    }

    public byte decodeByte() {
      return (byte)ReadInt(1);
    }
  
    public short decodeShort() {
      return (short)ReadInt(2);
    }

    public int decodeInt() {
      return (int)ReadInt(4);
    }
    
    public long decodeLong() {
      return (long)ReadInt(8);
    }
    

    [StructLayout(LayoutKind.Explicit)]
    private struct Int32SingleUnion {
      [FieldOffset(0)] public int AsInt;
      [FieldOffset(0)] public float AsFloat;
    };

    public float decodeFloat() {
      Int32SingleUnion u;
      u.AsFloat = 0; // Avoid error messge
      u.AsInt = (int)ReadInt(4);
      return u.AsFloat;
    }
    
    public double decodeDouble() {
      return BitConverter.Int64BitsToDouble(ReadInt(8));
    }

    public String decodeString() {
      int length = decodeInt();
      byte[] buf = new byte[length];
      ReadBytes(buf, length);
      return Encoding.UTF8.GetString(buf);
    }

  }

} 
