using System;

namespace se.lth.control.labcomm2014 {

  public interface Decoder {

    void runOne();
    void run();
    void register(SampleDispatcher dispatcher,
		  SampleHandler handler);
    void registerSampleRef(SampleDispatcher dispatcher);

    bool decodeBoolean();
    byte decodeByte();
    short decodeShort();
    int decodeInt();
    long decodeLong();
    float decodeFloat();
    double decodeDouble();
    String decodeString();
    int decodePacked32();
    SampleDispatcher decodeSampleRef();

  }

} 
