namespace se.lth.control.labcomm {

  using System;
  using System.IO;
  using System.Text;
  using System.Runtime.InteropServices;

  public class LabCommEncoderChannel : LabCommEncoder {

    private Stream writer;
    private MemoryStream bytes = new MemoryStream();
    private LabCommEncoderRegistry registry = new LabCommEncoderRegistry();
    byte[] buf = new byte[8];

    public LabCommEncoderChannel(Stream writer, bool emitVersion) {
      this.writer = writer;
      if (emitVersion) {
	encodeString(LabComm.VERSION);
      }
    }

    public LabCommEncoderChannel(Stream writer) : this(writer, true) {
    }

    public void register(LabCommDispatcher dispatcher) {
      int index = registry.add(dispatcher);
      encodePacked32(LabComm.SAMPLE);
      encodePacked32(index);
      encodeString(dispatcher.getName());
      byte[] signature = dispatcher.getSignature();
      for (int i = 0 ; i < signature.Length ; i++) {
	encodeByte(signature[i]);
      }
      end(null);
    }

    public void begin(Type c) {
      encodePacked32(registry.getTag(c));
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
      encodePacked32(buf.Length);
      bytes.Write(buf, 0, buf.Length);
    }

    public void encodePacked32(Int64 value) {
      byte[] tmp = new byte[5];
      Int64 v = value & 0xffffffff;
      int i;
  
      for (i = 0 ; i == 0 || v != 0 ; i++, v = (v >> 7)) {
        tmp[i] = (byte)(v & 0x7f);
      }
      for (i = i - 1 ; i >= 0 ; i--) {
        encodeByte((byte)(tmp[i] | (i!=0?0x80:0x00)));
      }
    }
  }
}
