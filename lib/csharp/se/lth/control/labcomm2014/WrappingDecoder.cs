using System;

namespace se.lth.control.labcomm2014 {

  public class WrappingDecoder: Decoder {

    private Decoder decoder;

    public WrappingDecoder(Decoder decoder) {
      this.decoder = decoder;
    }

    public virtual void runOne() {
      decoder.runOne();
    }

    public virtual void run() {
      decoder.run();
    }    

    public virtual void register(SampleDispatcher dispatcher, 
		  SampleHandler handler) {
      decoder.register(dispatcher, handler);
    }
                  
    public virtual void registerSampleRef(SampleDispatcher dispatcher) {
      decoder.registerSampleRef(dispatcher);
    }

    public virtual bool decodeBoolean() {
      return decoder.decodeBoolean();
    }
    
    public virtual byte decodeByte() {
      return decoder.decodeByte();
    }
    
    public virtual short decodeShort() {
      return decoder.decodeShort();
    }
    
    public virtual int decodeInt() {
      return decoder.decodeInt();
    }
    
    public virtual long decodeLong() {
      return decoder.decodeLong();
    }
    
    public virtual float decodeFloat() {
      return decoder.decodeFloat();
    }
    
    public virtual double decodeDouble() {
      return decoder.decodeDouble();
    }
    
    public virtual String decodeString() {
      return decoder.decodeString();
    }
    
    public virtual int decodePacked32() {
      return decoder.decodePacked32();
    }
    
    public virtual SampleDispatcher decodeSampleRef() {
      return decoder.decodeSampleRef();
    }
    
  }

} 
