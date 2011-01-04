package se.lth.control.labcomm;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class LabCommEncoderChannel implements LabCommEncoder {

  private OutputStream writer;
  private ByteArrayOutputStream bytes;
  private DataOutputStream data;
  private LabCommEncoderRegistry registry;

  public LabCommEncoderChannel(OutputStream writer) {
    this.writer = writer;
    bytes = new ByteArrayOutputStream();
    data = new DataOutputStream(bytes);
    registry = new LabCommEncoderRegistry();
  }

  public void register(LabCommDispatcher dispatcher) throws IOException {
    int index = registry.add(dispatcher);
    encodeInt(LabComm.SAMPLE);
    encodeInt(index);
    encodeString(dispatcher.getName());
    byte[] signature = dispatcher.getSignature();
    for (int i = 0 ; i < signature.length ; i++) {
      encodeByte(signature[i]);
    }
    end(null);
  }

  public void begin(Class<? extends LabCommSample> c) throws IOException {
    encodeInt(registry.getTag(c));
  }

  public void end(Class<? extends LabCommSample> c) throws IOException {
    data.flush();
    bytes.writeTo(writer);
    bytes.reset();
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
    data.writeShort(0); // HACK...
    data.writeUTF(value);
  }

}

