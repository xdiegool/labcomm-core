namespace se.lth.control.labcomm2014 {

  using System;
  using System.IO;
  using System.Text;
  using System.Runtime.InteropServices;

  public class EncoderChannel : Encoder {

    private Stream writer;
    private MemoryStream bytes = new MemoryStream();
    private EncoderRegistry def_registry = new EncoderRegistry();
    private EncoderRegistry ref_registry = new EncoderRegistry();
    byte[] buf = new byte[8];
    private int current_tag; 

    public EncoderChannel(Stream writer) {
      this.writer = writer;

      begin(Constant.VERSION);
      encodeString(Constant.CURRENT_VERSION);
      end(null);
    }

    public void register(SampleDispatcher dispatcher) {
      int index = def_registry.add(dispatcher);
      begin(Constant.SAMPLE_DEF);
      encodePacked32(index);
      encodeIntentions(dispatcher.getName());
      byte[] signature = dispatcher.getSignature();
      encodePacked32(signature.Length);
      for (int i = 0 ; i < signature.Length ; i++) {
	encodeByte(signature[i]);
      }
      end(null);
    }

    public void registerSampleRef(SampleDispatcher dispatcher) {
      int index = ref_registry.add(dispatcher);
      begin(Constant.SAMPLE_REF);
      encodePacked32(index);
      encodeIntentions(dispatcher.getName());
      byte[] signature = dispatcher.getSignature();
      encodePacked32(signature.Length);
      for (int i = 0 ; i < signature.Length ; i++) {
	encodeByte(signature[i]);
      }
      end(null);
    }

    private void begin(int tag) {
      current_tag = tag;
      bytes.SetLength(0);
    }

    public void begin(SampleDispatcher identity) {
      begin(def_registry.getTag(identity));
    }

    public void end(SampleDispatcher identity) {
      WritePacked32(writer, current_tag);
      WritePacked32(writer, bytes.Length);
      bytes.WriteTo(writer);
      bytes.SetLength(0);
      writer.Flush();
    }

    private void WritePacked32(Stream s, Int64 value) {
      Int64 v = value & 0xffffffff;
      int i;
  
      for (i = 0 ; i == 0 || v != 0 ; i++, v = (v >> 7)) {
        buf[i] = (byte)(v & 0x7f | (i!=0?0x80:0x00));
      }
      for (i = i - 1 ; i >= 0 ; i--) {
        s.WriteByte(buf[i]);
      }      
    }

    private void WriteInt(Int64 value, int length) {
      for (int i = length - 1 ; i >= 0 ; i--) {
	buf[i] = (byte)(value & 0xff);
	value = value >> 8;
      }
      bytes.Write(buf, 0, length);
    }

    public void encodeBoolean(bool value) {
      WriteInt(value ? 1 : 0, 1);
    }

    public void encodeByte(byte value) {
      WriteInt(value, 1);
    }

    public void encodeShort(short value) {
      WriteInt(value, 2);
    }

    public void encodeInt(int value) {
      WriteInt(value, 4);
    }

    public void encodeLong(long value) {
      WriteInt(value, 8);
    }

    [StructLayout(LayoutKind.Explicit)]
    private struct Int32SingleUnion {
      [FieldOffset(0)] public int AsInt;
      [FieldOffset(0)] public float AsFloat;
    };

    public void encodeFloat(float value) {
      Int32SingleUnion u;
      u.AsInt = 0; // Avoid error messge
      u.AsFloat = value;
      WriteInt(u.AsInt, 4);
    }

    public void encodeDouble(double value) {
      WriteInt(BitConverter.DoubleToInt64Bits(value), 8);
    }

    public void encodeString(String value) {
      byte[] buf = Encoding.UTF8.GetBytes(value);
      encodePacked32(buf.Length);
      bytes.Write(buf, 0, buf.Length);
    }

    public void encodePacked32(Int64 value) {
      WritePacked32(bytes, value);
    }

    private void encodeIntentions(String name) {
      encodePacked32(1); // one intention field
      encodePacked32(0); // empty key: name
      encodeString(name);
    }

    public void encodeSampleRef(SampleDispatcher value) {
      int index = 0;
      try {
        index = ref_registry.getTag(value);
      } catch (NullReferenceException) {
      }
      WriteInt(index, 4);
    }

  }
}
