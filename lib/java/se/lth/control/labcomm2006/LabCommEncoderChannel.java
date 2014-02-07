package se.lth.control.labcomm2006;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class LabCommEncoderChannel implements LabCommEncoder {

  private LabCommWriter writer;
  private ByteArrayOutputStream bytes;
  private DataOutputStream data;
  private LabCommEncoderRegistry registry;

  public LabCommEncoderChannel(LabCommWriter writer, 
			       boolean emitVersion) throws IOException {
    this.writer = writer;
    bytes = new ByteArrayOutputStream();
    data = new DataOutputStream(bytes);
    registry = new LabCommEncoderRegistry();
    if (emitVersion) {
        throw new RuntimeError("Labcomm 2006 does not support emitVersion");
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
    encodePacked32(LabComm.SAMPLE);
    encodePacked32(index);
    encodeString(dispatcher.getName());
    byte[] signature = dispatcher.getSignature();
    for (int i = 0 ; i < signature.length ; i++) {
      encodeByte(signature[i]);
    }
    end(null);
  }

  public void begin(Class<? extends LabCommSample> c) throws IOException {
    encodePacked32(registry.getTag(c));
  }

  public void end(Class<? extends LabCommSample> c) throws IOException {
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

  /**
     method for API harmonization with labcomm2013.
     Labcomm2006 encodes lengths etc as 32 bit ints.
  */
  public inline void encodePacked32(long value) throws IOException {
    encodeInt(value);
  }
}

