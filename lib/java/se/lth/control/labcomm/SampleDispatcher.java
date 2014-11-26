package se.lth.control.labcomm;

public interface SampleDispatcher <T> {
    
  public Class getSampleClass();
    
  public String getName();

  public byte[] getSignature();

  public void decodeAndHandle(Decoder decoder,
			      SampleHandler handler) throws Exception;

  /** return the tag SAMPLE_DEF or TYPE_DEF, for use
   *  by encoder.register.
   *  TODO: refactor types, moving this to a super-interface
   *  applicable to both type and sample defs.
   */ 
  public byte getTypeDeclTag();
}

