package se.lth.control.labcomm;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class LabCommEncoderChannel implements LabCommEncoder {

  private LabCommWriter writer;
  private ByteArrayOutputStream bytes;
  private DataOutputStream data;
  private LabCommEncoderRegistry registry;
  private int current_tag; 

  public LabCommEncoderChannel(LabCommWriter writer, 
			       boolean emitVersion) throws IOException {
    this.writer = writer;
    bytes = new ByteArrayOutputStream();
    data = new DataOutputStream(bytes);
    registry = new LabCommEncoderRegistry();
    if (emitVersion) {
      encodeString(LabComm.VERSION);
      data.flush();
      writer.write(bytes.toByteArray());
      bytes.reset();
    }
  }

  public LabCommEncoderChannel(LabCommWriter writer) throws IOException {
    this(writer, true);
  }

  public LabCommEncoderChannel(OutputStream writer, 
			       boolean emitVersion) throws IOException {
    this(new WriterWrapper(writer), emitVersion);
  }

  public LabCommEncoderChannel(OutputStream writer) throws IOException {
    this(new WriterWrapper(writer), true);
  }

  public void register(LabCommDispatcher dispatcher) throws IOException {
    int index = registry.add(dispatcher);
    begin(LabComm.SAMPLE);
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

  public void begin(Class<? extends LabCommSample> c) throws IOException {
    begin(registry.getTag(c));
  }

  public void end(Class<? extends LabCommSample> c) throws IOException {
    data.flush();
    WritePacked32(writer, current_tag);
    WritePacked32(writer, bytes.size());
    writer.write(bytes.toByteArray());
    bytes.reset();
  }

  private void WritePacked32(LabCommWriter s, long value) throws IOException {
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
    //data.writeShort(0); // HACK...
    //data.writeUTF(value);

    //kludge, to replace above hack with packed length
    ByteArrayOutputStream tmpb = new ByteArrayOutputStream();
    DataOutputStream tmps = new DataOutputStream(tmpb);

    tmps.writeUTF(value);
    tmps.flush();
    byte[] tmp = tmpb.toByteArray();
  
    encodePacked32(tmp.length-2);
    for (int i = 2 ; i < tmp.length ; i++) {
      encodeByte(tmp[i]);
    }
  }

  public void encodePacked32(long value) throws IOException {
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
}

