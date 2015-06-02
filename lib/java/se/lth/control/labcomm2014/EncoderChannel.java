package se.lth.control.labcomm2014;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class EncoderChannel implements Encoder {

  private Writer writer;
  private ByteArrayOutputStream bytes = new ByteArrayOutputStream();
  private DataOutputStream data = new DataOutputStream(bytes);
  private EncoderRegistry def_registry = new EncoderRegistry();
  private EncoderRegistry ref_registry = new EncoderRegistry();
  private int current_tag;

  private EncoderChannel(Writer writer) throws IOException {
    this.writer = writer;

    begin(Constant.VERSION);
    encodeString(Constant.CURRENT_VERSION);
    end(null);
  }

  public EncoderChannel(OutputStream writer) throws IOException {
    this(new WriterWrapper(writer));
  }

  public void register(SampleDispatcher dispatcher) throws IOException {
    int index = def_registry.add(dispatcher);
    begin(Constant.SAMPLE_DEF);
    encodePacked32(index);
    encodeString(dispatcher.getName());
    byte[] signature = dispatcher.getSignature();
    encodePacked32(signature.length);
    for (int i = 0 ; i < signature.length ; i++) {
      encodeByte(signature[i]);
    }
    end(null);
  }

  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    int index = ref_registry.add(dispatcher);
    begin(Constant.SAMPLE_REF);
    encodePacked32(index);
    encodeString(dispatcher.getName());
    byte[] signature = dispatcher.getSignature();
    encodePacked32(signature.length);
    for (int i = 0 ; i < signature.length ; i++) {
      encodeByte(signature[i]);
    }
    end(null);
  }

  private void begin(int tag) {
    current_tag = tag;
    bytes.reset();
  }

  public void begin(SampleDispatcher dispatcher) throws IOException {
    begin(def_registry.getTag(dispatcher));
  }

  public void end(SampleDispatcher dispatcher) throws IOException {
    data.flush();
    WritePacked32(writer, current_tag);
    WritePacked32(writer, bytes.size());
    writer.write(bytes.toByteArray());
    bytes.reset();
  }

  private void WritePacked32(Writer s, long value) throws IOException {
    byte[] tmp1 = new byte[5];
    byte[] tmp2 = new byte[1];
    long v = value & 0xffffffff;
    int i;

    for (i = 0 ; i == 0 || v != 0 ; i++, v = (v >> 7)) {
      tmp1[i] = (byte)(v & 0x7f | (i!=0?0x80:0x00));
    }
    for (i = i - 1 ; i >= 0 ; i--) {
      tmp2[0] = tmp1[i];
      writer.write(tmp2);
    }
  }

  public void encodeBoolean(boolean value) throws IOException{
    data.writeBoolean(value);
  }

  public void encodeByte(byte value) throws IOException {
    data.writeByte(value);
  }

  public void encodeShort(short value) throws IOException {
    data.writeShort(value);
  }

  public void encodeInt(int value) throws IOException {
    data.writeInt(value);
  }

  public void encodeLong(long value) throws IOException {
    data.writeLong(value);
  }

  public void encodeFloat(float value) throws IOException {
    data.writeFloat(value);
  }

  public void encodeDouble(double value) throws IOException {
    data.writeDouble(value);
  }

  public void encodeString(String value) throws IOException {
    ByteArrayOutputStream tmpb = new ByteArrayOutputStream();
    DataOutputStream tmps = new DataOutputStream(tmpb);

    tmps.writeUTF(value);
    tmps.flush();
    byte[] tmp = tmpb.toByteArray();

    //the two first bytes written by writeUTF is the length of
    //the string, so we skip those
    encodePacked32(tmp.length-2);
    for (int i = 2 ; i < tmp.length ; i++) {
      encodeByte(tmp[i]);
    }
  }

  public void encodePacked32(int value) throws IOException {
    byte[] tmp = new byte[5];
    long v = value & 0xffffffff;
    int i;

    for (i = 0 ; i == 0 || v != 0 ; i++, v = (v >> 7)) {
      tmp[i] = (byte)(v & 0x7f);
    }
    for (i = i - 1 ; i >= 0 ; i--) {
      encodeByte((byte)(tmp[i] | (i!=0?0x80:0x00)));
    }
  }

  public void encodeSampleRef(SampleDispatcher value) throws IOException {
    int index = 0;
    try {
      index = ref_registry.getTag(value);
    } catch (NullPointerException e) {
        // encode 0 for unregistered ref types
    }
    data.writeInt(index);
  }

}

