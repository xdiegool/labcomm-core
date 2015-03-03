namespace se.lth.control.labcomm2006 {

  using System;
  using System.IO;
  using System.Text;
  using System.Runtime.InteropServices;

  public class EncoderChannel : Encoder {

    private Stream writer;
    private MemoryStream bytes = new MemoryStream();
    private EncoderRegistry def_registry = new EncoderRegistry();
    byte[] buf = new byte[8];
    private int current_tag; 

    public EncoderChannel(Stream writer) {
      this.writer = writer;
    }

    public void register(SampleDispatcher dispatcher) {
      int index = def_registry.add(dispatcher);
      begin(Constant.SAMPLE_DEF);
      encodeInt(index);
      encodeString(dispatcher.getName());
      byte[] signature = dispatcher.getSignature();
      for (int i = 0 ; i < signature.Length ; i++) {
	encodeByte(signature[i]);
      }
      end(null);
    }

    private void begin(int tag) {
      current_tag = tag;
      bytes.SetLength(0);
      encodeInt(tag);
    }

    public void begin(Type c) {
      begin(def_registry.getTag(c));
    }

    public void end(Type c) {
      bytes.WriteTo(writer);
      bytes.SetLength(0);
      writer.Flush();
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
      encodeInt(buf.Length);
      bytes.Write(buf, 0, buf.Length);
    }

  }

}
