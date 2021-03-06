package se.lth.control.labcomm2014;
import java.util.Iterator;
import java.io.IOException;

public interface SampleDispatcher <T extends SampleType>{

  public Class<T> getSampleClass();

  public String getName();

  public byte[] getSignature();

  public int getNumIntentions();

  public byte[] getIntentionBytes();

  public void decodeAndHandle(Decoder decoder,
			      SampleHandler handler) throws Exception;

  /** @return true if the type depends on one or more typedefs
   */
  public boolean hasDependencies();

  public void encodeTypeDef(Encoder e, int index) throws IOException;

  /** return the tag SAMPLE_DEF or TYPE_DEF, for use
   *  by encoder.register.
   *  TODO: refactor types, moving this to a super-interface
   *  applicable to both type and sample defs.
   */
  public byte getTypeDeclTag();

  public void registerTypeDeps(Encoder e) throws IOException;

  public Iterator<SampleDispatcher> getDependencyIterator();

  public DataType getDataType();
}

