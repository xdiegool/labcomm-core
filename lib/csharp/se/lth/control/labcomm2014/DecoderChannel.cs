namespace se.lth.control.labcomm2014 {

  using System;
  using System.IO;
  using System.Runtime.InteropServices;
  using System.Text;

  public class DecoderChannel : Decoder {

    private Stream stream;
    private DecoderRegistry def_registry = new DecoderRegistry();
    private DecoderRegistry ref_registry = new DecoderRegistry();
    byte[] buf = new byte[8];

    public DecoderChannel(Stream stream) {
      this.stream = stream;
    }

    public void runOne() {
      bool done = false;
      while (!done) {
	int tag = decodePacked32();
        int length = decodePacked32();
	switch (tag) {
        case Constant.VERSION: {
          String version = decodeString();
          if (version != Constant.CURRENT_VERSION) {
  	    throw new IOException("LabComm version mismatch " +
			          version + " != " + Constant.CURRENT_VERSION);
          }
        } break;
        case Constant.SAMPLE_DEF: {
          int index = decodePacked32();
          String name = decodeIntentions();
          int signature_length = decodePacked32();
          byte[] signature = new byte[signature_length];
          ReadBytes(signature, signature_length);
	  def_registry.add(index, name, signature);
        } break;
        case Constant.SAMPLE_REF: {
          int index = decodePacked32();
          String name = decodeIntentions();
          int signature_length = decodePacked32();
          byte[] signature = new byte[signature_length];
          ReadBytes(signature, signature_length);
	  ref_registry.add(index, name, signature);
        } break;
        case Constant.TYPE_DEF:
        case Constant.TYPE_BINDING: {
            for(int i=0; i<length;i++){
                decodeByte();
            }
        } break;
        default: {
          DecoderRegistry.Entry e = def_registry.get(tag);
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

    public void register(SampleDispatcher dispatcher,
			 SampleHandler handler) {
      def_registry.add(dispatcher, handler);
    }

    public void registerSampleRef(SampleDispatcher dispatcher) {
      ref_registry.add(dispatcher, null);
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
      int length = decodePacked32();
      byte[] buf = new byte[length];
      ReadBytes(buf, length);
      return Encoding.UTF8.GetString(buf);
    }

    public int decodePacked32() {
      Int64 res = 0;
      bool cont = true;

      do {
        Int64 c = decodeByte();
	res = (res << 7) | (c & 0x7f);
        cont = (c & 0x80) != 0;
      } while(cont);

      return (int) (res & 0xffffffff);
    }

  private byte[] decodeBytes() {
    int len = decodePacked32();
    byte[] result = new byte[len];

    for(int i=0; i<len; i++) {
      result[i] = decodeByte();
    }
    return result;
  }

  private String decodeIntentions() {
    int numIntentions = decodePacked32();
    string name = "";
    for(int i = 0; i<numIntentions; i++) {
      byte[] key = decodeBytes();
      byte[] val = decodeBytes();

      if(key.Length == 0) {
        name = Encoding.UTF8.GetString(val, 0, val.Length);
      }
    }
    return name;
  }

    public Type decodeSampleRef() {
      int index = (int)ReadInt(4);
      try {
        DecoderRegistry.Entry e = ref_registry.get(index);
        return e.getSampleDispatcher().getSampleClass();
      } catch (NullReferenceException) {
        return null;
      }
    }

  }

}
