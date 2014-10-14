package se.lth.control.labcomm2006;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class EncoderChannel implements Encoder {

  private Writer writer;
  private ByteArrayOutputStream bytes;
  private DataOutputStream data;
  private EncoderRegistry registry;

  public EncoderChannel(Writer writer, 
                        boolean emitVersion) throws IOException {
    this.writer = writer;
    bytes = new ByteArrayOutputStream();
    data = new DataOutputStream(bytes);
    registry = new EncoderRegistry();
    if (emitVersion) {
        throw new IllegalArgumentException("Labcomm 2006 does not support emitVersion");
    }
  }

  public EncoderChannel(Writer writer) throws IOException {
    this(writer, false);
  }

  public EncoderChannel(OutputStream writer, 
                        boolean emitVersion) throws IOException {
    this(new WriterWrapper(writer), emitVersion);
  }

  public EncoderChannel(OutputStream writer) throws IOException {
    this(new WriterWrapper(writer), false);
  }

  public void register(SampleDispatcher dispatcher) throws IOException {
    int index = registry.add(dispatcher);
    encodePacked32(Constant.SAMPLE);
    encodePacked32(index);
    encodeString(dispatcher.getName());
    byte[] signature = dispatcher.getSignature();
    for (int i = 0 ; i < signature.length ; i++) {
      encodeByte(signature[i]);
    }
    end(null);
  }

  public void begin(Class<? extends Sample> c) throws IOException {
    encodePacked32(registry.getTag(c));
  }

  public void end(Class<? extends Sample> c) throws IOException {
    data.flush();
    //XXX when writer was a stream, it was probably a bit more GC efficient:
    //bytes.writeTo(writer);
    writer.write(bytes.toByteArray());
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

  /**
     method for API harmonization with labcomm20141009.
     Labcomm2006 encodes lengths etc as 32 bit ints.
  */
  public void encodePacked32(long value) throws IOException {
    if(value > Integer.MAX_VALUE) {
      throw new IllegalArgumentException("Value too large, must fit in 32 bits");
    }
    encodeInt((int) value);
  }
}

