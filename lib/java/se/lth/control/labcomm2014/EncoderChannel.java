package se.lth.control.labcomm2014;

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

  private EncoderChannel(Writer writer, boolean emitVersion) throws IOException {
    this.writer = writer;

    if(emitVersion){
      begin(Constant.VERSION);
      encodeString(Constant.CURRENT_VERSION);
      end(null);
    }
  }
  public EncoderChannel(Writer writer) throws IOException {
      this(writer, true);
  }

  public EncoderChannel(OutputStream writer) throws IOException {
    this(new WriterWrapper(writer), true);
  }

  private EncoderChannel(OutputStream writer, boolean emitVersion) throws IOException {
    this(new WriterWrapper(writer), emitVersion);
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
        //HERE BE DRAGONS! numintentions does not include name
//        encodePacked32(dispatcher.getNumIntentions()+1);
        encodePacked32(1);
        encodePacked32(0); // the empty key == name
        encodeString(dispatcher.getName());
//        byte[] intentions = dispatcher.getIntentionBytes();
//        for (int k = 0 ; k < intentions.length ; k++) {
//            encodeByte(intentions[k]);
//        }
        byte[] signature = dispatcher.getSignature();
        encodePacked32(signature.length);
        for (int i = 0 ; i < signature.length ; i++) {
            encodeByte(signature[i]);
        }
        end(null);
        int tindex;
        if(dispatcher.hasDependencies()){
            tindex = registerTypeDef(dispatcher);
        } else {
            tindex = Constant.TYPE_BIND_SELF;
        }
        bindType(index, tindex);

  }

  private static class WrappedEncoder extends EncoderChannel{
      private Encoder wrapped;

      public WrappedEncoder(Encoder e, OutputStream s, boolean emitVersion) throws IOException {
          super(s,emitVersion);
          this.wrapped = e;
      }
      public int getTypeId(Class<? extends SampleType> c) throws IOException{
         return wrapped.getTypeId(c);
      }
  }


  private int registerTypeDef(SampleDispatcher dispatcher) throws IOException {
      // check if already registered
        try {
            return type_def_registry.getTag(dispatcher);
        } catch (IOException e) {
            //otherwise, add to the registry
            int index = type_def_registry.add(dispatcher);
            //wrap encoder to get encoded length of signature
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            EncoderChannel wrapped = new WrappedEncoder(this, baos, false);
            dispatcher.encodeTypeDef(wrapped, index);
            wrapped.flush();
            byte b[] = baos.toByteArray();

            begin(Constant.TYPE_DEF);
            encodePacked32(index);
            //HERE BE DRAGONS! numintentions does not include name
//          encodePacked32(dispatcher.getNumIntentions()+1);
            encodePacked32(1);
            encodePacked32(0); // the empty key == name
            encodeString(dispatcher.getName());
            encodePacked32(b.length);
            for(int i = 0; i<b.length; i++) {
                encodeByte(b[i]);
            }
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
     //HERE BE DRAGONS! numintentions does not include name
//        encodePacked32(dispatcher.getNumIntentions()+1);
    encodePacked32(1);
    encodePacked32(0); // the empty key == name
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

  /* aux. method used to allow nesting encoders to find encoded size */
  private void flush() throws IOException{
    data.flush();
    writer.write(bytes.toByteArray());
    bytes.reset();
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
        //we want to return 0 for unregistered ref types
    }
    data.writeInt(index);
  }

}

