package se.lth.control.labcomm;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;

public class EncoderChannel implements Encoder {

  private Writer writer;
  private ByteArrayOutputStream bytes = new ByteArrayOutputStream();
  private DataOutputStream data = new DataOutputStream(bytes);
  private EncoderRegistry sample_def_registry = new EncoderRegistry();
  private EncoderRegistry sample_ref_registry = new EncoderRegistry();
  private EncoderRegistry type_def_registry = new EncoderRegistry();
  private int current_tag;

  public EncoderChannel(Writer writer) throws IOException {
    this.writer = writer;

    begin(Constant.VERSION);
    encodeString(Constant.CURRENT_VERSION);
    end(null);
  }

  public EncoderChannel(OutputStream writer) throws IOException {
    this(new WriterWrapper(writer));
  }

  private void bindType(int sampleId, int typeId) throws IOException {
        begin(Constant.TYPE_BINDING);
        encodePacked32(sampleId);
        encodePacked32(typeId);
        end(null);
  }

  private void registerSample(SampleDispatcher dispatcher) throws IOException {
        int index = sample_def_registry.add(dispatcher);
        begin(dispatcher.getTypeDeclTag());
        encodePacked32(index);
        encodeString(dispatcher.getName());
        byte[] signature = dispatcher.getSignature();
        encodePacked32(signature.length);
        for (int i = 0 ; i < signature.length ; i++) {
            encodeByte(signature[i]);
        }
        end(null);
        int tindex = registerTypeDef(dispatcher);
        bindType(index, tindex);

  }

  private int registerTypeDef(SampleDispatcher dispatcher) throws IOException {
      //XXX A bit crude; maybe add boolean registry.contains(...) and check
      //    if already registered
        try {
            return type_def_registry.getTag(dispatcher);
        } catch (IOException e) {
            int index = type_def_registry.add(dispatcher);
            begin(dispatcher.getTypeDeclTag());
            encodePacked32(index);
            encodeString(dispatcher.getName());
            dispatcher.encodeTypeDef(this, index);
            end(null);
            return index;
        }
  }

  public void register(SampleDispatcher dispatcher) throws IOException {
    switch (dispatcher.getTypeDeclTag())  {
        case Constant.SAMPLE_DEF: {
            registerSample(dispatcher);
            break;
        }
        case Constant.TYPE_DEF: {
            registerTypeDef(dispatcher);
            break;
        }
        default:
            throw new Error("Unknown typeDeclTag: "+dispatcher.getTypeDeclTag());
    }
  }

  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    System.err.println(dispatcher);
    int index = sample_ref_registry.add(dispatcher);
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

  public void begin(int tag) {
    current_tag = tag;
    bytes.reset();
  }

  public void begin(Class<? extends SampleType> c) throws IOException {
    begin(sample_def_registry.getTag(c));
  }

  public void end(Class<? extends SampleType> c) throws IOException {
    data.flush();
    WritePacked32(writer, current_tag);
    WritePacked32(writer, bytes.size());
    writer.write(bytes.toByteArray());
    bytes.reset();
  }

  /** 
   * @return the id of a TYPE_DEF 
   */
  public int getTypeId(Class<? extends SampleType> c) throws IOException {
    return type_def_registry.getTag(c);
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

  public void encodeSampleRef(Class value) throws IOException {
    int index = 0;
    try {
      index = sample_ref_registry.getTag(value);
    } catch (NullPointerException e) {
    }
    data.writeInt(index);
  }

}

