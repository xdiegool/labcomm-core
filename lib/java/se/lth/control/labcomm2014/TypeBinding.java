package se.lth.control.labcomm2014;

import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.util.Iterator;
import java.util.NoSuchElementException;
import se.lth.control.labcomm2014.Decoder;
import se.lth.control.labcomm2014.DecoderChannel;
import se.lth.control.labcomm2014.SampleDispatcher;
import se.lth.control.labcomm2014.SampleHandler;

public class TypeBinding implements BuiltinType {
    private int sampleIndex;
    private int typeIndex;

  public TypeBinding(int sampleIndex, int typeIndex) {
      this.sampleIndex = sampleIndex;
      this.typeIndex = typeIndex;
  }

  public int getSampleIndex() {
    return sampleIndex;
  }

  public int getTypeIndex() {
    return typeIndex;
  }

  public boolean isSelfBinding() {
      return typeIndex == Constant.TYPE_BIND_SELF;
  }

  public interface Handler extends SampleHandler {
    public void handle_TypeBinding(TypeBinding value) throws Exception;
  }

  public static void register(Decoder d, Handler h) throws IOException {
    d.register(Dispatcher.singleton(), h);
  }

  public static void register(Encoder e) throws IOException {
    register(e,false);
  }

  public static void register(Encoder e, boolean sendMetaData) throws IOException {
    throw new IOException("cannot send TypeDefs");
  }

 static class Dispatcher implements SampleDispatcher<TypeBinding> {

    private static Dispatcher singleton;

    public synchronized static Dispatcher singleton() {
      if(singleton==null) singleton=new Dispatcher();
      return singleton;
    }

    public Class<TypeBinding> getSampleClass() {
      return TypeBinding.class;
    }

    public String getName() {
      return "TypeBinding";
    }

    public byte getTypeDeclTag() {
      throw new Error("Should not be called");
    }

    public boolean isSample() {
      throw new Error("Should not be called");
    }
    public boolean hasStaticSignature() {
      throw new Error("Should not be called");
    }

    public byte[] getSignature() {
      return null; // not used for matching
    }

  public int getNumIntentions() {
      return 0;
  }

  public byte[] getIntentionBytes() {
     return null; // not used for matching
  }

    public void encodeTypeDef(Encoder e, int index) throws IOException{
      throw new Error("Should not be called");
    }

    public void registerTypeDeps(Encoder e) throws IOException{
      throw new Error("Should not be called");
    }

    public void decodeAndHandle(Decoder d,
                                SampleHandler h) throws Exception {
      ((Handler)h).handle_TypeBinding(TypeBinding.decode(d));
    }

    public boolean hasDependencies() {
        return false;
    }

    public Iterator<SampleDispatcher> getDependencyIterator() {
        return new Iterator<SampleDispatcher>() {
            public boolean hasNext() {
                return false;
            }

            public SampleDispatcher next() throws NoSuchElementException {
                throw new NoSuchElementException();
            }
            public void remove() throws UnsupportedOperationException {
                throw new UnsupportedOperationException();
            }
        };
    }

    public DataType getDataType() {
        throw new Error("not implemented");
    }
  }

  public static void encode(Encoder e, TypeBinding value) throws IOException {
    throw new Error("Should not be called");
  }

  /* HERE BE DRAGONS!
   * This exposes (and relies on the stability of) indices that are
   * internal to a decoder.
   *
   * The SAMPLE_DEF and TYPE_DEF must already have been received for the
   * indices to be known, so this can be changed to instead return
   * references to the SampleDispactcher corresponding to the sample type
   * and the matching TypeDef.
   *
   * Sketch:
   *
   * SampleDispatcher sd = d.getDispatcherForId(sampleIndex);
   * TypeDef td = d.getTypeDefForId(typeIndex);
   *
   * return new TypeBinding(sd, td);
   *
   * assuming that the Decoder keeps a registry for TypeDefs
   */
  public static TypeBinding decode(Decoder d) throws IOException {
    TypeBinding result;
    int sampleIndex = d.decodePacked32();
    int typeIndex = d.decodePacked32();

    result = new TypeBinding(sampleIndex, typeIndex);
    return result;
  }
}

